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

#ifndef HASHTABLE_H
#define HASHTABLE_H

typedef void (cgc_free_value_fn) (void *e);

typedef struct entry {
  char *key;
  void *val;
  struct entry *next;
} cgc_entry_t;

typedef struct htbl {
  unsigned int size;
  unsigned int count;
  cgc_entry_t **table;
  cgc_entry_t *head;
  cgc_entry_t *tail;
  cgc_free_value_fn *free_value;
} cgc_htbl_t;

cgc_htbl_t* cgc_htbl_create(int size, cgc_free_value_fn *fptr);
int cgc_htbl_put(cgc_htbl_t *tab, char *key, void *val);
void* cgc_htbl_get(cgc_htbl_t *tab, char *key);
void cgc_htbl_destroy(cgc_htbl_t *tab);

#endif
