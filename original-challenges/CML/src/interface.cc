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
#include <cstdlib.h>
#include <cstdint.h>
#include "cmlnode.h"
#include "interface.h"
#include "query.h"
#include "smalloc.h"

enum class Op
{
    INIT = 0,
    LOAD_DATA = 1,
    GET_ATTR = 2,
    SET_ATTR = 3,
    LIST_ATTR = 4,
    GET_TAG = 5,
    GET_NS = 6,
    QUERY = 7,
    VERSION_CHECK = 8
};

static cgc_uint16_t cgc_swap16(cgc_uint16_t x)
{
    return (x >> 8) | (x << 8);
}

#ifdef DEBUG
static void print_node(cgc_Node *root, int depth)
{
    char tmp[32];
    cgc_Attribute **pattr = nullptr;
    cgc_memset(tmp, '\t', depth);
    tmp[depth] = 0;
    cgc_fprintf(stderr, "%s+ %08X [%s] %s\n", tmp, root, root->cgc_ns() == nullptr ? "default" : root->cgc_ns()->cgc_cstr(), root->cgc_tag()->cgc_cstr());
    if (root->cgc_cls() == NodeClass::TEXTNODE)
        cgc_fprintf(stderr, "%s  \"%s\"\n", tmp, reinterpret_cast<TextNode*>(root)->text());
    while ((pattr = root->cgc_attr().cgc_next(pattr)) != nullptr)
    {
        cgc_fprintf(stderr, "%s  %s = \"%s\"\n", tmp, (*pattr)->cgc_name()->cgc_cstr(), (*pattr)->cgc_get()->cgc_cstr());
    }
    for (cgc_Node *child = root->cgc_first(); child != nullptr; child = child->cgc_next())
    {
        print_node(child, depth + 1);
    }
}
#endif

cgc_Interface::cgc_Interface(cgc_FILE *input, cgc_FILE *output)
    : d_in(input), d_out(output), d_next(0)
{
    cgc_fbuffered(d_out, 1);
}

void cgc_Interface::cgc_send_descriptor(unsigned int desc)
{
    cgc_fwrite(&desc, sizeof(desc), d_out);
}

void cgc_Interface::cgc_send_error(Error cgc_error)
{
    unsigned char code = static_cast<unsigned char>(cgc_error);
    cgc_fwrite(&code, sizeof(code), d_out);
}

void cgc_Interface::cgc_send_string(const char *str)
{
    cgc_uint16_t cgc_size = cgc_strlen(str);
    cgc_uint16_t size_s = cgc_swap16(cgc_size);
    cgc_fwrite(&size_s, sizeof(size_s), d_out);
    cgc_fwrite(str, cgc_size, d_out);
}

cgc_Node *cgc_Interface::cgc_get_node()
{
    unsigned int desc;
    if (cgc_fread(&desc, sizeof(desc), d_in) != sizeof(desc))
        return nullptr;
    cgc_Node **result = d_descriptors.cgc_lookup(desc);
    return result == nullptr ? nullptr : *result;
}

unsigned char *cgc_Interface::cgc_get_string_binary(cgc_uint16_t *psize)
{
    cgc_uint16_t cgc_size;
    unsigned char *data;
    if (cgc_fread(&cgc_size, sizeof(cgc_size), d_in) != sizeof(cgc_size))
        return nullptr;
    cgc_size = cgc_swap16(cgc_size);
    data = static_cast<unsigned char *>(cgc_safe_malloc(cgc_size));
    if (cgc_fread(data, cgc_size, d_in) != cgc_size)
    {
        cgc_free(data);
        return nullptr;
    }
    *psize = cgc_size;
    return data;
}

char *cgc_Interface::cgc_get_string()
{
    cgc_uint16_t cgc_size;
    char *data;
    if (cgc_fread(&cgc_size, sizeof(cgc_size), d_in) != sizeof(cgc_size))
        return nullptr;
    cgc_size = cgc_swap16(cgc_size);
    data = static_cast<char *>(cgc_safe_malloc(cgc_size + 1));
    if (cgc_fread(data, cgc_size, d_in) != cgc_size)
    {
        cgc_free(data);
        return nullptr;
    }
    data[cgc_size] = 0;
    return data;
}

unsigned int cgc_Interface::cgc_new_descriptor(cgc_Node *node)
{
    unsigned int desc = ++d_next;
    d_descriptors.cgc_insert(desc, node);
    return desc;
}

bool cgc_Interface::cgc_process()
{
    unsigned char op;
    if (cgc_fread(&op, sizeof(op), d_in) != sizeof(op))
        return false;
#define DO_OP(x, y) case Op::x: if (! y ()) return false; break;
    switch (static_cast<Op>(op))
    {
    DO_OP(INIT, cgc_op_init_parser)
    DO_OP(LOAD_DATA, cgc_op_load_data)
    DO_OP(GET_ATTR, cgc_op_get_attr)
    DO_OP(SET_ATTR, cgc_op_set_attr)
    DO_OP(LIST_ATTR, cgc_op_list_attr)
    DO_OP(GET_TAG, cgc_op_get_tag)
    DO_OP(GET_NS, cgc_op_get_ns)
    DO_OP(QUERY, cgc_op_query)
    DO_OP(VERSION_CHECK, cgc_op_version_check)
    default:
        cgc_send_error(Error::INVALID_OP);
        break;
    }
#undef DO_OP
    cgc_fflush(d_out);
    return true;
}

