// SPDX-License-Identifier: zlib-acknowledgement
#pragma once

#include "base-context.h"

// TODO: cycle counter for arm will be in relevent CMSIS folder
#include "base-types.h"

// IMPORTANT: uses stdlib math
#include "base-math.h"

// TODO: handle allocation failure on arm (nil nodes?)
// uses stdlib malloc
#include "base-memory.h"

#include "base-string.h"

// implements breakpoints, logging (stacktrace), timing, randomness
// TODO: distinguish between desktop and embedded functionality
#if defined(PLATFORM_ARM_EABI)
  // TODO(Ryan): Change to cortex specific
  //  --cpu=Cortex-M0plus defines __TARGET_CPU_CORTEX_M0PLUS 
  //  TO SEE: armcc --cpu=xx --list_macros /dev/null/
  #include "base-dev-arm-none-eabi.h"
#else
  #include "base-dev-linux.h"
  #include "base-file.h"
  #include "base-repetition.h"
#endif

#include "base-map.h"
#include "base-profiler.h"

/*

// TODO(Ryan):
// #include "base-thread.h"
*/
