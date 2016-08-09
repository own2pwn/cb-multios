/*

Author: Debbie Nuttall <debbie@cromulence.co>

Copyright (c) 2015 Cromulence LLC

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
#ifndef VGF_H
#define VGF_H
#include "canvas.h"

#define VGF_MAGIC 0x78330909
#define VGF_VERSION 0x0001
#define VGF_MAX_WIDTH 512
#define VGF_MAX_HEIGHT 512

#define VGF_OBJECT_TRIANGLE 1
#define VGF_OBJECT_RECTANGLE 0 
#define VGF_OBJECT_LINE 2 
#define VGF_OBJECT_CIRCLE 3
#define VGF_OBJECT_SQUARE 4
#define VGF_OBJECT_SPRAY 5
#define VGF_OBJECT_LAST 100

#pragma pack(push, 1)

typedef struct VGF_Header_s {
  cgc_uint32_t magic;
  cgc_uint16_t version;
  cgc_uint16_t y_size;
  cgc_uint16_t x_size;
  cgc_uint8_t layers;
} cgc_VGF_Header;


typedef struct VGF_Object_s {
  cgc_uint8_t type;
  cgc_uint8_t layer;
  cgc_uint8_t color;
  cgc_uint8_t fill;
} cgc_VGF_Object;

typedef struct VGF_Triangle_s {
  cgc_uint16_t  x0;
  cgc_uint16_t  y0;
  cgc_uint16_t  x1;
  cgc_uint16_t  y1;
  cgc_uint16_t  x2;
  cgc_uint16_t  y2;
} cgc_VGF_Triangle;

typedef struct VGF_Rectangle_s {
  cgc_uint16_t x_start;
  cgc_uint16_t y_start;
  cgc_uint16_t x_len;
  cgc_uint16_t y_len;
} cgc_VGF_Rectangle;

typedef struct VGF_Square_s {
  cgc_uint16_t x_start;
  cgc_uint16_t y_start;
  cgc_uint16_t x_len;
} cgc_VGF_Square;

typedef struct VGF_Line_s {
  cgc_uint16_t x_start;
  cgc_uint16_t y_start;
  cgc_uint16_t x_end;
  cgc_uint16_t y_end;
} cgc_VGF_Line;

typedef struct VGF_Circle_s {
  cgc_uint16_t x_start;
  cgc_uint16_t y_start;
  cgc_uint16_t radius;
} cgc_VGF_Circle;

typedef struct VGF_Spray_s {
  cgc_uint16_t x_start;
  cgc_uint16_t y_start;
  cgc_uint16_t width;
  cgc_uint16_t density;
  cgc_uint32_t magic;
} cgc_VGF_Spray;

#pragma pack(pop)

int cgc_VGFVerify(cgc_VGF_Header *header);
int cgc_VGFProcess(cgc_uint8_t **file_data_in, cgc_uint8_t *file_end, cgc_Canvas *c);

#endif