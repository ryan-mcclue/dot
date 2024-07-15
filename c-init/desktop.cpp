// SPDX-License-Identifier: zlib-acknowledgement
#if !TEST_BUILD
#define PROFILER 1
#endif

#include "base/base-inc.h"
#include <raylib.h>

typedef struct State State;
struct State {
  MemArena *arena;
  MemArena *frame_arena;
};

#if TEST_BUILD
int testable_main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
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

  profiler_init();

  State *state = MEM_ARENA_PUSH_STRUCT_ZERO(arena, State);
  state->arena = arena;
  state->frame_arena = mem_arena_allocate(GB(1), MB(64));

  u32 screen_width = 1080;
  u32 screen_height = 720;
  SetTraceLogLevel(LOG_ERROR); 
  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
  InitWindow(screen_width, screen_height, "title");
  SetTargetFPS(60);
  u64 frame_counter = 0;
  for (b32 quit = false; !quit; frame_counter += 1)
  {  
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawFPS(10, 10);
    EndDrawing();

    quit = WindowShouldClose();
    #if ASAN_ENABLED
      if (GetTime() >= 5.0) quit = true;
    #endif
    mem_arena_clear(state->frame_arena);
  }
  CloseWindow();

  profiler_end_and_print();
  // PROFILE_BANDWIDTH(), PROFILE_BLOCK(), PROFILE_FUNCTION(), 

  // NOTE(Ryan): Run explicitly so as to not register a leak for arenas
  LSAN_RUN();
  return 0;
}

PROFILER_END_OF_COMPILATION_UNIT
