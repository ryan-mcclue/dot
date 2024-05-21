// SPDX-License-Identifier: zlib-acknowledgement
#if !defined(BASE_MEMORY_H)
#define BASE_MEMORY_H

// IMPORTANT(Ryan): Although using memory arenas restricts arbitrary lifetimes, this provides more benefits than negatives.
// In majority of cases, a large number of allocations can be bucketed into same arena

// IMPORTANT(Ryan): In essence, OS is ultimate garbage collector as it releases page table for us.
// So, we generally shouldn't have to perform manual garbage collection.
// That is, no periodic code interuptions to determine lifetimes and possibly free

#include <string.h>
#include <stdlib.h>

#define MEMORY_ZERO(p, n) memset((p), 0, (n))
#define MEMORY_ZERO_STRUCT(p) MEMORY_ZERO((p), sizeof(*(p)))
#define MEMORY_ZERO_ARRAY(a) MEMORY_ZERO((a), sizeof(a[0]))

#define MEMORY_COPY(d, s, n) memmove((d), (s), (n))
#define MEMORY_COPY_STRUCT(dst, src) do { \
  ASSERT(sizeof(*(dst)) == sizeof(*(src))); MEMORY_COPY((dst), (src), sizeof(*(dst))); } while(0)
#define MEMORY_COPY_ARRAY(dst, src) do { \
  ASSERT(sizeof(dst) == sizeof(src)); MEMORY_COPY((dst), (src), sizeof(src)); } while(0)

#define MEMORY_MATCH(a, b, n) (memcmp((a), (b), (n)) == 0)

#define KB(x) ((x) << 10)
#define MB(x) ((x) << 20)
#define GB(x) (((u64)x) << 30)
#define TB(x) (((u64)x) << 40)

typedef struct MemArena MemArena;
struct MemArena
{
  void *memory;
  memory_index commit_pos;
  memory_index max;
  memory_index pos;
  memory_index align;
};

INTERNAL MemArena *
mem_arena_allocate(memory_index cap, memory_index roundup_granularity)
{
  u64 rounded_size = memory_index_round_to_nearest(cap, roundup_granularity);
  MemArena *result = (MemArena *)malloc(rounded_size);
  if (result == NULL) return result;

  result->memory = result + sizeof(MemArena);
  result->max = cap;
  result->pos = sizeof(MemArena);
  result->align = sizeof(memory_index);

  return result;
}

INTERNAL void
mem_arena_deallocate(MemArena *arena)
{
  free(arena);
}
 
#define MEM_ARENA_PUSH_ARRAY(a,T,c) (T*)mem_arena_push((a), sizeof(T)*(c))
#define MEM_ARENA_PUSH_ARRAY_ZERO(a,T,c) (T*)mem_arena_push_zero((a), sizeof(T)*(c))
#define MEM_ARENA_POP_ARRAY(a,T,c) mem_arena_pop((a), sizeof(T)*(c))

#define MEM_ARENA_PUSH_STRUCT(a,T) (T*)mem_arena_push((a), sizeof(T))
#define MEM_ARENA_PUSH_STRUCT_ZERO(a,T) (T*)mem_arena_push_zero((a), sizeof(T))

INTERNAL void *
mem_arena_push_aligned(MemArena *arena, memory_index size, memory_index align)
{
  memory_index clamped_align = CLAMP_BOTTOM(align, arena->align);

  memory_index pos = arena->pos;

  memory_index pos_address = INT_FROM_PTR(arena) + pos;
  memory_index aligned_pos = ALIGN_POW2_UP(pos_address, clamped_align);
  memory_index alignment_size = aligned_pos - pos_address;

  if (pos + alignment_size + size <= arena->max)
  {
    memory_index new_pos = pos + alignment_size + size;
    arena->pos = new_pos;

    return (u8 *)arena + pos + alignment_size;
  }
  else
  {
    // TODO(Ryan): Handle out-of-memory (conceivable on say limited VPS, etc.)
    return (u8 *)arena + pos;
  }
}

INTERNAL void *
mem_arena_push(MemArena *arena, memory_index size)
{
  return mem_arena_push_aligned(arena, size, arena->align);
}

INTERNAL void *
mem_arena_push_zero(MemArena *arena, memory_index size)
{
  void *memory = mem_arena_push(arena, size);

  MEMORY_ZERO(memory, size);

  return memory;
}

