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

#include <algorithm.h>

template <class T1, class T2>
struct cgc_pair
{
public:
    cgc_pair() {}
    cgc_pair(T1 left_, T2 right_) : left(left_), right(right_) {}
    cgc_pair(const cgc_pair& other)
    {
        left = other.left;
        right = other.right;
    }
    cgc_pair(cgc_pair&& other)
    {
        std::cgc_swap(left, other.left);
        std::cgc_swap(right, other.right);
    }

    cgc_pair& operator=(const cgc_pair& other)
    {
        left = other.left;
        right = other.right;
        return *this;
    }
    cgc_pair& operator=(cgc_pair&& other)
    {
        std::cgc_swap(left, other.left);
        std::cgc_swap(right, other.right);
        return *this;
    }

    T1 left;
    T2 right;
};
