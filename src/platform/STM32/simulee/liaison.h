/*
 * TrueGlide - carte simulée : liaison avec l'extérieur (page du navigateur, ou TCP pour les essais sur l'ordinateur).
 * Free software under the GPL-3.0 license, WITHOUT ANY WARRANTY; see the LICENSE file.
 */

#pragma once

#include <stdint.h>

// port 0 : le port USB (VCP) de la carte
#define TG_PORT_USB 0

// octets reçus pour un port (appelé par la page, ou par la boucle TCP sur l'ordinateur)
void tg_entree(int port, uint8_t *octets, int nombre);
// octets reçus en attente pour un port, et lecture
uint32_t tgEnAttente(int port);
uint8_t tgLire(int port);
// octets envoyés par la carte sur un port
void tgEcrire(int port, const uint8_t *octets, int nombre);
// relève les octets arrivés (TCP sur l'ordinateur ; rien dans le navigateur)
void tgRelever(void);

// valeurs des registres à la mise sous tension (memoire.c)
void tgMemoireInit(void);

// mémoire des réglages (fichier eeprom.bin) et redémarrage
void tgConfigCharger(void);
void tgRedemarrer(void);
