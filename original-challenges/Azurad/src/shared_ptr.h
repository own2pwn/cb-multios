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

template <class T>
class cgc_shared_ptr
{
private:
    struct cgc_cb
    {
        cgc_cb(T* p) : _p(p), use_count(0) {}
        ~cgc_cb()
        {
            delete _p;
        }

        T* _p;
        cgc_size_t use_count;
    };
public:
    cgc_shared_ptr() : _p(nullptr), _cb(nullptr) {}
    explicit cgc_shared_ptr(T* p) : cgc_shared_ptr()
    {
        cgc_reset(p);
    }
    ~cgc_shared_ptr()
    {
        cgc_reset(nullptr);
    }
    cgc_shared_ptr(const cgc_shared_ptr& other)
    {
        _p = other._p;
        _cb = other._cb;
        if (_cb != nullptr)
            _cb->use_count++;
    }
    cgc_shared_ptr& operator=(const cgc_shared_ptr& other)
    {
        cgc_reset(nullptr);
        _p = other._p;
        _cb = other._cb;
        if (_cb != nullptr)
            _cb->use_count++;
        return *this;
    }
    void cgc_reset(T* p)
    {
        if (_cb != nullptr)
        {
            _cb->use_count--;
            if (_cb->use_count == 0)
            {
                delete _cb;
            }
        }
        _p = p;
        if (_p != nullptr)
        {
            _cb = new cgc_cb(_p);
            _cb->use_count++;
        }
    }
    T* cgc_get() const
    {
        return _p;
    }
    explicit operator bool() const
    {
        return _p != nullptr;
    }
private:
    T* _p;
    cgc_cb* _cb;
};
