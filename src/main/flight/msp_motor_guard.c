/*
 * This file is part of TrueGlide (based on Betaflight).
 *
 * TrueGlide is free software. You can redistribute this software and/or modify this software under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdbool.h>

#include "platform.h"

#include "flight/msp_motor_guard.h"

static bool guardActive;
static int guardSource;
static timeMs_t guardLastMs;

void mspMotorGuardNoteSet(int source, timeMs_t now)
{
    guardActive = true;
    guardSource = source;
    guardLastMs = now;
}

void mspMotorGuardNoteActivity(int source, timeMs_t now)
{
    if (guardActive && source == guardSource) {
        guardLastMs = now;
    }
}

void mspMotorGuardClear(void)
{
    guardActive = false;
}

bool mspMotorGuardExpired(timeMs_t now)
{
    if (guardActive && cmpTimeMs(now, guardLastMs) > MSP_MOTOR_GUARD_TIMEOUT_MS) {
        guardActive = false;
        return true;
    }
    return false;
}

bool mspMotorGuardActive(void)
{
    return guardActive;
}
