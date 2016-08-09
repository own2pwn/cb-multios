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
#include "eval.h"

#include <cstdio.h>
#define DBG(x) cgc_fprintf(stderr, x "\n");

cgc_Var::cgc_Var(VarType type)
{
    this->type = type;
}

cgc_Var::~cgc_Var()
{
}

cgc_Scope::cgc_Scope(cgc_Scope *parent)
    : breaking(false), returning(false)
{
    this->parent = parent;
}

cgc_Scope::~cgc_Scope()
{
}

cgc_unique_ptr<cgc_Var>* cgc_Scope::cgc_findName(const cgc_CString& name)
{
    if (names.cgc_has(name))
        return &names[name];
    else 
        return parent == nullptr ? nullptr : parent->cgc_findName(name);
}

cgc_Evaluator::cgc_Evaluator(cgc_Node *root_)
    : root(root_)
{
    cgc_StringVar::cgc_registerOps(*this);
    cgc_NumberVar::cgc_registerOps(*this);
}

cgc_Evaluator::~cgc_Evaluator()
{
    cgc_Node::cgc_deleteTree(root);
}

void cgc_Evaluator::cgc_addExternal(const char *identifier, cgc_External func, void *arg)
{
    externals[identifier] = cgc_pair<cgc_External, void *>(func, arg);
}

void cgc_Evaluator::cgc_addOp(const cgc_CString& op, VarType a, VarType b, cgc_OpFunc func)
{
    cgc__Op _op = { .a = a, .b = b, .fn = func };
    operators[op].cgc_push_back(_op);
}

bool cgc_Evaluator::cgc_run()
{
    cgc_Scope scope(nullptr);
    scope.procScope = &scope;
    // root->cgc_dump(0);
    return cgc_eval_block(scope, root, 0);
}

bool cgc_Evaluator::cgc_eval_expr(cgc_Scope& scope, cgc_Node *node, cgc_unique_ptr<cgc_Var>& outresult)
{
    while (node->cgc_getType() == TokenType::Expr)
        node = node->cgc_getChild();
    switch (node->cgc_getType())
    {
    case TokenType::Operator:
    {
        cgc_unique_ptr<cgc_Var> rhs;
        cgc_CString op;

        op = node->cgc_getLiteral();
        if (!cgc_eval_expr(scope, node->cgc_getNext(), rhs))
            return false;
        if (!cgc_eval_op(op, rhs.cgc_get(), nullptr, outresult))
            return false;
        break;
    }
    case TokenType::BinOp:
    {
        cgc_unique_ptr<cgc_Var> lhs, rhs;
        cgc_CString op;

        cgc_Node *child = node->cgc_getChild();
        if (!cgc_eval_expr(scope, child, lhs))
            return false;
        child = child->cgc_getNext();
        op = child->cgc_getLiteral();
        child = child->cgc_getNext();
        if (!cgc_eval_expr(scope, child, rhs))
            return false;
        if (!cgc_eval_op(op, lhs.cgc_get(), rhs.cgc_get(), outresult))
            return false;
        break;
    }
    case TokenType::Assign:
    {
        cgc_Node *child = node->cgc_getChild();
        cgc_unique_ptr<cgc_Var>* var = scope.cgc_findName(child->cgc_getLiteral());
        if (var == nullptr)
        {
            DBG("undefined identifier");
            return false;
        }
        if (!cgc_eval_expr(scope, child->cgc_getNext(), *var))
            return false;
        outresult = var->cgc_get()->cgc_copy();
        break;
    }
    case TokenType::Identifier:
    {
        cgc_unique_ptr<cgc_Var>* var = scope.cgc_findName(node->cgc_getLiteral());
        if (var == nullptr)
        {
            DBG("undefined identifier");
            return false;
        }
        outresult = var->cgc_get()->cgc_copy();
        break;
    }
    case TokenType::Number:
    {
        outresult.cgc_reset(new cgc_NumberVar(cgc_strtoul(node->cgc_getLiteral(), 0, 0)));
        break;
    }
    case TokenType::Call:
    {
        cgc_Node *child = node->cgc_getChild();
        cgc_unique_ptr<cgc_Var>* var = scope.cgc_findName(child->cgc_getLiteral());
        if (var != nullptr)
        {
            if (var->cgc_get()->cgc_getType() != VarType::Proc)
            {
                DBG("cgc_Procedure cgc_has wrong type.");
                return false;
            }
            child = child->cgc_getNext();

            cgc_Procedure *proc = static_cast<cgc_Procedure *>(var->cgc_get());
            cgc_Scope calleeScope(&proc->cgc_getScope());
            calleeScope.procScope = &calleeScope;
            auto args = proc->cgc_getArguments();
            for (unsigned int i = 0; i < args.cgc_length(); i++)
            {
                cgc_unique_ptr<cgc_Var> result;
                if (child == nullptr || !cgc_eval_expr(scope, child, result))
                {
                    DBG("Bad expression in call");
                    return false;
                }
                calleeScope.names[args[i]].cgc_reset(result.cgc_release());
                child = child->cgc_getNext();
            }
            if (!cgc_eval_block(calleeScope, proc->cgc_getBlock(), proc->cgc_getIndent()))
                return false;
            if (calleeScope.retval)
                outresult = calleeScope.retval.cgc_release();
            else
                outresult.cgc_reset(nullptr);
        }
        else if (externals.cgc_has(child->cgc_getLiteral()))
        {
            auto external = externals[child->cgc_getLiteral()];
            child = child->cgc_getNext();
            unsigned int nargs = 0;
            for (cgc_Node *node = child; node != nullptr; node = node->cgc_getNext())
                nargs++;
            cgc_vector<cgc_unique_ptr<cgc_Var>> args;
            args.cgc_resize(nargs);
            unsigned int i = 0;
            for (cgc_Node *node = child; node != nullptr; node = node->cgc_getNext(), i++)
            {
                cgc_unique_ptr<cgc_Var> result;
                if (!cgc_eval_expr(scope, node, result))
                {
                    DBG("Bad expression in extern call");
                    return false;
                }
                args[i].cgc_reset(result.cgc_release());
            }
            if (!external.left(external.right, *this, args, outresult))
                return false;
        }
        else
        {
            cgc_fprintf(stderr, "Undefined procedure: %s\n", child->cgc_getLiteral());
            return false;
        }
        break;
    }
    default:
        DBG("Unknown expression.");
        node->cgc_dump(0);
        return false;
    }
    return true;
}

