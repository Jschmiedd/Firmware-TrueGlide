/*
 * This file is part of TrueGlide (based on Betaflight).
 *
 * TrueGlide is free software. You can redistribute this software and/or modify this software under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <stdbool.h>

#include "common/time.h"

// TrueGlide (TG-A1-5, 07/11/2026, 2e relecture) : sécurité du test moteurs par MSP (onglet Moteurs).
// Une valeur reçue par MSP_SET_MOTOR reste appliquée tant que la carte est désarmée. Si le port qui l'a envoyée ne
// parle plus à la carte pendant MSP_MOTOR_GUARD_TIMEOUT_MS, les moteurs sont remis à l'arrêt. Seul ce port compte :
// le trafic MSP d'un autre port (vidéo HD Walksnail / DJI en DisplayPort, MSP par la radio) n'entretient pas le test.
#define MSP_MOTOR_GUARD_TIMEOUT_MS 1000

// Valeur de test reçue par MSP_SET_MOTOR depuis le port « source » (descripteur MSP).
void mspMotorGuardNoteSet(int source, timeMs_t now);
// Trame MSP quelconque reçue depuis le port « source ».
void mspMotorGuardNoteActivity(int source, timeMs_t now);
// Moteurs remis à l'arrêt par ailleurs (enregistrement EEPROM) : plus rien à surveiller.
void mspMotorGuardClear(void);
// Vrai une seule fois quand le délai est dépassé : l'appelant doit alors remettre les moteurs à l'arrêt.
bool mspMotorGuardExpired(timeMs_t now);
// Pour les tests et le diagnostic.
bool mspMotorGuardActive(void);
