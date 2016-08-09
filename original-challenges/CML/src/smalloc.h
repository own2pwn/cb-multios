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

#include <cstdlib.h>
#include <cstdint.h>

#include "assert.h"

static inline void *cgc_safe_calloc(unsigned int count, unsigned int cgc_size)
{
    unsigned int result = count * cgc_size;
    if ((result / count) != cgc_size)
        cgc_die("detected overflow");
    void *mem = cgc_calloc(count, cgc_size);
    if (mem == nullptr)
        cgc_die("out of memory");
    return mem;
}

static inline void *cgc_safe_malloc(unsigned int cgc_size)
{
    void *mem = cgc_malloc(cgc_size);
    if (mem == nullptr)
        cgc_die("out of memory");
    return mem;
}
