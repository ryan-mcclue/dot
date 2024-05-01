// SPDX-License-Identifier: zlib-acknowledgement
#pragma once

#include "external/cmsis/core_cm4.h"

#if defined(DEBUG_BUILD)
  #define BP() \
  do \
  { \
    if (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk) \
    { \
      __asm("bkpt 1"); \
    } \
  } while (0)
#else
  #define BP()
#endif

INTERNAL void
initialise_cpu_timer(void)
{
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CTRL |=  DWT_CTRL_CYCCNTENA_Msk;
}

INTERNAL u32
read_cpu_timer(void)
{
  return DWT->CYCCNT;
}

INTERNAL void 
qemu_arm_exit(void) 
{
  register u32 r0 __asm__("r0");
  r0 = 0x18;
  register u32 r1 __asm__("r1");
  r1 = 0x20026;
  __asm__ volatile("bkpt #0xAB");
}
