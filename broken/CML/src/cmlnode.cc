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
#include "cmlnode.h"

static const cgc_String *NAMESPACE_ATTR = cgc_String::cgc_intern("namespace");
static const cgc_String *SPEC_ATTR = cgc_String::cgc_intern("cgc_spec");
static const cgc_String *VERSION_ATTR = cgc_String::cgc_intern("cgc_version");

cgc_CmlNode::cgc_CmlNode(const cgc_String *cgc_ns, const cgc_String *cgc_tag)
    : cgc_Node(cgc_ns, cgc_tag, NodeClass::CMLNODE)
{
    d_spec = cgc_String::cgc_intern("");
    d_version = cgc_String::cgc_intern("1.0");
    d_childns = cgc_String::cgc_intern("");
}

void cgc_CmlNode::cgc_set_attr(const char *cgc_name, cgc_String *value)
{
    cgc_Node::cgc_set_attr(cgc_name, value);

    const cgc_String *sname = cgc_String::cgc_intern(cgc_name);
    if (sname == NAMESPACE_ATTR)
    {
        d_childns = value;
    }
    else if (sname == SPEC_ATTR)
    {
        d_spec = value;
    }
    else if (sname == VERSION_ATTR)
    {
        char *end;
        unsigned int major, minor = 0;
        major = cgc_strtoul(value->cgc_cstr(), &end, 10);
        if (end != NULL)
        {
            end++;
            minor = cgc_strtoul(end, &end, 10);
        }
        d_version = value;
        d_version_code = ((unsigned long long)major << 32) | minor;
    }
}
