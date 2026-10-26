/*
 * TrueGlide - carte simulée : démarrage, temps et redémarrage, à la place des fichiers du microcontrôleur
 * (common/stm32/system.c, STM32/system_stm32f4xx.c, STM32/persistent.c).
 * Free software under the GPL-3.0 license, WITHOUT ANY WARRANTY; see the LICENSE file.
 *
 * Le temps vient de l'horloge de l'ordinateur ou du navigateur. L'horloge simulée du processeur tourne à la
 * fréquence de la vraie carte (SystemCoreClock), pour que les durées affichées en cycles restent justes.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "platform.h"

#include "drivers/io.h"
#include "drivers/io_impl.h"
#include "drivers/light_led.h"
#include "drivers/persistent.h"
#include "drivers/system.h"
#include "drivers/time.h"

#include "simulee/liaison.h"

// fréquence du processeur de la vraie carte (STM32F405 : 168 MHz)
uint32_t SystemCoreClock = 168000000;
static uint32_t valeurHse;

// horloge système tirée de la PLL (2 : PLLP), elle-même tirée du quartz externe (1 : HSE), comme la vraie carte
int SystemSYSCLKSource(void)
{
    return 2;
}

int SystemPLLSource(void)
{
    return 1;
}

static struct timespec debut;

static uint64_t nanosDepuisDebut(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (uint64_t)(t.tv_sec - debut.tv_sec) * 1000000000ull + (uint64_t)t.tv_nsec - (uint64_t)debut.tv_nsec;
}

void systemInit(void)
{
    clock_gettime(CLOCK_MONOTONIC, &debut);
    tgMemoireInit();
    tgConfigCharger();
    persistentObjectInit();
}

void systemClockSetHSEValue(uint32_t frequency)
{
    valeurHse = frequency;
    UNUSED(valeurHse);
}

void cycleCounterInit(void)
{
}

bool isMPUSoftReset(void)
{
    return false;
}

void OverclockRebootIfNecessary(uint32_t overclockLevel)
{
    UNUSED(overclockLevel);
}

uint32_t micros(void)
{
    return (uint32_t)(nanosDepuisDebut() / 1000u);
}

uint32_t microsISR(void)
{
    return micros();
}

uint32_t millis(void)
{
    return (uint32_t)(nanosDepuisDebut() / 1000000u);
}

uint32_t getCycleCounter(void)
{
    return (uint32_t)(nanosDepuisDebut() * (SystemCoreClock / 1000000u) / 1000u);
}

int32_t clockCyclesToMicros(int32_t clockCycles)
{
    return clockCycles / (int32_t)(SystemCoreClock / 1000000u);
}

float clockCyclesToMicrosf(int32_t clockCycles)
{
    return clockCycles / (SystemCoreClock / 1000000.0f);
}

int32_t clockCyclesTo10thMicros(int32_t clockCycles)
{
    return 10 * clockCycles / (int32_t)(SystemCoreClock / 1000000u);
}

int32_t clockCyclesTo100thMicros(int32_t clockCycles)
{
    return 100 * clockCycles / (int32_t)(SystemCoreClock / 1000000u);
}

uint32_t clockMicrosToCycles(uint32_t us)
{
    return us * (SystemCoreClock / 1000000u);
}

void delayMicroseconds(uint32_t us)
{
    const uint64_t fin = nanosDepuisDebut() + (uint64_t)us * 1000u;
    while (nanosDepuisDebut() < fin) {
    }
}

void delay(uint32_t ms)
{
    delayMicroseconds(ms * 1000u);
}

// redémarrage : la page relance la carte avec sa mémoire des réglages (comme un redémarrage de la vraie carte)
void systemReset(void)
{
    printf("[carte] redémarrage\n");
    tgRedemarrer();
}

void systemResetToBootloader(bootloaderRequestType_e requestType)
{
    UNUSED(requestType);
    printf("[carte] mode chargeur de démarrage : pas dans la carte simulée, redémarrage\n");
    tgRedemarrer();
}

void systemResetToMsc(int timezoneOffsetMinutes)
{
    UNUSED(timezoneOffsetMinutes);
    tgRedemarrer();
}

void systemResetFromMsc(void)
{
    tgRedemarrer();
}

void indicateFailure(failureMode_e mode, int codeRepeatsRemaining)
{
    UNUSED(codeRepeatsRemaining);
    printf("[carte] défaut %d\n", mode);
}

void failureMode(failureMode_e mode)
{
    indicateFailure(mode, 10);
    systemReset();
}

void initialiseMemorySections(void)
{
}

// broches non utilisées : comme la vraie carte (entrée avec rappel au niveau haut)
static void unusedPinInit(IO_t io)
{
    if (IOGetOwner(io) == OWNER_FREE) {
        IOConfigGPIO(io, IOCFG_IPU);
    }
}

void unusedPinsInit(void)
{
    IOTraversePins(unusedPinInit);
}

// objets gardés pendant un redémarrage logiciel (raison du redémarrage, demande de chargeur…)
static uint32_t objetsPersistants[PERSISTENT_OBJECT_COUNT];

void persistentObjectInit(void)
{
}

uint32_t persistentObjectRead(persistentObjectId_e id)
{
    return id < PERSISTENT_OBJECT_COUNT ? objetsPersistants[id] : 0;
}

void persistentObjectWrite(persistentObjectId_e id, uint32_t value)
{
    if (id < PERSISTENT_OBJECT_COUNT) {
        objetsPersistants[id] = value;
    }
}
