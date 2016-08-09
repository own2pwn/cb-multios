/*
 * Copyright (C) Narf Industries <info@narfindustries.com>
 *
 * Permission is hereby granted, cgc_free of charge, to any person obtaining a
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

#ifndef LL_H
#define LL_H
#define ENOTFOUND 15
#define OPOKAY 0

#define HEADALLOCNODENULL 2
#define HEADNODENULL 1
#define UNLINKFAIL 3
#define UNLINKNULL 4
#define LINKFAIL 5
#include <libcgc.h>


typedef struct alloc_node {
    unsigned int refcount;
    struct alloc_node *next;
    void *contents;
} cgc_alloc_node_t;

typedef struct alloc_head {
    cgc_size_t n_nodes;
    cgc_alloc_node_t *n;

} cgc_alloc_head_t;

int cgc_remove_node(cgc_alloc_head_t *head, cgc_alloc_node_t *t);
int cgc_add_node(cgc_alloc_head_t *head, cgc_alloc_node_t *a);
int cgc_add_node_tail(cgc_alloc_head_t *head, cgc_alloc_node_t *a);

cgc_alloc_node_t * cgc_get_node(cgc_alloc_head_t *head, cgc_alloc_node_t *a);
cgc_alloc_head_t * cgc_init_ll();
cgc_alloc_node_t * cgc_get_head(cgc_alloc_head_t *h);
int cgc_count_entries(cgc_alloc_head_t *head);

#endif




