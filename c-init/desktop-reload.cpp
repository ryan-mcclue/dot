// SPDX-License-Identifier: zlib-acknowledgement
#include "desktop.h"

#include "base/base-inc.h"
#include <raylib.h>

GLOBAL State *g_state = NULL;

EXPORT void 
code_preload(State *state)
{}

EXPORT void 
code_postload(State *state)
{}

EXPORT void 
code_update(State *state)
{ 
  BeginDrawing();
  ClearBackground(RAYWHITE);

  f32 dt = GetFrameTime();
  u32 rw = GetRenderWidth();
  u32 rh = GetRenderHeight();

  if (IsKeyPressed(KEY_F)) 
  {
    if (IsWindowMaximized()) RestoreWindow();
    else MaximizeWindow();
  }

  DrawFPS(10, 10);

  EndDrawing();
}
