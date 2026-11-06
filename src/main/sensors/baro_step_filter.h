/*
 * This file is part of TrueGlide, derived from Betaflight.
 * TrueGlide - Copyright (C) 2026 TalkTrueFPV.
 * Free software under the GPL-3.0 license, WITHOUT ANY WARRANTY; see the LICENSE file.
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

// TrueGlide: a barometer sample more than BARO_MAX_STEP_CM away from the previous kept one (impossible within one
// sample, 50 ms) is dropped; after BARO_MAX_DROPPED_SAMPLES in a row the new level is accepted, so a real change is
// followed. Fix of the OSD altitude jumps seen on a SpeedyBee F405 Mini (DPS310): always active, it only drops
// samples that cannot be real.
#define BARO_MAX_STEP_CM          2000.0f
#define BARO_MAX_DROPPED_SAMPLES  5

typedef struct baroStepFilter_s {
    float lastKeptAltitude;
    bool haveLastKept;
    uint8_t droppedSamples;
} baroStepFilter_t;

bool baroStepFilterAccept(baroStepFilter_t *filter, float altitudeCm);