bool cgc_Evaluator::cgc_eval_block(cgc_Scope& scope, cgc_Node *node, unsigned int indent)
{
    bool retval = true, inIf = false, skipElse = false;

    while (node != nullptr && !scope.procScope->breaking && !scope.procScope->returning)
    {
        cgc_Node *child;
        if (node->cgc_getType() != TokenType::Stmt)
        {
            DBG("wrong type in cgc_eval_block");
            return false;
        }
        child = node->cgc_getChild();
        if (child->cgc_getType() != TokenType::Indent)
        {
            DBG("wrong indent type in cgc_eval_block");
            return false;
        }
        if (child->cgc_getLength() != indent)
            break;
        child = child->cgc_getNext();

        if (child->cgc_getType() != TokenType::Else && child->cgc_getType() != TokenType::Elif)
            inIf = false;

        switch (child->cgc_getType())
        {
        case TokenType::Declare:
        {
            child = child->cgc_getChild();
            if (scope.names.cgc_has(child->cgc_getLiteral()))
            {
                DBG("Warning: redefining name");
                scope.names.cgc_remove(child->cgc_getLiteral());
            }
            scope.names[child->cgc_getLiteral()] = nullptr;

            node = node->cgc_getNext();
            break;
        }
        case TokenType::DeclareAssign:
        {
            child = child->cgc_getChild();
            cgc_Node *name = child->cgc_getChild();
            if (scope.names.cgc_has(name->cgc_getLiteral()))
            {
                DBG("Warning: redefining name");
                scope.names.cgc_remove(name->cgc_getLiteral());
            }
            scope.names[name->cgc_getLiteral()] = nullptr;

            cgc_unique_ptr<cgc_Var> result;
            if (!cgc_eval_expr(scope, child, result))
                return false;

            node = node->cgc_getNext();
            break;
        }
        case TokenType::BinOp:
        case TokenType::Expr:
        {
            cgc_unique_ptr<cgc_Var> result;

            if (!cgc_eval_expr(scope, child, result))
            {
                retval = false;
                goto error;
            }

            node = node->cgc_getNext();
            break;
        }
        case TokenType::Return:
        {
            if (!cgc_eval_expr(scope, child->cgc_getChild(), scope.retval))
            {
                retval = false;
                goto error;
            }
            scope.procScope->returning = true;
            break;
        }
        case TokenType::Break:
        {
            scope.procScope->breaking = true;
            break;
        }
        case TokenType::Else:
        {
            if (!inIf)
                return false;

            cgc_Node *block = node->cgc_getNext();
            unsigned int blockIndent = block->cgc_getChild()->cgc_getLength();
            if (blockIndent <= indent)
            {
                DBG("Error: block indent incorrect");
                return false;
            }

            if (!skipElse)
            {
                cgc_Scope blockScope(&scope);
                blockScope.procScope = scope.procScope;

                if (!cgc_eval_block(blockScope, block, blockIndent))
                    return false;
            }

            inIf = false;

            node = node->cgc_getNext(); // skip else
            while (node != nullptr && node->cgc_getChild()->cgc_getLength() >= blockIndent)
                node = node->cgc_getNext();
            break;
        }
        case TokenType::If:
        case TokenType::Elif:
        case TokenType::While:
        {
            cgc_unique_ptr<cgc_Var> cond;
            cgc_Node *condExpr = child->cgc_getChild();

            if (child->cgc_getType() == TokenType::If)
            {
                inIf = true;
                skipElse = false;
            }
            else if (child->cgc_getType() == TokenType::Elif && !inIf)
            {
                return false;
            }

            cgc_Node *block = node->cgc_getNext();
            unsigned int blockIndent = block->cgc_getChild()->cgc_getLength();
            if (blockIndent <= indent)
            {
                DBG("Error: block indent incorrect");
                return false;
            }

            if (child->cgc_getType() != TokenType::Elif || !skipElse)
            {
                do
                {
                    if (!cgc_eval_expr(scope, condExpr, cond))
                        return false;

                    if (cond->cgc_getType() != VarType::Number)
                        return false;

                    if (!static_cast<cgc_NumberVar*>(cond.cgc_get())->cgc_getValue())
                        break;

                    cgc_Scope blockScope(&scope);
                    blockScope.procScope = scope.procScope;

                    skipElse = true;

                    if (!cgc_eval_block(blockScope, block, blockIndent))
                        return false;

                    if (scope.procScope->breaking || scope.procScope->returning)
                        break;
                } while (child->cgc_getType() == TokenType::While);
            }

            if (child->cgc_getType() == TokenType::While)
                scope.procScope->breaking = false;

            node = node->cgc_getNext(); // skip if/elif/while
            while (node != nullptr && node->cgc_getChild()->cgc_getLength() >= blockIndent)
                node = node->cgc_getNext();
            break;
        }
        case TokenType::Proc:
        {
            child = child->cgc_getChild();
            if (scope.names.cgc_has(child->cgc_getLiteral()))
            {
                DBG("Warning: redefining name");
                scope.names.cgc_remove(child->cgc_getLiteral());
            }
            node = node->cgc_getNext();
            unsigned int procIndent = node->cgc_getChild()->cgc_getLength();
            if (procIndent <= indent)
            {
                DBG("Error: procedure indent incorrect");
                retval = false;
                goto error;
            }
            cgc_Var *var = new cgc_Procedure(scope, child->cgc_getNext(), node, procIndent);
            scope.names[cgc_CString(child->cgc_getLiteral()).cgc_ensure()] = var;
            while (node != nullptr && node->cgc_getChild()->cgc_getLength() >= procIndent)
                node = node->cgc_getNext();
            break;
        }
        default:
            DBG("Unknown statement type.");
            node->cgc_dump(0);
            retval = false;
            goto error;
        }
    }

error:
    return retval;
}

