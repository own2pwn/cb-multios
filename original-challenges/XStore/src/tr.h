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

#ifndef TR_H
#define TR_H
#include <stdio.h>

typedef void* cgc_T;
typedef struct tr_node {
    char *key;
    unsigned int prio;
    cgc_T value;
    struct tr_node *left, *right;
} cgc_tr_node_t;
typedef cgc_tr_node_t* cgc_tr_t;
typedef void (*cgc_tr_destroy_value) (cgc_T value);
typedef void (*cgc_tr_apply_fn) (cgc_tr_t root, void *arg);

extern cgc_tr_destroy_value destroy_value_fn;

cgc_tr_t cgc_tr_insert(cgc_tr_t root, char *key, cgc_T value);
cgc_tr_t cgc_tr_delete(cgc_tr_t root, char *key);
cgc_T cgc_tr_find(cgc_tr_t root, char *key);
void cgc_tr_destroy(cgc_tr_t root);

void cgc_tr_apply(cgc_tr_t root, cgc_tr_apply_fn apply, void *arg);
void cgc_tr_print(cgc_tr_t root);

#endif
