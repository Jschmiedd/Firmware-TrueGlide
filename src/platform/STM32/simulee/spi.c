/*
 * TrueGlide - carte simulée : bus SPI.
 * Free software under the GPL-3.0 license, WITHOUT ANY WARRANTY; see the LICENSE file.
 *
 * Le pilote SPI du microcontrôleur (STM32/bus_spi_stdperiph.c) est gardé pour tout le reste (broches, horloges,
 * DMA réservés : `resource`, `dma` restent ceux de la vraie carte). Seuls les échanges d'octets passent ici : chaque
 * transfert est remis à la puce simulée choisie par sa broche de sélection (CS), comme sur la carte. Les transferts
 * se font sans DMA, tout de suite, et les fonctions de rappel des pilotes sont appelées comme à la fin d'un DMA.
 */

#include <stdbool.h>
#include <stdint.h>

#include "platform.h"

#ifdef USE_SPI

#include "drivers/bus_spi.h"
#include "drivers/bus_spi_impl.h"
#include "drivers/io.h"

#include "simulee/peripheriques.h"

static const extDevice_t *appareil;     // appareil dont la séquence est en cours
static bool selectionne;                // broche CS au niveau bas

void spiSequenceStart(const extDevice_t *dev)
{
    dev->bus->initSegment = true;
    appareil = dev;
    selectionne = false;
    spiProcessSegmentsPolled(dev);
}

bool spiInternalReadWriteBufPolled(spiResource_t *spiInstance, const uint8_t *txData, uint8_t *rxData, int len)
{
    UNUSED(spiInstance);
    const tgPuceSpi_t *puce = appareil ? tgPuceSpi(appareil->busType_u.spi.csnPin) : NULL;

    if (puce && !selectionne) {
        puce->debut();
    }
    selectionne = true;
    for (int i = 0; i < len; i++) {
        const uint8_t b = txData ? txData[i] : 0xFF;
        const uint8_t r = puce ? puce->octet(b) : 0xFF;
        if (rxData) {
            rxData[i] = r;
        }
    }
    if (appareil && appareil->bus->curSegment->negateCS) {
        if (puce) {
            puce->fin();
        }
        selectionne = false;
    }
    return true;
}

#endif
