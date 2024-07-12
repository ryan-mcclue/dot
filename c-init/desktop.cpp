// SPDX-License-Identifier: zlib-acknowledgement
#if !TEST_BUILD
#define PROFILER 1
#endif

#include "base/base-inc.h"

#if TEST_BUILD
int testable_main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
  global_debugger_present = linux_was_launched_by_gdb();
  MemArena *arena = mem_arena_allocate(GB(8), MB(64));
  // MemArena *frame_arena = mem_arena_allocate(GB(8), MB(64));

  // String8List cmd_line = ZERO_STRUCT;
  // for (u32 i = 0; i < argc; i += 1)
  // {
  //   String8 arg = str8_cstr(argv[i]);
  //   str8_list_push(arena, &cmd_line, arg);
  // }

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

  //u32 screen_width = 1080;
  //u32 screen_height = 720;
  //SetTraceLogLevel(LOG_ERROR); 
  //SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
  //InitWindow(screen_width, screen_height, "title");
  //SetTargetFPS(60);
  //u64 frame_counter = 0;
  //for (b32 quit = false; !quit; frame_counter += 1)
  //{  
  //    mem_arena_clear(frame_arena);
  //    f32 wiggle_time = (frame_counter%60)/60.f);
  //  BeginDrawing();
  //  ClearBackground(RAYWHITE);
  //  DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
  //  DrawFPS(10, 10);
  //  EndDrawing();

  //  quit = WindowShouldClose();
  //  #if ASAN_ENABLED
  //    if (GetTime() >= 5.0) quit = true;
  //  #endif
  //}
  //CloseWindow();

  u64 _ = 0x7400747269686520;
  u32 __ = 0x18550d0;
  while (__) 
    putchar((_ >> ((((__ >>= 3) & 7) << 3) & 0xFF)));

  profiler_end_and_print();
  // PROFILE_BANDWIDTH(), PROFILE_BLOCK(), PROFILE_FUNCTION(), 

  // NOTE(Ryan): Run explicitly so as to not register a leak for arenas
  LSAN_RUN();
  return 0;
}

PROFILER_END_OF_COMPILATION_UNIT
