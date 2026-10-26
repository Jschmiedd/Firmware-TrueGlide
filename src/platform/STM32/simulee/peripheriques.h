/*
 * TrueGlide - carte simulée : puces reliées aux bus de la carte (gyroscope, OSD, mémoire flash, baromètre).
 * Free software under the GPL-3.0 license, WITHOUT ANY WARRANTY; see the LICENSE file.
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "drivers/io_types.h"

// une puce sur un bus SPI : début (CS au niveau bas), un octet échangé, fin (CS au niveau haut)
typedef struct {
    void (*debut)(void);
    uint8_t (*octet)(uint8_t envoye);
    void (*fin)(void);
} tgPuceSpi_t;

// puce sélectionnée par cette broche CS, d'après le config.h de la vraie carte (NULL : rien ne répond)
const tgPuceSpi_t *tgPuceSpi(IO_t cs);

// une puce sur un bus I2C (adresse sur 7 bits) : écriture et lecture de registres ; false si personne ne répond
bool tgI2cEcrire(int bus, uint8_t adresse, uint8_t reg, uint8_t nombre, const uint8_t *octets);
bool tgI2cLire(int bus, uint8_t adresse, uint8_t reg, uint8_t nombre, uint8_t *octets);
