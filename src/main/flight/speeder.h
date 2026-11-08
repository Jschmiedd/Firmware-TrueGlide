/*
 * This file is part of Cleanflight and Betaflight.
 *
 * Cleanflight and Betaflight are free software. You can redistribute
 * this software and/or modify this software under the terms of the
 * GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Cleanflight and Betaflight are distributed in the hope that they
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Speeder transition (TrueGlide).
 *
 * For speed quads that take off "standing" (fuselage vertical, 90 degrees) and
 * fly fast with the fuselage close to horizontal (0 degrees). In ANGLE mode,
 * while the SPEEDER switch is on, the level target is pitched forward
 * progressively, at speeder_transition_rate degrees per second, up to
 * speeder_cruise_angle. Switching it off (or failsafe) pitches back up the
 * same way, to hover.
 *
 * The pilot keeps full control around the transition angle (+/- angle_limit),
 * the final target is limited to SPEEDER_MAX_ANGLE (attitude estimation is not
 * reliable closer to 90 degrees).
 *
 * Optional (speeder_fpv_mix = ON): roll and yaw sticks are rotated by the
 * current transition angle, like fpv_mix_degrees but following the transition,
 * so roll stays a roll around the flight path at speed and a normal roll in hover.
 *
 * Nothing changes unless a SPEEDER switch is set up in the Modes tab.
 */

#pragma once

#include <stdbool.h>

#define SPEEDER_MAX_ANGLE 80.0f
// TrueGlide (audit du 07/11/2026, F5) : retour à plat en failsafe, GPS Rescue et ALT/POS HOLD : rapide (80° en 1/3 s)
// mais progressif (pas de cabrage brutal), quel que soit speeder_transition_rate (5 à 180 °/s)
#define SPEEDER_FAST_RETURN_RATE 240.0f

// Pure logic (unit tested)
float speederStepOffset(float offset, float target, float ratePerS, float dT);
void speederMixRollYaw(float angleDeg, float *roll, float *yaw);

// Firmware glue
void speederUpdate(float dT);           // once per PID loop
float speederGetPitchOffset(void);      // degrees, positive = nose down
bool speederIsEngaged(void);
void speederApplyFpvMix(float *roll, float *yaw);
