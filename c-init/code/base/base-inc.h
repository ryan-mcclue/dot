// SPDX-License-Identifier: zlib-acknowledgement
#if !defined(BASE_INC_H)
#define BASE_INC_H

#include "base/base-context.h"
#include "base/base-types.h"
#if PLATFORM_LINUX
  #if !TEST_BUILD
    #define PROFILER 1
  #endif
  #include "base/base-profiler.h"
#endif
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
  #include "base-thread.h"
#endif

#if PLATFORM_LINUX || PLATFORM_MAC || PLATFORM_WINDOWS
  #include "base/base-file.h"
  #include "base/base-repetition.h"
#endif



#endif
