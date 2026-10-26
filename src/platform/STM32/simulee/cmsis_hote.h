/*
 * TrueGlide - carte simulée : fonctions CMSIS du cœur ARM Cortex-M pour l'ordinateur ou le navigateur.
 * Free software under the GPL-3.0 license, WITHOUT ANY WARRANTY; see the LICENSE file.
 *
 * Inclus avant tout autre fichier (-include) dans la construction de la carte simulée : il prend la place de
 * cmsis_gcc.h, dont les instructions ARM (interruptions, barrières mémoire, attente) n'existent pas ailleurs. La carte
 * simulée n'a pas d'interruptions : les activer ou les couper ne fait rien ; le niveau BASEPRI est simulé par
 * build/atomic.h.
 */

#pragma once

#include <stdint.h>

#define __CMSIS_GCC_H

#ifndef __has_builtin
#define __has_builtin(x) (0)
#endif

#define __ASM                                  __asm
#define __INLINE                               inline
#define __STATIC_INLINE                        static inline
#define __STATIC_FORCEINLINE                   __attribute__((always_inline)) static inline
#define __NO_RETURN                            __attribute__((__noreturn__))
#define __USED                                 __attribute__((used))
#define __WEAK                                 __attribute__((weak))
#define __PACKED                               __attribute__((packed, aligned(1)))
#define __PACKED_STRUCT                        struct __attribute__((packed, aligned(1)))
#define __PACKED_UNION                         union __attribute__((packed, aligned(1)))
#define __ALIGNED(x)                           __attribute__((aligned(x)))
#define __RESTRICT                             __restrict
#define __COMPILER_BARRIER()                   __asm volatile("" ::: "memory")

struct __attribute__((packed)) T_UINT32 { uint32_t v; };
struct __attribute__((packed, aligned(1))) T_UINT16_WRITE { uint16_t v; };
struct __attribute__((packed, aligned(1))) T_UINT16_READ { uint16_t v; };
struct __attribute__((packed, aligned(1))) T_UINT32_WRITE { uint32_t v; };
struct __attribute__((packed, aligned(1))) T_UINT32_READ { uint32_t v; };
#define __UNALIGNED_UINT32(x)                  (((struct T_UINT32 *)(x))->v)
#define __UNALIGNED_UINT16_WRITE(addr, val)    (void)((((struct T_UINT16_WRITE *)(void *)(addr))->v) = (val))
#define __UNALIGNED_UINT16_READ(addr)          (((const struct T_UINT16_READ *)(const void *)(addr))->v)
#define __UNALIGNED_UINT32_WRITE(addr, val)    (void)((((struct T_UINT32_WRITE *)(void *)(addr))->v) = (val))
#define __UNALIGNED_UINT32_READ(addr)          (((const struct T_UINT32_READ *)(const void *)(addr))->v)

// interruptions : aucune dans la carte simulée
extern uint32_t tgPrimask;
__STATIC_FORCEINLINE void __enable_irq(void) { tgPrimask = 0; }
__STATIC_FORCEINLINE void __disable_irq(void) { tgPrimask = 1; }
__STATIC_FORCEINLINE uint32_t __get_PRIMASK(void) { return tgPrimask; }
__STATIC_FORCEINLINE void __set_PRIMASK(uint32_t priMask) { tgPrimask = priMask; }
__STATIC_FORCEINLINE void __enable_fault_irq(void) { }
__STATIC_FORCEINLINE void __disable_fault_irq(void) { }
__STATIC_FORCEINLINE uint32_t __get_FAULTMASK(void) { return 0; }
__STATIC_FORCEINLINE void __set_FAULTMASK(uint32_t faultMask) { (void)faultMask; }
__STATIC_FORCEINLINE uint32_t __get_CONTROL(void) { return 0; }
__STATIC_FORCEINLINE void __set_CONTROL(uint32_t control) { (void)control; }
__STATIC_FORCEINLINE uint32_t __get_IPSR(void) { return 0; }      // jamais dans une interruption
__STATIC_FORCEINLINE uint32_t __get_APSR(void) { return 0; }
__STATIC_FORCEINLINE uint32_t __get_xPSR(void) { return 0; }
__STATIC_FORCEINLINE uint32_t __get_PSP(void) { return 0; }
__STATIC_FORCEINLINE void __set_PSP(uint32_t topOfProcStack) { (void)topOfProcStack; }
__STATIC_FORCEINLINE uint32_t __get_MSP(void) { return (uint32_t)(uintptr_t)__builtin_frame_address(0); }
__STATIC_FORCEINLINE void __set_MSP(uint32_t topOfMainStack) { (void)topOfMainStack; }
__STATIC_FORCEINLINE uint32_t __get_FPSCR(void) { return 0; }
__STATIC_FORCEINLINE void __set_FPSCR(uint32_t fpscr) { (void)fpscr; }