bool cgc_Interface::cgc_op_load_data()
{
    char *data;
    cgc_Node *node;
    data = cgc_get_string();
    if (data == nullptr)
    {
        return false;
    }
    if (d_parser == nullptr)
    {
        cgc_send_error(Error::NOT_READY);
        goto done;
    }
    node = d_parser->cgc_parse(data);
    if (node == nullptr)
    {
        cgc_send_error(Error::INVALID_DATA);
        goto done;
    }
    cgc_send_error(Error::SUCCESS);
    cgc_send_descriptor(cgc_new_descriptor(node));
done:
    cgc_free(data);
    return true;
}

bool cgc_Interface::cgc_op_get_attr()
{
    cgc_Node *node = cgc_get_node();
    char *cgc_name = cgc_get_string();
    if (node == nullptr)
    {
        cgc_free(cgc_name);
        cgc_send_error(Error::INVALID_DESC);
        return true;
    }
    if (cgc_name == nullptr)
        return false;
    cgc_Attribute *cgc_attr = node->cgc_get_attr(cgc_name);
    cgc_free(cgc_name);
    if (cgc_attr == nullptr)
    {
        cgc_send_error(Error::NOT_FOUND);
        return true;
    }
    cgc_send_error(Error::SUCCESS);
    cgc_send_string(cgc_attr->cgc_get()->cgc_cstr());
    return true;
}

bool cgc_Interface::cgc_op_set_attr()
{
    cgc_Node *node = cgc_get_node();
    char *cgc_name = cgc_get_string();
    char *value = cgc_get_string();
    if (node == nullptr)
    {
        cgc_free(cgc_name);
        cgc_free(value);
        cgc_send_error(Error::INVALID_DESC);
        return true;
    }
    if (cgc_name == nullptr || value == nullptr)
        return false;
    node->cgc_set_attr(cgc_name, cgc_String::cgc_create(value));
    cgc_free(cgc_name);
    cgc_free(value);
    cgc_send_error(Error::SUCCESS);
    return true;
}

bool cgc_Interface::cgc_op_init_parser()
{
    unsigned char chr[6];
    if (cgc_fread(chr, sizeof(chr), d_in) != sizeof(chr))
        return false;
    // check for duplicates
    for (unsigned int i = 0; i < sizeof(chr) - 1; i++)
    {
        if (cgc_memchr(&chr[i + 1], chr[i], sizeof(chr) - i - 1) != nullptr)
        {
            cgc_send_error(Error::INVALID_DATA);
            return true;
        }
    }
    d_parser = new cgc_Parser(chr[0], chr[1], chr[2], chr[3], chr[4], chr[5]);
    cgc_send_error(Error::SUCCESS);
    return true;
}

bool cgc_Interface::cgc_op_list_attr()
{
    cgc_Node *node = cgc_get_node();
    if (node == nullptr)
    {
        cgc_send_error(Error::INVALID_DESC);
        return true;
    }
    cgc_uint16_t cgc_size = node->cgc_attr().cgc_length();
    cgc_uint16_t size_s = cgc_swap16(cgc_size);
    cgc_send_error(Error::SUCCESS);
    cgc_fwrite(&size_s, sizeof(size_s), d_out);
    cgc_Attribute **pattr = nullptr;
    while ((pattr = node->cgc_attr().cgc_next(pattr)) != nullptr)
    {
        cgc_send_string((*pattr)->cgc_name()->cgc_cstr());
    }
    return true;
}

bool cgc_Interface::cgc_op_get_tag()
{
    cgc_Node *node = cgc_get_node();
    if (node == nullptr)
    {
        cgc_send_error(Error::INVALID_DESC);
        return true;
    }
    cgc_send_error(Error::SUCCESS);
    cgc_send_string(node->cgc_tag()->cgc_cstr());
    return true;
}

bool cgc_Interface::cgc_op_get_ns()
{
    cgc_Node *node = cgc_get_node();
    if (node == nullptr)
    {
        cgc_send_error(Error::INVALID_DESC);
        return true;
    }
    if (node->cgc_ns() == nullptr)
    {
        cgc_send_error(Error::NOT_FOUND);
    }
    else
    {
        cgc_send_error(Error::SUCCESS);
        cgc_send_string(node->cgc_ns()->cgc_cstr());
    }
    return true;
}

bool cgc_Interface::cgc_op_query()
{
    cgc_Node *node = cgc_get_node();
    if (node == nullptr)
    {
        cgc_send_error(Error::INVALID_DESC);
        return true;
    }
    cgc_uint16_t cgc_size;
    unsigned char *data = cgc_get_string_binary(&cgc_size);
    if (data == nullptr)
    {
        return false;
    }
    cgc_Query qs(data, cgc_size);
    if (qs.cgc_error() != cgc_Query::Error::SUCCESS)
    {
        cgc_send_error(Error::INVALID_DATA);
        return true;
    }
    cgc_Node *result = qs.cgc_match(node);
    if (result == nullptr || qs.cgc_error() != cgc_Query::Error::SUCCESS)
    {
        cgc_send_error(Error::NOT_FOUND);
        return true;
    }
    cgc_send_error(Error::SUCCESS);
    cgc_send_descriptor(cgc_new_descriptor(result));
    return true;
}

bool cgc_Interface::cgc_op_version_check()
{
    cgc_Node *node = cgc_get_node();
    if (node == nullptr)
    {
        cgc_send_error(Error::INVALID_DESC);
        return true;
    }
    if (node->cgc_cls() != NodeClass::CMLNODE)
    {
        cgc_send_error(Error::INVALID_DATA);
        return true;
    }
    unsigned long long v = static_cast<cgc_CmlNode*>(node)->cgc_version_code();
    if (v == *(unsigned long long *)0x4347C001)
    {
        cgc_send_error(Error::SUCCESS);
    }
    else
    {
        cgc_send_error(Error::NOT_FOUND);
    }
    return true;
}
