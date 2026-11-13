/*
 * This file is part of TrueGlide, derived from Betaflight.
 * TrueGlide - Copyright (C) 2026 TalkTrueFPV.
 * Free software under the GPL-3.0 license, WITHOUT ANY WARRANTY; see the LICENSE file.
 */

#include <math.h>

#include "platform.h"

#include "sensors/baro_step_filter.h"

bool baroStepFilterAccept(baroStepFilter_t *filter, float altitudeCm)
{
    if (filter->haveLastKept && fabsf(altitudeCm - filter->lastKeptAltitude) > BARO_MAX_STEP_CM
        && filter->droppedSamples < BARO_MAX_DROPPED_SAMPLES) {
        filter->droppedSamples++;
        return false;
    }
    filter->droppedSamples = 0;
    filter->lastKeptAltitude = altitudeCm;
    filter->haveLastKept = true;
    return true;
}
