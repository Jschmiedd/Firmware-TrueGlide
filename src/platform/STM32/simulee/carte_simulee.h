/*
 * TrueGlide - carte simulée : le firmware d'une vraie carte STM32F4, compilé pour l'ordinateur ou le navigateur.
 * Free software under the GPL-3.0 license, WITHOUT ANY WARRANTY; see the LICENSE file.
 *
 * Inclus à la fin de platform/platform.h quand TG_CARTE_SIMULEE est défini (jamais dans une vraie construction).
 * Les registres du microcontrôleur deviennent de la mémoire ordinaire : les adresses de base des périphériques
 * (définies par l'en-tête du fabricant et par CMSIS) sont redéfinies vers des tableaux de la carte simulée. Les
 * macros étant développées à l'usage, toutes les adresses qui en dérivent (TIM1, SPI1, GPIOA, DMA2_Stream0, SCB…)
 * suivent. Le code de la vraie carte, ses tables (broches, timers, DMA) et ses réglages restent inchangés.
 */

#pragma once

#include <stdint.h>

// tailles des zones simulées (registres du F4)
#define TG_PERIPH_TAILLE   0x00080000u   // APB1, APB2, AHB1 : 0x40000000 - 0x4007FFFF
#define TG_AHB2_TAILLE     0x00061000u   // AHB2 (USB OTG FS, RNG) : 0x50000000 - 0x50060FFF
#define TG_COEUR_TAILLE    0x00043000u   // ITM, DWT, SCS (NVIC, SCB, SysTick), TPI, DBGMCU : 0xE0000000 - 0xE0042FFF

// la vraie carte calcule en virgule flottante matérielle (__FPU_USED = 1) : certaines valeurs par défaut en
// dépendent (nombre de voies AUX) ; sur l'ordinateur, CMSIS le met à 0
#undef __FPU_USED
#define __FPU_USED 1U

// zone des réglages : 16 Ko dans la mémoire flash de la STM32F405 (secteur 1)
#define TG_CONFIG_TAILLE   (16 * 1024)

extern uint8_t tgPeripheriques[TG_PERIPH_TAILLE];
extern uint8_t tgAhb2[TG_AHB2_TAILLE];
extern uint8_t tgCoeur[TG_COEUR_TAILLE];
extern uint32_t tgIdentifiantUnique[3];

// mémoire système de la puce (0x1FFF7800 - 0x1FFF7A2F : zone OTP, identifiant unique, taille de la flash, étalonnages)
#define TG_SYSTEME_TAILLE  0x230u
extern uint8_t tgSysteme[TG_SYSTEME_TAILLE];
#define TG_SYSTEME(adresse) ((uintptr_t)tgSysteme + ((adresse) - 0x1FFF7800UL))

#undef PERIPH_BASE
#define PERIPH_BASE        ((uintptr_t)tgPeripheriques)
#undef AHB2PERIPH_BASE
#define AHB2PERIPH_BASE    ((uintptr_t)tgAhb2)

#define TG_COEUR(adresse)  ((uintptr_t)tgCoeur + ((adresse) - 0xE0000000UL))
#undef ITM_BASE
#define ITM_BASE           TG_COEUR(0xE0000000UL)
#undef DWT_BASE
#define DWT_BASE           TG_COEUR(0xE0001000UL)
#undef SCS_BASE
#define SCS_BASE           TG_COEUR(0xE000E000UL)
#undef CoreDebug_BASE
#define CoreDebug_BASE     TG_COEUR(0xE000EDF0UL)
#undef TPI_BASE
#define TPI_BASE           TG_COEUR(0xE0040000UL)
#undef DBGMCU_BASE
#define DBGMCU_BASE        TG_COEUR(0xE0042000UL)

