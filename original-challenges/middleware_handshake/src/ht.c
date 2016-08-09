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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "common.h"
#include "ht.h"

#define INITIAL_COUNT 4
#define MAX_COUNT 8192
#define THRESHOLD (0.5)

static unsigned int cgc__hash(unsigned int size, const char *k)
{
    unsigned int result = 0x3505;
    size_t i;
    for (i = 0; k[i]; i++)
        result = result * 37 + cgc_tolower(k[i]);
    return result % size;
}

static int cgc__do_insert(struct cgc_ht_node_t **tbl, unsigned int size, struct cgc_ht_node_t *node)
{
    unsigned int idx = cgc__hash(size, node->k), i;
    // linear probing
    for (i = idx; tbl[i] != NULL; i = (i + 1) & (size-1))
        if (i == ((idx - 1) & (size-1)))
            return FAILURE;

    tbl[i] = node;
    return SUCCESS;
}

static int cgc__ht_resize(cgc_ht_t *tbl)
{
    unsigned int new_size = tbl->tbl_size * 2;
    struct cgc_ht_node_t **new_entries, *iter;

    // allocate new table
    new_entries = cgc_calloc(new_size, sizeof(cgc_ht_node_t *));
    if (new_entries == NULL)
        return FAILURE;

    // re-insert nodes into new table
    for (iter = tbl->first; iter != NULL; iter = iter->next)
    {
        if (cgc__do_insert(new_entries, new_size, iter) != SUCCESS)
            goto fail;
    }

    cgc_free(tbl->tbl);
    tbl->tbl = new_entries;
    tbl->tbl_size = new_size;
    return SUCCESS;

fail:
    cgc_free(new_entries);
    return FAILURE;
}

int cgc_ht_init(cgc_ht_t *tbl)
{
    cgc_memset(tbl, 0, sizeof(cgc_ht_t));
    tbl->tbl_size = INITIAL_COUNT;
    return cgc__ht_resize(tbl);
}

int cgc_ht_compare(const char *a, const char *b)
{
    return cgc_strcasecmp(a, b);
}

int cgc_ht_lookup(cgc_ht_t *tbl, const char *k, cgc_ht_node_t **node)
{
    unsigned int idx = cgc__hash(tbl->tbl_size, k), i;

    for (i = idx; tbl->tbl[i] != NULL; i = (i + 1) % tbl->tbl_size)
        if (cgc_ht_compare(tbl->tbl[i]->k, k) == 0)
            break;

    if (tbl->tbl[i] == NULL)
        return FAILURE;

    *node = tbl->tbl[i];
    return SUCCESS;
}

int cgc_ht_delete(cgc_ht_t *tbl, const char *k, void **v)
{
    unsigned int idx, i, last;
    cgc_ht_node_t *node = NULL;
    if (cgc_ht_lookup(tbl, k, &node) != SUCCESS)
        return FAILURE;
    
    *v = node->value;

    if (node->prev == NULL)
        tbl->first = node->next;
    else
        node->prev->next = node->next;
    if (node->next)
        node->next->prev = node->prev;

    idx = cgc__hash(tbl->tbl_size, node->k);
    for (i = idx; tbl->tbl[i] != node; i = (i + 1) % tbl->tbl_size) ;

    tbl->tbl[i] = NULL;
    tbl->tbl_count--;

    last = i;
    for (i = (i + tbl->tbl_size - 1) % tbl->tbl_size; tbl->tbl[i] != NULL; i = (i + tbl->tbl_size - 1) % tbl->tbl_size)
        idx = i;
    for (i = (last + 1) % tbl->tbl_size; tbl->tbl[i] != NULL; i = (i + 1) % tbl->tbl_size)
    {
        unsigned int idx2 = cgc__hash(tbl->tbl_size, tbl->tbl[i]->k);
        if ((idx <= last && idx <= idx2 && idx2 <= last) ||
            (idx > last && (idx <= idx2 || idx2 <= last)))
        {
            tbl->tbl[last] = tbl->tbl[i];
            tbl->tbl[i] = NULL;
            last = i;
        }
    }

    cgc_free(node->k);
    cgc_free(node);
    return SUCCESS;
}

int cgc_ht_insert(cgc_ht_t *tbl, const char *k_, void *v)
{
    char *k;
    cgc_ht_node_t *node = NULL;
    if (cgc_ht_lookup(tbl, k_, &node) == SUCCESS)
        return FAILURE;

    if (tbl->tbl_count == MAX_COUNT)
        return FAILURE;

    k = cgc_strdup(k_);
    if (k == NULL)
        return FAILURE;
    node = cgc_malloc(sizeof(cgc_ht_node_t));
    node->k = k;
    node->value = v;

    if (tbl->tbl_count+1 >= tbl->tbl_size * THRESHOLD)
    {
        if (cgc__ht_resize(tbl) != SUCCESS)
            goto fail;
    }

    if (cgc__do_insert(tbl->tbl, tbl->tbl_size, node) != SUCCESS)
        goto fail;

    tbl->tbl_count++;
    node->prev = NULL;
    node->next = tbl->first;
    if (node->next)
        node->next->prev = node;
    tbl->first = node;
    return SUCCESS;

fail:
    cgc_free(k);
    cgc_free(node);
    return FAILURE;
}

cgc_ht_node_t *cgc_ht_first(cgc_ht_t *tbl)
{
    return tbl->first;
}

cgc_ht_node_t *cgc_ht_next(cgc_ht_t *tbl, cgc_ht_node_t *n)
{
    return n->next;
}
