/*
 * TrueGlide - carte simulée : mémoire des réglages, à la place de common/stm32/config_flash.c.
 * Free software under the GPL-3.0 license, WITHOUT ANY WARRANTY; see the LICENSE file.
 *
 * La zone a la taille de celle de la vraie carte. Comme la mémoire flash, elle est effacée (0xFF) quand une
 * sauvegarde commence au début de la zone. Elle est gardée dans le fichier eeprom.bin (mémoire de la page dans le
 * navigateur, que la page conserve d'un redémarrage à l'autre).
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "platform.h"

#include "config/config_streamer.h"
#include "config/config_streamer_impl.h"

#include "simulee/liaison.h"

#define FICHIER "eeprom.bin"

__attribute__((aligned(8))) uint8_t tgConfigZone[TG_CONFIG_TAILLE];

void tgConfigCharger(void)
{
    memset(tgConfigZone, 0xff, sizeof(tgConfigZone));
    FILE *f = fopen(FICHIER, "rb");
    if (f) {
        const size_t n = fread(tgConfigZone, 1, sizeof(tgConfigZone), f);
        fclose(f);
        printf("[carte] réglages lus : %u octets\n", (unsigned)n);
    }
}

void configUnlock(void)
{
}

void configClearFlags(void)
{
}

configStreamerResult_e configWriteWord(uintptr_t address, config_streamer_buffer_type_t *buffer)
{
    const uintptr_t debut = (uintptr_t)tgConfigZone;
    if (address < debut || address + CONFIG_STREAMER_BUFFER_SIZE > debut + sizeof(tgConfigZone)) {
        return CONFIG_RESULT_ADDRESS_INVALID;
    }
    if (address == debut) {
        memset(tgConfigZone, 0xff, sizeof(tgConfigZone)); // effacement du secteur, comme la mémoire flash
    }
    memcpy((void *)address, buffer, CONFIG_STREAMER_BUFFER_SIZE);
    return CONFIG_RESULT_SUCCESS;
}

void configLock(void)
{
    FILE *f = fopen(FICHIER, "wb");
    if (f) {
        fwrite(tgConfigZone, 1, sizeof(tgConfigZone), f);
        fclose(f);
    }
}
