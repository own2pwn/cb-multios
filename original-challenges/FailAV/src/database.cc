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
#include <cstdlib.h>
#include <cstring.h>

#include "database.h"

cgc_Database::cgc_Database()
{
    d_prefix_root = new cgc_Database::cgc_PrefixNode();
    cgc_rebuild_fsm();
}

cgc_Database::~cgc_Database()
{
    d_prefix_root->~cgc_PrefixNode();
    cgc_free(d_prefix_root);
}

void cgc_Database::cgc_PrefixNode::cgc_add(cgc_Database::cgc_PrefixNode **root, const unsigned char *word, unsigned int len)
{
    cgc_PrefixNode *rnode = *root;
    int idx = rnode->cgc_get_next_idx(word[0]);
    if (idx < 0)
    {
        // expand root
        *root = rnode = static_cast<cgc_PrefixNode *>(cgc_realloc(rnode, cgc_get_alloc_size(rnode->d_num_edges + 1)));
        if (rnode == NULL)
            _terminate(1);
        rnode->d_num_edges++;

        // allocate node
        idx = rnode->d_num_edges - 1;
        rnode->d_next[idx] = (cgc_PrefixNode *) cgc_malloc(cgc_get_alloc_size(0));
cgc_PrefixNode *tmp = new cgc_PrefixNode(len == 1, word[0], 0);
cgc_memcpy(rnode->d_next[idx], tmp, cgc_get_alloc_size(0));
delete tmp;

    }
    else
    {
        if (len == 1)
            rnode->d_next[idx]->d_word = 1;
    }

    if (len > 1)
        cgc_add(&rnode->d_next[idx], word + 1, len - 1);
}

void cgc_Database::cgc_PrefixNode::cgc_remove(cgc_Database::cgc_PrefixNode **root, const unsigned char *word, unsigned int len)
{
    cgc_PrefixNode *rnode = *root;
    int idx = rnode->cgc_get_next_idx(word[0]);
    if (idx < 0)
        return;

    if (len == 1)
    {
        rnode->d_next[idx]->d_word = 0;
    }
    else
    {
        cgc_remove(&rnode->d_next[idx], word + 1, len - 1);
    }

    if (rnode->d_next[idx]->d_num_edges == 0 && !rnode->d_next[idx]->d_word)
    {
        // cgc_remove from tree
        cgc_PrefixNode *node = rnode->d_next[idx];
        if (rnode->d_num_edges > 1 && idx < rnode->d_num_edges-1)
            cgc_memmove(&rnode->d_next[idx], &rnode->d_next[idx+1], (rnode->d_num_edges - idx - 1) * sizeof(cgc_PrefixNode *));
        rnode->d_num_edges--;

        // deallocate
        node->~cgc_PrefixNode();
        cgc_free(node);
    }
}

template <typename F>
void cgc_Database::cgc_PrefixNode::cgc_traverse_tree(cgc_Database::cgc_PrefixNode *root, F f)
{
    f(root);
    for (unsigned int i = 0; i < root->d_num_edges; i++)
    {
        cgc_traverse_tree(root->d_next[i], f);
    }
}

void cgc_Database::cgc_add(const unsigned char *word, unsigned int len)
{
    cgc_Database::cgc_PrefixNode::cgc_add(&d_prefix_root, word, len);
    cgc_rebuild_fsm();
}

void cgc_Database::cgc_remove(const unsigned char *word, unsigned int len)
{
    cgc_Database::cgc_PrefixNode::cgc_remove(&d_prefix_root, word, len);
    cgc_rebuild_fsm();
}

void cgc_Database::cgc_rebuild_fsm()
{
    cgc_Database::cgc_PrefixNode *qhead = NULL, *qtail = NULL;

    auto qpush = [&qhead, &qtail] (cgc_Database::cgc_PrefixNode *node) {
        if (qhead == NULL) qhead = node;
        if (qtail != NULL) qtail->d_tmp = node;
        qtail = node;
    };
    auto qpop = [&qhead, &qtail] () -> cgc_Database::cgc_PrefixNode * {
        cgc_Database::cgc_PrefixNode *node = qhead;
        if (node == NULL) return NULL;
        qhead = node->d_tmp;
        if (qhead == NULL) qtail = NULL;
        return node;
    };

    // traverse tree and reset
    cgc_Database::cgc_PrefixNode::cgc_traverse_tree(d_prefix_root, [](cgc_Database::cgc_PrefixNode *node) {
        node->d_fail = NULL;
        node->d_tmp = NULL;
    });

    // queue root children
    for (unsigned int i = 0; i < d_prefix_root->d_num_edges; i++)
    {
        cgc_Database::cgc_PrefixNode *node = d_prefix_root->d_next[i];
        qpush(node);
        node->d_fail = d_prefix_root;
    }

    // traverse queue
    while (cgc_Database::cgc_PrefixNode *node = qpop())
    {
        for (unsigned int i = 0; i < node->d_num_edges; i++)
        {
            cgc_Database::cgc_PrefixNode *next = node->d_next[i], *fail = node->d_fail, *v;
            qpush(next);
            
            while ((v = fail->cgc_get_next(next->d_value)) == NULL)
                fail = fail->d_fail;

            next->d_fail = v;
        }
    }
}

bool cgc_Database::cgc_query(const unsigned char *haystack, unsigned int len) const
{
    cgc_Database::cgc_PrefixNode *q = d_prefix_root;
    for (unsigned int i = 0; i < len; i++)
    {
        cgc_Database::cgc_PrefixNode *v;
        while ((v = q->cgc_get_next(haystack[i])) == NULL)
            q = q->d_fail;
        q = v;
        if (q->d_word)
            return true;
    }
    return false;
}
