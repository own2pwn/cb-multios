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
#pragma once

#include <cstdlib.h>

class cgc_Database
{
private:
    class cgc_PrefixNode
    {
    public:
        cgc_PrefixNode() : d_root(1), d_word(0), d_value(0), d_num_edges(0) {}
        cgc_PrefixNode(bool word, unsigned char value, unsigned short num_edges) : d_root(0), d_word(word), d_value(value), d_num_edges(num_edges) {}
        ~cgc_PrefixNode()
        {
            for (unsigned int i = 0; i < d_num_edges; i++)
            {
                d_next[i]->~cgc_PrefixNode();
                cgc_free(d_next[i]);
            }
        }

        cgc_PrefixNode *cgc_get_next(unsigned char ch)
        {
            int i = cgc_get_next_idx(ch);
            if (i >= 0)
                return d_next[i];
            if (d_root)
                return this;
            return NULL;
        }
        int cgc_get_next_idx(unsigned char ch)
        {
            for (unsigned int i = 0; i < d_num_edges; i++)
                if (d_next[i]->d_value == ch)
                    return i;
            return -1;
        }

        static void cgc_add(cgc_PrefixNode **root, const unsigned char *word, unsigned int len);
        static void cgc_remove(cgc_PrefixNode **root, const unsigned char *word, unsigned int len);
        template <typename F>
        static void cgc_traverse_tree(cgc_PrefixNode *root, F f);
        static unsigned int cgc_get_alloc_size(unsigned int len)
        {
            return sizeof(cgc_PrefixNode) + len * sizeof(cgc_PrefixNode *);
        }

        unsigned char d_root : 1;
        unsigned char d_word : 1;
        unsigned char d_value;
        unsigned short d_num_edges;
        cgc_PrefixNode *d_fail;
        cgc_PrefixNode *d_tmp;
        cgc_PrefixNode *d_next[];
    };

    void cgc_rebuild_fsm();
public:
    cgc_Database();
    ~cgc_Database();

    cgc_Database(const cgc_Database &) = delete;
    cgc_Database& operator=(const cgc_Database &) = delete;

    void cgc_add(const unsigned char *word, unsigned int len);
    void cgc_remove(const unsigned char *word, unsigned int len);
    bool cgc_query(const unsigned char *haystack, unsigned int len) const;
private:
    cgc_PrefixNode *d_prefix_root;
};
