// SPDX-License-Identifier: zlib-acknowledgement
#if !defined(BASE_CONTEXT_H)
#define BASE_CONTEXT_H

// NOTE(Ryan): Compiler/platform/architecture
#if defined(_MSC_VER)
  #define COMPILER_MSVC 1
  #if defined(_WIN32)
    #define PLATFORM_WINDOWS 1
  #else
    #warn Unknown msvc platform
  #endif

  #if defined(_M_AMD64)
   #define ARCH_X64 1
  #elif defined(_M_IX86)
   #define ARCH_X86 1
  #elif defined(_M_ARM64)
   #define ARCH_ARM64 1
  #elif defined(_M_ARM)
   #define ARCH_ARM32 1
  #else
    #warn Unknown msvc architecture
  #endif
#elif defined(__clang__)
  #define COMPILER_CLANG 1
  
  #if defined(__APPLE__) && defined(__MACH__)
    #define PLATFORM_MAC 1
  #elif defined(__gnu_linux__)
    #define PLATFORM_LINUX 1
  #else
    #warn Unknown clang platform
  #endif
  
  #if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
    #define ARCH_X64 1
  #elif defined(i386) || defined(__i386) || defined(__i386__)
    #define ARCH_X86 1
  #elif defined(__aarch64__)
    #define ARCH_ARM64 1
  #elif defined(__arm__)
    #define ARCH_ARM32 1
  #else
    #warn Unknown clang architecture
  #endif
#elif defined(__GNUC__) || defined(__GNUG__)
  #if __GNUC__ < 10
    #warning GCC 10+ required for builtin static analysis
  #endif

  #define COMPILER_GCC 1
  #if defined(__gnu_linux__)
    #define PLATFORM_LINUX 1
  #elif defined(__ARM_ARCH_PROFILE) && __ARM_ARCH_PROFILE == 'M'
    #define PLATFORM_CORTEXM4 1
  #elif defined(ESP_PLATFORM) && ESP_PLATFORM == 1
    #define PLATFORM_ESP32
  #else
    #warn Unknown gcc platform
  #endif

  #if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
    #define ARCH_X64 1
  #elif defined(i386) || defined(__i386) || defined(__i386__)
    #define ARCH_X86 1
  #elif defined(__aarch64__)
    #define ARCH_ARM64 1
  #elif defined(__arm__)
    #define ARCH_ARM32 1
  #elif defined(__XTENSA__)
    #define ARCH_XTENSA 1
  #else
    #warn Unknown gcc architecture
  #endif
#else
  #warn Compiler is not supported
#endif

// NOTE(Ryan): Zero compiler/platform/architecture
#if !defined(COMPILER_MSVC)
  #define COMPILER_MSVC 0
#endif
#if !defined(COMPILER_GCC)
  #define COMPILER_GCC 0
#endif
#if !defined(COMPILER_CLANG)
  #define COMPILER_CLANG 0
#endif

#if !defined(PLATFORM_WINDOWS)
  #define PLATFORM_WINDOWS 0
#endif
#if !defined(PLATFORM_LINUX)
  #define PLATFORM_LINUX 0
#endif
#if !defined(PLATFORM_MAC)
  #define PLATFORM_MAC 0
#endif
#if !defined(PLATFORM_CORTEXM4)
  #define PLATFORM_CORTEXM4 0
#endif
#if !defined(PLATFORM_ESP32)
  #define PLATFORM_ESP32 0
#endif

#if !defined(ARCH_X86)
  #define ARCH_X86 0
#endif
#if !defined(ARCH_X64)
  #define ARCH_X64 0
#endif
#if !defined(ARCH_ARM64)
  #define ARCH_ARM64 0
#endif
#if !defined(ARCH_ARM32)
  #define ARCH_ARM32 0
#endif
#if !defined(ARCH_XTENSA)
  #define ARCH_XTENSA 0
#endif

// NOTE(Ryan): Language
#if defined(__cplusplus)
  #define LANG_CPP 1
  #define LANG_C 0
  #define EXPORT_BEGIN extern "C" {
  #define EXPORT_END }
  #define EXPORT extern "C"
  #define ZERO_STRUCT {}
  #define RESTRICT __restrict__
  #define CCOMPOUND(type) type
#else
  #define LANG_C 1
  #define LANG_CPP 0
  #define EXPORT_BEGIN
  #define EXPORT_END
  #define EXPORT
  #define ZERO_STRUCT {0}
  #define RESTRICT restrict
  #define CCOMPOUND(type) (type)
#endif

// NOTE(Ryan): Thread local
#if COMPILER_MSVC
  #define THREAD_LOCAL __declspec(thread)
#elif PLATFORM_LINUX || PLATFORM_MAC
  #define THREAD_LOCAL __thread
#else
  #define THREAD_LOCAL
#endif

// NOTE(Ryan): Sanitisers
#if defined(__SANITIZE_ADDRESS__)
  #define ASAN_ENABLED 1
#else
  #define ASAN_ENABLED 0
#endif
#if ASAN_ENABLED && COMPILER_MSVC
  #define NO_ASAN __declspec(no_sanitize_address)
  #define LSAN_RUN()
#elif ASAN_ENABLED && (PLATFORM_LINUX || PLATFORM_MAC)
  #include <sanitizer/lsan_interface.h>
  #define NO_ASAN __attribute__((no_sanitize("address")))
  #define LSAN_RUN() __lsan_do_leak_check(); __lsan_disable()
#else
  #define NO_ASAN
  #define LSAN_RUN()
#endif

// NOTE(Ryan): Extensions
#if COMPILER_GCC || COMPILER_CLANG
  #define LIKELY(x)   __builtin_expect(!!(x), 1) 
  #define UNLIKELY(x) __builtin_expect(!!(x), 0)
  #define ISO_EXTENSION __extension__
  // NOTE(Ryan): Used to ensure .text/program-memory aligned on ARM
  #define PROGMEM const __attribute__((aligned(4)))
  #define SECTION(x) __attribute__((section(x)))
  // NOTE(Ryan): ISO C++ doesn't like anonymous structs
  #define IGNORE_WARNING_PEDANTIC() \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Wpedantic\"")
  #define IGNORE_WARNING_POP() \
    _Pragma("GCC diagnostic pop")
#elif COMPILER_MSVC
  #define LIKELY(x)
  #define UNLIKELY(x)
  #define ISO_EXTENSION
  #define PROGMEM
  #define SECTION(x) __declspec(allocate(x))
  #define IGNORE_WARNING_PEDANTIC() \
    __pragma(warning(push)) \
    __pragma(warning(disable:4668))
  #define IGNORE_WARNING_POP() \
    __pragma(warning(pop))
#else
  #define LIKELY(x)
  #define UNLIKELY(x)
  #define ISO_EXTENSION
  #define PROGMEM
  #define SECTION(x)
  #define IGNORE_WARNING_PEDANTIC()
  #define IGNORE_WARNING_POP()
#endif

#endif
