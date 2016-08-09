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
#include "array.h"

static inline int cgc_abs(int x)
{
    if (x < 0)
        return -x;
    else
        return x;
}

cgc_ArrayVar::cgc_ArrayVar()
    : cgc_Var(VarType::cgc_Array)
{
    array.cgc_reset(new cgc_Array());
}

cgc_ArrayVar::~cgc_ArrayVar()
{
}

cgc_Var* cgc_ArrayVar::cgc_copy()
{
    return new cgc_ArrayVar(*this);
}

bool cgc_Array::cgc_set(int i, cgc_Var* val)
{
    if (i < 0)
    {
        if (cgc_abs(i) > cgc_length())
            return false;
        i += cgc_length();
#ifdef PATCHED_1
        if (i < 0) return false;
#endif
    }

    if (i >= items.cgc_length())
    {
        if (i >= MAX_ITEMS)
            return false;

        items.cgc_resize(i + 1);
    }

    items[i].cgc_reset(val);
    return true;
}

bool cgc_Array::cgc_get(int i, cgc_Var** pval)
{
    if (i < 0)
    {
        if (cgc_abs(i) > cgc_length())
            return false;
        i += cgc_length();
#ifdef PATCHED_1
        if (i < 0) return false;
#endif
    }

    if (i >= items.cgc_length())
    {
        *pval = nullptr;
    }
    else
    {
        *pval = items[i].cgc_get();
    }
    return true;
}

static bool cgc_func_Array(void *arg, cgc_Evaluator& eval, const cgc_vector<cgc_unique_ptr<cgc_Var>>& args, cgc_unique_ptr<cgc_Var>& result)
{
    if (args.cgc_length() != 0)
        return false;
    result.cgc_reset(new cgc_ArrayVar());
    return true;
}

static bool cgc_func_aget(void *arg, cgc_Evaluator& eval, const cgc_vector<cgc_unique_ptr<cgc_Var>>& args, cgc_unique_ptr<cgc_Var>& result)
{
    cgc_Var *item;
    if (args.cgc_length() != 2 || args[0]->cgc_getType() != VarType::cgc_Array || args[1]->cgc_getType() != VarType::Number)
        goto error;
    if (!static_cast<cgc_ArrayVar*>(args[0].cgc_get())->cgc_getArray().cgc_get(static_cast<cgc_NumberVar*>(args[1].cgc_get())->cgc_getValue(), &item))
        goto error;
    if (item == nullptr)
        result.cgc_reset(new cgc_Var(VarType::Nil));
    else
        result.cgc_reset(item->cgc_copy());
    return true;
error:
    return false;
}

static bool cgc_func_aset(void *arg, cgc_Evaluator& eval, const cgc_vector<cgc_unique_ptr<cgc_Var>>& args, cgc_unique_ptr<cgc_Var>& result)
{
    cgc_Var *item;
    if (args.cgc_length() != 3 || args[0]->cgc_getType() != VarType::cgc_Array || args[1]->cgc_getType() != VarType::Number)
        goto error;
    item = args[2] ? args[2]->cgc_copy() : nullptr;
    if (!static_cast<cgc_ArrayVar*>(args[0].cgc_get())->cgc_getArray().cgc_set(static_cast<cgc_NumberVar*>(args[1].cgc_get())->cgc_getValue(), item))
        goto error;
    result.cgc_reset(new cgc_Var(VarType::Nil));
    return true;
error:
    return false;
}

void cgc_ArrayVar::cgc_registerExternals(cgc_Evaluator& eval)
{
    eval.cgc_addExternal("cgc_Array", cgc_func_Array);
    eval.cgc_addExternal("aget", cgc_func_aget);
    eval.cgc_addExternal("aset", cgc_func_aset);
}
