/*
 * This file is part of Betaflight.
 *
 * Betaflight is free software. You can redistribute this software
 * and/or modify this software under the terms of the GNU General
 * Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later
 * version.
 *
 * Betaflight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this software.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 */

// TrueGlide: the motor speed is "fresh" only after a new speed frame, not after an extended telemetry frame
// (temperature, voltage, current), which carries no speed (drivers/dshot.c updateDshotTelemetry)

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

extern "C" {
    #include "platform.h"
    #include "drivers/dshot.h"
    #include "pg/motor.h"
    #include "pg/pg.h"
    #include "pg/pg_ids.h"

    #include "build/debug.h"

    extern dshotTelemetryState_t dshotTelemetryState;
    extern uint8_t dshotMotorCount;

    // stubs
    bool useDshotTelemetry = false;
    uint8_t debugMode = 0;
    int16_t debug[DEBUG16_VALUE_COUNT];
    uint8_t atomic_BASEPRI;
    motorConfig_t motorConfig_System;
    bool featureIsEnabled(uint32_t) { return false; }
}

#include "unittest_macros.h"
#include "gtest/gtest.h"

// one new raw frame from the driver for motor 0
static void frame(uint16_t raw)
{
    dshotTelemetryState.motorState[0].rawValue = raw;
    dshotTelemetryState.motorState[0].rawValueSeq++;
    dshotTelemetryState.rawValueState = DSHOT_RAW_VALUE_STATE_NOT_PROCESSED;
    updateDshotTelemetry();
}

// a cycle without any new frame
static void noFrame(void)
{
    dshotTelemetryState.rawValueState = DSHOT_RAW_VALUE_STATE_NOT_PROCESSED;
    updateDshotTelemetry();
}

TEST(DshotTelemetryTest, ExtendedFrameDoesNotMakeTheSpeedFresh)
{
    useDshotTelemetry = true;
    dshotMotorCount = 1;
    // EDT already seen on this motor (temperature type present), so extended frames are decoded as such
    dshotTelemetryState.motorState[0].telemetryTypes = (1 << DSHOT_TELEMETRY_TYPE_eRPM) | (1 << DSHOT_TELEMETRY_TYPE_TEMPERATURE);

    frame(0x0110);      // speed frame (odd type nibble)
    EXPECT_TRUE(isDshotMotorTelemetryFresh(0));

    for (int i = 0; i < DSHOT_TELEMETRY_STALE_FRAMES + 2; i++) {
        frame(0x0200 | 35);    // temperature frames only: 35 °C, no speed
    }
    EXPECT_EQ(35u, dshotTelemetryState.motorState[0].telemetryData[DSHOT_TELEMETRY_TYPE_TEMPERATURE]);
    EXPECT_FALSE(isDshotMotorTelemetryFresh(0));   // the speed is as old as these frames

    frame(0x0110);
    EXPECT_TRUE(isDshotMotorTelemetryFresh(0));

    for (int i = 0; i < DSHOT_TELEMETRY_STALE_FRAMES + 2; i++) {
        noFrame();
    }
    EXPECT_FALSE(isDshotMotorTelemetryFresh(0));
}
