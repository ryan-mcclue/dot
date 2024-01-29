// SPDX-License-Identifier: zlib-acknowledgement
#pragma once

#if defined(__GNUC__)
  #define COMPILER_GCC
  #define COMPILER_GCC_STR "gcc (" #__GNUC__ "." #__GNUC_MINOR__ "." #__GNUC_PATCHLEVEL__ ") "

  #if __GNUC__ < 10
    #warning GCC 10+ required for builtin static analysis
  #endif

  // NOTE(Ryan): Architecture
  #if defined(__x86_64__)
    #define ARCH_X86_64
    #define ARCH_X86_64_STR "x86_64 "
  #else
    #define ARCH_X86_64_STR
  #endif

  #if defined(__arm__) && defined(__aarch64__)
    #define ARCH_ARM64
    #define ARCH_ARM64_STR "arm64 "
  #else
    #define ARCH_ARM64_STR
  #endif

  #if defined(__arm__) && !defined(__aarch64__)
    #define ARCH_ARM32
    #define ARCH_ARM32_STR "arm32 "
  #else
    #define ARCH_ARM32_STR
  #endif


  #if defined(__XTENSA__)
    #define ARCH_XTENSA
    #define ARCH_XTENSA_STR "xtensa "
  #else
    #define ARCH_XTENSA_STR
  #endif

  #if defined(IDF_VER)
    #define IDF_FRAMEWORK
    #define IDF_VERSION_STR "idf (" #IDF_VER_MAJOR "." #IDF_VER_MINOR "." #IDF_VER_PATCH ") "
  #else
    #define IDF_VERSION_STR
  #endif

  #if defined(__gnu_linux__)
    #define PLATFORM_LINUX
    #define PLATFORM_LINUX_STR "linux "
  #else
    #define PLATFORM_LINUX_STR
  #endif
  
  #if defined(__ARM_ARCH_PROFILE) && __ARM_ARCH_PROFILE == 'M'
    #define PLATFORM_CORTEXM
    #define PLATFORM_CORTEXM_STR "cortex-m4 "
  #else
    #define PLATFORM_CORTEXM_STR
  #endif

  #if defined(ESP_PLATFORM) && ESP_PLATFORM == 1
    #define PLATFORM_ESP32
    #define PLATFORM_ESP32_STR "esp32 "
  #else
    #define PLATFORM_ESP32_STR
  #endif

  #if defined(__cplusplus)
    #define LANG_CPP
    #if __cplusplus <= 199711L
      #define LANG_CPP_STR "c++-98 "
    #elif __cplusplus <= 201103L
      #define LANG_CPP_STR "c++-11 "
    #else
      #define LANG_CPP_STR "c++ "
    #endif
  #else
    #define LANG_C
    #if __STDC_VERSION__ <= 199901L
      #define LANG_C_STR "c99 "
    #elif __STDC_VERSION__ <= 201112L
      #define LANG_C_STR "c11 "
    #else
      #define LANG_C_STR "c "
    #endif
  #endif
  
  #if defined(LANG_CPP)
    // NOTE(Ryan): Avoid confusing auto-indenter
    #define EXPORT_BEGIN extern "C" {
    #define EXPORT_END }
    #define EXPORT extern "C"
    #define ZERO_STRUCT {}
    #define RESTRICT __restrict__
  #else
    #define EXPORT_BEGIN
    #define EXPORT_END
    #define EXPORT
    #define RESTRICT restrict
    #define ZERO_STRUCT {0}
  #endif

  // NOTE(Ryan): Sanitiser
  #if defined(__SANITIZE_ADDRESS__)
    #define NO_ASAN __attribute__((__no_sanitize_address__))
    #include <sanitizer/lsan_interface.h>
    // NOTE(Ryan): Prevent unreachables resulting from sanitiser being run after main
    #define LSAN_RUN() __lsan_do_leak_check(); __lsan_disable()
  #else
    #define NO_ASAN
    #define LSAN_RUN()
  #endif

  // NOTE(Ryan): Extensions
  #define CASE_FALLTHROUGH __attribute__((fallthrough))
  #define NEVER_INLINE   __attribute__((noinline))
  #define USED __attribute__((used,noinline))
  #define ALWAYS_INLINE __attribute__((optimize("inline-functions"),always_inline))
  #define PACKED __attribute__((packed))
  #define NORETURN __attribute__((noreturn))
  #define UNUSED __attribute__((unused))
  #define UNREACHABLE() __builtin_unreachable()
  #define WEAK __attribute__((weak))
  #define LIKELY(x)   __builtin_expect(!!(x), 1) 
  #define UNLIKELY(x) __builtin_expect(!!(x), 0)
  #define ISO_EXTENSION __extension__
  // NOTE(Ryan): Used to ensure .text/program-memory aligned on ARM
  #define PROGMEM const __attribute__ ((aligned(4)))
  // TODO(Ryan): Could place in specific section and watch for it in gdb?
  // #define READ_ONLY __attribute__((section(".roglob"))) 
  // watch *(int *)0xaddress_of_variable_in_roglob_section

  // TODO(Ryan): tail cail compiler macro?
  // https://blog.reverberate.org/2021/04/21/musttail-efficient-interpreters.html
  
  // TODO(Ryan): Synchronisation atomics, e.g:
  // #define AtomicAdd64(ptr, v) _InterlockedExchangeAdd64((ptr), (v))
  // #define MEMORY_BARRIER()
  
  #if defined(PLATFORM_LINUX)
    #define THREAD_LOCAL __thread
  #else
    #define THREAD_LOCAL 
  #endif

  // NOTE(Ryan): 
  #define PUSH_OPTIMISATION_MODE() \
    _Pragma("GCC push_options") \
    _Pragma("GCC optimize (\"O3\")")

  #define POP_OPTIMISATION_MODE() \
    _Pragma("GCC pop_options")

  #define IGNORE_WARNING_PADDED() \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Wpadded\"")

  #define IGNORE_WARNING_PEDANTIC() \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Wpedantic\"")

  #define IGNORE_WARNING_POP() \
    _Pragma("GCC diagnostic pop")

  #define CONTEXT_STR \
    ARCH_X86_64_STR ARCH_ARM64_STR ARCH_ARM32_STR ARCH_XTENSA_STR \
    PLATFORM_LINUX_STR PLATFORM_CORTEXM_STR PLATFORM_ESP32_STR \
    COMPILER_GCC_STR \
    IDF_VERSION_STR \
    LANG_CPP_STR LANG_C_STR

    typedef struct ElfNoteSection ElfNoteSection;
    struct ElfNoteSection
    {
      char namesz[4]; // sizeof("GNU")
      char descsz[4]; // sizeof(20-char-sha1)
      char type[4];
      char *data;
    };
    
    extern const ElfNoteSection g_note_build_id;
    #define GET_BUILD_ID_PTR() \
       (u8 *)&g_note_build_id.data[g_note_build_id.namesz]
    // g_note_build_id.decsz number of bytes
    
    // const uint8_t gcau8DeviceName[12] __attribute__ ((section (“.image_header_body”))) =
    // {‘N’, ‘u’, ‘c’, ‘l’, ‘e’, ‘o’, ‘L’, ‘4’, ‘3’, ‘2’, ‘K’, ‘C’ };

#else
  #error Compiler not supported
#endif