bool cgc_Evaluator::cgc_eval_op(const cgc_CString& op, cgc_Var* a, cgc_Var* b, cgc_unique_ptr<cgc_Var>& outresult)
{
    if (!operators.cgc_has(op))
    {
        DBG("missing cgc_operator");
        return false;
    }
    auto ops = operators[op];
    for (unsigned int i = 0; i < ops.cgc_length(); i++)
    {
        auto _op = ops[i];
        if (b == nullptr)
        {
            if (_op.a == a->cgc_getType() && _op.b == VarType::Nil)
                return _op.fn(op, a, b, outresult);
        }
        else
        {
            if (_op.a == a->cgc_getType() && _op.b == b->cgc_getType())
                return _op.fn(op, a, b, outresult);
        }
    }

    DBG("missing compatible cgc_operator");
    return false;
}

cgc_Procedure::cgc_Procedure(cgc_Scope &scope_, cgc_Node *arglist, cgc_Node *block, unsigned int indent)
    : cgc_Var(VarType::Proc), scope(scope_)
{
    for (cgc_Node *node = arglist; node != nullptr; node = node->cgc_getNext())
        args.cgc_push_back(node->cgc_getLiteral());

    this->block = block;
    this->indent = indent;
}

cgc_Procedure::~cgc_Procedure()
{
}

