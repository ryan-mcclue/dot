#if !defined(DESKTOP_H)
#define DESKTOP_H

#include "base/base-inc.h"
#include "desktop-assets.h"

typedef struct State State;
INTROSPECT() struct State
{
  b32 is_initialised;

  Assets assets;

  MemArena *arena;
  MemArena *frame_arena;
  u64 frame_counter;
};

typedef void (*code_preload_t)(State *s);
typedef void (*code_update_t)(State *s);
typedef void (*code_postload_t)(State *s);

typedef struct ReloadCode ReloadCode;
struct ReloadCode
{
  code_preload_t preload;
  code_update_t update;
  code_postload_t postload;
};

#endif
