// SPDX-License-Identifier: zlib-acknowledgement
#if !defined(DESKTOP_ASSETS_H)

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

typedef struct ImageNode ImageNode;
struct ImageNode
{
  String8 key;
  ImageNode *hash_chain_next;
  ImageNode *hash_collection_next;
  Image value;
};
typedef struct ImageSlot ImageSlot;
struct ImageSlot
{
  ImageNode *first;
  ImageNode *last;
};
typedef struct ImageMap ImageMap;
struct ImageMap
{
  ImageSlot *slots;
  ImageNode *collection;
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
  ImageMap images;
  TextureMap textures;
};

#endif
