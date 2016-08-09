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

#include "hashmap.h"
#include "stringmanager.h"

class cgc_String
{
private:
    cgc_String(const char *str);
    inline ~cgc_String() {}
public:
    static cgc_String *cgc_create(const char *str);
    static inline const cgc_String *cgc_intern(const char *str)
    {
        return cgc_StringManager::cgc_get().cgc_intern(str);
    }

    inline const char *cgc_cstr() const { return d_data; }
    inline void cgc_destroy() { cgc_free(this); }
private:
    char d_data[];
};

class StringComparator
{
public:
    static inline unsigned int cgc_hash(const cgc_String *s)
    {
        return CStringComparator::cgc_hash(s->cgc_cstr());
    }
    static inline bool cgc_equals(const cgc_String *a, const cgc_String *b)
    {
        return CStringComparator::cgc_equals(a->cgc_cstr(), b->cgc_cstr());
    }
};

class InternedComparator
{
public:
    static inline unsigned int cgc_hash(const cgc_String *s)
    {
        cgc_intptr_t p = reinterpret_cast<cgc_intptr_t>(s);
        return (p * 761886451) ^ (p << 16);
    }
    static inline bool cgc_equals(const cgc_String *a, const cgc_String *b)
    {
        return a == b;
    }
};
