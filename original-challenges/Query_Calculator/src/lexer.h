/*

Copyright (c) 2016 Cromulence LLC

Authors: Bryce Kerley <bk@cromulence.com>

Permission is hereby granted, cgc_free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#pragma once
#include "types.h"

typedef enum lexeme_flavor_enum {
  F_ERROR = 0,
  F_FIND,
  F_IDENTIFIER,
  F_INTEGER_LITERAL,
  F_CHARACTER_LITERAL,
  F_PLUS,
  F_MINUS,
  F_ASTERISK,
  F_SOLIDUS,
  F_FROM,
  F_COMMA,
  F_OPEN_PAREN,
  F_CLOSE_PAREN
} cgc_lexeme_flavor;

typedef struct {
  cgc_lexeme_flavor flavor;
  cgc_uint16 bytes_len;
  char* bytes;
} cgc_lexeme;

typedef struct lexer_list_str {
  cgc_lexeme* content;
  struct lexer_list_str* next;
} cgc_lexer_list;

cgc_lexer_list* cgc_lex_string(cgc_uint16 cgc_len, char* str);
void cgc_lexer_test();
