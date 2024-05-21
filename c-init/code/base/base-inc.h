// SPDX-License-Identifier: zlib-acknowledgement
#if !defined(BASE_INC_H)
#define BASE_INC_H

#include "base/base-context.h"
#include "base/base-types.h"
// IMPORTANT: Uses stdlib math
#include "base/base-math.h"
// IMPORTANT: Uses stdlib malloc
#include "base/base-memory.h"
#include "base/base-string.h"

// NOTE(Ryan):
//  - Desktop:
//    * cycle counter, breakpoints, stacktrace, randomness, file info, command execution
//  - Embedded:
//    * cycle counter, breakpoints,  
#if PLATFORM_CORTEXM4
  // IMPORTANT: Uses CMSIS 
  #include "base/base-dev-cortexm4.h"
  // gcc -dM -E - > file.txt
#elif PLATFORM_LINUX 
  #include "base/base-dev-linux.h"
#endif

#if PLATFORM_LINUX || PLATFORM_MAC || PLATFORM_WINDOWS
  #include "base/base-file.h"
  #include "base/base-repetition.h"
  #include "base/base-profiler.h"
#endif


// TODO(Ryan):
// #include "base-thread.h"

#endif
