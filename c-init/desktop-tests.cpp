// SPDX-License-Identifier: zlib-acknowledgement
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <setjmp.h>
#include <limits.h>

#include "desktop-reload.cpp"

EXPORT_BEGIN
#include <cmocka.h>
EXPORT_END

EXPORT void mov_all_bytes_asm(u8 *data, u32 count);
INTERNAL void
mov_all_bytes_asm_repeat(RepetitionTester *tester, u32 count)
{
  while (update_tester(tester))
  {
    TIME_TEST(tester)
    {
      u8 *m = (u8 *)malloc(count);
      if (m != NULL)
      {
        mov_all_bytes_asm(m, count);
        tester_count_bytes(tester, count);
        free(m);
      } 
      else
      {
        tester_set_error(tester, "Malloc failed");
      }
    }
  }

}

INTERNAL void
repetition_test(void)
{
  // NOTE(Ryan): Unique for each function to repeat on
  RepetitionTester tester = ZERO_STRUCT;

  u32 count = 1000;
  printf("\n--- Repetition Test (mov_all_bytes_asm) ---\n");
  tester_init_new_wave(&tester, count, linux_estimate_cpu_timer_freq());
  // this should run for 10 seconds
  mov_all_bytes_asm_repeat(&tester, count);
}

void
test_example(void **state)
{

}

int 
main(void)
{
  global_debugger_present = linux_was_launched_by_gdb();

  MemArena *arena = mem_arena_allocate(GB(8), MB(64));
  ThreadContext tctx = thread_context_allocate(GB(8), MB(64));
  tctx.is_main_thread = true;
  thread_context_set(&tctx);
  thread_context_set_name("Main Thread");

#if RELEASE_BUILD
  linux_set_cwd_to_self();
#else
  linux_append_ldlibrary(str8_lit("./build"));
#endif

  State *state = MEM_ARENA_PUSH_STRUCT_ZERO(arena, State);
  g_state = state;
  state->arena = arena;
  state->frame_arena = mem_arena_allocate(GB(1), MB(64));
  state->assets.arena = mem_arena_allocate(GB(1), MB(64));

  #define REPETITION 1
  #if REPETITION
    repetition_test(); 
    return 0;
  #else
	const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_example),
  };

  int cmocka_res = cmocka_run_group_tests(tests, NULL, NULL);

  return cmocka_res;
  #endif
}
