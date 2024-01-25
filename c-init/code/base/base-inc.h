// SPDX-License-Identifier: zlib-acknowledgement
#pragma once

#include "base-context.h"

#include "base-types.h"

// IMPORTANT: Uses stdlib math
#include "base-math.h"

// IMPORTANT: Uses stdlib malloc
#include "base-memory.h"

#include "base-string.h"
#include "base-map.h"

// NOTE(Ryan):
//  - Desktop:
//    * cycle counter, breakpoints, stacktrace, randomness, file info, command execution
//  - Embedded:
//    * cycle counter, breakpoints,  
#if defined(PLATFORM_CORTEXM)
  // IMPORTANT: Uses CMSIS 
  #include "base-dev-cortexm4.h"
  // gcc -dM -E - > file.txt
#else
  #include "base-dev-linux.h"
  #include "base-file.h"
  #include "base-repetition.h"
  #include "base-profiler.h"
#endif

// TODO(Ryan):
// #include "base-thread.h"
