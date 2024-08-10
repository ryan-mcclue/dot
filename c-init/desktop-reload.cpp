// SPDX-License-Identifier: zlib-acknowledgement
#if !TEST_BUILD
#define PROFILER 1
#endif

#include "desktop.h"

GLOBAL State *g_state = NULL;

#include "desktop-assets.cpp"

EXPORT void 
code_preload(State *state)
{
  profiler_init();
  
  assets_preload(state);
}

EXPORT void 
code_postload(State *state)
{}

EXPORT void
code_profiler_end_and_print(State *state)
{
  profiler_end_and_print();
}

EXPORT void 
code_update(State *state)
{ 
  PROFILE_FUNCTION() {
  g_state = state;

  if (!state->is_initialised)
  {
    state->is_initialised = true;
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

  // TODO: more performant to auto-generate enum into array, instead of hashmap lookup every frame

  //Camera2D cam = ZERO_STRUCT;
  //cam.zoom = 1.f;

  Vector2 pos = {50.f, 50.f};
  f32 font_size = 64.f;
  DrawTextEx(assets_get_font(str8_lit("assets/Alegreya-Regular.ttf")),
             "hi there", pos, font_size, 0.f, BLUE);
  //DrawTextEx(assets_get_font(str8_lit("assets/Alegreya-BoldItalic.ttf")),
  //           "hello there", pos, font_size, 0.f, ORANGE);

  //for (u32 i = 0; i < 1000; i += 1)
  //{
  //  u32 x = (i % 16), y = (i / 16);
  //  x *= tile_width;
  //  y *= tile_height;
  //  y -= (full_tile_height - tile_height);
  //  DrawTexture(assets_get_texture(str8_lit("assets/Male_3_Idle0.png")), 0, 0, WHITE); 
  //}
  
  DrawTexture(assets_get_texture(str8_lit("assets/Male_3_Idle0.png")), 0, 0, WHITE); 
  //DrawTexture(assets_get_texture(str8_lit("assets/Male_3_Run8.png")), 0, 0, WHITE); 

  EndDrawing();
  }
}

PROFILER_END_OF_COMPILATION_UNIT
