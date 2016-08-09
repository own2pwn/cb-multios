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

template <typename T>
class cgc_CList
{
    public:
        cgc_CList() : m_head(NULL), m_size(0) {}
        cgc_CList(const cgc_CList<T> & l);
        ~cgc_CList();

        cgc_size_t cgc_GetSize() const { return m_size; }
        bool cgc_IsEmpty() const { return m_size == 0; }
        T cgc_GetAt(int idx) const ;

        void cgc_Append(T item);
        void cgc_InsertAt(T item, int idx);
        T cgc_RemoveAt(int idx);
        void cgc_Remove(T item);
        bool cgc_Has(T item);

    private:
        class cgc_CNode {
            friend class cgc_CList<T>;
            private:
                T m_data;
                cgc_CNode *m_next;
            public:
                cgc_CNode(T data, cgc_CNode *next) : m_data(data), m_next(next) {}
        };

        cgc_CNode *m_head;
        cgc_size_t m_size;
};

template <typename T>
cgc_CList<T>::cgc_CList(const cgc_CList<T> &l) : m_head(NULL), m_size(0)
{
    cgc_CNode *cur = l.m_head;
    while (cur)
    {
        this->cgc_Append(cur->m_data);
        cur = cur->m_next;
    }
}

template <typename T>
cgc_CList<T>::~cgc_CList()
{
    while (!this->cgc_IsEmpty())
        this->cgc_RemoveAt(0);
}

template <typename T>
void cgc_CList<T>::cgc_Append(T d)
{
    cgc_CNode *node = new cgc_CNode(d, NULL);
    if (this->cgc_IsEmpty())
        m_head = node;
    else
    {
        cgc_CNode *cur = m_head;
        while (cur->m_next)
            cur = cur->m_next;
        cur->m_next = node;
    }
    m_size++;
}

template <typename T>
void cgc_CList<T>::cgc_InsertAt(T d, int idx)
{
    cgc_CNode *cur = m_head, *prev = NULL;
    if (idx > m_size)
        return;
    if (idx == m_size)
    {
        this->cgc_Append(d);
        return;
    }
    while (idx--)
    {
        prev = cur;
        cur = cur->m_next;
    }
    cgc_CNode *node = new cgc_CNode(d, cur);
    if (prev)
        prev->m_next = node;
}

template <typename T>
T cgc_CList<T>::cgc_GetAt(int idx) const
{
    cgc_CNode *cur = m_head;
    if (idx >= m_size)
        return NULL;
    while (idx--)
        cur = cur->m_next;
    return cur->m_data;
}

template <typename T>
T cgc_CList<T>::cgc_RemoveAt(int idx)
{
    cgc_CNode *cur = m_head, *prev = NULL;
    if (idx >= m_size)
        return  NULL;
    while (idx--)
    {
        prev = cur;
        cur = cur->m_next;
    }
    if (prev)
        prev->m_next = cur->m_next;
    else
        m_head = cur->m_next;
    m_size--;
    T& d = cur->m_data;
    delete cur;
    return d;
}

template <typename T>
void cgc_CList<T>::cgc_Remove(T item)
{
    cgc_CNode *cur = m_head, *prev = NULL;
    if (cgc_IsEmpty())
        return;
    while (cur)
    {
        if (cur->m_data == item)
        {
            if (cur == m_head)
                m_head = cur->m_next;
            else
                prev->m_next = cur->m_next;
            m_size--;
            delete cur;
            return;
        }
        prev = cur;
        cur = cur->m_next;
    }
}

template <typename T>
bool cgc_CList<T>::cgc_Has(T item)
{
    cgc_CNode *cur = m_head;
    if (cgc_IsEmpty())
        return false;
    while (cur)
    {
        if (cur->m_data == item)
            return true;
        cur = cur->m_next;
    }
    return false;
}
