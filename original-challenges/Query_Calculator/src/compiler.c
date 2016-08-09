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

#include "compiler.h"
#include "malloc.h"
#include "stdlib.h"
#include "string.h"
#include "stack.h"

typedef struct compiler_state_str {
  cgc_lexer_list* remaining;
  cgc_uint64 current_position;
  cgc_compiler clr;
  cgc_operation* last_op;
  cgc_lexeme* error_lexeme;
} cgc_compiler_state;

void cgc_compile_uint64(cgc_compiler* cmp, cgc_uint64 val);
void cgc_compile_lexeme_flavor(cgc_compiler* cmp, cgc_lexeme_flavor flav);

cgc_compiler_state cgc_compile_preamble(cgc_compiler_state in);
cgc_compiler_state cgc_compile_expression(cgc_compiler_state in);
cgc_compiler_state cgc_compile_conclusion(cgc_compiler_state in);
cgc_compiler_state cgc_compile_function(cgc_compiler_state in);
cgc_compiler_state cgc_compile_operand(cgc_compiler_state in);

cgc_compiler* cgc_compiler_error(cgc_uint64 position, cgc_lexeme* cgc_lexeme);

cgc_compiler* cgc_compile(cgc_lexer_list* lexemes) {
  cgc_compiler_state initial;
  initial.remaining = lexemes;
  initial.current_position = 0;
  initial.clr.find_list =
    initial.last_op =
    cgc_calloc(sizeof(cgc_operation));
  initial.error_lexeme = initial.clr.error_lexeme = NULL;
  

  cgc_compiler_state after_preamble = cgc_compile_preamble(initial);
  if (after_preamble.error_lexeme) {
    return cgc_compiler_error(after_preamble.current_position,
                          after_preamble.error_lexeme);
  }

  cgc_compiler_state after_expression = cgc_compile_expression(after_preamble);

  if (after_expression.error_lexeme) {
    return cgc_compiler_error(after_expression.current_position,
                          after_expression.error_lexeme);
  }

  cgc_compiler_state after_conclusion = cgc_compile_conclusion(after_expression);

  if (after_conclusion.error_lexeme) {
    return cgc_compiler_error(after_conclusion.current_position,
                          after_conclusion.error_lexeme);
  }

  cgc_compiler* out = cgc_calloc(sizeof(cgc_compiler));
  cgc_memcpy(out, &after_conclusion.clr, sizeof(cgc_compiler));
  
  return out;
}

cgc_compiler_state cgc_compile_expression(cgc_compiler_state in) {
  cgc_stack_elem* add_stack = cgc_stack_create();
  cgc_stack_elem* mul_stack = cgc_stack_create();

  cgc_compiler_state out = in;
  
  cgc_uint8 expect_operand = 1;

  cgc_lexeme* current_lexeme = out.remaining->content;
  cgc_lexeme_flavor current_flavor = current_lexeme->flavor;
  
  while (1) {
      
    if (expect_operand && (F_INTEGER_LITERAL == current_flavor)) {
      out.last_op->type = OP_PUSH_SINT32;
      out.last_op->sint32_value =
        cgc_strtol(current_lexeme->bytes, NULL, 10);

      out.last_op->next = cgc_calloc(sizeof(cgc_operation));
      out.last_op = out.last_op->next;

      if (!cgc_stack_empty(mul_stack)) {
        out.last_op->type = cgc_stack_pop_operation_type(mul_stack);
        out.last_op->next = cgc_calloc(sizeof(cgc_operation));
        out.last_op = out.last_op->next;
      }

      expect_operand = 0;
    }
    else if (expect_operand && (F_CHARACTER_LITERAL == current_flavor)) {
      out.last_op->type = OP_PUSH_CHARACTER_LITERAL;
      out.last_op->character_literal_length = current_lexeme->bytes_len;
      out.last_op->character_literal_value = current_lexeme->bytes;

      out.last_op->next = cgc_calloc(sizeof(cgc_operation));
      out.last_op = out.last_op->next;

      if (!cgc_stack_empty(mul_stack)) {
        out.last_op->type = cgc_stack_pop_operation_type(mul_stack);
        out.last_op->next = cgc_calloc(sizeof(cgc_operation));
        out.last_op = out.last_op->next;
      }
    }
    else if (expect_operand && (F_IDENTIFIER == current_flavor)) {
      out = cgc_compile_function(out);
    }
    else if (expect_operand) {
      out.error_lexeme = current_lexeme;
      return out;
    }
    // not expecting operand
    else if ((F_PLUS == current_flavor) || (F_MINUS == current_flavor)) {
      cgc_operation_type op_type =
        (current_flavor == F_PLUS) ? OP_ADD : OP_SUBTRACT;

      if (!cgc_stack_empty(add_stack)) {
        out.last_op->type = cgc_stack_pop_operation_type(add_stack);
        out.last_op->next = cgc_calloc(sizeof(cgc_operation));
        out.last_op = out.last_op->next;
      }
      
      cgc_stack_push_operation_type(add_stack, op_type);
      expect_operand = 1;
    }
    else if ((F_ASTERISK == current_flavor) || (F_SOLIDUS == current_flavor)) {
      cgc_operation_type op_type =
        (current_flavor == F_ASTERISK) ? OP_MULTIPLY : OP_DIVIDE;
      
      cgc_stack_push_operation_type(mul_stack, op_type);
      expect_operand = 1;
    }
    else {
      out.error_lexeme = current_lexeme;
      return out;
    }

    cgc_lexer_list* next = out.remaining->next;
    
    current_lexeme = next->content;
    current_flavor = current_lexeme->flavor;

    out.remaining = next;
    out.current_position++;

    
    if ((current_flavor == F_FROM) ||
        (current_flavor == F_COMMA) ||
        (current_flavor == F_CLOSE_PAREN)) {
      break;
    }
  }
  
  while (!cgc_stack_empty(add_stack)) {
    out.last_op->type = cgc_stack_pop_operation_type(add_stack);
    out.last_op->next = cgc_calloc(sizeof(cgc_operation));
    out.last_op = out.last_op->next;
  }
  
  return out;
}

