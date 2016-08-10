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

#include "set.h"

void cgc_with_set(void (^block)(cgc_set_t set)) {
  cgc_set_cell blob[SET_CELLS];
  for (size_t i = 0; i < SET_CELLS; i++) {
    blob[i] = 0;
  }
  cgc_set_t set = blob;
  block(set);
}

void cgc_add_set(cgc_set_t set, cgc_uint16 value) {
  cgc_uint8 mask = 1 << (value & 0x7);
  size_t offset = value >> 3;

  set[offset] |= mask;
}

void cgc_subtract_set(cgc_set_t set, cgc_uint16 value) {
  cgc_uint8 mask = ~(1 << (value & 0x7));
  size_t offset = value >> 3;

  set[offset] &= mask;
}

cgc_uint8 cgc_check_set(cgc_set_t set, cgc_uint16 value) {
  cgc_uint8 mask = 1 << (value & 0x7);
  size_t offset = value >> 3;

  return set[offset] & mask;
}