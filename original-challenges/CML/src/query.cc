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
#include "query.h"

static cgc_uint16_t cgc_swap16(cgc_uint16_t x)
{
    return (x >> 8) | (x << 8);
}

cgc_Query::cgc_Query(const unsigned char *src, unsigned int cgc_length)
{
    d_insn[0].d_op = Op::ERROR;

    cgc_parse(src, cgc_length);
}

cgc_Query::~cgc_Query()
{
    cgc_destroy_insns();
}

void cgc_Query::cgc_destroy_insns()
{
    for (unsigned int i = 0; i < MAX_DEPTH + 1; i++)
    {
        if (d_insn[i].d_op == Op::ERROR || d_insn[i].d_op == Op::RETURN)
            break;
        switch (d_insn[i].d_op)
        {
        // just here to prevent compiler warnings
        case Op::ERROR:
        case Op::RETURN:
            break;
        case Op::ANCESTORS:
        case Op::DESCENDANTS:
        case Op::PARENT:
        case Op::CHILDREN:
        case Op::SIBLINGS:
        case Op::INDEX:
            break;
        case Op::TAG:
        case Op::NAMESPACE:
            d_insn[i].d_op1_s->cgc_destroy();
            break;
        case Op::ATTRIBUTE:
            d_insn[i].d_op1_s->cgc_destroy();
            d_insn[i].d_op2_s->cgc_destroy();
            break;
        }
    }
    d_insn[0].d_op = Op::ERROR;
}

void cgc_Query::cgc_parse(const unsigned char *str, unsigned int cgc_length)
{
    unsigned int i = 0;
    cgc_Instruction *cur = &d_insn[0];
    while (i < cgc_length)
    {
        const unsigned char *end;
        if (cur == &d_insn[MAX_DEPTH])
        {
            d_error = Error::RECURSION;
            return;
        }
        cur->d_op = static_cast<Op>(str[i]);
        switch (cur->d_op)
        {
        default:
        case Op::ERROR:
        case Op::RETURN:
            goto invalid;
        case Op::ANCESTORS:
        case Op::DESCENDANTS:
        case Op::PARENT:
        case Op::CHILDREN:
        case Op::SIBLINGS:
            i++;
            break;
        case Op::INDEX:
            i++;
            if (cgc_length - i < 2)
                goto invalid;
            cur->d_op1_u = cgc_swap16(*(cgc_uint16_t *)&str[i]);
            i += 2;
            break;
        case Op::TAG:
        case Op::NAMESPACE:
            i++;
            end = static_cast<const unsigned char *>(cgc_memchr(&str[i], 0, cgc_length - i));
            if (end == nullptr)
                goto invalid;
            cur->d_op1_s = cgc_String::cgc_create(reinterpret_cast<const char *>(&str[i]));
            i += end - &str[i] + 1;
            break;
        case Op::ATTRIBUTE:
            i++;
            end = static_cast<const unsigned char *>(cgc_memchr(&str[i], 0, cgc_length - i));
            if (end == nullptr)
                goto invalid;
            cur->d_op1_s = cgc_String::cgc_create(reinterpret_cast<const char *>(&str[i]));
            i += end - &str[i] + 1;

            end = static_cast<const unsigned char *>(cgc_memchr(&str[i], 0, cgc_length - i));
            if (end == nullptr)
                goto invalid;
            cur->d_op2_s = cgc_String::cgc_create(reinterpret_cast<const char *>(&str[i]));
            i += end - &str[i] + 1;
            break;
        }
        cur++;
    }
    cur->d_op = Op::RETURN;
    d_error = Error::SUCCESS;
    return;

invalid:
    d_error = Error::INVALID;
    d_insn[0].d_op = Op::ERROR;
    return;
}

cgc_Node *cgc_Query::cgc_apply(cgc_Node *root, cgc_Instruction *cur, unsigned int index)
{
    cgc_Attribute *cgc_attr;
    cgc_Node *node, *child;
    if (d_depth == MAX_RECURSION)
    {
        d_error = Error::RECURSION;
        return nullptr;
    }
    if (static_cast<int>(cur->d_op) & 0x80)
    {
        // test
        switch (cur->d_op)
        {
        case Op::RETURN:
            d_error = Error::SUCCESS;
            return root;
        case Op::TAG:
            if (cgc_strcmp(root->cgc_tag()->cgc_cstr(), cur->d_op1_s->cgc_cstr()) != 0)
                return nullptr;
            break;
        case Op::NAMESPACE:
            if (root->cgc_ns() == nullptr || cgc_strcmp(root->cgc_ns()->cgc_cstr(), cur->d_op1_s->cgc_cstr()) != 0)
                return nullptr;
            break;
        case Op::ATTRIBUTE:
            cgc_attr = root->cgc_get_attr(cur->d_op1_s->cgc_cstr());
            if (cgc_attr == nullptr || cgc_strcmp(cgc_attr->cgc_get()->cgc_cstr(), cur->d_op2_s->cgc_cstr()) != 0)
                return nullptr;
            break;
        case Op::INDEX:
            if (cur->d_op1_u != index)
                return nullptr;
            break;
        default:
            goto invalid;
        }
        d_depth++;
        node = cgc_apply(root, cur + 1, index);
        d_depth--;
        return node;
    }
    else
    {
        // target
        if (cur->d_op == Op::ANCESTORS)
        {
            if (root->cgc_parent() == nullptr)
                return nullptr;
            d_depth++;
            node = cgc_apply(root->cgc_parent(), cur + 1, 0);
            d_depth--;
            if (node != nullptr)
                return node;
            d_depth++;
            node = cgc_apply(root->cgc_parent(), cur, 0);
            d_depth--;
            return node;
        }
        else if (cur->d_op == Op::PARENT)
        {
            if (root->cgc_parent() == nullptr)
                return nullptr;
            d_depth++;
            node = cgc_apply(root->cgc_parent(), cur + 1, 0);
            d_depth--;
            return node;
        }
        else if (cur->d_op == Op::DESCENDANTS)
        {
            for (child = root->cgc_first(); child != nullptr; child = child->cgc_next())
            {
                d_depth++;
                node = cgc_apply(child, cur + 1, 0);
                d_depth--;
                if (node != nullptr)
                    return node;
            }
            for (child = root->cgc_first(); child != nullptr; child = child->cgc_next())
            {
                d_depth++;
                node = cgc_apply(child, cur, 0);
                d_depth--;
                if (node != nullptr)
                    return node;
            }
            return nullptr;
        }
        else if (cur->d_op == Op::CHILDREN)
        {
            index = 0;
            for (child = root->cgc_first(); child != nullptr; child = child->cgc_next())
            {
                d_depth++;
                node = cgc_apply(child, cur + 1, index++);
                d_depth--;
                if (node != nullptr)
                    return node;
            }
            return nullptr;
        }
        else if (cur->d_op == Op::SIBLINGS)
        {
            if (root->cgc_parent() == nullptr)
                return nullptr;
            index = 0;
            for (child = root->cgc_parent()->cgc_first(); child != nullptr; child = child->cgc_next())
            {
                d_depth++;
                node = cgc_apply(child, cur + 1, index++);
                d_depth--;
                if (node != nullptr)
                    return node;
            }
            return nullptr;
        }
        else
        {
            goto invalid;
        }
    }
invalid:
    d_error = Error::INVALID;
    return nullptr;
}

cgc_Node *cgc_Query::cgc_match(cgc_Node *root)
{
    d_depth = 0;
    d_error = Error::NOT_FOUND;
    return cgc_apply(root, &d_insn[0], 0);
}
