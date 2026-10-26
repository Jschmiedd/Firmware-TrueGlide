/*
 * TrueGlide - carte simulée : bus I2C.
 * Free software under the GPL-3.0 license, WITHOUT ANY WARRANTY; see the LICENSE file.
 *
 * Le pilote I2C du microcontrôleur (STM32/bus_i2c_stm32f4xx.c) est gardé pour les broches et l'initialisation ;
 * les échanges passent ici, tout de suite, vers les puces simulées (peripheriques.c). Une adresse où rien ne
 * répond compte une erreur, comme sur la vraie carte.
 */

#include <stdbool.h>
#include <stdint.h>

#include "platform.h"

#ifdef USE_I2C

#include "drivers/bus_i2c.h"

#include "simulee/peripheriques.h"

static uint16_t erreurs;

bool i2cWriteBuffer(i2cDevice_e device, uint8_t addr_, uint8_t reg_, uint8_t len_, uint8_t *data)
{
    if (!tgI2cEcrire(device, addr_, reg_, len_, data)) {
        erreurs++;
        return false;
    }
    return true;
}

bool i2cWrite(i2cDevice_e device, uint8_t addr_, uint8_t reg_, uint8_t data)
{
    return i2cWriteBuffer(device, addr_, reg_, 1, &data);
}

bool i2cReadBuffer(i2cDevice_e device, uint8_t addr_, uint8_t reg_, uint8_t len, uint8_t *buf)
{
    if (!tgI2cLire(device, addr_, reg_, len, buf)) {
        erreurs++;
        return false;
    }
    return true;
}

bool i2cRead(i2cDevice_e device, uint8_t addr_, uint8_t reg_, uint8_t len, uint8_t *buf)
{
    return i2cReadBuffer(device, addr_, reg_, len, buf);
}

bool i2cBusy(i2cDevice_e device, bool *error)
{
    UNUSED(device);
    if (error) {
        *error = false;
    }
    return false;
}

uint16_t i2cGetErrorCounter(void)
{
    return erreurs;
}

#endif
