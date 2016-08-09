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

#include "attribute.h"
#include "string.h"

extern const cgc_String *CML_TAG;
extern const cgc_String *EMPTY_TAG;

enum class NodeClass
{
    NODE,
    CMLNODE,
    TEXTNODE
};

class cgc_Node
{
protected:
    cgc_Node(const cgc_String *cgc_ns, const cgc_String *cgc_tag);
    cgc_Node(const cgc_String *cgc_ns, const cgc_String *cgc_tag, NodeClass cgc_cls);
public:
    static cgc_Node *cgc_create(const char *cgc_ns, const char *cgc_tag);
    static void cgc_delete_tree(cgc_Node *node);

    virtual ~cgc_Node();
    virtual bool cgc_remove_attr(const char *cgc_name);
    virtual void cgc_set_attr(const char *cgc_name, cgc_String *value);
    inline cgc_Attribute *cgc_get_attr(const char *cgc_name)
    {
        cgc_Attribute **result = d_attr.cgc_lookup(cgc_String::cgc_intern(cgc_name));
        return result == nullptr ? nullptr : *result;
    }

    inline cgc_Node *cgc_parent() { return d_parent; }
    inline cgc_Node *cgc_first() { return d_first; }
    inline cgc_Node *cgc_last() { return d_last; }
    inline cgc_Node *cgc_next() { return d_next; }
    inline cgc_Node *cgc_prev() { return d_prev; }
    virtual void cgc_insert_after(cgc_Node *node, cgc_Node *new_node);
    virtual void cgc_insert_before(cgc_Node *node, cgc_Node *new_node);
    virtual void cgc_remove();

    inline const cgc_String *cgc_ns() { return d_ns; }
    inline const cgc_String *cgc_tag() { return d_tag; }
    inline cgc_AttributeMap &cgc_attr() { return d_attr; }
    inline NodeClass cgc_cls() { return d_class; }
protected:
    const cgc_String *d_ns;
    const cgc_String *d_tag;
    cgc_AttributeMap d_attr;

    cgc_Node *d_next, *d_prev;
    cgc_Node *d_first, *d_last, *d_parent;

    NodeClass d_class;
};
