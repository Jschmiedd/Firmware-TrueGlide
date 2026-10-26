/*
 * TrueGlide - carte simulée : mémoire des registres du microcontrôleur (voir carte_simulee.h).
 * Free software under the GPL-3.0 license, WITHOUT ANY WARRANTY; see the LICENSE file.
 */

#include <stdint.h>

#include "platform.h"

__attribute__((aligned(4096))) uint8_t tgPeripheriques[TG_PERIPH_TAILLE];
__attribute__((aligned(4096))) uint8_t tgAhb2[TG_AHB2_TAILLE];
__attribute__((aligned(4096))) uint8_t tgCoeur[TG_COEUR_TAILLE];

// identifiant unique de la puce simulée (remplacé par la page, par exemple par celui de la carte récoltée)
uint32_t tgIdentifiantUnique[3] = { 0x54475349, 0x4d554c45, 0x45000001 };

// niveau d'interruption simulé (build/atomic.h)
uint8_t atomic_BASEPRI;
uint32_t tgPrimask;

// mémoire système : taille de la flash (1024 Ko pour la F405) et étalonnages d'usine typiques de la STM32F4
// (tension de référence interne, capteur de température à 30 °C et 110 °C)
uint8_t tgSysteme[TG_SYSTEME_TAILLE] = {
    [0x222] = 0x00, [0x223] = 0x04,
    [0x22A] = 0xF7, [0x22B] = 0x05,
    [0x22C] = 0xAC, [0x22D] = 0x03,
    [0x22E] = 0xC2, [0x22F] = 0x04,
};

// valeurs des registres à la mise sous tension, quand elles comptent pour le firmware
void tgMemoireInit(void)
{
    // mesures internes de l'ADC1 (canaux injectés) : tension de référence nominale (étalonnage d'usine, soit 3,3 V)
    // et capteur de température de la puce à 35 °C ; conversion terminée
    ADC1->JDR1 = 0x05F7;
    ADC1->JDR2 = 0x03AC + 17;
    ADC1->SR |= ADC_FLAG_JEOC;
}
