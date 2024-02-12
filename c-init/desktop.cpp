// SPDX-License-Identifier: zlib-acknowledgement
#if !defined(TEST_BUILD)
#define PROFILER
#endif

#include "base/base-inc.h"

#if defined(TEST_BUILD)
int testable_main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
  global_debugger_present = linux_was_launched_by_gdb();
  MemArena *perm_arena = mem_arena_allocate(GB(8), MB(64));

  ThreadContext tctx = thread_context_allocate(GB(8), MB(64))
  tctx.is_main_thread = true;
  thread_context_set(&tctx);
  thread_context_set_name("Main Thread");

#if defined(RELEASE_BUILD)
  linux_set_cwd_to_self();
#else
  linux_append_ldlibrary(str8_lit("./build"));
#endif

  profiler_init();


  profiler_end_and_print();

  return 0;
}

PROFILER_END_OF_COMPILATION_UNIT
