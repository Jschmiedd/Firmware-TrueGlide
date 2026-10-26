/*
 * TrueGlide - carte simulée : puces de la carte.
 * Free software under the GPL-3.0 license, WITHOUT ANY WARRANTY; see the LICENSE file.
 *
 * Chaque puce répond comme la vraie, juste assez pour que les pilotes du firmware la reconnaissent et la règlent :
 * mêmes registres d'identification et d'état. La carte simulée est posée à plat et immobile : gyroscope à zéro,
 * accéléromètre à 1 g sur l'axe Z. Aucune vidéo n'arrive sur la puce OSD. La mémoire flash est vide au démarrage.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "platform.h"

#include "drivers/io.h"

#include "simulee/peripheriques.h"

// ---------------------------------------------------------------------------------------------------------------
// gyroscope et accéléromètre InvenSense ICM42688P (registres en 4 bancs, lecture : bit 7 de l'adresse à 1)

#define ICM_BANQUE_SEL      0x76
#define ICM_WHO_AM_I        0x75
#define ICM_DEVICE_CONFIG   0x11
#define ICM_ACCEL_Z1        0x23

static uint8_t icmRegistres[4][128];
static uint8_t icmBanque;
static uint8_t icmAdresse;
static bool icmLecture;
static int icmPosition;

static void icmReinitialiser(void)
{
    memset(icmRegistres, 0, sizeof(icmRegistres));
    icmBanque = 0;
}

static uint8_t icmValeur(uint8_t reg)
{
    if (reg == ICM_WHO_AM_I) {
        return 0x47; // ICM42688P
    }
    if (reg == ICM_BANQUE_SEL) {
        return icmBanque;
    }
    if (icmBanque == 0 && reg >= 0x1D && reg <= 0x2A) {
        // température (25 °C : 0), accéléromètre (Z : 1 g à ±16 g, 2048 points par g), gyroscope (immobile)
        return reg == ICM_ACCEL_Z1 ? 0x08 : 0x00;
    }
    return icmRegistres[icmBanque][reg & 0x7F];
}

static void icmDebut(void)
{
    icmPosition = 0;
}

static uint8_t icmOctet(uint8_t envoye)
{
    if (icmPosition++ == 0) {
        icmAdresse = envoye & 0x7F;
        icmLecture = envoye & 0x80;
        return 0xFF;
    }
    const uint8_t reg = icmAdresse++;
    if (icmLecture) {
        return icmValeur(reg);
    }
    if (reg == ICM_BANQUE_SEL) {
        icmBanque = envoye & 0x03;
    } else if (icmBanque == 0 && reg == ICM_DEVICE_CONFIG && (envoye & 0x01)) {
        icmReinitialiser(); // réinitialisation logicielle
    } else {
        icmRegistres[icmBanque][reg & 0x7F] = envoye;
    }
    return 0xFF;
}

static void rien(void)
{
}

static const tgPuceSpi_t icm42688p = { icmDebut, icmOctet, rien };

// ---------------------------------------------------------------------------------------------------------------
// puce OSD MAX7456 (ou AT7456E) : paires adresse / donnée ; lecture : bit 7 de l'adresse à 1

#define MAX_DMM     0x04
#define MAX_DMDI    0x07
#define MAX_OSDM    0x0C
#define MAX_STAT    0x20
#define MAX_FIN     0xFF

static uint8_t maxRegistres[128];
static enum { MAX_ADRESSE, MAX_DONNEE, MAX_LIRE, MAX_SUITE } maxEtat;
static uint8_t maxAdresse;

static void maxDebut(void)
{
    maxEtat = MAX_ADRESSE;
}

static uint8_t maxOctet(uint8_t envoye)
{
    switch (maxEtat) {
    case MAX_ADRESSE:
        if (envoye == MAX_FIN) {
            return 0xFF;
        }
        maxAdresse = envoye & 0x7F;
        maxEtat = (envoye & 0x80) ? MAX_LIRE : MAX_DONNEE;
        return 0xFF;
    case MAX_LIRE:
        maxEtat = MAX_ADRESSE;
        if (maxAdresse == MAX_STAT) {
            return 0x04; // aucune caméra branchée : perte de synchronisation, mémoire des caractères libre
        }
        return maxRegistres[maxAdresse];
    case MAX_DONNEE:
        // le bit de réinitialisation de VM0 retombe tout seul
        maxRegistres[maxAdresse] = maxAdresse == 0x00 ? (envoye & ~0x02) : envoye;
        maxEtat = (maxAdresse == MAX_DMDI && (maxRegistres[MAX_DMM] & 0x01)) ? MAX_SUITE : MAX_ADRESSE;
        return 0xFF;
    case MAX_SUITE:
        // écriture de l'écran en incrémentation automatique, jusqu'à 0xFF
        if (envoye == MAX_FIN) {
            maxEtat = MAX_ADRESSE;
        }
        return 0xFF;
    }
    return 0xFF;
}

static void maxInitialiser(void)
{
    maxRegistres[MAX_OSDM] = 0x1B; // valeur à la mise sous tension
}

static const tgPuceSpi_t max7456 = { maxDebut, maxOctet, rien };

// ---------------------------------------------------------------------------------------------------------------
// mémoire flash Winbond W25Q128FV (16 Mo), vide au démarrage ; pages de 4 Ko créées à la première écriture

#define FL_TAILLE       (16u * 1024u * 1024u)
#define FL_PAGE         4096u

static uint8_t *flPages[FL_TAILLE / FL_PAGE];
static uint8_t flCommande;
static uint32_t flAdresse;
static int flPosition;
static bool flEcritureAutorisee;

static uint8_t flLireOctet(uint32_t adresse)
{
    adresse %= FL_TAILLE;
    const uint8_t *page = flPages[adresse / FL_PAGE];
    return page ? page[adresse % FL_PAGE] : 0xFF;
}

static void flProgrammer(uint32_t adresse, uint8_t valeur)
{
    adresse %= FL_TAILLE;
    uint8_t **page = &flPages[adresse / FL_PAGE];
    if (!*page) {
        if (valeur == 0xFF) {
            return;
        }
        *page = malloc(FL_PAGE);
        memset(*page, 0xFF, FL_PAGE);
    }
    (*page)[adresse % FL_PAGE] &= valeur; // la programmation ne fait que passer des bits à 0
}

static void flEffacer(uint32_t adresse, uint32_t taille)
{
    adresse = (adresse % FL_TAILLE) & ~(taille - 1);
    for (uint32_t p = adresse / FL_PAGE; p < (adresse + taille) / FL_PAGE; p++) {
        free(flPages[p]);
        flPages[p] = NULL;
    }
}

static void flDebut(void)
{
    flPosition = 0;
}

static uint8_t flOctet(uint8_t envoye)
{
    const int position = flPosition++;
    if (position == 0) {
        flCommande = envoye;
        flAdresse = 0;
        if (envoye == 0x06) {
            flEcritureAutorisee = true;
        } else if (envoye == 0x04) {
            flEcritureAutorisee = false;
        } else if ((envoye == 0xC7 || envoye == 0x60) && flEcritureAutorisee) {
            flEffacer(0, FL_TAILLE);
            flEcritureAutorisee = false;
        }
        return 0xFF;
    }
    switch (flCommande) {
    case 0x9F: { // identification JEDEC : Winbond, W25Q128FV
        static const uint8_t id[] = { 0xEF, 0x40, 0x18, 0x00 };
        return position <= 4 ? id[position - 1] : 0x00;
    }
    case 0x05: // état : jamais occupée
        return flEcritureAutorisee ? 0x02 : 0x00;
    case 0x35:
    case 0x15:
        return 0x00;
    case 0x03: // lecture
        if (position <= 3) {
            flAdresse = (flAdresse << 8) | envoye;
            return 0xFF;
        }
        return flLireOctet(flAdresse++);
    case 0x0B: // lecture rapide : un octet d'attente après l'adresse
        if (position <= 3) {
            flAdresse = (flAdresse << 8) | envoye;
            return 0xFF;
        }
        return position == 4 ? 0xFF : flLireOctet(flAdresse++);
    case 0x02: // programmation d'une page
        if (position <= 3) {
            flAdresse = (flAdresse << 8) | envoye;
            return 0xFF;
        }
        if (flEcritureAutorisee) {
            flProgrammer((flAdresse & ~0xFFu) | ((flAdresse + (uint32_t)(position - 4)) & 0xFFu), envoye);
        }
        return 0xFF;
    case 0x20: // effacement 4 Ko
    case 0xD8: // effacement 64 Ko
        flAdresse = (flAdresse << 8) | envoye;
        if (position == 3 && flEcritureAutorisee) {
            flEffacer(flAdresse, flCommande == 0x20 ? 4096u : 65536u);
            flEcritureAutorisee = false;
        }
        return 0xFF;
    default:
        return 0xFF;
    }
}

static void flFin(void)
{
    if (flCommande == 0x02) {
        flEcritureAutorisee = false;
    }
}

static const tgPuceSpi_t w25q128 = { flDebut, flOctet, flFin };

// ---------------------------------------------------------------------------------------------------------------
// câblage de la vraie carte (config.h) : broche CS -> puce

const tgPuceSpi_t *tgPuceSpi(IO_t cs)
{
    static bool pret;
    if (!pret) {
        icmReinitialiser();
        maxInitialiser();
        pret = true;
    }
#if defined(GYRO_1_CS_PIN) && defined(USE_GYRO_SPI_ICM42688P)
    if (cs == IOGetByTag(IO_TAG(GYRO_1_CS_PIN))) {
        return &icm42688p;
    }
#endif
#if defined(MAX7456_SPI_CS_PIN) && defined(USE_MAX7456)
    if (cs == IOGetByTag(IO_TAG(MAX7456_SPI_CS_PIN))) {
        return &max7456;
    }
#endif
#if defined(FLASH_CS_PIN) && defined(USE_FLASH_W25Q128FV)
    if (cs == IOGetByTag(IO_TAG(FLASH_CS_PIN))) {
        return &w25q128;
    }
#endif
    return NULL;
}