cgc_compiler_state cgc_compile_function(cgc_compiler_state in) {
  cgc_compiler_state out = in;
  cgc_lexeme* func_name = out.remaining->content;
  out.remaining = out.remaining->next;

  if (F_OPEN_PAREN != out.remaining->content->flavor) {
    out.error_lexeme = out.remaining->content;
    return out;
  }

  out.remaining = out.remaining->next;

  while (F_CLOSE_PAREN != out.remaining->content->flavor) {
    out = cgc_compile_expression(out);
    if (out.error_lexeme) return out;
    if (F_CLOSE_PAREN == out.remaining->content->flavor) break;
    out.remaining = out.remaining->next;
  }

  out.last_op->type = OP_FUNCALL;
  out.last_op->function_name = func_name->bytes;

  out.last_op->next = cgc_calloc(sizeof(cgc_operation));
  out.last_op = out.last_op->next;
  
  return out;
}

cgc_compiler_state cgc_compile_preamble(cgc_compiler_state in) {
  cgc_compiler_state out = in;
  cgc_lexeme* current_lexeme = in.remaining->content;

  if (F_FIND != current_lexeme->flavor) {
    out.error_lexeme = current_lexeme;
  }

  out.remaining = out.remaining->next;
  
  return out;
}

cgc_compiler_state cgc_compile_conclusion(cgc_compiler_state in) {
  cgc_compiler_state out = in;
  if (F_FROM != out.remaining->content->flavor) {
    out.error_lexeme = out.remaining->content;
    return out;
  }

  out.remaining = out.remaining->next;
  out.clr.relation_name = out.remaining->content->bytes;

  return out;
}

cgc_compiler* cgc_compiler_error(cgc_uint64 position, cgc_lexeme* cgc_lexeme) {
  cgc_compiler* error_clr = cgc_calloc(sizeof(cgc_compiler));
  error_clr->error_position = position;
  error_clr->error_lexeme = cgc_lexeme;
  return error_clr;
}

void cgc_operation_free(cgc_operation* op) {
  if (NULL == op) return;
  cgc_operation* next = op->next;
  cgc_free(op);
  return cgc_operation_free(next);
}

void cgc_compiler_free(cgc_compiler* cgc_compiler) {
  cgc_operation_free(cgc_compiler->find_list);
  cgc_free(cgc_compiler->relation_name);
  cgc_free(cgc_compiler);
}

cgc_compiler* cgc_compile_str(char* str) {
  cgc_lexer_list* ll = cgc_lex_string(cgc_strlen(str), str);
  return cgc_compile(ll);
}

cgc_operation* cgc_clr_chase(cgc_operation* op, cgc_uint16 count) {
  if (count == 0) return op;
  return cgc_clr_chase(op->next, count - 1);
}

void cgc_assert_operation(cgc_operation_type expected_type,
                      cgc_sint32 expected_value,
                      cgc_operation* actual_operation) {
  if (expected_type != actual_operation->type) _terminate(-1);
  if (OP_PUSH_SINT32 != actual_operation->type) return;
  if (expected_value != actual_operation->sint32_value) _terminate(-1);
}

#define assert_op(type, value, count) cgc_assert_operation(type, value, cgc_clr_chase(clr->find_list, count));

void cgc_compiler_test() {
  cgc_compiler* clr = cgc_compile_str("FIND 2 2 FROM error");

  if (NULL == clr) _terminate(-1);
  if (NULL == clr->error_lexeme) _terminate(-1);

  clr = cgc_compile_str("FIND 1 + 2 FROM dual");

  if (NULL == clr) _terminate(-1);
  if (NULL != clr->error_lexeme) _terminate(-1);

  assert_op(OP_PUSH_SINT32, 1, 0);
  assert_op(OP_PUSH_SINT32, 2, 1);
  assert_op(OP_ADD, 0, 2);

  clr = cgc_compile_str("FIND 1 + 2 * 3 - 4 FROM dual");

  if (NULL == clr) _terminate(-1);
  if (NULL != clr->error_lexeme) _terminate(-1);

  assert_op(OP_PUSH_SINT32, 1, 0);
  assert_op(OP_PUSH_SINT32, 2, 1);
  assert_op(OP_PUSH_SINT32, 3, 2);
  assert_op(OP_MULTIPLY, 0, 3);
  assert_op(OP_ADD, 0, 4);
  assert_op(OP_PUSH_SINT32, 4, 5);
  assert_op(OP_SUBTRACT, 0, 6);

  clr = cgc_compile_str("FIND func('abc', 1, 2) FROM cool123");

  if (NULL == clr) _terminate(-1);
  if (NULL != clr->error_lexeme) _terminate(-1);

  assert_op(OP_PUSH_CHARACTER_LITERAL, 0, 0);
  assert_op(OP_PUSH_SINT32, 1, 1);
  assert_op(OP_PUSH_SINT32, 2, 2);
  assert_op(OP_FUNCALL, 0, 3);
}
