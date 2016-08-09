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


namespace std
{
    template <typename T>
    T* cgc_addressof(T& ref)
    {
        return reinterpret_cast<T*>(&reinterpret_cast<char&>(ref));
    }
};

template <class T>
class cgc_vector
{
public:
    cgc_vector() : cgc_size(0), allocated(0), items(nullptr) {};
    cgc_vector(unsigned int initial)
    {
        cgc_enlarge(initial);
    }
    cgc_vector(const cgc_vector &other)
    {
        cgc_size = other.cgc_size;
        allocated = other.cgc_size;
        if (cgc_size)
        {
            items = (T *)cgc_malloc(sizeof(T) * cgc_size);
            for (unsigned int i = 0; i < cgc_size; i++)
                items[i] = other.items[i];
        }
    }
    ~cgc_vector()
    {
        for (unsigned int i = 0; i < allocated; i++)
        {
            items[i].~T();
        }
        if (allocated)
        {
            cgc_free(items);
            items = nullptr;
        }
    }

    cgc_vector(cgc_vector &&) = delete;
    cgc_vector& operator=(const cgc_vector& other)
    {
        cgc_size = other.cgc_size;
        allocated = other.cgc_size;
        if (cgc_size)
        {
            items = (T *)cgc_malloc(sizeof(T) * cgc_size);
            for (unsigned int i = 0; i < cgc_size; i++)
                items[i] = other.items[i];
        }
        return *this;
    }

    unsigned int cgc_length() const
    {
        return cgc_size;
    }

    T& operator[] (unsigned int i)
    {
        if (i >= cgc_size)
            cgc_exit(1);
        return items[i];
    }

    const T& operator[] (unsigned int i) const
    {
        if (i >= cgc_size)
            cgc_exit(1);
        return items[i];
    }

    void cgc_push_back(const T& item)
    {
        if (cgc_size == allocated)
            cgc_enlarge(allocated == 0 ? 8 : allocated * 2);
        items[cgc_size++] = item;
    }

    void cgc_enlarge(unsigned int cgc_length)
    {
        if (cgc_length > allocated)
        {
            items = (T *)cgc_realloc(items, sizeof(T) * cgc_length);
            if (items == nullptr)
                cgc_exit(1);

            T *start = std::cgc_addressof(items[allocated]);
            T *cgc_end = std::cgc_addressof(items[cgc_length]);
            while (start < cgc_end)
            {
                T *tmp = new T();
cgc_memcpy(start++, tmp, sizeof(T));
delete tmp;

            }

            allocated = cgc_length;
        }
    }

    void cgc_resize(unsigned int cgc_length)
    {
        cgc_enlarge(cgc_length);

        if (cgc_size < cgc_length)
        {
            // items are already cgc_set to default values
            cgc_size = cgc_length;
        }
        else
        {
            // clear items
            for (unsigned int i = cgc_length; i < cgc_size; i++)
                items[i] = T();
            cgc_size = cgc_length;
        }
    }
private:
    unsigned int cgc_size;
    unsigned int allocated;
    T *items;
};
