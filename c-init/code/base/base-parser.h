// SPDX-License-Identifier: zlib-acknowledgement

#if 0
  String8 file = str8_read_entire_file(perm_arena, str8_lit("example.lang"));
  ASSERT(file.size != 0);
  TokenArray tokens = get_token_array(perm_arena, file);

  PRINT_U64(tokens.count);
  for (u64 i = 0; i < tokens.count; i += 1)
  {
    Token token = tokens.tokens[i];
    switch (token.type)
    {
#define CASE(n) case TOKEN_TYPE_##n: puts("TOKEN_TYPE_" #n); break
      CASE(NEWLINE);
      CASE(WHITESPACE);
#undef CASE
    }
  }
#endif

typedef enum
{
  TOKEN_TYPE_NULL = 0, 
   TOKEN_TYPE_NEWLINE,
  TOKEN_TYPE_ERROR,
   TOKEN_TYPE_WHITESPACE,
   TOKEN_TYPE_IDENTIFIER,
   TOKEN_TYPE_SYMBOL,
  TOKEN_TYPE_OPEN_BRACE,
  TOKEN_TYPE_CLOSE_BRACE,
  TOKEN_TYPE_COLON,
  TOKEN_TYPE_STRING_LITERAL,
  TOKEN_TYPE_NUMERIC_LITERAL,

  TOKEN_TYPE_COUNT,
} TOKEN_TYPE;

typedef struct Token Token;
struct Token
{
  TOKEN_TYPE type;
  RangeU64 range;
};

GLOBAL Token global_null_token = {
  TOKEN_TYPE_NULL,
  ZERO_STRUCT
};

typedef struct TokenNode TokenNode;
struct TokenNode
{
  TokenNode *next;
  Token token;
};

typedef struct TokenList TokenList;
struct TokenList
{
  TokenNode *first, *last;
  u64 count;
};

typedef struct TokenArray TokenArray;
struct TokenArray
{
  Token *tokens;
  u64 count;
};

INTERNAL TokenArray 
token_array_from_list(MemArena *arena, TokenList *list)
{
  TokenArray result = ZERO_STRUCT;
  result.tokens = MEM_ARENA_PUSH_ARRAY(arena, Token, list->count);

  u64 i = 0;
  for (TokenNode *n = list->first; n != NULL; n = n->next)
  {
    MEMORY_COPY(result.tokens + i, &n->token, sizeof(Token));
    i += 1;
  }

  result.count = list->count;

  return result;
}

INTERNAL TokenArray
get_token_array(MemArena *arena, String8 str)
{
  MemArenaTemp temp = mem_arena_temp_begin(&arena, 1);

  TokenList token_list = ZERO_STRUCT;

  TOKEN_TYPE cur_token_type = TOKEN_TYPE_NULL;
  u64 cur_token_start = 0;

  u64 off = 0;
  u64 inc = 0;
  while (off <= str.size)
  {
    char ch = str.content[off];
    char ch_next = (off+1 < str.size ? str.content[off+1] : '\0');

    b32 ender = false;

    inc = (cur_token_type != TOKEN_TYPE_NULL) ? 1 : 0;

    if (off == str.size && cur_token_type != TOKEN_TYPE_NULL)
    {
      ender = true;
      inc = 1;
    }

    switch (cur_token_type)
    {
      default:
        // identify start of token; following case statements determine end 
      case TOKEN_TYPE_NULL:
      {
        if ((ch == '\r' && ch_next == '\n') || ch == '\n')
        {
          cur_token_type = TOKEN_TYPE_NEWLINE;
          cur_token_start = off;
          inc = 0;
        }
        else if (ch == ' ' || ch == '\t' || ch == '\v' || ch == '\f')
        {
          cur_token_type = TOKEN_TYPE_WHITESPACE;
          cur_token_start = off;
          inc = 1;
        }
        else
        {
          cur_token_start = off;
          cur_token_type = TOKEN_TYPE_ERROR;
          inc = 1;
        }
      } break;
      case TOKEN_TYPE_NEWLINE:
      {
        if (ch == '\r' && ch_next == '\n') inc = 2, ender = true;
        else if (ch == '\n') inc = 1, ender = true;
      } break;
      case TOKEN_TYPE_WHITESPACE:
      {
        if (ch != ' ' && ch != '\t' && ch != '\v' && ch != '\f')
        {
          ender = true;
          inc = 0;
        }
      } break;
      case TOKEN_TYPE_ERROR:
      {
        ender = true;
        inc = 0;
      } break;
    }

    if (ender)
    {
      TokenNode *token_node = MEM_ARENA_PUSH_STRUCT(temp.arena, TokenNode);
      token_node->token.type = cur_token_type; 
      token_node->token.range = range_u64(cur_token_start, off + inc);
      
      SLL_QUEUE_PUSH(token_list.first, token_list.last, token_node);
      token_list.count += 1;

      cur_token_type = TOKEN_TYPE_NULL;
      cur_token_start = token_node->token.range.max;
    }

    off += inc;
  }

  TokenArray result = token_array_from_list(arena, &token_list);

  mem_arena_temp_end(temp);

  return result;
}

