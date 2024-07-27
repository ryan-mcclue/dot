// SPDX-License-Identifier: zlib-acknowledgement

#define ASSETS_NUM_SLOTS 256

INTERNAL Font
assets_get_font(String8 key)
{
  u64 hash = str8_hash(key);
  u64 slot_i = hash % ASSETS_NUM_SLOTS;
  FontSlot *slot = g_state->assets.fonts.slots + slot_i;
  for (FontNode *n = slot->first; n != NULL; n = n->hash_chain_next)
  {
    if (str8_match(n->key, key, 0)) return n->value;
  }
  
  char cpath[256] = ZERO_STRUCT;
  str8_to_cstr(key, cpath, sizeof(cpath)); 

  // TODO(Ryan): Add parameters to asset keys
  // NOTE: will get default font if failed, so always valid
  Font v = LoadFontEx(cpath, 64, NULL, 0);

  FontNode *n = MEM_ARENA_PUSH_STRUCT(g_state->assets.arena, FontNode);
  n->key = key;
  n->value = v;

  __SLL_QUEUE_PUSH(slot->first, slot->last, n, hash_chain_next);
  __SLL_STACK_PUSH(g_state->assets.fonts.collection, n, hash_collection_next);

  return v;
}

INTERNAL Texture
assets_get_texture(String8 key)
{
  u64 hash = str8_hash(key);
  u64 slot_i = hash % ASSETS_NUM_SLOTS;
  TextureSlot *slot = g_state->assets.textures.slots + slot_i;
  for (TextureNode *n = slot->first; n != NULL; n = n->hash_chain_next)
  {
    if (str8_match(n->key, key, 0)) return n->value;
  }

  char cpath[256] = ZERO_STRUCT;
  str8_to_cstr(key, cpath, sizeof(cpath)); 

  Texture v = LoadTexture(cpath);

  SetTextureFilter(v, TEXTURE_FILTER_BILINEAR);

  TextureNode *n = MEM_ARENA_PUSH_STRUCT(g_state->assets.arena, TextureNode);
  n->key = key;
  n->value = v;

  __SLL_QUEUE_PUSH(slot->first, slot->last, n, hash_chain_next);
  __SLL_STACK_PUSH(g_state->assets.textures.collection, n, hash_collection_next);

  return v;
}

INTERNAL void
assets_preload(State *state)
{
  for (FontNode *n = state->assets.fonts.collection; n != NULL; n = n->hash_collection_next)
  {
    UnloadFont(n->value);
  }
  for (TextureNode *n = state->assets.textures.collection; n != NULL; n = n->hash_collection_next)
  {
    UnloadTexture(n->value);
  }

  state->assets.fonts = ZERO_STRUCT;
  state->assets.textures = ZERO_STRUCT;

  mem_arena_clear(state->assets.arena);

  state->assets.fonts.slots = MEM_ARENA_PUSH_ARRAY_ZERO(state->assets.arena, FontSlot, ASSETS_NUM_SLOTS);
  state->assets.textures.slots = MEM_ARENA_PUSH_ARRAY_ZERO(state->assets.arena, TextureSlot, ASSETS_NUM_SLOTS);
}
