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
#include "lexer.h"
#include "compiler.h"
#include "types.h"

typedef enum stack_elem_type_enum {
  ST_TOP = 0,
  ST_SINT32,
  ST_UINT64,
  ST_SINT64,
  ST_LEXEME_FLAVOR,
  ST_OPERATION_TYPE,
  ST_STR
} cgc_stack_elem_type;

typedef struct stack_elem_str {
  struct stack_elem_str* next;
  cgc_stack_elem_type type;
  union {
    cgc_sint32 cgc_sint32;
    cgc_uint64 cgc_uint64;
    cgc_sint64 cgc_sint64;
    cgc_lexeme_flavor cgc_lexeme_flavor;
    cgc_operation_type cgc_operation_type;
    char* str;
  };
} cgc_stack_elem;

cgc_stack_elem* cgc_stack_create();
void cgc_stack_free(cgc_stack_elem* stk);

cgc_uint64 cgc_stack_empty(cgc_stack_elem* stk);

void cgc_stack_push_sint32(cgc_stack_elem* stk, cgc_sint32 i);
void cgc_stack_push_uint64(cgc_stack_elem* stk, cgc_uint64 i);
void cgc_stack_push_sint64(cgc_stack_elem* stk, cgc_sint64 i);
void cgc_stack_push_lexeme_flavor(cgc_stack_elem* stk, cgc_lexeme_flavor f);
void cgc_stack_push_operation_type(cgc_stack_elem* stk, cgc_operation_type t);
void cgc_stack_push_str(cgc_stack_elem* stk, char* s);

cgc_sint32 cgc_stack_pop_sint32(cgc_stack_elem* stk);
cgc_uint64 cgc_stack_pop_uint64(cgc_stack_elem* stk);
cgc_sint64 cgc_stack_pop_sint64(cgc_stack_elem* stk);
cgc_lexeme_flavor cgc_stack_pop_lexeme_flavor(cgc_stack_elem* stk);
cgc_operation_type cgc_stack_pop_operation_type(cgc_stack_elem* stk);
char* cgc_stack_pop_str(cgc_stack_elem* stk);

void cgc_stack_test();