typedef enum
{
  JSON_NODE_TYPE_ARRAY,
  JSON_NODE_TYPE_OBJECT,
  JSON_NODE_TYPE_STRING,
  JSON_NODE_TYPE_NUMBER,
  JSON_NODE_TYPE_BOOLEAN,
  JSON_NODE_TYPE_NULL,

  JSON_NODE_TYPE_COUNT,
} JSON_NODE_TYPE;

typedef struct JsonNode JsonNode;
struct JsonNode
{
  JSON_NODE_TYPE type;
  String8 label;
  union
  {
    b32 boolean;
    f32 number;
    String8 string;
    JsonNode *first_child;
  };

  JsonNode *next;
};

INTERNAL char *
token_get_type(TOKEN_TYPE type)
{
  switch (type)
  {
#define CASE(n) case TOKEN_TYPE_##n: return "TOKEN_TYPE_" #n; break
    CASE(IDENTIFIER);
    CASE(OPEN_BRACKET);
    CASE(CLOSE_BRACKET);
    CASE(IDENTIFER);
    CASE(COLON);
    CASE(STRING_LITERAL);
    CASE(NUMERIC_LITERAL);
#undef CASE
  }
}

INTERNAL void
token_expect_error(TOKEN_TYPE type)
{
  printf("Error: Expected type %s\n", token_get_type(type));
  exit(1);
}

INTERNAL void
advance_token(Token **token, Token *token_opl, u32 inc)
{
  if (*token + inc < token_opl) *token += inc;
  else *token = &global_null_token;
}

INTERNAL void
expect_next_token(Token **token, Token *token_opl, TOKEN_TYPE type)
{
  advance_token(token, token_opl, 1);
  if (token->type != type) token_expect_error(type);
}

INTERNAL JsonNode *
parse_token_json_object(String8 data, Token **token, Token *token_opl)
{
  JsonNode *first_child = NULL, *last_child = NULL;

  while (token < token_opl)
  {
    expect_next_token(token, token_opl, TOKEN_TYPE_STRING_LITERAL);
    String8 label = str8_range_u64(data, token->range);

    expect_next_token(token, token_opl, TOKEN_TYPE_COLON);

    advance_token(token, token_opl, 1);

    expect_next_token(token, token_opl, TOKEN_TYPE_STRING_LITERAL);
    String8 label = str8_range_u64(data, token->range);

    expect_next_token(token, token_opl, TOKEN_TYPE_COLON);
    JsonNode *child = parse_token_json(data, label, token, token_opl);
    if (child != NULL) SLL_QUEUE_PUSH(first_child, last_child, child);

    if (token->type == TOKEN_TYPE_CLOSE_BRACE) break;
    else if (token->type != TOKEN_TYPE_COMMA) token_expect_error(TOKEN_TYPE_COMMA);
  }

  return first_child;
}

INTERNAL JsonNode *
parse_token_json_array(String8 data, Token **token, Token *token_opl)
{
  JsonNode *first_child = NULL, *last_child = NULL;

  while (token < token_opl)
  {
    String8 label = ZERO_STRUCT;
    advance_token(token, token_opl, 1);

    JsonNode *child = parse_token_json(data, label, token, token_opl);
    if (child != NULL) SLL_QUEUE_PUSH(first_child, last_child, child);

    if (token->type == TOKEN_TYPE_CLOSE_BRACKET) break;
    else if (token->type != TOKEN_TYPE_COMMA) token_expect_error(TOKEN_TYPE_COMMA);
  }

  return first_child;
}

#if 0
INTERNAL JsonNode *
parse_token_json(String8 data, String8 label, Token *token, Token *token_opl)
{
  String8 token_str = str8_range(data, token->range);

  JsonNode *first_child = NULL;
  b32 valid = true;
  if (token->type == TOKEN_TYPE_OPEN_BRACKET)
  {
    first_child = parse_token_json_object(data, token, token_opl);
  }
  else if (token->type == TOKEN_TYPE_NUMERIC_LITERAL)
  {

  }
  else if (token->type == TOKEN_TYPE_STRING_LITERAL)
  {

  }
  else if (token->type == TOKEN_TYPE_IDENTIFIER)
  {

  }
  else
  {
    valid = false;
  }
  
  // base case is if the element is standalone
  if (valid)
  {
    JsonNode *result = MEM_ARENA_PUSH_STRUCT_ZERO(arena, JsonNode);
    result->label = label;// will only have label if object
    result->value = value; // unionify
    result->first_child = first_child;
  }
  else
  {
    return NULL;
  }
}

INTERNAL JsonNode *
parse_tokens_json(String8 data, TokenArray *tokens)
{
  Token *token_opl = tokens->tokens + tokens->count;
  String8 empty_label = ZERO_STRUCT;
  JsonNode *root = parse_token_json(data, empty_label, tokens->tokens, token_opl);
}

INTERNAL JsonNode *
lookup_json_label(JsonNode *node, String8 label)
{
  JsonNode *result = NULL;

  for (JsonNode *n = node; n != NULL; n = n->next)
  {
    if (str8_match(n->label, label, 0)) return n;
  }

  return result;
}
#endif

// Instead of printing intermittedly, return message_list in result struct?

// functional programming involves managing a lot of complexity
// e.g. function return value pair, but one item could be multiple types to indicate error etc.

