/*

Author: Bryce Kerley <bk@cromulence.co>

Copyright (c) 2014-2015 Cromulence LLC

Permission is hereby granted, free of charge, to any person obtaining a copy
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

#pragma once

#include "isopropyl.h"

typedef struct {
  cgc_uint32 type;
  cgc_uint32 length;
  void* value;
} cgc_protocol_frame;

cgc_protocol_frame* cgc_allocate_frame(cgc_protocol_frame template);

cgc_protocol_frame* cgc_expect_frame(cgc_uint32 type);
cgc_protocol_frame* cgc_receive_frame();

void cgc_send_frame(cgc_protocol_frame* payload);
void cgc_send_empty_frame(cgc_uint32 type);
void cgc_expect_empty_frame(cgc_uint32 type);

void cgc_free_frame(cgc_protocol_frame* fr);
