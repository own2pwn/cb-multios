/*

Author: Bryce Kerley <bk@cromulence.co>

Copyright (c) 2016 Cromulence LLC

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

#include <libcgc.h>
#include "types.h"

#define c(NAME, SIZE) cgc_chk(SIZE, sizeof(NAME))

void cgc_chk(cgc_size_t expected, cgc_size_t got);

// check assumptions about type sizes
void cgc_types_check() {
  c(cgc_uint8, 1);
  c(cgc_sint8, 1);
  c(cgc_uint16, 2);
  c(cgc_sint16, 2);
  c(cgc_uint32, 4);
  c(cgc_sint32, 4);
  c(cgc_uint64, 8);
  c(cgc_sint64, 8);
  c(cgc_float32, 4);
  c(cgc_float64, 8);
}

void cgc_chk(cgc_size_t expected, cgc_size_t got) {
  if (expected == got) return;

  _terminate(-1);
}
