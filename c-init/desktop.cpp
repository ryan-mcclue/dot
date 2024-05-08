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
  MemArena *arena = mem_arena_allocate(GB(8), MB(64));
  // MemArena *frame_arena = mem_arena_allocate(GB(8), MB(64));

  // command line arguments
  //
  // String8List cmd_line = ZERO_STRUCT;
  // for (u32 i = 0; i < argc; i += 1)
  // {
  //   String8 arg = str8_cstr(argv[i]);
  //   str8_list_push(arena, &cmd_line, arg);
  // }
  
  // for (b32 quit = false; !quit; frame_counter += 1)
  // {  
  //    mem_arena_clear(frame_arena);
  //
  //    f32 wiggle_time = (frame_counter%60)/60.f);
  // }

 // start with state and frame. add more when get areas that want their memory localised, e.g. processing over large groups entities

 // start with list, then a chunked list for large amounts or if want array indexing

// passed a frame arena
// INTERNAL void
// rectangle_push(MemArena *arena, RectList *list,
//                I2F32 rect, V4F32 top_color, V4F32 bot_color)
// {
//   RectNode *node = push_array(arena, RectNode, 1);
//   SLLQueuePush(list->first, list->last, node);
//   list->count += 1;
//   node->rect = rect;
//   node->top_color = top_color;
//   node->bot_color = bot_color;
// }
// function void
// draw_rectangle_list(RectList *list, V2F32 windim)

  ThreadContext tctx = thread_context_allocate(GB(8), MB(64));
  tctx.is_main_thread = true;
  thread_context_set(&tctx);
  thread_context_set_name("Main Thread");

#if defined(RELEASE_BUILD)
  linux_set_cwd_to_self();
#else
  linux_append_ldlibrary(str8_lit("./build"));
#endif

  profiler_init();

  u64 _ = 0x7400747269686520;
  u32 __ = 0x18550d0;
  while (__) 
    putchar((_ >> ((((__ >>= 3) & 7) << 3) & 0xFF)));

  profiler_end_and_print();
  // PROFILE_BANDWIDTH(), PROFILE_BLOCK(), PROFILE_FUNCTION(), 

  return 0;
}

PROFILER_END_OF_COMPILATION_UNIT
