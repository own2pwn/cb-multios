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
#include "node.h"
#include "cmlnode.h"
#include "textnode.h"

const cgc_String *CML_TAG = cgc_String::cgc_intern("cml");
const cgc_String *EMPTY_TAG = cgc_String::cgc_intern("");

cgc_Node::cgc_Node(const cgc_String *cgc_ns, const cgc_String *cgc_tag, NodeClass cgc_cls)
    : d_ns(cgc_ns), d_tag(cgc_tag), d_next(nullptr), d_prev(nullptr),
      d_first(nullptr), d_last(nullptr), d_parent(nullptr), d_class(cgc_cls)
{
}

cgc_Node::cgc_Node(const cgc_String *cgc_ns, const cgc_String *cgc_tag)
    : cgc_Node(cgc_ns, cgc_tag, NodeClass::NODE)
{
}

cgc_Node::~cgc_Node()
{
    // cgc_free all of the attributes
    cgc_Attribute **pattr = nullptr;
    while ((pattr = d_attr.cgc_next(pattr)) != nullptr)
    {
        delete (*pattr);
    }
}

cgc_Node *cgc_Node::cgc_create(const char *cgc_ns, const char *cgc_tag)
{
    const cgc_String *s = cgc_String::cgc_intern(cgc_tag);
    const cgc_String *nss = cgc_ns == nullptr ? nullptr : cgc_String::cgc_intern(cgc_ns);
    if (s == CML_TAG && nss == nullptr)
        return new cgc_CmlNode(nss, s);
    else if (s == EMPTY_TAG && nss == nullptr)
        return new cgc_TextNode(nss, s);
    else
        return new cgc_Node(nss, s);
}

void cgc_Node::cgc_delete_tree(cgc_Node *node)
{
    cgc_Node *child, *cgc_next;
    for (child = node->cgc_first(); child != nullptr; child = cgc_next)
    {
        cgc_next = child->cgc_next();
        cgc_delete_tree(child);
    }
    delete node;
}

bool cgc_Node::cgc_remove_attr(const char *cgc_name)
{
    return d_attr.cgc_remove(cgc_String::cgc_intern(cgc_name));
}

void cgc_Node::cgc_set_attr(const char *cgc_name, cgc_String *value)
{
    const cgc_String *sname = cgc_String::cgc_intern(cgc_name);
    cgc_Attribute **result, *cgc_attr;
    result = d_attr.cgc_lookup(sname);
    if (result == nullptr)
    {
        cgc_attr = new cgc_Attribute(sname);
        d_attr.cgc_insert(sname, cgc_attr);
    }
    else
        cgc_attr = *result;
    cgc_attr->cgc_set(value);
}

void cgc_Node::cgc_insert_after(cgc_Node *node, cgc_Node *new_node)
{
    if (node == nullptr)
        node = cgc_last();
    if (node == nullptr)
    {
        d_first = d_last = new_node;
        new_node->d_next = new_node->d_prev = nullptr;
    }
    else if (node == d_last)
    {
        d_last = new_node;
        node->d_next = new_node;
        new_node->d_prev = node;
        new_node->d_next = nullptr;
    }
    else
    {
        new_node->d_prev = node;
        new_node->d_next = node->d_next;
        new_node->d_prev->d_next = new_node;
        new_node->d_next->d_prev = new_node;
    }
    new_node->d_parent = this;
}

void cgc_Node::cgc_insert_before(cgc_Node *node, cgc_Node *new_node)
{
    if (node == nullptr)
        node = cgc_first();
    if (node == nullptr)
    {
        d_first = d_last = new_node;
        new_node->d_next = new_node->d_prev = nullptr;
    }
    else if (node == d_first)
    {
        d_first = new_node;
        node->d_prev = new_node;
        new_node->d_prev = nullptr;
        new_node->d_next = node;
    }
    else
    {
        new_node->d_prev = node->d_prev;
        new_node->d_next = node;
        new_node->d_prev->d_next = new_node;
        new_node->d_next->d_prev = new_node;
    }
    new_node->d_parent = this;
}

void cgc_Node::cgc_remove()
{
    if (d_parent == nullptr)
        return;
    if (d_next != nullptr)
        d_next->d_prev = d_prev;
    if (d_prev != nullptr)
        d_prev->d_next = d_next;
    if (d_parent->d_first == this)
        d_parent->d_first = d_next;
    if (d_parent->d_last == this)
        d_parent->d_last = d_prev;
    d_parent = d_next = d_prev = nullptr;
}
