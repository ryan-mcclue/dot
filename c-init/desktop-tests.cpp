// SPDX-License-Identifier: zlib-acknowledgement
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <setjmp.h>
#include <limits.h>

#include "desktop.cpp"

EXPORT_BEGIN
#include <cmocka.h>
EXPORT_END

#define TILE_HEIGHT 128
#define FULL_TILE_HEIGHT 512
#define TILE_WIDTH 256
INTERNAL Vector2
tile_to_world(Vector2 tile)
{
  Vector2 res = ZERO_STRUCT;
  Vector2 origin = {0, 0};

  res.x = (origin.x * TILE_WIDTH * 1.0f) + (tile.x - tile.y) * (TILE_WIDTH * 0.5f);
  res.y = (origin.y * TILE_HEIGHT * 1.0f) + (tile.x + tile.y) * (TILE_HEIGHT * 0.5f);
  res.y -= (FULL_TILE_HEIGHT - TILE_HEIGHT);

  return res;
}

INTERNAL void
tile_to_world_repeat(RepetitionTester *tester, Vector2 tile)
{
  while (update_tester(tester))
  {
    TIME_TEST(tester)
    {
      Vector2 res = tile_to_world(tile); 

      if (!f32_eq(res.x, f32_inf()))
      {
        tester_count_bytes(tester, sizeof(tile));
      }
      else
      {
        tester_set_error(tester, "tile_to_world failed");
      }
    }
  }
}

INTERNAL void
repetition_test(void)
{
  // NOTE(Ryan): Unique for each function to repeat on
  RepetitionTester tester = ZERO_STRUCT;

  printf("\n--- tile_to_world ---\n");
  tester_init_new_wave(&tester, sizeof(Vector2), linux_estimate_cpu_timer_freq());
  // this should run for 10 seconds
  tile_to_world_repeat(&tester, {5, 5});
}

void
test_example(void **state)
{

}

int 
main(void)
{
  global_debugger_present = linux_was_launched_by_gdb();
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