INTERNAL void
mem_arena_set_pos_back(MemArena *arena, memory_index pos)
{
  memory_index clamped_pos = CLAMP_BOTTOM(sizeof(*arena), pos);

  if (arena->pos > clamped_pos)
  {
    arena->pos = clamped_pos;
  }
}

INTERNAL void
mem_arena_pop(MemArena *arena, memory_index size)
{
  mem_arena_set_pos_back(arena, arena->pos - size);
}

INTERNAL void
mem_arena_clear(MemArena *arena)
{
  mem_arena_pop(arena, arena->pos);
}

typedef struct ThreadContext ThreadContext;
struct ThreadContext
{
  MemArena *arenas[2];
  char *file_name;
  u64 line_number;
  char thread_name[32];
  u32 thread_name_size;
  b32 is_main_thread;
};

THREAD_LOCAL ThreadContext *tl_tctx = NULL;

INTERNAL ThreadContext
thread_context_allocate(memory_index cap, memory_index roundup_granularity)
{
  ThreadContext tctx = ZERO_STRUCT;
  for (u32 arena_idx = 0; arena_idx < ARRAY_COUNT(tctx.arenas); arena_idx += 1)
  {
    tctx.arenas[arena_idx] = mem_arena_allocate(cap, roundup_granularity);
  }
  return tctx;
}

INTERNAL void thread_context_set(ThreadContext *tctx) { tl_tctx = tctx; }
INTERNAL ThreadContext *thread_context_get(void) { return tl_tctx; }

INTERNAL void
thread_context_deallocate(ThreadContext *tctx)
{
  for (u32 arena_idx = 0; arena_idx < ARRAY_COUNT(tctx->arenas); arena_idx += 1)
  {
    mem_arena_deallocate(tctx->arenas[arena_idx]);
  }
}

INTERNAL void
thread_context_set_name(char *name)
{
  ThreadContext *tctx = thread_context_get();
  tctx->thread_name_size = MIN(strlen(name), sizeof(tctx->thread_name));
  MEMORY_COPY(tctx->thread_name, name, tctx->thread_name_size);
}

INTERNAL char *
thread_context_get_name(void)
{
  ThreadContext *tctx = thread_context_get();
  return tctx->thread_name;
}

INTERNAL b32
thread_context_is_main(void)
{
  ThreadContext *tctx = thread_context_get();
  return tctx->is_main_thread;
}

#define THREAD_CONTEXT_RECORD_LOCATION() thread_context_set_file_and_line(__FILE__, __LINE__)
INTERNAL void
thread_context_set_file_and_line(char *file, int line)
{
  ThreadContext *tctx = thread_context_get();
  tctx->file_name = file;
  tctx->line_number = line;
}

typedef struct MemArenaTemp MemArenaTemp;
struct MemArenaTemp
{
  MemArena *arena;
  memory_index pos;
};

INTERNAL MemArenaTemp
mem_arena_temp_begin(MemArena **conflicts, u32 conflict_count)
{
  MemArenaTemp temp = ZERO_STRUCT;
  ThreadContext *tctx = thread_context_get();
  for (u32 tctx_idx = 0; tctx_idx < ARRAY_COUNT(tctx->arenas); tctx_idx += 1)
  {
    b32 is_conflicting = 0;
    for (MemArena **conflict = conflicts; conflict < conflicts+conflict_count; conflict += 1)
    {
      if (*conflict == tctx->arenas[tctx_idx])
      {
        is_conflicting = 1;
        break;
      }
    }

    if (is_conflicting == 0)
    {
      temp.arena = tctx->arenas[tctx_idx];
      temp.pos = temp.arena->pos;
      break;
    }
  }

  return temp;
}

INTERNAL void
mem_arena_temp_end(MemArenaTemp temp)
{
  mem_arena_set_pos_back(temp.arena, temp.pos);
}

#define MEM_ARENA_TEMP_BLOCK(name, conflicts, conflict_count) \
  MemArenaTemp name = ZERO_STRUCT; \
  DEFER_LOOP(name = mem_arena_temp_begin(conflicts, conflict_count), mem_arena_temp_end(temp))

// gpt4 plus with web requests plugin
// python server on source folder
// ngrok http 8000
#endif
