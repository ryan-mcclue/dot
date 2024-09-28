// SPDX-License-Identifier: zlib-acknowledgement
#if !defined(BASE_FILE_H)
#define BASE_FILE_H

typedef struct AsyncFile AsyncFile;
struct AsyncFile
{
  FILE *fp;
  memory_index file_size;
};

INTERNAL AsyncFile
open_async_file(const char *name)
{
  FILE *fp = fopen(name, "rb");
  fseek(fp, 0, SEEK_END);
  memory_index file_size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  return {fp, file_size};
}

INTERNAL String8 
str8_read_entire_file(MemArena *arena, String8 file_name)
{
  String8 result = ZERO_STRUCT;

  char buf[512] = ZERO_STRUCT;
  str8_to_cstr(file_name, buf, sizeof(buf));
  FILE *file = fopen(buf, "rb");

  if (file != NULL)
  {
    fseek(file, 0, SEEK_END);
    u64 file_size = (u64)ftell(file);
    fseek(file, 0, SEEK_SET);
    result.content = MEM_ARENA_PUSH_ARRAY(arena, u8, file_size + 1);
    if (result.content != NULL)
    {
      result.size = file_size;
      PROFILE_BANDWIDTH("fread", file_size)
      {
        fread(result.content, 1, file_size, file);
      }
      result.content[result.size] = '\0';
    }
    fclose(file);
  }
  else
  {
    WARN("Failed to open file %.*s\n\t%s\n", str8_varg(file_name), strerror(errno));
  }

  return result;
}

INTERNAL void
str8_write_entire_file(String8 file_name, String8 data)
{
  char buf[512] = ZERO_STRUCT;
  str8_to_cstr(file_name, buf, sizeof(buf));
	FILE *file = fopen(buf, "w+");

  if (file != NULL)
  {
	  fwrite(data.content, 1, data.size, file);
	  fclose(file);
  }
  else
  {
    WARN("Failed to open file %.*s\n\t%s\n", str8_varg(file_name), strerror(errno));
  }
}

INTERNAL void
str8_append_to_file(String8 file_name, String8 data)
{
  char buf[512] = ZERO_STRUCT;
  str8_to_cstr(file_name, buf, sizeof(buf));
	FILE *file = fopen(buf, "a");

  if (file != NULL)
  {
	  fwrite(data.content, 1, data.size, file);
	  fclose(file);
  }
  else
  {
    WARN("Failed to open file %.*s\n\t%s\n", str8_varg(file_name), strerror(errno));
  }
}

INTERNAL void
str8_copy_file(MemArena *arena, String8 source_file, String8 dest_file)
{
  String8 source_file_data = str8_read_entire_file(arena, source_file);
  str8_write_entire_file(dest_file, source_file_data);
}

#endif
