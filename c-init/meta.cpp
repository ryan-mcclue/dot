// SPDX-License-Identifier: zlib-acknowledgement
#include "base/base-inc.h"

/*
introspect("hi") struct Person
{
  u32 age;
  f32 weight;
  String8 name;
  u32 count;
  counted_pointer(count) u32 *array;
};
 */


// IMPORTANT: this is just basic introspect; for full metaprogramming, generate AST and generate from that
enum META_TYPE
{
  meta_type_u32,
  meta_type_string8,
  meta_type_f32
};

struct MemberDefinition
{
  META_TYPE type;
  String8 name;
  u32 offset;
};

INTROSPECT(category: "hi") struct Name
{
  int x;
  char *y;
};

typedef enum
{
  TOKEN_TYPE_EOS,
  TOKEN_TYPE_NULL,
  TOKEN_TYPE_IDENTIFIER,
  TOKEN_TYPE_OPEN_PAREN,
  TOKEN_TYPE_CLOSE_PAREN,
  TOKEN_TYPE_OPEN_BRACE,
  TOKEN_TYPE_CLOSE_BRACE,
  TOKEN_TYPE_OPEN_BRACKET,
  TOKEN_TYPE_CLOSE_BRACKET,
  TOKEN_TYPE_COLON,
  TOKEN_TYPE_STRING,
  TOKEN_TYPE_ASTERISK,
  TOKEN_TYPE_SEMICOLON,
} TOKEN_TYPE ;

struct Token {
  TOKEN_TYPE type;
  RangeU32 range;
};

typedef struct TokenNode TokenNode;
struct TokenNode {
  TokenNode *next;
  Token token;
};

typedef struct TokenArray TokenArray;
struct TokenArray {
  Token *tokens;
  u32 count;
};

typedef struct Tokeniser Tokeniser;
struct Tokeniser {
  MemArena *arena;
  String8 f;
  TokenArray array;
  u32 at;
};

typedef struct MetaStruct MetaStruct;
struct MetaStruct 
{
  String8 name; 
  MetaStruct *next;
};
GLOBAL MetaStruct *first_meta_struct;

#define TOKEN_VARG(f, t) \
  range_u32_dim(t.range), (f.content + t.range.min)

INTERNAL void
dump_struct(void *struct_ptr, MemberDefinition *defs, Token name_token, u32 count, u32 indent_level)
{
  // for (u32 i = 0; i < indent_level; i += 1) output_str[i] = ' ';
  //printf("%.*s\n", name_token);
  for (u32 i = 0; i < count; i += 1)
  {
    MemberDefinition mb = defs[i]; 
    void *member_ptr = (u8 *)struct_ptr + mb.offset;

    if (mb.is_pointer)
    {
      member_ptr = *(void **)member_ptr;
    }

    switch (mb.type)
    {
      case meta_type_u32:
      {
        printf("%s - %u", mb.name, *(* u32)member_ptr);
      } break;
      case meta_type_v32:
      {
        
      } break;
      META_STRUCT_DUMP(member_ptr)
    }
  }

}

