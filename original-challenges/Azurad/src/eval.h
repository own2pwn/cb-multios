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

#include "ccstring.h"
#include "parser.h"
#include "map.h"
#include "pair.h"
#include "unique_ptr.h"

class cgc_Evaluator;
class cgc_Scope;

enum class VarType
{
    Nil,
    Proc,
    String,
    Number,
    cgc_Array
};

class cgc_Var
{
public:
    cgc_Var() : cgc_Var(VarType::Nil) {}
    cgc_Var(VarType type);
    virtual ~cgc_Var();

    VarType cgc_getType() const { return type; }
    virtual cgc_Var* cgc_copy() { return new cgc_Var(type); }
protected:
    VarType type;
};

class cgc_Procedure : public cgc_Var
{
public:
    cgc_Procedure(cgc_Scope &scope, cgc_Node *arglist, cgc_Node *block, unsigned int indent);
    ~cgc_Procedure();

    cgc_Procedure(const cgc_Procedure &other)
        : cgc_Var(other.type), scope(other.scope)
    {
        block = other.block;
        indent = other.indent;
        args = other.args;
    }
    cgc_Procedure& operator=(const cgc_Procedure &) = delete;

    cgc_Var* cgc_copy();

    const cgc_vector<cgc_CString>& cgc_getArguments() const { return args; }
    cgc_Node *cgc_getBlock() { return block; }
    unsigned int cgc_getIndent() { return indent; }
    cgc_Scope& cgc_getScope() { return scope; }
private:
    cgc_Scope &scope;
    cgc_Node *block;
    unsigned int indent;
    cgc_vector<cgc_CString> args;
};

class cgc_NumberVar : public cgc_Var
{
public:
    cgc_NumberVar(unsigned int val);
    ~cgc_NumberVar();

    cgc_NumberVar(const cgc_NumberVar &other)
        : cgc_Var(other.type)
    {
        value = other.value;
    }
    cgc_NumberVar& operator=(const cgc_NumberVar &) = delete;

    static void cgc_registerOps(cgc_Evaluator &);
    cgc_Var* cgc_copy();

    unsigned int cgc_getValue() { return value; }
private:
    unsigned int value;
};

class cgc_StringVar : public cgc_Var
{
public:
    cgc_StringVar();
    ~cgc_StringVar();

    cgc_StringVar(const cgc_StringVar &other)
        : cgc_Var(other.type)
    {
        data = other.data;
    }
    cgc_StringVar& operator=(const cgc_StringVar &) = delete;

    static void cgc_registerOps(cgc_Evaluator &);
    cgc_Var* cgc_copy();

    char *cgc_getBuffer() { return data.cgc_length() == 0 ? nullptr : &data[0]; }
    unsigned int cgc_getLength() { return data.cgc_length(); }
    void cgc_resize(unsigned int cgc_length) { data.cgc_resize(cgc_length); }
private:
    cgc_vector<char> data;
};

class cgc_Scope
{
    friend class cgc_Evaluator;
public:
    cgc_Scope(cgc_Scope *parent);
    ~cgc_Scope();

    cgc_unique_ptr<cgc_Var>* cgc_findName(const cgc_CString& name);

    cgc_Scope *parent, *procScope;
    cgc_map<cgc_CString, cgc_unique_ptr<cgc_Var>> names;
    cgc_unique_ptr<cgc_Var> retval;
    bool breaking;
    bool returning;
};

class cgc_Evaluator
{
public:
    typedef bool (*cgc_External) (void *arg, cgc_Evaluator& eval, const cgc_vector<cgc_unique_ptr<cgc_Var>>& args, cgc_unique_ptr<cgc_Var>& result);
    typedef bool (*cgc_OpFunc) (const cgc_CString& op, cgc_Var* a, cgc_Var* b, cgc_unique_ptr<cgc_Var>& result);

    cgc_Evaluator(cgc_Node *root);
    ~cgc_Evaluator();

    bool cgc_run();
    void cgc_addExternal(const char *identifier, cgc_External func, void *arg);
    void cgc_addExternal(const char *identifier, cgc_External func) { cgc_addExternal(identifier, func, nullptr); }
    void cgc_addOp(const cgc_CString& op, VarType a, VarType b, cgc_OpFunc func);
private:
    bool cgc_eval_block(cgc_Scope& scope, cgc_Node *node, unsigned int indent);
    bool cgc_eval_expr(cgc_Scope& scope, cgc_Node *node, cgc_unique_ptr<cgc_Var>& outresult);
    bool cgc_eval_op(const cgc_CString& op, cgc_Var* a, cgc_Var* b, cgc_unique_ptr<cgc_Var>& outresult);
private:
    struct cgc__Op
    {
        VarType a;
        VarType b;
        cgc_OpFunc fn;
    };
    cgc_Node *root;
    cgc_map<cgc_CString, cgc_pair<cgc_External, void *>> externals;
    cgc_map<cgc_CString, cgc_vector<cgc__Op>> operators;
};
