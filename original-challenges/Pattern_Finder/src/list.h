/*
 * Copyright (c) 2015 Kaprica Security, Inc.
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
#ifndef __LIST_H__
#define __LIST_H__

#include <stdio.h>

typedef struct cgc_list cgc_list;
struct cgc_list
{
  cgc_list* Next;
  void* Value;
};

void cgc_AllocateAndInitializeListHead(cgc_list** List, void* Value);
void cgc_AppendToList(cgc_list* List, void* Value);
void cgc_UniqAppendToList(cgc_list* List, void* Value);
void cgc_FreeList(cgc_list* List);
cgc_list* cgc_UniqExtendList(cgc_list* L1, cgc_list* L2);
size_t cgc_LenList(cgc_list* List);
int cgc_CheckForCycle(cgc_list *List);

#endif /* __LIST_H__ */