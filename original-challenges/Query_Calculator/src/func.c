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

#include <libcgc.h>
#include "string.h"
#include "char.h"
#include "func.h"
#include "malloc.h"
#include "protocol.h"
#include "eval.h"

#define FUNC(name, func) if (0 == cgc_strcmp(name, func_name)) return func(stk);

void cgc_ord(cgc_stack_elem* stk) {
  cgc_sint32 offset = cgc_stack_pop_sint32(stk);
  char* body = cgc_stack_pop_str(stk);

#ifdef PATCHED_1
  if (offset < 1) _terminate(-1);
  if (offset > cgc_strlen(body)) _terminate(-1);
#endif
  
  cgc_sint32 char_value = ((unsigned char*)body)[offset - 1];
  
  cgc_stack_push_sint32(stk, char_value);
}

void cgc_upcase(cgc_stack_elem* stk) {
  char* body = cgc_stack_pop_str(stk);
  cgc_uint32 cgc_len = cgc_strlen(body);
  char* new = cgc_calloc(cgc_len + 1);
  for (cgc_uint32 i = 0; i <= cgc_len; i++) {
    char cur = body[i];
    new[i] = cgc_char_is_lower(cur) ? (cur - 0x20) : cur;
  }
  cgc_stack_push_str(stk, new);
}


void cgc_downcase(cgc_stack_elem* stk) {
  char* body = cgc_stack_pop_str(stk);
  cgc_uint32 cgc_len = cgc_strlen(body);
  char* new = cgc_calloc(cgc_len + 1);
  for (cgc_uint32 i = 0; i <= cgc_len; i++) {
    char cur = body[i];
    new[i] = cgc_char_is_upper(cur) ? (cur + 0x20) : cur;
  }
  cgc_stack_push_str(stk, new);
}

void cgc_len(cgc_stack_elem* stk) {
  char* body = cgc_stack_pop_str(stk);
  cgc_uint32 cgc_len = cgc_strlen(body);
  cgc_stack_push_sint32(stk, cgc_len);
}

void cgc_idx(cgc_stack_elem* stk) {
  char needle = cgc_stack_pop_str(stk)[0];
  char* haystack = cgc_stack_pop_str(stk);
  cgc_uint32 cgc_len = cgc_strlen(haystack);
  for (cgc_uint32 i = 0; i <= cgc_len; i++) {
    char cur = haystack[i];
    if (cur == needle) {
      cgc_stack_push_sint32(stk, i + 1);
      return;
    }
  }
}

void cgc_ridx(cgc_stack_elem* stk) {
  char needle = cgc_stack_pop_str(stk)[0];
  char* haystack = cgc_stack_pop_str(stk);
  cgc_uint32 cgc_len = cgc_strlen(haystack);
  for (cgc_sint32 i = cgc_len; i >= 0; i--) {
    char cur = haystack[i];
    if (cur == needle) {
      cgc_stack_push_sint32(stk, i + 1);
      return;
    }
  }
}

void cgc_chomp(cgc_stack_elem* stk) {
  char* body = cgc_stack_pop_str(stk);
  cgc_uint32 cgc_len = cgc_strlen(body);
  char* new = cgc_calloc(cgc_len + 1);

  cgc_uint32 cgc_idx = 0;
  cgc_uint32 jdx = 0;
  while (cgc_char_is_whitespace(body[cgc_idx]) && (0 != body[cgc_idx])) {
    cgc_idx++;
  }

  while (!cgc_char_is_whitespace(body[cgc_idx]) && (0 != body[cgc_idx])) {
    new[jdx] = body[cgc_idx];
    cgc_idx++;
    jdx++;
  }
  
  cgc_stack_push_str(stk, new);
}

void cgc_func_eval(cgc_stack_elem* stk) {
  char* body = cgc_stack_pop_str(stk);
  cgc_sint32 result = cgc_eval_string(body);
  cgc_stack_push_sint32(stk, result);
}

void cgc_func_dispatch(cgc_stack_elem* stk, char* func_name) {
  FUNC("cgc_ord", cgc_ord);
  FUNC("cgc_upcase", cgc_upcase);
  FUNC("cgc_downcase", cgc_downcase);
  FUNC("cgc_len", cgc_len);
  FUNC("cgc_idx", cgc_idx);
  FUNC("cgc_ridx", cgc_ridx);
  FUNC("cgc_chomp", cgc_chomp);
  FUNC("cgc_eval", cgc_func_eval);

  cgc_protocol_send_str("cgc_eval error, unrecognized func");
  _terminate(-1);
}
