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
 * Battery guard (TrueGlide).
 *
 * - The motors are never cut in flight because of a low battery: the pilot
 *   decides, warned by the OSD and the beeper as in Betaflight.
 * - battery_arm_min_cell_voltage: arming is refused while the average cell
 *   voltage is below it (battery measured, cell count known).
 * - battery_rth_cell_voltage: when the average cell voltage stays below it for
 *   battery_rth_delay seconds in flight, GPS Rescue brings the quad back home,
 *   only if the rescue is available at that moment (home point, GPS, heading).
 *   The pilot cancels it by moving the roll or pitch stick past
 *   BATTERY_GUARD_CANCEL_STICK; it then stays off until the next disarm.
 *
 * Both are disabled by default (0): nothing changes compared with Betaflight.
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "common/time.h"

#define BATTERY_GUARD_CANCEL_STICK      0.3f    // stick deflection (0..1) that cancels the return

typedef enum {
    BATTERY_RTH_IDLE = 0,       // voltage fine, or not long enough below the threshold
    BATTERY_RTH_ACTIVE,         // return requested
    BATTERY_RTH_CANCELLED,      // cancelled by the pilot until the next disarm
    BATTERY_RTH_UNAVAILABLE,    // voltage low long enough but the rescue cannot fly: warning only
} batteryRthState_e;

typedef struct batteryRthStatus_s {
    batteryRthState_e state;
    float secondsBelow;
} batteryRthStatus_t;

// Pure logic, no hardware access (unit tested)
bool batteryGuardArmingBlocked(uint16_t armMinCellCv, uint8_t cellCount, uint16_t cellCv);
void batteryRthReset(batteryRthStatus_t *s);
void batteryRthStep(batteryRthStatus_t *s, uint16_t thresholdCv, uint8_t delayS, bool armed, uint8_t cellCount,
                    uint16_t cellCv, bool rescueAvailable, float stickDeflection, float dt);

// Firmware glue
void batteryGuardUpdate(timeUs_t currentTimeUs);
bool batteryGuardArmingDisabled(void);
bool batteryGuardRthRequested(void);
batteryRthState_e batteryGuardRthState(void);
