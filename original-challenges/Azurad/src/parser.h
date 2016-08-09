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

#include <cstdio.h>
#include "vector.h"

namespace std
{
    typedef unsigned int size_t;
};

class cgc_Node;

enum class TokenType
{
    Ignore,
    Optional,
    Repeat,
    Or,
    And,

    Indent,
    Identifier,
    Number,
    String,
    Discard,
    Var,
    Assign,
    Operator,
    Declare,
    DeclareAssign,
    Call,
    Stmt,
    Expr,
    Block,
    While,
    If,
    Elif,
    Else,
    BinOp,
    Proc,
    Break,
    Return
};

class cgc_Token
{
public:
    cgc_Token();
    cgc_Token(TokenType type);
    cgc_Token(const char *str);
    cgc_Token(const char *str, TokenType type);
    cgc_Token(const char *str, size_t len, TokenType type);
    cgc_Token(const cgc_Token &child, TokenType type);

    bool cgc_parse(const char *input, size_t cgc_length, cgc_Node **pNode, size_t *pLength) const;
    void cgc_set(const cgc_Token &child);
    void cgc_setAfter(cgc_Token &after);

    cgc_Token operator++(int ignored);
    cgc_Token operator+(cgc_Token rhs);
    cgc_Token operator|(cgc_Token rhs);
    cgc_Token operator&();
protected:
    bool cgc_parse_(const char *input, size_t cgc_length, cgc_Node **pNode, size_t *pLength) const;
    void cgc_after_(const char *input, size_t cgc_length, cgc_Node **pNode, size_t *pLength) const;
protected:
    TokenType type;

    cgc_vector<cgc_Token> children;
    cgc_Token *indirect, *after;
    const char *literal;
    size_t cgc_length;
};

class cgc_Node
{
    friend class cgc_Token;
public:
    cgc_Node(TokenType type);
    ~cgc_Node();

    static void cgc_deleteTree(cgc_Node *root);
    void cgc_dump(unsigned int level);
    void cgc_setLiteral(const char *start, size_t cgc_length);

    TokenType cgc_getType() { return type; }
    cgc_Node *cgc_getChild() { return child; }
    cgc_Node *cgc_getNext() { return next; }
    unsigned int cgc_getLength() { return cgc_length; }
    const char *cgc_getLiteral() { return literal; }
protected:
    char *literal;
    unsigned int cgc_length;
    TokenType type;
    cgc_Node *child;

    cgc_Node *next;
};

class cgc_Parser
{
public:
    cgc_Parser(const char *input);
    ~cgc_Parser();

    bool cgc_parse();
    cgc_Node *cgc_releaseRoot()
    {
        cgc_Node *result = root;
        root = nullptr;
        return result;
    }
private:
    const char *input;
    size_t cgc_length;
    size_t parsedLength;

    cgc_Node *root;
};

cgc_Token operator "" _T(const char *, STD_SIZE_T);
