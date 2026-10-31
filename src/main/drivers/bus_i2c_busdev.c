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
#include <string.h>

#include "platform.h"

#if defined(USE_I2C)

#include "common/utils.h"

#include "drivers/bus.h"
#include "drivers/bus_i2c.h"
#include "drivers/bus_i2c_busdev.h"
#include "drivers/bus_i2c_errors.h"
#include "drivers/bus_i2c_impl.h"

static uint8_t i2cRegisteredDeviceCount = 0;

#if I2C_TRAIT_STATE
// TrueGlide: error of the last transfer of each device (drivers/bus_i2c_errors.h)
static i2cErrorTrack_t i2cErrorTrack[I2CDEV_COUNT];

// keep the error of the transfer that just ended for the device it addressed
static void i2cNoteFinishedTransfer(i2cDevice_e device)
{
    bool error = false;
    const bool busy = i2cBusy(device, &error);
    i2cErrorTrackNote(&i2cErrorTrack[device], busy, error, i2cDevice[device].state.addr >> 1);
}

// before a transfer to dev: the previous one is noted, dev's own old error is cleared (only if the bus is free: a
// busy bus refuses the transfer and dev's error stays the current one)
static void i2cBeforeTransfer(const extDevice_t *dev)
{
    const i2cDevice_e device = dev->bus->busType_u.i2c.device;
    i2cNoteFinishedTransfer(device);
    if (!i2cBusy(device, NULL)) {
        i2cErrorTrackStart(&i2cErrorTrack[device], dev->busType_u.i2c.address);
    }
}
#else
static void i2cBeforeTransfer(const extDevice_t *dev)
{
    UNUSED(dev);
}
#endif

bool i2cBusWriteRegister(const extDevice_t *dev, uint8_t reg, uint8_t data)
{
    if (!dev->bus) {
        return false;
    }
    i2cBeforeTransfer(dev);
    return i2cWrite(dev->bus->busType_u.i2c.device, dev->busType_u.i2c.address, reg, data);
}

bool i2cBusWriteRegisterStart(const extDevice_t *dev, uint8_t reg, uint8_t data)
{
    // Need a static value, not on the stack
    static uint8_t byte;

    if (!dev->bus) {
        return false;
    }

    if (i2cBusy(dev->bus->busType_u.i2c.device, NULL)) {
        return false;
    }

    byte = data;

    i2cBeforeTransfer(dev);
    return i2cWriteBuffer(dev->bus->busType_u.i2c.device, dev->busType_u.i2c.address, reg, sizeof(byte), &byte);
}

bool i2cBusReadRegisterBuffer(const extDevice_t *dev, uint8_t reg, uint8_t *data, uint8_t length)
{
    if (!dev->bus) {
        return false;
    }
    i2cBeforeTransfer(dev);
    return i2cRead(dev->bus->busType_u.i2c.device, dev->busType_u.i2c.address, reg, length, data);
}

uint8_t i2cBusReadRegister(const extDevice_t *dev, uint8_t reg)
{
    uint8_t data = 0xFF;
    if (dev->bus) {
        i2cBeforeTransfer(dev);
        i2cRead(dev->bus->busType_u.i2c.device, dev->busType_u.i2c.address, reg, 1, &data);
    }
    return data;
}

bool i2cBusReadRegisterBufferStart(const extDevice_t *dev, uint8_t reg, uint8_t *data, uint8_t length)
{
    if (!dev->bus) {
        return false;
    }
    i2cBeforeTransfer(dev);
    return i2cReadBuffer(dev->bus->busType_u.i2c.device, dev->busType_u.i2c.address, reg, length, data);
}

bool i2cBusBusy(const extDevice_t *dev, bool *error)
{
    if (!dev->bus) {
        if (error) {
            *error = true;
        }
        return true;
    }
    // TrueGlide: the driver's error flag belongs to the bus and is reset by the next transfer of any device. The error
    // reported here is the one of this device's last transfer, kept per device (a compass error is not a barometer
    // error, and a compass transfer in between does not erase a barometer error). Without the transfer state (HAL
    // platforms) the owner is unknown: no error is reported (no sample dropped, as in Betaflight).
#if I2C_TRAIT_STATE
    const i2cDevice_e device = dev->bus->busType_u.i2c.device;
    i2cNoteFinishedTransfer(device);
    const bool busy = i2cBusy(device, NULL);
    if (error) {
        *error = !busy && i2cErrorTrackFailed(&i2cErrorTrack[device], dev->busType_u.i2c.address);
    }
#else
    const bool busy = i2cBusy(dev->bus->busType_u.i2c.device, NULL);
    if (error) {
        *error = false;
    }
#endif
    return busy;
}

bool i2cBusSetInstance(extDevice_t *dev, uint32_t device)
{
    // I2C bus structures to associate with external devices
    static busDevice_t i2cBus[I2CDEV_COUNT];

    if ((device < 1) || (device > I2CDEV_COUNT)) {
        return false;
    }

    dev->bus = &i2cBus[I2C_CFG_TO_DEV(device)];
    dev->bus->busType = BUS_TYPE_I2C;
    dev->bus->busType_u.i2c.device = I2C_CFG_TO_DEV(device);

    return true;
}

void i2cBusDeviceRegister(const extDevice_t *dev)
{
    UNUSED(dev);

    i2cRegisteredDeviceCount++;
}

uint8_t i2cGetRegisteredDeviceCount(void)
{
    return i2cRegisteredDeviceCount;
}
#endif
