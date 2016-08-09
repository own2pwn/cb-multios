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

#include <cstring.h>
#include "map.h"
#include "unique_ptr.h"

// simple wrapper around a null-terminated string
class cgc_CString
{
public:
    cgc_CString() : ptr(nullptr) {}
    cgc_CString(const char *str) : ptr(str) {}
    cgc_CString(const cgc_CString& other)
    {
        if (other._ptr)
        {
            ptr = other.ptr;
            cgc_ensure();
        }
        else
        {
            _ptr.cgc_reset(nullptr);
            ptr = other.ptr;
        }
    }
    cgc_CString(cgc_CString&& other)
    {
        _ptr.cgc_reset(other._ptr.cgc_release());
        ptr = other.ptr;
        other.ptr = nullptr;
    }

    cgc_CString& operator=(const cgc_CString& other)
    {
        if (other._ptr)
        {
            ptr = other.ptr;
            cgc_ensure();
        }
        else
        {
            _ptr.cgc_reset(nullptr);
            ptr = other.ptr;
        }
        return *this;
    }
    cgc_CString& operator=(cgc_CString&& other)
    {
        _ptr.cgc_reset(other._ptr.cgc_release());
        ptr = other.ptr;
        other.ptr = nullptr;
        return *this;
    }

    bool operator==(const cgc_CString& other) const
    {
        return cgc_strcmp(ptr, other.ptr) == 0;
    }

    cgc_CString& cgc_ensure()
    {
        if (!_ptr)
        {
            cgc_size_t len = cgc_length() + 1;
            _ptr.cgc_reset(new char [len]);
            cgc_memcpy(_ptr.cgc_get(), ptr, len);
            ptr = _ptr.cgc_get();
        }
        return *this;
    }

    const char *cgc_c_str() const
    {
        return ptr;
    }

    cgc_size_t cgc_hash() const;
    cgc_size_t cgc_length() const
    {
        return cgc_strlen(ptr);
    }
private:
    const char *ptr;
    cgc_unique_ptr<char[]> _ptr;
};

template <>
struct cgc_hash<cgc_CString>
{
    cgc_size_t operator() (const cgc_CString& t)
    {
        return t.cgc_hash();
    }
};

