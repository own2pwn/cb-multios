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

template <class T>
class cgc_LinkedList
{
public:
    class cgc_Node
    {
        friend class cgc_LinkedList;
    private:
        inline cgc_Node(T value_) : cgc_next(NULL), cgc_prev(NULL), cgc_value(value_)
        {
        }
    public:
        cgc_Node *cgc_next;
        cgc_Node *cgc_prev;
        T cgc_value;
    };
    class cgc_Iterator
    {
        friend class cgc_LinkedList;
    public:
        T cgc_value() const
        {
            return node->cgc_value;
        }
        void cgc_next()
        {
            node = node->cgc_next;
        }
        void cgc_prev()
        {
            node = node->cgc_prev;
        }
        bool cgc_empty() const
        {
            return node == NULL;
        }
    private:
        inline cgc_Iterator(cgc_Node *node_) : node(node_)
        {
        }

        cgc_Node *node;
    };
    typedef void (*cgc_destructor) (T cgc_value);
    static void cgc_deleteDestructor(T cgc_value)
    {
        delete cgc_value;
    }

    inline cgc_LinkedList(cgc_destructor dtor_) : dtor(dtor_), head(NULL), tail(NULL), length(0)
    {
    }
    inline cgc_LinkedList() : cgc_LinkedList(NULL)
    {
    }
    inline ~cgc_LinkedList()
    {
        cgc_Node *cgc_next;
        for (cgc_Node *node = head; node != NULL; node = cgc_next)
        {
            cgc_next = node->cgc_next;

            if (dtor)
                dtor(node->cgc_value);
            delete node;
        }
    }
    unsigned int cgc_getLength() const
    {
        return length;
    }
    inline void cgc_append(T cgc_value)
    {
        cgc_Node *node = new cgc_Node(cgc_value);
        cgc_insertNode(tail, node);
        length++;
    }
    inline void cgc_prepend(T cgc_value)
    {
        cgc_Node *node = new cgc_Node(cgc_value);
        cgc_insertNode(NULL, node);
        length++;
    }
    inline cgc_Iterator cgc_begin() const
    {
        return cgc_Iterator(head);
    }
    inline cgc_Iterator cgc_end() const
    {
        return cgc_Iterator(tail);
    }
    inline void cgc_removeAt(cgc_Iterator &it)
    {
        if (it.cgc_empty())
            return;

        cgc_Node *node = it.node;
        it.cgc_next();

        cgc_removeNode(node);
        delete node;
        length--;
    }
private:
    inline void cgc_insertNode(cgc_Node *cgc_prev, cgc_Node *newNode)
    {
        if (cgc_prev == NULL)
        {
            newNode->cgc_next = head;
            head = newNode;
        }
        else
        {
            newNode->cgc_next = cgc_prev->cgc_next;
            cgc_prev->cgc_next = newNode;
        }

        newNode->cgc_prev = cgc_prev;
        if (newNode->cgc_next)
            newNode->cgc_next->cgc_prev = newNode;

        if (tail == cgc_prev)
            tail = newNode;
    }
    inline void cgc_removeNode(cgc_Node *node)
    {
        if (node->cgc_next)
            node->cgc_next->cgc_prev = node->cgc_prev;
        if (node->cgc_prev)
            node->cgc_prev->cgc_next = node->cgc_next;
        if (node == head)
            head = node->cgc_next;
        if (node == tail)
            tail = node->cgc_prev;
    }

    cgc_destructor dtor;
    cgc_Node *head;
    cgc_Node *tail;
    unsigned int length;
};
