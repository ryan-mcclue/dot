// SPDX-License-Identifier: zlib-acknowledgement
#if !defined(DESKTOP_ASSETS_H)

#include <raylib.h>

typedef struct FontNode FontNode;
struct FontNode
{
  String8 key;
  FontNode *hash_chain_next;
  FontNode *hash_collection_next;
  Font value;
};
typedef struct FontSlot FontSlot;
struct FontSlot
{
  FontNode *first;
  FontNode *last;
};
typedef struct FontMap FontMap;
struct FontMap
{
  FontSlot *slots;
  FontNode *collection;
};

typedef struct TextureNode TextureNode;
struct TextureNode
{
  String8 key;
  TextureNode *hash_chain_next;
  TextureNode *hash_collection_next;
  Texture value;
};
typedef struct TextureSlot TextureSlot;
struct TextureSlot
{
  TextureNode *first;
  TextureNode *last;
};
typedef struct TextureMap TextureMap;
struct TextureMap
{
  TextureSlot *slots;
  TextureNode *collection;
};

typedef struct Assets Assets;
struct Assets
{
  MemArena *arena;
  FontMap fonts;
  TextureMap textures;
};

#endif
