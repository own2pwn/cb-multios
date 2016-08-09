/*
 * Copyright (c) 2014 Kaprica Security, Inc.
 *
 * Permission is hereby granted, cgc_free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */
#include <stdlib.h>
#include <string.h>

#include "stack.h"

char *cgc_peek_top(cgc_stack_t *stack)
{
    if(stack == NULL)
        return NULL;

    return stack->data;
}

int cgc_push(cgc_stack_t **stack, char *data)
{
    if (data == NULL)
        return -1;

    cgc_stack_t *top = cgc_malloc(sizeof(cgc_stack_t));
    top->data = data;

    if(*stack == NULL) {
        top->next = NULL;
        *stack = top;
    } else {
        top->next = *stack;
        *stack = top;
    }

    return 0;
}

int cgc_push_copy(cgc_stack_t **stack, char *data, size_t size)
{
    if (data == NULL)
        return -1;

    int data_len = cgc_strlen(data) + 1;
    if (data_len > size)
        return -1;

    cgc_stack_t *top = cgc_malloc(sizeof(cgc_stack_t));
    top->data = cgc_malloc(data_len);
    cgc_memcpy(top->data, data, data_len);

    if(*stack == NULL) {
        top->next = NULL;
        *stack = top;
    } else {
        top->next = *stack;
        *stack = top;
    }

    return 0;
}

char *cgc_pop_copy(cgc_stack_t **stack)
{
    if(*stack == NULL)
        return NULL;

    cgc_stack_t *top = *stack;
    char *popped_data = top->data;
    *stack = top->next;
    cgc_free(top);

    return popped_data;
}


void cgc_clear_stack(cgc_stack_t **stack)
{
    if (*stack == NULL)
        return;

    cgc_stack_t *top = *stack, *old_top;
    while(top->next != NULL) {
        cgc_free(top->data);
        old_top = top;
        top = top->next;
        cgc_free(old_top);
    }
    cgc_free(top->data);
    cgc_free(top);
    *stack = NULL;
}
