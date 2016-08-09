/*
 * Copyright (c) 2016 Kaprica Security, Inc.
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

#include <cstring.h>
#include "smalloc.h"

template <typename K, typename V, typename C>
class cgc_HashMap
{
private:
    class cgc_Node
    {
    public:
        cgc_Node(K key, V value) : d_value(value), d_key(key) {}
    public:
        V d_value;
        K d_key;
        cgc_Node *d_next;
    };
    void cgc_resize(int new_size)
    {
        cgc_Node **nodes = static_cast<cgc_Node **>(cgc_safe_calloc(new_size, sizeof(cgc_Node *)));
        for (unsigned int i = 0; i < d_size; i++)
        {
            cgc_Node *node, *cgc_next;
            for (node = d_nodes[i]; node != nullptr; node = cgc_next)
            {
                unsigned int idx = C::cgc_hash(node->d_key) % new_size;
                cgc_next = node->d_next;
                node->d_next = nodes[idx];
                nodes[idx] = node;
            }
        }
        d_nodes = nodes;
        d_size = new_size;
    }
    cgc_Node *cgc_find_node(K key, cgc_Node **cgc_parent) const
    {
        if (d_length == 0)
            return nullptr;
        cgc_Node *node = d_nodes[C::cgc_hash(key) % d_size];
        if (cgc_parent != nullptr)
            *cgc_parent = nullptr;
        for (; node != nullptr; node = node->d_next)
        {
            if (C::cgc_equals(key, node->d_key))
                return node;
            if (cgc_parent != nullptr)
                *cgc_parent = node;
        }
        return nullptr;
    }
public:
    cgc_HashMap(int initial_size)
    {
        d_length = 0;
        d_size = initial_size;
        d_nodes = nullptr;
    }
    cgc_HashMap() : cgc_HashMap(DEFAULT_SIZE) {}
    ~cgc_HashMap()
    {
        cgc_free(d_nodes);
    }
    inline unsigned int cgc_length() const
    {
        return d_length;
    }
    inline unsigned int cgc_size() const
    {
        return d_size;
    }
    void cgc_insert(K key, V value)
    {
        d_length++;
        if (d_nodes == nullptr)
        {
            int initial_size = d_size;
            d_size = 0;
            cgc_resize(initial_size);
        }
        if (d_length * 2 > d_size)
            cgc_resize(d_size * 2);

        unsigned int idx = C::cgc_hash(key) % d_size;
        cgc_Node *node = new cgc_Node(key, value);
        node->d_next = d_nodes[idx];
        d_nodes[idx] = node;
    }
    V *cgc_lookup(K key) const
    {
        cgc_Node *node;
        if (d_length == 0)
            return nullptr;
        node = cgc_find_node(key, nullptr);
        return node == nullptr ? nullptr : &node->d_value;
    }
    bool cgc_remove(K key)
    {
        cgc_Node *node, *cgc_parent;
        node = cgc_find_node(key, &cgc_parent);
        if (node == nullptr)
            return false;
        if (cgc_parent == nullptr)
            d_nodes[C::cgc_hash(key) % d_size] = node->d_next;
        else
            cgc_parent->d_next = node->d_next;
        d_length--;
        delete node;
        return true;
    }
    V *cgc_next(V *cgc_prev) const
    {
        unsigned int idx = 0;
        if (d_length == 0)
            return nullptr;
        cgc_Node *node = reinterpret_cast<cgc_Node *>(cgc_prev);
        if (node != nullptr)
        {
            if (node->d_next != nullptr)
                return &node->d_next->d_value;
            idx = (C::cgc_hash(node->d_key) % d_size) + 1;
        }
        for (; idx < d_size; idx++)
        {
            if (d_nodes[idx] != nullptr)
                return &d_nodes[idx]->d_value;
        }
        return nullptr;
    }
private:
    static const int DEFAULT_SIZE = 10;
    unsigned int d_length;
    unsigned int d_size;
    cgc_Node **d_nodes;
};

class CStringComparator
{
public:
    static inline unsigned int cgc_hash(const char *str)
    {
        unsigned int h = 0x14159265;
        for (unsigned int i = 0; str[i] != 0; i++)
            h = (h * 43) ^ str[i];
        return h;
    }
    static inline bool cgc_equals(const char *a, const char *b)
    {
        return cgc_strcmp(a, b) == 0;
    }
};

class UIntComparator
{
public:
    static inline unsigned int cgc_hash(unsigned int x)
    {
        unsigned int h = 0x14159265;
        h *= x;
        h ^= (x >> 16);
        h ^= x;
        return h;
    }
    static inline bool cgc_equals(unsigned int a, unsigned int b)
    {
        return a == b;
    }
};