// attente, barrières : rien à attendre
#define __NOP()                                __asm volatile ("" ::: "memory")
#define __WFI()                                __asm volatile ("" ::: "memory")
#define __WFE()                                __asm volatile ("" ::: "memory")
#define __SEV()                                __asm volatile ("" ::: "memory")
#define __BKPT(value)                          ((void)(value))
__STATIC_FORCEINLINE void __ISB(void) { __COMPILER_BARRIER(); }
__STATIC_FORCEINLINE void __DSB(void) { __COMPILER_BARRIER(); }
__STATIC_FORCEINLINE void __DMB(void) { __COMPILER_BARRIER(); }

// calculs sur les bits
__STATIC_FORCEINLINE uint32_t __REV(uint32_t value) { return __builtin_bswap32(value); }
__STATIC_FORCEINLINE uint32_t __REV16(uint32_t value) { return ((value & 0x00ff00ffu) << 8) | ((value >> 8) & 0x00ff00ffu); }
__STATIC_FORCEINLINE int16_t __REVSH(int16_t value) { return (int16_t)__builtin_bswap16((uint16_t)value); }
__STATIC_FORCEINLINE uint32_t __ROR(uint32_t op1, uint32_t op2)
{
    op2 %= 32u;
    return op2 ? (op1 >> op2) | (op1 << (32u - op2)) : op1;
}
__STATIC_FORCEINLINE uint32_t __RBIT(uint32_t value)
{
    uint32_t r = 0;
    for (int i = 0; i < 32; i++) {
        r = (r << 1) | ((value >> i) & 1u);
    }
    return r;
}
__STATIC_FORCEINLINE uint8_t __CLZ(uint32_t value) { return value ? (uint8_t)__builtin_clz(value) : 32u; }

// accès exclusifs : un seul fil, ils réussissent toujours
__STATIC_FORCEINLINE uint8_t __LDREXB(volatile uint8_t *addr) { return *addr; }
__STATIC_FORCEINLINE uint16_t __LDREXH(volatile uint16_t *addr) { return *addr; }
__STATIC_FORCEINLINE uint32_t __LDREXW(volatile uint32_t *addr) { return *addr; }
__STATIC_FORCEINLINE uint32_t __STREXB(uint8_t value, volatile uint8_t *addr) { *addr = value; return 0; }
__STATIC_FORCEINLINE uint32_t __STREXH(uint16_t value, volatile uint16_t *addr) { *addr = value; return 0; }
__STATIC_FORCEINLINE uint32_t __STREXW(uint32_t value, volatile uint32_t *addr) { *addr = value; return 0; }
__STATIC_FORCEINLINE void __CLREX(void) { }

__STATIC_FORCEINLINE int32_t __SSAT(int32_t val, uint32_t sat)
{
    if (sat >= 1u && sat <= 32u) {
        const int32_t max = (int32_t)((1u << (sat - 1u)) - 1u);
        const int32_t min = -1 - max;
        return val > max ? max : (val < min ? min : val);
    }
    return val;
}
__STATIC_FORCEINLINE uint32_t __USAT(int32_t val, uint32_t sat)
{
    if (sat <= 31u) {
        const uint32_t max = ((1u << sat) - 1u);
        return val > (int32_t)max ? max : (val < 0 ? 0u : (uint32_t)val);
    }
    return (uint32_t)val;
}

// registres du microcontrôleur : tous les fichiers (y compris la bibliothèque du fabricant) voient la mémoire simulée
#include "stm32f4xx.h"
#include "simulee/carte_simulee.h"
