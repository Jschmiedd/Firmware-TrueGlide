/*
 * This file is part of TrueGlide (based on Betaflight).
 *
 * TrueGlide is free software. You can redistribute this software and/or modify this software under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 */

// TrueGlide, 3e relecture indépendante du 07/11/2026 (C1) : la carte démarre en DShot ; le protocole est changé en
// mémoire (PWM) sans redémarrage, puis les bornes sont recalculées (MSP_SET_PID_ADVANCED, enregistrement...). La valeur
// d'arrêt doit rester celle du pilote DShot qui tourne (commande d'arrêt), jamais 1000 (≈ 48 % des gaz en DShot).

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

extern "C" {
    #include "platform.h"
    #include "pg/pg.h"
    #include "pg/pg_ids.h"
    #include "pg/motor.h"
    #include "drivers/motor.h"
    #include "drivers/dshot.h"
    #include "drivers/dshot_command.h"
    #include "fc/rc_controls.h"

    PG_REGISTER(motorConfig_t, motorConfig, PG_MOTOR_CONFIG, 0);
    PG_REGISTER(flight3DConfig_t, flight3DConfig, PG_MOTOR_3D_CONFIG, 0);

    static bool feature3d;
    bool featureIsEnabled(uint32_t mask) { (void)mask; return feature3d; }
    float scaleRangef(float x, float srcFrom, float srcTo, float destFrom, float destTo)
    {
        return (x - srcFrom) * (destTo - destFrom) / (srcTo - srcFrom) + destFrom;
    }
    // PWM : comme pwm_output.c (analogInitEndpoints)
    void analogInitEndpoints(const motorConfig_t *mc, float outputLimit, float *outputLow, float *outputHigh, float *disarm, float *deadbandMotor3dHigh, float *deadbandMotor3dLow)
    {
        (void)outputLimit; (void)deadbandMotor3dHigh; (void)deadbandMotor3dLow;
        *disarm = feature3d ? flight3DConfig()->neutral3d : mc->mincommand;
        *outputLow = mc->mincommand;
        *outputHigh = mc->maxthrottle;
    }
    static int dshotInits, pwmInits;
    bool dshotPwmDevInit(motorDevice_t *device, const motorDevConfig_t *c) { (void)device; (void)c; dshotInits++; return true; }
    bool motorPwmDevInit(motorDevice_t *device, const motorDevConfig_t *c, uint16_t idle) { (void)device; (void)c; (void)idle; pwmInits++; return true; }
    bool isDshotBitbangActive(const motorDevConfig_t *c) { (void)c; return false; }
    bool dshotBitbangDevInit(motorDevice_t *device, const motorDevConfig_t *c) { (void)device; (void)c; return false; }
    uint32_t millis(void) { return 0; }
    uint8_t launchControlState;
    bool useDshotTelemetry;
    void delayMicroseconds(uint32_t us) { (void)us; }
    uint16_t getBatteryVoltage(void) { return 1600; }
}

#include "unittest_macros.h"
#include "gtest/gtest.h"

static float disarmFor(const motorConfig_t *mc)
{
    float low, high, disarm = -1, d3h, d3l;
    motorInitEndpoints(mc, 1.0f, &low, &high, &disarm, &d3h, &d3l);
    return disarm;
}

TEST(MotorEndpointsTest, ProtocolChangedInRamKeepsRunningDriverStop)
{
    feature3d = false;
    motorConfigMutable()->mincommand = 1000;
    motorConfigMutable()->maxthrottle = 2000;
    motorConfigMutable()->dev.motorProtocol = MOTOR_PROTOCOL_DSHOT600;
    EXPECT_EQ(DSHOT_CMD_MOTOR_STOP, disarmFor(motorConfig()));   // mixerInit, avant le pilote
    motorDevInit(4);                                              // pilote DShot démarré
    EXPECT_EQ(1, dshotInits);
    EXPECT_TRUE(isMotorProtocolDshot());

    // onglet Moteurs : protocole mis en PWM en mémoire, pas de redémarrage, bornes recalculées
    motorConfigMutable()->dev.motorProtocol = MOTOR_PROTOCOL_PWM;
    const float disarm = disarmFor(motorConfig());
    EXPECT_EQ(DSHOT_CMD_MOTOR_STOP, disarm);   // et non 1000 (≈ 48 % des gaz en DShot)
    EXPECT_TRUE(isMotorProtocolDshot());       // la commande DShot d'arrêt reste acceptée
    // la conversion d'une valeur MSP reste celle du pilote DShot
    EXPECT_EQ(DSHOT_CMD_MOTOR_STOP, dshotConvertFromExternal(1000));
}

TEST(MotorEndpointsTest, DshotStopIndependentOf3d)
{
    motorConfigMutable()->dev.motorProtocol = MOTOR_PROTOCOL_PWM;  // toujours changé en mémoire
    feature3d = true;
    EXPECT_EQ(DSHOT_CMD_MOTOR_STOP, disarmFor(motorConfig()));
    feature3d = false;
    EXPECT_EQ(DSHOT_CMD_MOTOR_STOP, disarmFor(motorConfig()));
}
