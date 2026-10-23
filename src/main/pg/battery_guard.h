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

#pragma once

#include <stdint.h>

#include "pg/pg.h"

typedef struct batteryGuardConfig_s {
    uint16_t arm_min_cell_voltage;   // 0.01 V per cell, arming refused below it, 0 = off
    uint16_t rth_cell_voltage;       // 0.01 V per cell, return to home below it, 0 = off
    uint8_t rth_delay;               // seconds the voltage must stay below rth_cell_voltage
} batteryGuardConfig_t;

PG_DECLARE(batteryGuardConfig_t, batteryGuardConfig);
