// SPDX-License-Identifier: zlib-acknowledgement
#include "base/base-inc.h"
#include <raylib.h>

#include "desktop.h"

GLOBAL State *g_state = NULL;

#include "desktop-assets.cpp"

EXPORT void 
code_preload(State *state)
{
  assets_preload();
}

EXPORT void 
code_postload(State *state)
{}

EXPORT void 
code_update(State *state)
{ 
  if (!state->is_initialised)
  {
    state->is_initialised = true;
    g_state = state;
  }

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

  DrawText();

  Rectangle src = {icon_index * 225.f, icon_index * 225.f, 225.f, 225.f};
  Rectangle dst = {
    region.x + region.width/2.0f - (icon_size/2.0f)*btn_scale,
    region.y + region.height/2.0f - (icon_size/2.0f)*btn_scale,
    icon_size*btn_scale,
    icon_size*btn_scale
  };
  Vector2 origin = {0.f, 0.f};
  DrawTexturePro(assets_get_texture(str8_lit("assets/something.png")),
                 src, dst, origin, 0.f, ColorBrightness(WHITE, -0.1f)); 
  //draw_rect();

  DrawFPS(10, 10);

  EndDrawing();
}