INTERNAL Tokeniser
lex(MemArena *arena, String8 str)
{
  TokenNode *first = NULL, *last = NULL;

  MemArenaTemp temp_arena = mem_arena_temp_begin(NULL, 0);
  u32 token_count;

  TOKEN_TYPE token_type = TOKEN_TYPE_NULL;
  u32 token_start = 0;
  u32 token_end = 0;

  u8 *at = str.content;
  while (at && at[0]) 
  {
    while (is_whitespace(at[0]))
    {
      at += 1;
    }
    if (at[0] == '/' && at[1] == '/')
    {
      at += 2;
      while (at[0] && at[0] != '\n' && at[0] != '\r')
      {
        at += 1;
      }
      if (at[0] && (at[0] == '\n' || at[0] == '\r')) at += 1;
      continue;
    } 
    else if (at[0] == '/' && at[1] == '*')
    {
      at += 2;
      while (at[0] && at[0] != '*' && at[1] && at[1] != '/')
      {
        at += 1;
      }
      continue;
    }

    token_start = at - str.content;
    token_end = token_start + 1;
    char ch = at[0];
    at += 1;
    switch (ch)
    {
      case '\0': { token_type = TOKEN_TYPE_EOS; } break;
      case '(': { token_type = TOKEN_TYPE_OPEN_PAREN; } break;
      case ')': { token_type = TOKEN_TYPE_CLOSE_PAREN; } break;
      case '{': { token_type = TOKEN_TYPE_OPEN_BRACE; } break;
      case '}': { token_type = TOKEN_TYPE_CLOSE_BRACE; } break;
      case '[': { token_type = TOKEN_TYPE_OPEN_BRACKET; } break;
      case ']': { token_type = TOKEN_TYPE_CLOSE_BRACKET; } break;
      case ';': { token_type = TOKEN_TYPE_SEMICOLON; } break;
      case ':': { token_type = TOKEN_TYPE_COLON; } break;
      case '*': { token_type = TOKEN_TYPE_ASTERISK; } break;
      case '"': 
      { 
        token_type = TOKEN_TYPE_STRING; 
        token_start = at - str.content;
        while (at[0] && at[0] != '"')
        {
          if (at[0] == '\\' && at[1]) at += 1;
          at += 1;
        }
        token_end = at - str.content;
        if (at[0] == '"') at += 1;
      } break;
      default:
      {
        if (is_alpha(ch))
        {
          token_type = TOKEN_TYPE_IDENTIFIER; 
          while (is_alpha(at[0]) || is_numeric(at[0]) || at[0] == '_')
          {
            at += 1;
          }
          token_end = at - str.content;
        }
#if 0
        else if (is_numeric(ch))
        {
          lex_number();
        }
#endif
        else
        {
          token_type = TOKEN_TYPE_NULL;
        }
      } break;
    }

    TokenNode *token_node = MEM_ARENA_PUSH_STRUCT(temp_arena.arena, TokenNode);
    token_node->token.type = token_type;
    token_node->token.range = range_u32(token_start, token_end);
    SLL_QUEUE_PUSH(first, last, token_node);
    token_count += 1;

    token_type = TOKEN_TYPE_NULL;
  }

  TokenArray ta = ZERO_STRUCT;
  ta.tokens = MEM_ARENA_PUSH_ARRAY(arena, Token, token_count);
  ta.count = token_count;

  u32 i = 0;
  for (TokenNode *n = first; n != NULL; n = n->next)
  {
    ta.tokens[i++] = n->token;
  }

  mem_arena_temp_end(temp_arena);


  Tokeniser t = {NULL, str, ta, 0};
  return t;
}

INTERNAL void
print_token(String8 f, Token t)
{
  switch (t.type)
  {
#define CASE(t) case TOKEN_TYPE_##t: printf("TOKEN_TYPE_"#t); break;
  CASE(NULL)
  CASE(IDENTIFIER)
  CASE(OPEN_PAREN)
  CASE(CLOSE_PAREN)
  CASE(OPEN_BRACE)
  CASE(CLOSE_BRACE)
  CASE(OPEN_BRACKET)
  CASE(CLOSE_BRACKET)
  CASE(COLON)
  CASE(STRING)
  CASE(ASTERISK)
  CASE(SEMICOLON)
  CASE(EOS)
#undef CASE
  }

  printf(": %.*s\n", t.range.max - t.range.min, f.content + t.range.min);
}

INTERNAL bool
token_equals(String8 f, Token t, String8 m)
{
  String8 src = str8(f.content + t.range.min, range_u32_dim(t.range));
  return str8_match(src, m, 0);
}

INTERNAL Token
consume_token(Tokeniser *t)
{
  if (t->at + 1 < t->array.count)
  {
    t->at += 1;
    return t->array.tokens[t->at - 1];
  }
  else
  {
    return ZERO_STRUCT;
  }
}

INTERNAL bool
require_token(Tokeniser *t, TOKEN_TYPE type)
{
  Token token = consume_token(t);
  return (token.type == type);
}

