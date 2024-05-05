// SPDX-License-Identifier: zlib-acknowledgement
#pragma once

#include "external/cmsis/core_cm4.h"

// TODO(Ryan): cmsis uses dsp for simd? (__ARM_FEATURE_DSP) && (__ARM_FEATURE_DSP == 1))
// uqadd8, so can do parallel 8-bit math?

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

#define cli() nvic_globalirq_disable()
#define sei() nvic_globalirq_enable()

INTERNAL void 
qemu_arm_exit(void) 
{
  register u32 r0 __asm__("r0");
  r0 = 0x18;
  register u32 r1 __asm__("r1");
  r1 = 0x20026;
  __asm__ volatile("bkpt #0xAB");
}
