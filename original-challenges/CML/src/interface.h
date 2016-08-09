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
#include <cstdio.h>
#include "hashmap.h"
#include "node.h"
#include "parser.h"

enum class Error
{
    SUCCESS = 0,
    INVALID_OP = 1,
    INVALID_DESC = 2,
    NOT_READY = 3,
    INVALID_DATA = 4,
    NOT_FOUND = 5
};

class cgc_Interface
{
private:
    cgc_Node *cgc_get_node();
    char *cgc_get_string();
    unsigned char *cgc_get_string_binary(cgc_uint16_t *psize);
    unsigned int cgc_new_descriptor(cgc_Node *node);
    void cgc_send_error(Error cgc_error);
    void cgc_send_descriptor(unsigned int desc);
    void cgc_send_string(const char *str);

    bool cgc_op_init_parser();
    bool cgc_op_load_data();
    bool cgc_op_get_attr();
    bool cgc_op_set_attr();
    bool cgc_op_list_attr();
    bool cgc_op_get_tag();
    bool cgc_op_get_ns();
    bool cgc_op_query();
    bool cgc_op_version_check();
public:
    cgc_Interface(cgc_FILE *input, cgc_FILE *output);

    bool cgc_process();
private:
    cgc_FILE *d_in, *d_out;
    unsigned int d_next;
    cgc_HashMap<unsigned int, cgc_Node *, UIntComparator> d_descriptors;    
    cgc_Parser *d_parser;
};
