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

#include <stdint.h>

extern "C" {
    #include "drivers/bus_i2c_errors.h"
}

#include "gtest/gtest.h"

// TrueGlide (recontrôle Fable N1): errors of the shared I2C bus, kept per device
static const uint8_t BARO = 0x77;      // DPS310
static const uint8_t COMPASS = 0x0D;   // QMC5883L

// The driver keeps one error flag per bus, reset by the next transfer of any device. Sequence seen by the barometer
// task: its read fails, then a compass read succeeds before the barometer looks at the bus.
TEST(I2cErrorsTest, CompassTransferInBetweenDoesNotEraseTheBarometerError)
{
    i2cErrorTrack_t track = {};
    i2cErrorTrackStart(&track, BARO);
    // barometer read over, with an error: noted when the compass transfer starts
    i2cErrorTrackNote(&track, false, true, BARO);
    i2cErrorTrackStart(&track, COMPASS);
    // compass read over, no error (the bus flag is now clear)
    i2cErrorTrackNote(&track, false, false, COMPASS);
    EXPECT_TRUE(i2cErrorTrackFailed(&track, BARO));
    EXPECT_FALSE(i2cErrorTrackFailed(&track, COMPASS));
}

TEST(I2cErrorsTest, CompassErrorIsNotABarometerError)
{
    i2cErrorTrack_t track = {};
    i2cErrorTrackStart(&track, COMPASS);
    i2cErrorTrackNote(&track, false, true, COMPASS);
    EXPECT_FALSE(i2cErrorTrackFailed(&track, BARO));
    EXPECT_TRUE(i2cErrorTrackFailed(&track, COMPASS));
}

TEST(I2cErrorsTest, NewTransferOfTheDeviceClearsItsOldError)
{
    i2cErrorTrack_t track = {};
    i2cErrorTrackNote(&track, false, true, BARO);
    i2cErrorTrackStart(&track, BARO);
    EXPECT_FALSE(i2cErrorTrackFailed(&track, BARO));
    // a transfer still running is not over: its error is not known yet
    i2cErrorTrackNote(&track, true, true, BARO);
    EXPECT_FALSE(i2cErrorTrackFailed(&track, BARO));
}

TEST(I2cErrorsTest, EveryAddressHasItsOwnBit)
{
    i2cErrorTrack_t track = {};
    for (int a = 0; a < 128; a++) {
        i2cErrorTrackNote(&track, false, true, a);
        for (int b = 0; b < 128; b++) {
            EXPECT_EQ(i2cErrorTrackFailed(&track, b), b == a);
        }
        i2cErrorTrackStart(&track, a);
    }
}