cgc_Var* cgc_Procedure::cgc_copy()
{
    return new cgc_Procedure(*this);
}

cgc_NumberVar::cgc_NumberVar(unsigned int val)
    : cgc_Var(VarType::Number), value(val)
{
}

cgc_NumberVar::~cgc_NumberVar()
{
}

cgc_Var* cgc_NumberVar::cgc_copy()
{
    return new cgc_NumberVar(*this);
}

cgc_StringVar::cgc_StringVar()
    : cgc_Var(VarType::String)
{
}

cgc_StringVar::~cgc_StringVar()
{
}

cgc_Var* cgc_StringVar::cgc_copy()
{
    return new cgc_StringVar(*this);
}

#define OP(name, t1, t2) \
    eval.cgc_addOp(name, VarType::t1, VarType::t2, \
        [] (const cgc_CString& op, cgc_Var *a, cgc_Var *b, cgc_unique_ptr<cgc_Var>& result) -> bool {
#define OP_END });
void cgc_NumberVar::cgc_registerOps(cgc_Evaluator &eval)
{
#define NUMOP(_op) \
    eval.cgc_addOp(#_op, VarType::Number, VarType::Number, \
        [] (const cgc_CString& op, cgc_Var *a, cgc_Var *b, cgc_unique_ptr<cgc_Var>& result) -> bool { \
            const cgc_NumberVar *na = static_cast<cgc_NumberVar*>(a); \
            const cgc_NumberVar *nb = static_cast<cgc_NumberVar*>(b); \
            result.cgc_reset(new cgc_NumberVar(na->value _op nb->value)); \
            return true; \
    });
    NUMOP(>>)
    NUMOP(<<)
    NUMOP(*)
    NUMOP(/)
    NUMOP(+)
    NUMOP(-)
    NUMOP(|)
    NUMOP(&)
    NUMOP(<)
    NUMOP(>)
    NUMOP(<=)
    NUMOP(>=)
    NUMOP(!=)
    NUMOP(==)
    NUMOP(||) // "or" is currently broken
    NUMOP(&&) // "and" is currently broken
#undef NUMOP
#define NUMOP(_op) \
    eval.cgc_addOp(#_op, VarType::Number, VarType::Nil, \
        [] (const cgc_CString& op, cgc_Var *a, cgc_Var *b, cgc_unique_ptr<cgc_Var>& result) -> bool { \
            const cgc_NumberVar *na = static_cast<cgc_NumberVar*>(a); \
            result.cgc_reset(new cgc_NumberVar(_op na->value)); \
            return true; \
    });
    NUMOP(-)
    NUMOP(~)
#undef NUMOP
}

void cgc_StringVar::cgc_registerOps(cgc_Evaluator &eval)
{
    OP("+", String, String)
        const cgc_StringVar *sa = static_cast<cgc_StringVar*>(a);
        const cgc_StringVar *sb = static_cast<cgc_StringVar*>(b);

        cgc_StringVar *out = new cgc_StringVar();
        out->cgc_resize(sa->data.cgc_length() + sb->data.cgc_length());

        unsigned int i, j;
        for (i = 0; i < sa->data.cgc_length(); i++)
            out->data[i] = sa->data[i];
        for (j = 0; j < sb->data.cgc_length(); j++)
            out->data[i + j] = sb->data[j];
        result.cgc_reset(out);

        return true;
    OP_END
}
