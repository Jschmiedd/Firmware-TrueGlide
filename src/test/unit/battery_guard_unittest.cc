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

#include <stdbool.h>
#include <stdint.h>

extern "C" {
    #include "platform.h"

    #include "common/maths.h"

    #include "fc/rc.h"
    #include "fc/runtime_config.h"

    #include "pg/pg.h"
    #include "pg/pg_ids.h"
    #include "pg/battery_guard.h"

    #include "flight/battery_guard.h"

    // stubs for the firmware glue
    uint8_t armingFlags = 0;
    float getRcDeflectionAbs(int) { return 0.0f; }
    uint8_t getBatteryCellCount(void) { return 4; }
    static uint16_t stubCellCv = 380;
    uint16_t getBatteryAverageCellVoltage(void) { return stubCellCv; }
    uint32_t millis(void) { return 0; }
    // GPS Rescue: isAvailable stays at its init value (true) while the rescue task does not run (not configured)
    static bool stubRescueConfigured = false;
    static bool stubRescueAvailable = true;
    bool gpsRescueIsConfigured(void) { return stubRescueConfigured; }
    bool gpsRescueIsAvailable(void) { return stubRescueAvailable; }
}

#include "unittest_macros.h"
#include "gtest/gtest.h"

TEST(BatteryGuardTest, DisabledByDefault)
{
    pgResetAll();
    EXPECT_EQ(0, batteryGuardConfig()->arm_min_cell_voltage);
    EXPECT_EQ(0, batteryGuardConfig()->rth_cell_voltage);
    EXPECT_EQ(10, batteryGuardConfig()->rth_delay);
}

TEST(BatteryGuardTest, ArmingBlockedOnlyWhenMeasuredAndLow)
{
    EXPECT_FALSE(batteryGuardArmingBlocked(0, 4, 300));      // off
    EXPECT_TRUE(batteryGuardArmingBlocked(370, 4, 369));
    EXPECT_FALSE(batteryGuardArmingBlocked(370, 4, 370));
    EXPECT_FALSE(batteryGuardArmingBlocked(370, 0, 300));    // no battery detected: unknown, not low
    EXPECT_FALSE(batteryGuardArmingBlocked(370, 4, 0));
}

static void steps(batteryRthStatus_t *s, int n, uint16_t cellCv, bool armed = true, bool avail = true, float stick = 0.0f)
{
    for (int i = 0; i < n; i++) {
        batteryRthStep(s, 350, 10, armed, 4, cellCv, avail, stick, 0.1f);
    }
}

TEST(BatteryGuardTest, ReturnOnlyAfterTheWholeDelay)
{
    batteryRthStatus_t s;
    batteryRthReset(&s);
    steps(&s, 95, 340);                                     // 9.5 s below: not yet
    EXPECT_EQ(BATTERY_RTH_IDLE, s.state);
    steps(&s, 6, 340);
    EXPECT_EQ(BATTERY_RTH_ACTIVE, s.state);
}

TEST(BatteryGuardTest, SagDuringPunchRestartsTheCount)
{
    batteryRthStatus_t s;
    batteryRthReset(&s);
    for (int k = 0; k < 10; k++) {
        steps(&s, 50, 340);                                 // 5 s sagging on a punch
        steps(&s, 1, 360);                                  // back above the threshold: count restarts
    }
    EXPECT_EQ(BATTERY_RTH_IDLE, s.state);
}

TEST(BatteryGuardTest, PilotCancelsUntilDisarm)
{
    batteryRthStatus_t s;
    batteryRthReset(&s);
    steps(&s, 110, 340);
    ASSERT_EQ(BATTERY_RTH_ACTIVE, s.state);
    steps(&s, 1, 340, true, true, 0.2f);                    // small stick input: still returning
    EXPECT_EQ(BATTERY_RTH_ACTIVE, s.state);
    steps(&s, 1, 340, true, true, 0.5f);                    // the pilot takes over
    EXPECT_EQ(BATTERY_RTH_CANCELLED, s.state);
    steps(&s, 200, 320);                                    // no new request while armed
    EXPECT_EQ(BATTERY_RTH_CANCELLED, s.state);
    steps(&s, 1, 320, false);                               // disarm resets
    EXPECT_EQ(BATTERY_RTH_IDLE, s.state);
}

