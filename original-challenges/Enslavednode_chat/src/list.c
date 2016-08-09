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

#include <libcgc.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "error.h"
#include "list.h"

cgc_list *cgc_init_list(const DATA d)
{
  cgc_list *new = cgc_calloc(1, sizeof(cgc_list));
  if (!new)
    error(EALLOC);
  new->d = d;
  return new;
}

void cgc_append_list(cgc_list **l, const DATA d, int dups)
{
  if (!*l) {
    *l = cgc_init_list(d);
    return;
  }


  cgc_list *i = *l;
  if (!dups) {
    while (i) {
      if (!cgc_strcmp((char *)d, (char *)i->d))
        return;
      i = i->n;
    }
  }

  cgc_list *new = cgc_init_list(d);

  i = *l;
  while (i->n)
    i = i->n;
  i->n = new;
  new->p = i;
  new->n = NULL;
  return;
}

cgc_list *cgc_concat_list(cgc_list *l1, cgc_list *l2)
{
  cgc_list *l = NULL;

  if (!l1)
    return l2;

  if (!l2)
    return l1;

  l = l1;
  while (l2) {
    cgc_append_list(&l1, l2->d, 1);
    l2 = l2->n;
  }

  return l;
}


size_t cgc_len_list(const cgc_list *l)
{
  if (!l)
    return 0;

  size_t s = 0;

  while (l)
    s++, l = l->n;

  return s;
}

const DATA cgc_lindex(const cgc_list *l, cgc_ssize_t i)
{
  if (!l)
    return NULL;

  if (i < 0) {
    size_t t = cgc_len_list(l);
    while (i < 0)
      t--, i++;
    i = t;
  }

  if (i >= cgc_len_list(l))
    return NULL;

  for (size_t k = 0; k < i; k++)
    l = l->n;

  return l->d;
}



cgc_list *cgc_copy_list(const cgc_list *l, size_t start, size_t stop)
{
  if (!l)
    return NULL;

  size_t len = cgc_len_list(l);

  if (stop == 0 || stop > len)
    stop = len;

  if (start >= stop)
    return NULL;

  cgc_list *p = (cgc_list *)l;
  cgc_list *copy = NULL;

  // Get to start
  for (size_t i = 0; i < start; i++)
    p = p->n;

  // Copy from start to stop
  for (size_t i = start; i < stop; i++, p = p->n)
    cgc_append_list(&copy, p->d, 1);

  return copy;
}

void cgc_free_list(const cgc_list *l, int leave_data)
{
  const cgc_list *it;
  for (it = l; it; it = it->n) {
    if (!leave_data)
      cgc_free((void *)it->d);
    if (it->p)
      cgc_free(it->p);
  }
  cgc_free((void *)it);
}

void cgc_free_list_of_lists(const cgc_list *l, int leave_data)
{
  const cgc_list *it;
  for (it = l; it; it = it->n) {
    cgc_free_list((cgc_list *)l->d, leave_data - 1);
    if (it->p)
      cgc_free(it->p);
  }
  cgc_free((void *)it);
}

const void *cgc_random_element(cgc_list *l)
{
  size_t i, x, len = cgc_len_list(l);
  if (cgc_random(&i, sizeof(size_t), &x) < 0)
    error(ERAND);

  if (x != sizeof(size_t))
    return NULL;
  i %= len;
  return cgc_lindex(l, i);
}
