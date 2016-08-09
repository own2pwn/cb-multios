/*
 * Copyright (C) Narf Industries <info@narfindustries.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "pool.h"
#include "stdlib.h"
#include "string.h"

#include "tokenizer.h"

#include "parser.h"

static int
cgc_stack_empty(struct ast_node **stack)
{
    return *stack == NULL;
}

static void
cgc_push_ast_node(struct ast_node **stack, struct ast_node *new)
{
    new->next = *stack;
    *stack = new;
}

static int
cgc_pop_ast_node(struct ast_node **stack, struct ast_node **out)
{
    if (cgc_stack_empty(stack))
        return EXIT_FAILURE;

    *out = *stack;
    *stack = (*stack)->next;
    return EXIT_SUCCESS;
}

static int
cgc_peek_ast_node(struct ast_node **stack, struct ast_node **out)
{
    if (cgc_stack_empty(stack))
        return EXIT_FAILURE;

    *out = *stack;
    return EXIT_SUCCESS;
}

static int  
cgc_push_bin_op(struct ast_node **output_stack, struct ast_node **operator_stack,
        struct ast_node *new)
{
    static const int bin_op_precedence[] = { 0, 1, 1, 2, 2 };
    struct ast_node *cur_operator, *lhs, *rhs, *n;

    while (!cgc_stack_empty(operator_stack)) {
        if (cgc_peek_ast_node(operator_stack, &cur_operator) < 0)
            return EXIT_FAILURE;

        if (cur_operator->type == AST_BINARY_OPERATOR) {
            // All binary operators are left-associative
            if (bin_op_precedence[new->expr.bin_op.type] <=
                    bin_op_precedence[cur_operator->expr.bin_op.type]) {
                if (cgc_pop_ast_node(operator_stack, &cur_operator) < 0)
                    return EXIT_FAILURE;

                if (cgc_pop_ast_node(output_stack, &rhs) < 0)
                    return EXIT_FAILURE;
                if (cgc_pop_ast_node(output_stack, &lhs) < 0)
                    return EXIT_FAILURE;

                // Can only assign to l-values
                if (cur_operator->expr.bin_op.type == BIN_ASSIGNMENT &&
                        (lhs->type != AST_VARIABLE &&
                            (lhs->type != AST_UNARY_OPERATOR ||
                             lhs->expr.bin_op.type != UN_DEREFERENCE))) {
                    return EXIT_FAILURE;
                }

                cur_operator->expr.bin_op.lhs = lhs;
                cur_operator->expr.bin_op.rhs = rhs;

                cgc_push_ast_node(output_stack, cur_operator);
                continue;
            } 
        } else if (cur_operator->type == AST_UNARY_OPERATOR) {
            // All unary operators have higher precedence than all binary operators
            if (cgc_pop_ast_node(operator_stack, &cur_operator) < 0)
                return EXIT_FAILURE;

            if (cgc_pop_ast_node(output_stack, &n) < 0)
                return EXIT_FAILURE;

            // Can only take address of l-value
            if (cur_operator->expr.un_op.type == UN_ADDRESS_OF &&
                    n->type != AST_VARIABLE)
                return EXIT_FAILURE;

            // Can only dereference variables
            if (cur_operator->expr.un_op.type == UN_DEREFERENCE &&
                    (n->type != AST_VARIABLE && n->type != AST_UNARY_OPERATOR))
                return EXIT_FAILURE;

            cur_operator->expr.un_op.n = n;

            cgc_push_ast_node(output_stack, cur_operator);
            continue;
        }

        break;
    }

    cgc_push_ast_node(operator_stack, new);
    return EXIT_SUCCESS;
}

static int
cgc_close_parenthesis(struct ast_node **output_stack, struct ast_node **operator_stack)
{
    struct ast_node *cur_operator, *lhs, *rhs, *n;
    while (!cgc_stack_empty(operator_stack)) {
        if (cgc_pop_ast_node(operator_stack, &cur_operator) < 0)
            return EXIT_FAILURE;

        if (cur_operator->type == AST_PARENTHESIS)
            return EXIT_SUCCESS;

        if (cur_operator->type == AST_BINARY_OPERATOR) {
            if (cgc_pop_ast_node(output_stack, &rhs) < 0)
                return EXIT_FAILURE;
            if (cgc_pop_ast_node(output_stack, &lhs) < 0)
                return EXIT_FAILURE;

            // Can only assign to l-values
            if (cur_operator->expr.bin_op.type == BIN_ASSIGNMENT &&
                    (lhs->type != AST_VARIABLE &&
                        (lhs->type != AST_UNARY_OPERATOR ||
                         lhs->expr.bin_op.type != UN_DEREFERENCE))) {
                return EXIT_FAILURE;
            }

            cur_operator->expr.bin_op.lhs = lhs;
            cur_operator->expr.bin_op.rhs = rhs;

            cgc_push_ast_node(output_stack, cur_operator);
        } else if (cur_operator->type == AST_UNARY_OPERATOR) {
            if (cgc_pop_ast_node(output_stack, &n) < 0)
                return EXIT_FAILURE;

            // Can only take address of l-value
            if (cur_operator->expr.un_op.type == UN_ADDRESS_OF &&
                    n->type != AST_VARIABLE)
                return EXIT_FAILURE;

            // Can only dereference variables
            if (cur_operator->expr.un_op.type == UN_DEREFERENCE &&
                    (n->type != AST_VARIABLE && n->type != AST_UNARY_OPERATOR))
                return EXIT_FAILURE;

            cur_operator->expr.un_op.n = n;

            cgc_push_ast_node(output_stack, cur_operator);
        }
    }

    return EXIT_FAILURE;
}

static int
cgc_clear_operator_stack(struct ast_node **output_stack, struct ast_node **operator_stack)
{
    return cgc_close_parenthesis(output_stack, operator_stack) == EXIT_FAILURE ?
        EXIT_SUCCESS : EXIT_FAILURE;
}

/* See https://en.wikipedia.org/wiki/Shunting-yard_algorithm */
int
cgc_parse(struct token *tokens, cgc_size_t n, struct ast *out)
{
    cgc_size_t i;
    struct ast_node *cur_node, *output_stack = NULL, *operator_stack = NULL;

    if (cgc_pool_init(&out->pool, sizeof(struct ast_node)) < 0)
        return EXIT_FAILURE;

    for (i = 0; i < n; i++) {
        if ((cur_node = cgc_pool_alloc(&out->pool)) == NULL)
            return EXIT_FAILURE;

        cgc_memset(cur_node, '\0', sizeof(struct ast_node));

        switch (tokens[i].type) {
        case TOK_CONSTANT:
            cur_node->type = AST_CONSTANT;
            cur_node->expr.constant = tokens[i].val.i;
            cgc_push_ast_node(&output_stack, cur_node);
            break;
        case TOK_VARIABLE:
            cur_node->type = AST_VARIABLE;
            cgc_memset(cur_node->expr.variable, '\0', 4);
            cgc_strncpy(cur_node->expr.variable, tokens[i].val.s, 4);
            cgc_push_ast_node(&output_stack, cur_node);
            break;
        case TOK_ASSIGNMENT:
            cur_node->type = AST_BINARY_OPERATOR;
            cur_node->expr.bin_op.type = BIN_ASSIGNMENT;
            if (cgc_push_bin_op(&output_stack, &operator_stack, cur_node) < 0)
                return EXIT_FAILURE;
            break;
        case TOK_ADD:
            cur_node->type = AST_BINARY_OPERATOR;
            cur_node->expr.bin_op.type = BIN_ADD;
            if (cgc_push_bin_op(&output_stack, &operator_stack, cur_node) < 0)
                return EXIT_FAILURE;
            break;
        case TOK_SUBTRACT:
            cur_node->type = AST_BINARY_OPERATOR;
            cur_node->expr.bin_op.type = BIN_SUBTRACT;
            if (cgc_push_bin_op(&output_stack, &operator_stack, cur_node) < 0)
                return EXIT_FAILURE;
            break;
        case TOK_MULTIPLY:
            cur_node->type = AST_BINARY_OPERATOR;
            cur_node->expr.bin_op.type = BIN_MULTIPLY;
            if (cgc_push_bin_op(&output_stack, &operator_stack, cur_node) < 0)
                return EXIT_FAILURE;
            break;
        case TOK_DIVIDE:
            cur_node->type = AST_BINARY_OPERATOR;
            cur_node->expr.bin_op.type = BIN_DIVIDE;
            if (cgc_push_bin_op(&output_stack, &operator_stack, cur_node) < 0)
                return EXIT_FAILURE;
            break;
        case TOK_NEGATE:
            cur_node->type = AST_UNARY_OPERATOR;
            cur_node->expr.un_op.type = UN_NEGATE;
            cgc_push_ast_node(&operator_stack, cur_node);
            break;
        case TOK_ADDRESS_OF:
            cur_node->type = AST_UNARY_OPERATOR;
            cur_node->expr.un_op.type = UN_ADDRESS_OF;
            cgc_push_ast_node(&operator_stack, cur_node);
            break;
        case TOK_DEREFERENCE:
            cur_node->type = AST_UNARY_OPERATOR;
            cur_node->expr.un_op.type = UN_DEREFERENCE;
            cgc_push_ast_node(&operator_stack, cur_node);
            break;
        case TOK_LEFT_PARENTHESIS:
            cur_node->type = AST_PARENTHESIS;
            cgc_push_ast_node(&operator_stack, cur_node);
            break;
        case TOK_RIGHT_PARENTHESIS:
            if (cgc_close_parenthesis(&output_stack, &operator_stack) < 0)
                return EXIT_FAILURE;
            break;
        }
    }

    if (cgc_clear_operator_stack(&output_stack, &operator_stack) < 0)
        return EXIT_FAILURE;

    if (cgc_pop_ast_node(&output_stack, &out->expr) < 0)
        return EXIT_FAILURE;

    if (!cgc_stack_empty(&output_stack))
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

void
cgc_ast_destroy(struct ast *ast)
{
    cgc_pool_destroy(&ast->pool);
    ast->expr = NULL;
}

