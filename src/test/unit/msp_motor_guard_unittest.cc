/*
 * This file is part of TrueGlide (based on Betaflight).
 *
 * TrueGlide is free software. You can redistribute this software and/or modify this software under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 */

// TG-A1-5, 2e relecture (point 1) : la remise à l'arrêt des moteurs ne doit pas être retardée par le trafic MSP d'un
// autre port que celui qui a lancé le test (Walksnail en DisplayPort, MSP par la radio).

#include <stdbool.h>
#include <stdint.h>

extern "C" {
    #include "flight/msp_motor_guard.h"
}

#include "unittest_macros.h"
#include "gtest/gtest.h"

static const int USB = 0;
static const int WALKSNAIL = 1;
static const int RADIO = 2;

TEST(MspMotorGuardTest, NothingToDoWithoutMotorSet)
{
    mspMotorGuardClear();
    EXPECT_FALSE(mspMotorGuardExpired(100000));
    mspMotorGuardNoteActivity(USB, 0);
    EXPECT_FALSE(mspMotorGuardExpired(100000));
}

TEST(MspMotorGuardTest, ExpiresOnceAfterTimeoutWithoutTraffic)
{
    mspMotorGuardClear();
    mspMotorGuardNoteSet(USB, 5000);
    EXPECT_FALSE(mspMotorGuardExpired(5000 + MSP_MOTOR_GUARD_TIMEOUT_MS));
    EXPECT_TRUE(mspMotorGuardExpired(5000 + MSP_MOTOR_GUARD_TIMEOUT_MS + 1));
    // une seule fois : ensuite plus rien à surveiller
    EXPECT_FALSE(mspMotorGuardExpired(20000));
    EXPECT_FALSE(mspMotorGuardActive());
}

TEST(MspMotorGuardTest, TrafficFromTestPortKeepsTestRunning)
{
    mspMotorGuardClear();
    mspMotorGuardNoteSet(USB, 0);
    // le logiciel interroge la carte toutes les 50 ms pendant 10 s
    for (timeMs_t t = 50; t <= 10000; t += 50) {
        mspMotorGuardNoteActivity(USB, t);
        ASSERT_FALSE(mspMotorGuardExpired(t));
    }
    EXPECT_TRUE(mspMotorGuardActive());
}

TEST(MspMotorGuardTest, WalksnailAndRadioTrafficDoNotKeepTestRunning)
{
    mspMotorGuardClear();
    mspMotorGuardNoteSet(USB, 1000);
    bool expired = false;
    timeMs_t expiredAt = 0;
    // USB coupé à 1000 ms : la vidéo HD et la radio continuent à parler MSP toutes les 10 ms
    for (timeMs_t t = 1010; t <= 4000 && !expired; t += 10) {
        mspMotorGuardNoteActivity(WALKSNAIL, t);
        mspMotorGuardNoteActivity(RADIO, t);
        if (mspMotorGuardExpired(t)) {
            expired = true;
            expiredAt = t;
        }
    }
    EXPECT_TRUE(expired);
    EXPECT_EQ(1000 + MSP_MOTOR_GUARD_TIMEOUT_MS + 10, expiredAt);
}

TEST(MspMotorGuardTest, NewMotorSetFromAnotherPortTakesOver)
{
    mspMotorGuardClear();
    mspMotorGuardNoteSet(USB, 0);
    mspMotorGuardNoteSet(RADIO, 500);
    mspMotorGuardNoteActivity(USB, 1400);   // l'ancien port ne compte plus
    EXPECT_TRUE(mspMotorGuardExpired(1501));
}

TEST(MspMotorGuardTest, ClearStopsWatching)
{
    mspMotorGuardClear();
    mspMotorGuardNoteSet(USB, 0);
    mspMotorGuardClear();   // enregistrement EEPROM : moteurs déjà remis à l'arrêt
    EXPECT_FALSE(mspMotorGuardExpired(5000));
}

TEST(MspMotorGuardTest, MillisWrapAround)
{
    mspMotorGuardClear();
    const timeMs_t start = 0xFFFFFF00u;
    mspMotorGuardNoteSet(USB, start);
    EXPECT_FALSE(mspMotorGuardExpired(start + 500));   // passe par 0
    EXPECT_TRUE(mspMotorGuardExpired(start + MSP_MOTOR_GUARD_TIMEOUT_MS + 1));
}
