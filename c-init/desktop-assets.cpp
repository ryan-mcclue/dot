// SPDX-License-Identifier: zlib-acknowledgement

#define ASSETS_NUM_SLOTS 256

INTERNAL void *
assets_find_(void *slots, String8 key, memory_index slot_size, memory_index key_offset, 
             memory_index next_offset, memory_index value_offset)
{
  u64 hash = str8_hash(key);
  u64 slot_i = hash % ASSETS_NUM_SLOTS;
  u8 *slot = (u8 *)slots + slot_i * slot_size; 

  u8 *first_node = slot;
  String8 node_key = *(String8 *)(first_node + key_offset);
  if (str8_match(node_key, key, 0)) return (first_node + value_offset);

  for (u8 *chain_node = (first_node + next_offset);
       chain_node != NULL; 
       chain_node = (chain_node + next_offset))
  {
    node_key = *(String8 *)(chain_node + key_offset);
    if (str8_match(node_key, key, 0)) return (u8 *)(chain_node + value_offset);
  }

  return NULL;
}

// NOTE(Ryan): gf2 (ctrl-d for assembly)
// with optimisations, compiler will determine offsets at compile time
#define ASSETS_FIND_FONT(key) \
  assets_find_(g_state->assets.font_assets.slots, key, sizeof(*slots), \
               OFFSET_OF_MEMBER(FontNode, key), \
               OFFSET_OF_MEMBER(FontNode, hash_chain_next), \
               OFFSET_OF_MEMBER(FontNode, value))
#define ASSETS_FIND_TEXTURE(slots, key) \
  assets_find_(g_state->assets.texture_assets.slots, key, sizeof(*slots), \
               OFFSET_OF_MEMBER(TextureNode, key), \
               OFFSET_OF_MEMBER(TextureNode, hash_chain_next), \
               OFFSET_OF_MEMBER(TextureNode, value))

INTERNAL Font
assets_get_font(String8 key)
{
  Font *p = (Font *)ASSETS_FIND_FONT(key);
  if (p != NULL) return *p;
  else
  {
    char cpath[256] = ZERO_STRUCT;
    str8_to_cstr(key, cpath, sizeof(cpath)); 

    // TODO(Ryan): Add parameters to asset keys
    // NOTE: will get default font if failed, so always valid
    Font v = LoadFontEx(cpath, 64, NULL, 0);

    FontNode *n = MEM_ARENA_PUSH_STRUCT(g_state->assets.arena, FontNode);
    n->key = key;
    n->value = v;

    u64 slot_i = str8_hash(key) % ASSETS_NUM_SLOTS;
    FontSlot *s = &g_state->assets.fonts.slots[slot_i];
    __SLL_QUEUE_PUSH(s->first, s->last, n, hash_chain_next);
    __SLL_STACK_PUSH(g_state->assets.fonts.collection, n, hash_collection_next);

    return v;
  }
}

INTERNAL Texture
assets_get_texture(String8 key)
{
  Texture *p = (Texture *)ASSETS_FIND_TEXTURE(key);
  if (p != NULL) return *p;
  else
  {
    char cpath[256] = ZERO_STRUCT;
    str8_to_cstr(key, cpath, sizeof(cpath)); 

    Texture v = LoadTexture(cpath);

    SetTextureFilter(v, TEXTURE_FILTER_BILINEAR);

    TextureNode *n = MEM_ARENA_PUSH_STRUCT(g_state->assets.arena, TextureNode);
    n->key = key;
    n->value = v;

    u64 slot_i = str8_hash(key) % ASSETS_NUM_SLOTS;
    TextureSlot *s = &g_state->assets.textures.slots[slot_i];
    __SLL_QUEUE_PUSH(s->first, s->last, n, hash_chain_next);
    __SLL_STACK_PUSH(g_state->assets.textures.collection, n, hash_collection_next);

    return v;
  }
}

INTERNAL void
assets_preload(void)
{
  for (FontNode *n = g_state->assets.fonts.collection; n != NULL; n = n->hash_collection_next)
  {
    UnloadFont(n->value);
  }
  for (TextureNode *n = g_state->assets.textures.collection; n != NULL; n = n->hash_collection_next)
  {
    UnloadTexture(n->value);
  }

  g_state->assets.fonts = ZERO_STRUCT;
  g_state->assets.textures = ZERO_STRUCT;

  mem_arena_clear(g_state->assets.arena);

  g_state->assets.fonts.slots = MEM_ARENA_PUSH_ARRAY_ZERO(g_state->assets.arena, FontSlot, ASSETS_NUM_SLOTS);
  g_state->assets.textures.slots = MEM_ARENA_PUSH_ARRAY_ZERO(g_state->assets.arena, TextureSlot, ASSETS_NUM_SLOTS);
}