TEST(BatteryGuardTest, StickHeldWhenTheReturnStartsDoesNotCancelIt)
{
    batteryRthStatus_t s;
    batteryRthReset(&s);
    steps(&s, 110, 340, true, true, 0.6f);                  // flying forward, stick held, battery low
    ASSERT_EQ(BATTERY_RTH_ACTIVE, s.state);
    steps(&s, 5, 340, true, true, 0.6f);                    // still held: the return goes on
    EXPECT_EQ(BATTERY_RTH_ACTIVE, s.state);
    steps(&s, 1, 340, true, true, 0.0f);                    // sticks released
    steps(&s, 1, 340, true, true, 0.6f);                    // a new input cancels
    EXPECT_EQ(BATTERY_RTH_CANCELLED, s.state);
}

TEST(BatteryGuardTest, NoRescueAvailableMeansWarningOnly)
{
    batteryRthStatus_t s;
    batteryRthReset(&s);
    steps(&s, 110, 340, true, false);
    EXPECT_EQ(BATTERY_RTH_UNAVAILABLE, s.state);
    steps(&s, 1, 340, true, true);                          // GPS back: the return starts
    EXPECT_EQ(BATTERY_RTH_ACTIVE, s.state);
}

TEST(BatteryGuardTest, OffOrUnmeasuredDoesNothing)
{
    batteryRthStatus_t s;
    batteryRthReset(&s);
    for (int i = 0; i < 300; i++) {
        batteryRthStep(&s, 0, 10, true, 4, 300, true, 0.0f, 0.1f);      // threshold 0 = off
    }
    EXPECT_EQ(BATTERY_RTH_IDLE, s.state);
    for (int i = 0; i < 300; i++) {
        batteryRthStep(&s, 350, 10, true, 0, 0, true, 0.0f, 0.1f);       // no battery measured
    }
    EXPECT_EQ(BATTERY_RTH_IDLE, s.state);
}

// Audit Fable 31/10/2026 (critique) : sans GPS Rescue configuré (ni interrupteur, ni failsafe GPS-RESCUE), le retour
// batterie ne doit jamais s'enclencher : le mode GPS RESCUE n'aurait pas de contrôleur (gaz au ralenti, manches ignorés).
static void runGuardSeconds(int seconds, timeUs_t *t)
{
    for (int i = 0; i < seconds * 10; i++) {
        *t += 100000;
        batteryGuardUpdate(*t);
    }
}

TEST(BatteryGuardTest, ReturnNeverEngagesWhenRescueNotConfigured)
{
    pgResetAll();
    batteryGuardConfigMutable()->rth_cell_voltage = 340;
    batteryGuardConfigMutable()->rth_delay = 2;
    armingFlags = ARMED;
    stubCellCv = 330;
    timeUs_t t = 1000;
    stubRescueConfigured = false;
    stubRescueAvailable = true;      // stale init value of the rescue state
    runGuardSeconds(5, &t);
    EXPECT_EQ(BATTERY_RTH_UNAVAILABLE, batteryGuardRthState());
    EXPECT_FALSE(batteryGuardRthRequested());

    stubRescueConfigured = true;     // switch or failsafe GPS-RESCUE: the rescue controller runs
    runGuardSeconds(5, &t);
    EXPECT_EQ(BATTERY_RTH_ACTIVE, batteryGuardRthState());
    EXPECT_TRUE(batteryGuardRthRequested());
    armingFlags = 0;
    stubCellCv = 380;
}
