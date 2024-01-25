// SPDX-License-Identifier: zlib-acknowledgement
#pragma once

// NOTE(Ryan): CMSIS
#include "core_cm4.h"

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
