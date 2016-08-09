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

#include "node.h"

class cgc_Query
{
private:
    enum class Op
    {
        ERROR = 0x00,
        ANCESTORS = 0x01,
        DESCENDANTS = 0x02,
        PARENT = 0x03,
        CHILDREN = 0x04,
        SIBLINGS = 0x05,
        RETURN = 0x80,
        TAG = 0x81,
        NAMESPACE = 0x82,
        ATTRIBUTE = 0x83,
        INDEX = 0x84
    };
    class cgc_Instruction
    {
    public:
        Op d_op;
        union {
            cgc_String *d_op1_s;
            unsigned int d_op1_u;
        };
        union {
            cgc_String *d_op2_s;
            unsigned int d_op2_u;
        };
    };
    void cgc_destroy_insns();
    void cgc_parse(unsigned const char *str, unsigned int cgc_length);
    cgc_Node *cgc_apply(cgc_Node *root, cgc_Instruction *cur, unsigned int index);
public:
    enum class Error
    {
        SUCCESS = 0,
        NOT_FOUND = 1,
        RECURSION = 2,
        INVALID = 3
    };
    cgc_Query(unsigned const char *str, unsigned int cgc_length);
    ~cgc_Query();

    cgc_Node *cgc_match(cgc_Node *root);
    inline Error cgc_error() { return d_error; }
private:
    static const int MAX_DEPTH = 256;
    static const int MAX_RECURSION = 1024;
    cgc_Instruction d_insn[MAX_DEPTH + 1];
    Error d_error;
    int d_depth;
};
