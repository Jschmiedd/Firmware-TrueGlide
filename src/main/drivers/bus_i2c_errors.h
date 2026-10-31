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

/*
 * TrueGlide: I2C errors per device.
 *
 * The I2C driver keeps one error flag per bus, reset when the next transfer starts. A device sharing the bus (a
 * compass next to the barometer) can then start and finish a transfer between the end of a failed barometer read and
 * the moment the barometer task looks at the flag: the error would be lost and a partial sample used. Here the error
 * of a finished transfer is kept for the device it addressed (7-bit address), until that same device starts a new
 * transfer. Pure logic, unit tested.
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct i2cErrorTrack_s {
    uint32_t failed[4];     // one bit per 7-bit address
} i2cErrorTrack_t;

// The last transfer on the bus addressed addr7. Once it is over (bus not busy) with an error, remember it for addr7.
static inline void i2cErrorTrackNote(i2cErrorTrack_t *track, bool busy, bool error, uint8_t addr7)
{
    if (!busy && error) {
        track->failed[(addr7 >> 5) & 3] |= 1u << (addr7 & 31);
    }
}

// A new transfer to addr7 starts: its previous error is no longer the current one.
static inline void i2cErrorTrackStart(i2cErrorTrack_t *track, uint8_t addr7)
{
    track->failed[(addr7 >> 5) & 3] &= ~(1u << (addr7 & 31));
}

// Did the last transfer to addr7 end with an error?
static inline bool i2cErrorTrackFailed(const i2cErrorTrack_t *track, uint8_t addr7)
{
    return (track->failed[(addr7 >> 5) & 3] >> (addr7 & 31)) & 1u;
}
