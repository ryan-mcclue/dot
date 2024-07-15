#!/usr/bin/python3
# SPDX-License-Identifier: zlib-acknowledgement

from pygments import lex
from pygments.lexers import CLexer
from pygments.token import Token

def read_file(f):
  c = ""
  with open(f, "r") as fp:
    c = fp.read()
  return c

def consume_token():
  if g_token_i < len(g_tokens) - 1:
    g_token_i += 1
    return g_tokens[g_token_i]
  else:
    return []

def get_token():
  if g_token_i < len(g_tokens):
    return g_tokens[g_token_i]
  else:
    return []

def token_equals(type, value=""):
  token = get_token()
  if token[0] == type:
    if value != "":
      return token[1] == value
    else:
      return True
  else:
    return False

def require_token(type, value=""):
  consume_token()
  return token_equals(type, value)

def parse_introspectable():
  if require_token(Token.Punctuation, "("):
    parse_introspectable_params()
  else:
    print("error")


def parse_introspect_params():
  while True:
    token = get_token()
    if (token[


code = read_file("desktop.h")
lexer = CLexer(stripall=True)
g_tokens = list(lexer.get_tokens(code))
g_token_i = 0
while g_token_i < len(g_tokens):
  token = g_tokens[g_token_i]
  token_type = token[0]
  token_value = token[1]

  #if token_type == Token.Name and token_value == "INTROSPECT":
  #  parse_introspectable()
  
  print(token)
  g_token_i += 1

#parse_introspectable():
#  if require_token(open_param):
#    parse_introspect_params()
#    if next_token == "struct":
#      parse_struct()
#    else:
#      error()
#  else:
#    error()