INTERNAL void
parse_introspectable_params(Tokeniser *t)
{
  while (true)
  {
    Token token = consume_token(t);
    if (token.type == TOKEN_TYPE_CLOSE_PAREN ||
        token.type == TOKEN_TYPE_EOS) break;
  }
}

INTERNAL void
parse_struct_member(Tokeniser *t, Token struct_type, Token member_type)
{
  bool is_pointer = false;
  while (true)
  {
    Token token = consume_token(t);
    if (token.type == TOKEN_TYPE_ASTERISK)
    {
      is_pointer = true;
    }
    if (token.type == TOKEN_TYPE_IDENTIFIER)
    {
      printf("    {%s, meta_type_%.*s, \"%.*s\", (uintptr_t)&(((%.*s *)0)->%.*s)}, \n", 
              is_pointer ? "MEMBER_TYPE_IS_POINTER" : "0",
              TOKEN_VARG(t->f, member_type),
              TOKEN_VARG(t->f, token),
              TOKEN_VARG(t->f, struct_type),
              TOKEN_VARG(t->f, token));
    }
    else if (token.type == TOKEN_TYPE_SEMICOLON ||
        token.type == TOKEN_TYPE_EOS) break;
  }
}


INTERNAL void
parse_struct(Tokeniser *t)
{
  Token name = consume_token(t);
  if (require_token(t, TOKEN_TYPE_OPEN_BRACE))
  {
    printf("GLOBAL MemberDefinition g_members_of_%.*s[] = \n", TOKEN_VARG(t->f, name));
    printf("{\n");
    while (true)
    {
      Token member_token = consume_token(t);
      if (member_token.type == TOKEN_TYPE_CLOSE_BRACE ||
          member_token.type == TOKEN_TYPE_EOS)
      {
        break;
      }
      else
      {
        parse_struct_member(t, name, member_token);
      }
    }
    printf("};\n");

    MetaStruct *s = MEM_ARENA_PUSH_STRUCT(t->arena, MetaStruct);
    s->name = str8_fmt(t->arena, "%.*s", TOKEN_VARG(t->f, name));
    SLL_STACK_PUSH(first_meta_struct, s);
  }
  else
  {
    WARN("struct requires {");
  }
}

INTERNAL void
parse_introspectable(Tokeniser *t)
{
  if (require_token(t, TOKEN_TYPE_OPEN_PAREN))
  {
    parse_introspectable_params(t);
    Token token = consume_token(t);
    if (token_equals(t->f, token, str8_lit("struct")))
    {
      parse_struct(t);
    }
    else
    {
      WARN("instropect just for structs");
    }
  }
  else
  {
    WARN("Require ( after instropect");
  }
}

int
main(int argc, char *argv[])
{
  global_debugger_present = linux_was_launched_by_gdb();
  MemArena *arena = mem_arena_allocate(GB(8), MB(64));

  ThreadContext tctx = thread_context_allocate(GB(8), MB(64));
  tctx.is_main_thread = true;
  thread_context_set(&tctx);
  thread_context_set_name("Main Thread");

  String8 f = str8_read_entire_file(arena, str8_lit("code/meta-test.h"));
  Tokeniser tokeniser = lex(arena, f);
  tokeniser.arena = arena;

  while (true)
  {
    Token t = consume_token(&tokeniser); 

    //print_token(f, t);

    if (t.type == TOKEN_TYPE_EOS) break;
    else if (token_equals(f, t, str8_lit("introspect")))
    {
      parse_introspectable(&tokeniser);
    }
  }

  printf("#define META_STRUCT_DUMP(member_ptr) \\ \n");
  for (MetaStruct *s = first_meta_struct; s != NULL; s = s->next)
  {
    printf("case meta_type_%.*s: dump_struct(member_ptr, members_of_%.*s, ARRAY_COUNT(members_of_%.*s), indent_level + 1); break; %s\n", 
            str8_varg(s->name), str8_varg(s->name), str8_varg(s->name), 
            s->next ? "\\" : "");
  }

  return 0;
}
