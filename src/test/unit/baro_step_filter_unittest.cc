/*
 * This file is part of TrueGlide, derived from Betaflight.
 * TrueGlide - Copyright (C) 2026 TalkTrueFPV.
 * Free software under the GPL-3.0 license, WITHOUT ANY WARRANTY; see the LICENSE file.
 */

extern "C" {
#include "sensors/baro_step_filter.h"
}

#include "gtest/gtest.h"

// Audit du 06/11/2026 : test du filtre des sauts d'altitude du baromètre.

TEST(BaroStepFilterUnittest, FirstSampleAndSmallStepsAreKept)
{
    baroStepFilter_t f = {};
    EXPECT_TRUE(baroStepFilterAccept(&f, 10000.0f));
    EXPECT_TRUE(baroStepFilterAccept(&f, 10500.0f));
    EXPECT_TRUE(baroStepFilterAccept(&f, 12500.0f));   // 20 m exactly: kept
}

TEST(BaroStepFilterUnittest, ImpossibleSpikeIsDropped)
{
    baroStepFilter_t f = {};
    EXPECT_TRUE(baroStepFilterAccept(&f, 10000.0f));
    EXPECT_FALSE(baroStepFilterAccept(&f, -6500000.0f));  // -65 km spike seen on the DPS310
    EXPECT_TRUE(baroStepFilterAccept(&f, 10010.0f));      // back to the real level: kept, counter reset
    EXPECT_EQ(0, f.droppedSamples);
}

TEST(BaroStepFilterUnittest, LastingNewLevelIsFollowedAfterFiveDrops)
{
    baroStepFilter_t f = {};
    EXPECT_TRUE(baroStepFilterAccept(&f, 0.0f));
    for (int i = 0; i < BARO_MAX_DROPPED_SAMPLES; i++) {
        EXPECT_FALSE(baroStepFilterAccept(&f, 5000.0f));
    }
    EXPECT_TRUE(baroStepFilterAccept(&f, 5000.0f));
    EXPECT_TRUE(baroStepFilterAccept(&f, 5100.0f));
}
