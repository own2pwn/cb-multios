/*

Copyright (c) 2016 Cromulence LLC

Authors: Dan Czarnecki <cgc@cromulence.com>

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

#ifndef __COMPRESS_H
#define __COMPRESS_H

#include "3dc.h"

#define ABS(x) ((x < 0) ? (-x) : (x))

// 8-bit color
//
//    1  1  1  1  1  1  1  1
//    R  R  R  G  G  B  B  B
//

cgc_uint8_t cgc_ClosestMatch(cgc_uint8_t type, cgc_int16_t val);
void cgc_Compress(cgc_t3DCPixel **px_list, cgc_uint8_t *compressed_data, cgc_uint16_t *data_len);
void cgc_Decompress(cgc_uint8_t *in_data, cgc_uint8_t *out_data, cgc_uint16_t *data_len);
void cgc_WriteOut(cgc_t3DCPixel **px_list, cgc_uint8_t *data, cgc_uint16_t data_len);

#endif
