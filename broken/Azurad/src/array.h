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

#include "eval.h"
#include "shared_ptr.h"

class cgc_Array
{
public:
    static const int MAX_ITEMS = 1000;

    int cgc_length()
    {
        return items.cgc_length();
    }
    bool cgc_set(int i, cgc_Var* val);
    bool cgc_get(int i, cgc_Var** pval);
private:
    cgc_vector<cgc_unique_ptr<cgc_Var>> items;
};

class cgc_ArrayVar : public cgc_Var
{
public:
    cgc_ArrayVar();
    ~cgc_ArrayVar();

    static void cgc_registerExternals(cgc_Evaluator& eval);

    virtual cgc_Var* cgc_copy();
    cgc_Array& cgc_getArray()
    {
        return *array.cgc_get();
    }
private:
    cgc_shared_ptr<cgc_Array> array;
};
