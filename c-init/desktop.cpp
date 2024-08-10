// SPDX-License-Identifier: zlib-acknowledgement
#if !TEST_BUILD
#define PROFILER 1
#endif

#include "desktop.h"

#include <dlfcn.h>

GLOBAL void *g_code_reload_handle = NULL;
GLOBAL char g_nil_update_err_msg[128];
INTERNAL void
code_nil_update(State *state)
{
  BeginDrawing();
  ClearBackground(BLACK);

  char *err = dlerror();
  if (err != NULL)
  {
    strncpy(g_nil_update_err_msg, err, sizeof(g_nil_update_err_msg));
  }

  f32 font_size = 64.0f;
  Font f = GetFontDefault();
  Vector2 size = MeasureTextEx(f, g_nil_update_err_msg, font_size, 0);
  Vector2 pos = {
    GetRenderWidth()/2.0f - size.x/2.0f,
    GetRenderHeight()/2.0f - size.y/2.0f,
  };
  DrawTextEx(f, g_nil_update_err_msg, pos, font_size, 0, RED);

  EndDrawing();
}
INTERNAL void code_nil(State *s) {}
GLOBAL ReloadCode g_nil_code = {
  .preload = code_nil,
  .update = code_nil_update,
  .postload = code_nil,
  .profiler_end_and_print = code_nil
};

INTERNAL ReloadCode 
code_reload(void)
{
  if (g_code_reload_handle != NULL) dlclose(g_code_reload_handle);

  g_code_reload_handle = dlopen("build/" BINARY_RELOAD_NAME, RTLD_NOW);
  if (g_code_reload_handle == NULL) return g_nil_code;

  ReloadCode result = ZERO_STRUCT;
  void *name = dlsym(g_code_reload_handle, "code_preload");
  if (name == NULL) return g_nil_code;
  result.preload = (code_preload_t)name;

  name = dlsym(g_code_reload_handle, "code_update");
  if (name == NULL) return g_nil_code;
  result.update = (code_update_t)name;

  name = dlsym(g_code_reload_handle, "code_postload");
  if (name == NULL) return g_nil_code;
  result.postload = (code_postload_t)name;

  name = dlsym(g_code_reload_handle, "code_profiler_end_and_print");
  if (name == NULL) return g_nil_code;
  result.profiler_end_and_print = (code_profiler_end_and_print_t)name;

  return result;
}


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

  //profiler_init();

  State *state = MEM_ARENA_PUSH_STRUCT_ZERO(arena, State);
  state->arena = arena;
  state->frame_arena = mem_arena_allocate(GB(1), MB(64));

  state->assets.arena = mem_arena_allocate(GB(1), MB(64));

  u32 screen_width = 1920;
  u32 screen_height = 1080;
  SetTraceLogLevel(LOG_WARNING); 
  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
  InitWindow(screen_width, screen_height, "Game");
  SetTargetFPS(60);

  ReloadCode code = code_reload();
  code.preload(state);
  u64 prev_code_reload_time = GetFileModTime("build/" BINARY_RELOAD_NAME);
  f32 reload_time = 0.25f;
  f32 reload_timer = reload_time;
  for (b32 quit = false; !quit; state->frame_counter += 1)
  {  
      u64 code_modify_time = GetFileModTime("build/" BINARY_RELOAD_NAME);
      if (code_modify_time > prev_code_reload_time)
      {
        // NOTE(Ryan): Prevent loading before object has been fully written
        reload_timer += GetFrameTime();
        if (reload_timer >= reload_time)
        {
          prev_code_reload_time = code_modify_time;
          code.preload(state);
          code = code_reload();
          code.postload(state);
          reload_timer = 0.f;
        }
      }

    code.update(state);

    quit = WindowShouldClose();
    #if ASAN_ENABLED
      if (GetTime() >= 5.0) quit = true;
    #endif
    mem_arena_clear(state->frame_arena);
  }
  CloseWindow();

  code.profiler_end_and_print(state);

  //profiler_end_and_print();
  // PROFILE_BANDWIDTH(), PROFILE_BLOCK(), PROFILE_FUNCTION(), 

  // NOTE(Ryan): Run explicitly so as to not register a leak for arenas
  LSAN_RUN();
  return 0;
}

