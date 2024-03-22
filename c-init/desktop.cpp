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
  // MemArena *frame_arena = mem_arena_allocate(GB(8), MB(64));

  // command line arguments
  // for (int i = 0; i < argc; i += 1){
  //   String8 arg = str8_cstring((U8*)argv[i]);
  //   str8_list_push(w32_perm_arena, &w32_cmd_line, arg);
  // }
  
  // U32 speaker_count = (args.node_count - 1)/2;
  // Speaker *speakers = push_array_zero(arena, Speaker, speaker_count);
  // String8Node *first_arg = args.first;
   
  // for(B32 quit = 0; quit == 0; frame_idx += 1)
  // for (true)
  // {  arena_pop_to(frame_arena, 0);
  //    frame_counter += 1;
  //    F32 wiggle_time = ((params->frame_counter%60)/60.f);
  //
  // }
  //

  // start with state and frame. add more when get areas that want there memory localised, e.g. processing over large groups
 Arena *state_arena = ArenaAlloc(Gigabytes(2));
 State *state = PushArray(state_arena, State, 1);
 state->arena = state_arena;
 state->name_chunk_arena = ArenaAlloc(Gigabytes(64));
 state->entities_arena = ArenaAlloc(Gigabytes(64));
 state->entities_base = PushArray(state->entities_arena, Entity, 0);
 UI_State *ui = UI_StateAlloc();


  // passed a frame arena
function void
rectangle_push(M_Arena *arena, RectList *list,
               I2F32 rect, V4F32 top_color, V4F32 bot_color){
  RectNode *node = push_array(arena, RectNode, 1);
  SLLQueuePush(list->first, list->last, node);
  list->count += 1;
  node->rect = rect;
  node->top_color = top_color;
  node->bot_color = bot_color;
}
function void
draw_rectangle_list(RectList *list, V2F32 windim)



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

  return 0;
}

PROFILER_END_OF_COMPILATION_UNIT
