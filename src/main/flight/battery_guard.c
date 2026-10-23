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

#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#include "platform.h"

#ifdef USE_BATTERY_GUARD

#include "common/maths.h"

#include "fc/rc.h"
#include "fc/runtime_config.h"

#include "flight/gps_rescue.h"

#include "pg/battery_guard.h"

#include "sensors/battery.h"

#include "battery_guard.h"

static batteryRthStatus_t rthStatus;
static timeUs_t lastUpdateUs;

bool batteryGuardArmingBlocked(uint16_t armMinCellCv, uint8_t cellCount, uint16_t cellCv)
{
    // only with a measured battery: without a cell count the voltage is unknown, not low
    return armMinCellCv > 0 && cellCount > 0 && cellCv > 0 && cellCv < armMinCellCv;
}

void batteryRthReset(batteryRthStatus_t *s)
{
    s->state = BATTERY_RTH_IDLE;
    s->secondsBelow = 0.0f;
}

void batteryRthStep(batteryRthStatus_t *s, uint16_t thresholdCv, uint8_t delayS, bool armed, uint8_t cellCount,
                    uint16_t cellCv, bool rescueAvailable, float stickDeflection, float dt)
{
    if (!armed || thresholdCv == 0) {
        batteryRthReset(s);
        return;
    }
    if (s->state == BATTERY_RTH_CANCELLED) {
        return;     // the pilot decided: no new request until the next disarm
    }
    if (s->state == BATTERY_RTH_ACTIVE) {
        if (stickDeflection >= BATTERY_GUARD_CANCEL_STICK) {
            s->state = BATTERY_RTH_CANCELLED;
        }
        return;
    }
    if (cellCount == 0 || cellCv == 0) {
        s->secondsBelow = 0.0f;     // voltage not measured: never act on it
        return;
    }
    // the voltage must stay below the threshold for the whole delay: a punch that sags the pack for a moment
    // restarts the count as soon as the voltage comes back up
    s->secondsBelow = (cellCv < thresholdCv) ? s->secondsBelow + dt : 0.0f;
    if (s->secondsBelow >= (float)MAX(delayS, 1)) {
        s->state = rescueAvailable ? BATTERY_RTH_ACTIVE : BATTERY_RTH_UNAVAILABLE;
    } else {
        s->state = BATTERY_RTH_IDLE;
    }
}

void batteryGuardUpdate(timeUs_t currentTimeUs)
{
    const float dt = lastUpdateUs ? cmpTimeUs(currentTimeUs, lastUpdateUs) * 1e-6f : 0.0f;
    lastUpdateUs = currentTimeUs;
    bool rescueAvailable = false;
#ifdef USE_GPS_RESCUE
    rescueAvailable = gpsRescueIsAvailable();
#endif
    const float stick = MAX(getRcDeflectionAbs(FD_ROLL), getRcDeflectionAbs(FD_PITCH));
    batteryRthStep(&rthStatus, batteryGuardConfig()->rth_cell_voltage, batteryGuardConfig()->rth_delay,
                   ARMING_FLAG(ARMED), getBatteryCellCount(), getBatteryAverageCellVoltage(), rescueAvailable,
                   stick, constrainf(dt, 0.0f, 1.0f));
}

bool batteryGuardArmingDisabled(void)
{
    return batteryGuardArmingBlocked(batteryGuardConfig()->arm_min_cell_voltage, getBatteryCellCount(),
                                     getBatteryAverageCellVoltage());
}

bool batteryGuardRthRequested(void)
{
    return rthStatus.state == BATTERY_RTH_ACTIVE;
}

batteryRthState_e batteryGuardRthState(void)
{
    return rthStatus.state;
}

#endif // USE_BATTERY_GUARD
