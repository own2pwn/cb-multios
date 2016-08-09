/*
 * Copyright (c) 2015 Kaprica Security, Inc.
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
#ifndef LIST_H_
#define LIST_H_

#include <libcgc.h>
#include "bool.h"

typedef struct list cgc_list_t;
struct list
{
    cgc_list_t *next;
    void *item;
};

cgc_bool cgc_insert(cgc_list_t **list, void *item);
cgc_bool cgc_insert_in_order(cgc_list_t **list, void *item, int (*compare)(void *a, void *b));
cgc_bool cgc_append(cgc_list_t **list, void *item);
void *cgc_popfront(cgc_list_t **list);
void *cgc_popback(cgc_list_t **list);
void *cgc_find(cgc_list_t *list, void *item, int (*compare)(void *a, void *b));
void *cgc_pop(cgc_list_t **list, void *item, int (*compare)(void *a, void *b));
void cgc_print(cgc_list_t *list, void (*print_item)(void *item));

#endif
