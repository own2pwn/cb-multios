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

#include <malloc.h>
#include "stack.h"

cgc_stack_elem* cgc_stack_create() {
  cgc_stack_elem* top = cgc_calloc(sizeof(cgc_stack_elem));
  top->next = NULL;
  top->type = ST_TOP;

  return top;
}

void cgc_stack_free(cgc_stack_elem* stk) {
  if (NULL == stk) return;
  
  cgc_stack_elem* next = stk->next;
  cgc_free(stk);

  return cgc_stack_free(next);
}

cgc_stack_elem_type cgc_stack_next(cgc_stack_elem* stk) {
  if (cgc_stack_empty(stk)) return ST_TOP;
  return stk->next->type;
}

cgc_uint64 cgc_stack_empty(cgc_stack_elem* stk) {
  if (NULL == stk->next) return 1;
  return 0;
}

void cgc_stack_push_sint32(cgc_stack_elem* stk, cgc_sint32 i) {
  cgc_stack_elem* new_node = cgc_calloc(sizeof(cgc_stack_elem));
  cgc_stack_elem* previous_top = stk->next;
  new_node->next = previous_top;
  new_node->type = ST_SINT32;
  new_node->cgc_sint32 = i;
  stk->next = new_node;
}

void cgc_stack_push_uint64(cgc_stack_elem* stk, cgc_uint64 i) {
  cgc_stack_elem* new_node = cgc_calloc(sizeof(cgc_stack_elem));
  cgc_stack_elem* previous_top = stk->next;
  new_node->next = previous_top;
  new_node->type = ST_UINT64;
  new_node->cgc_uint64 = i;
  stk->next = new_node;
}

void cgc_stack_push_sint64(cgc_stack_elem* stk, cgc_sint64 i) {
  cgc_stack_elem* new_node = cgc_calloc(sizeof(cgc_stack_elem));
  cgc_stack_elem* previous_top = stk->next;
  new_node->next = previous_top;
  new_node->type = ST_SINT64;
  new_node->cgc_sint64 = i;
  stk->next = new_node;
}

void cgc_stack_push_lexeme_flavor(cgc_stack_elem* stk, cgc_lexeme_flavor f) {
  cgc_stack_elem* new_node = cgc_calloc(sizeof(cgc_stack_elem));
  cgc_stack_elem* previous_top = stk->next;
  new_node->next = previous_top;
  new_node->type = ST_LEXEME_FLAVOR;
  new_node->cgc_lexeme_flavor = f;
  stk->next = new_node;
}

void cgc_stack_push_operation_type(cgc_stack_elem* stk, cgc_operation_type t) {
  cgc_stack_elem* new_node = cgc_calloc(sizeof(cgc_stack_elem));
  cgc_stack_elem* previous_top = stk->next;
  new_node->next = previous_top;
  new_node->type = ST_OPERATION_TYPE;
  new_node->cgc_operation_type = t;
  stk->next = new_node;
}

void cgc_stack_push_str(cgc_stack_elem* stk, char* str) {
  cgc_stack_elem* new_node = cgc_calloc(sizeof(cgc_stack_elem));
  cgc_stack_elem* previous_top = stk->next;
  new_node->next = previous_top;
  new_node->type = ST_STR;
  new_node->str = str;
  stk->next = new_node;
}

cgc_sint32 cgc_stack_pop_sint32(cgc_stack_elem* stk) {
  cgc_stack_elem* old_top = stk->next;
  cgc_stack_elem* new_top = stk->next->next;
  cgc_sint32 val = old_top->cgc_sint32;
  stk->next = new_top;
  cgc_free(old_top);
  return val;
}

cgc_uint64 cgc_stack_pop_uint64(cgc_stack_elem* stk) {
  cgc_stack_elem* old_top = stk->next;
  cgc_stack_elem* new_top = stk->next->next;
  cgc_uint64 val = old_top->cgc_uint64;
  stk->next = new_top;
  cgc_free(old_top);
  return val;
}

cgc_sint64 cgc_stack_pop_sint64(cgc_stack_elem* stk) {
  cgc_stack_elem* old_top = stk->next;
  cgc_stack_elem* new_top = stk->next->next;
  cgc_sint64 val = old_top->cgc_sint64;
  stk->next = new_top;
  cgc_free(old_top);
  return val;
}

cgc_lexeme_flavor cgc_stack_pop_lexeme_flavor(cgc_stack_elem* stk) {
  cgc_stack_elem* old_top = stk->next;
  cgc_stack_elem* new_top = stk->next->next;
  cgc_lexeme_flavor val = old_top->cgc_lexeme_flavor;
  stk->next = new_top;
  cgc_free(old_top);
  return val;
}

cgc_operation_type cgc_stack_pop_operation_type(cgc_stack_elem* stk) {
  cgc_stack_elem* old_top = stk->next;
  cgc_stack_elem* new_top = stk->next->next;
  cgc_operation_type val = old_top->cgc_operation_type;
  stk->next = new_top;
  cgc_free(old_top);
  return val;
}

char* cgc_stack_pop_str(cgc_stack_elem* stk) {
  cgc_stack_elem* old_top = stk->next;
  cgc_stack_elem* new_top = stk->next->next;
  char* val = old_top->str;
  stk->next = new_top;
  cgc_free(old_top);
  return val;
}

void cgc_assert_lexeme_flavor_equal(cgc_lexeme_flavor expected, cgc_lexeme_flavor actual) {
  if (expected == actual) return;
  _terminate(-1);
}

void cgc_assert_uint64_equal(cgc_uint64 expected, cgc_uint64 actual) {
  if (expected == actual) return;
  _terminate(-1);
}

void cgc_refute_uint64_equal(cgc_uint64 expected, cgc_uint64 actual) {
  if (expected != actual) return;
  _terminate(-1);
}

void cgc_stack_test() {
  cgc_stack_elem* first = cgc_stack_create();

  cgc_refute_uint64_equal(0, cgc_stack_empty(first));

  cgc_stack_push_uint64(first, 12345);
  cgc_assert_uint64_equal(0, cgc_stack_empty(first));

  cgc_assert_uint64_equal(12345, cgc_stack_pop_uint64(first));
  cgc_refute_uint64_equal(0, cgc_stack_empty(first));
  
  cgc_stack_push_uint64(first, 444);
  cgc_assert_uint64_equal(0, cgc_stack_empty(first));

  cgc_stack_push_lexeme_flavor(first, F_ASTERISK);
  cgc_assert_lexeme_flavor_equal(F_ASTERISK, cgc_stack_pop_lexeme_flavor(first));

  cgc_assert_uint64_equal(0, cgc_stack_empty(first));

  cgc_assert_uint64_equal(444, cgc_stack_pop_uint64(first));
  cgc_refute_uint64_equal(0, cgc_stack_empty(first));
}
