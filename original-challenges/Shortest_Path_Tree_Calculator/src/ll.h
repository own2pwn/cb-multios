/*

Author: Joe Rogers <joe@cromulence.com>

Copyright (c) 2015 Cromulence LLC

Permission is hereby granted, cgc_free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/
#ifndef LL_H
#define LL_H
#include <libcgc.h>
#include "graph.h"

cgc_pNode cgc_AddNode(cgc_pNode Element);
cgc_pEdge cgc_AddEdge(cgc_pEdge Element);
cgc_pNode cgc_RemoveNode(cgc_pNode Element);
cgc_pEdge cgc_RemoveEdge(cgc_pEdge Element);
cgc_pEdge cgc_FindEdge(cgc_pNode NodeA, cgc_pNode NodeZ);
cgc_pNode cgc_FindNode(cgc_uint32_t TargetName);
cgc_uint8_t cgc_DestroyNodes(void);
cgc_uint8_t cgc_DestroyEdges(void);
#endif
