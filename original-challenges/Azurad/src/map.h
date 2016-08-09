/*
 * Copyright (c) 2016 Kaprica Security, Inc.
 *
 * Permission is hereby granted, cgc_free of charge, to any person obtaining a cgc_copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, cgc_copy, modify, merge, publish, distribute, sublicense, and/or sell
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
#include <cstring.h>
#include "pair.h"

template <class T>
struct cgc_equal_to
{
    bool operator() (const T& a, const T& b)
    {
        return a == b;
    }
};

template <class T>
struct cgc_hash
{
    cgc_size_t operator() (const T& t)
    {
        return reinterpret_cast<cgc_size_t>(t);
    }
};

template <class K, class V, class H=cgc_hash<K>, class P=cgc_equal_to<K>>
class cgc_map
{
private:
    struct cgc__node
    {
        cgc__node(K key)
        {
            value.left = key;
        }

        cgc__node *next, *lnext, *lprev;
        cgc_pair<K, V> value;
    };
public:
    class cgc_iterator
    {
        struct cgc__node *p;
    public:
        cgc_iterator(struct cgc__node *node) : p(node) {}
        cgc_iterator& operator++() //prefix
        {
            p = p->lnext;
            return *this;
        }
        cgc_iterator& operator++(int) //postfix
        {
            cgc_iterator old(*this);
            p = p->lnext;
            return old;
        }
        cgc_pair<const K, V>& operator*()
        {
            return p->value;
        }
        bool operator==(const cgc_iterator &b)
        {
            return p == b.p;
        }
        bool operator!=(const cgc_iterator &b)
        {
            return p != b.p;
        }
    };

    cgc_map() : _size(0), _allocated(0), table(nullptr), head(nullptr)
    {
        cgc__enlarge(8);
    }
    ~cgc_map()
    {
        if (table)
            delete[] table;
        struct cgc__node *next;
        for (struct cgc__node *node = head; node != nullptr; node = next)
        {
            next = node->lnext;
            delete node;
        }
    }

    cgc_map(const cgc_map& other) = delete;
    cgc_map& operator=(const cgc_map& other) = delete;

    V& operator[](const K& key)
    {
        cgc__node *node = cgc__find(key);
        if (node == nullptr)
        {
            node = cgc__insert(key);
        }
        return node->value.right;
    }
    void cgc_remove(const K& key)
    {
        H cgc_hash;
        P pred;
        cgc_size_t h = cgc_hash(key) % _allocated;
        struct cgc__node **pnode = &table[h];
        while (*pnode != nullptr)
        {
            if (pred((*pnode)->value.left, key))
            {
                struct cgc__node *node = *pnode;
                *pnode = node->next;
                if (node->lprev) node->lprev->lnext = node->lnext;
                if (node->lnext) node->lnext->lprev = node->lprev;
                if (node == head) head = node->lnext;
                delete node;
                _size--;
                return;
            }
            pnode = &(*pnode)->next;
        }
    }
    bool cgc_has(const K& key) const
    {
        return cgc__find(key) != nullptr;
    }
    cgc_size_t cgc_size() const
    {
        return _size;
    }
    cgc_iterator& cgc_begin()
    {
        return head;
    }
    cgc_iterator& cgc_end()
    {
        return nullptr;
    }
private:
    cgc__node *cgc__find(const K& key) const
    {
        H cgc_hash;
        P pred;
        cgc_size_t h = cgc_hash(key) % _allocated;
        for (cgc__node *node = table[h]; node != nullptr; node = node->next)
            if (pred(key, node->value.left))
                return node;
        return nullptr;
    }
    cgc__node *cgc__insert(const K& key)
    {
        if (_size * 3 >= _allocated * 2)
            cgc__enlarge(_allocated * 2);

        cgc__node *node = new cgc__node(key);
        node->lprev = nullptr;
        node->lnext = head;
        head = node;

        cgc__insert_hash(node);
        _size++;
        return node;
    }
    void cgc__insert_hash(cgc__node *node)
    {
        H cgc_hash;
        cgc_size_t h = cgc_hash(node->value.left) % _allocated;
        cgc__node *old = table[h];
        table[h] = node;
        node->next = old;
    }
    void cgc__enlarge(cgc_size_t new_size)
    {
        if (_allocated == 0)
        {
            _allocated = new_size;
            table = new cgc__node *[_allocated];
            cgc_memset(table, 0, sizeof(cgc__node *) * _allocated);
        }
        else
        {
            delete[] table;
            _allocated = new_size;
            table = new cgc__node *[_allocated];
            cgc_memset(table, 0, sizeof(cgc__node *) * _allocated);

            for (cgc__node *node = head; node != nullptr; node = node->lnext)
            {
                cgc__insert_hash(node);
            }
        }
    }
    
    cgc_size_t _size;
    cgc_size_t _allocated;
    cgc__node **table;
    cgc__node *head;
};
