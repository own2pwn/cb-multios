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
#ifndef CANVAS_H
#define CANVAS_H

#define CANVAS_MAX_LAYERS 6
#define CANVAS_MAX_COLORS 20
#define CANVAS_DEFAULT_COLOR CANVAS_MAX_COLORS

#pragma pack(push, 1)
typedef struct RGB_Color_s {
  cgc_uint8_t red;
  cgc_uint8_t blue;
  cgc_uint8_t green;
} cgc_RGB_Color;
#pragma pack(pop)

typedef struct Canvas_s {
  cgc_uint16_t y_size;
  cgc_uint16_t x_size;
  cgc_uint8_t num_layers;
  cgc_uint8_t *layers[CANVAS_MAX_LAYERS];
  cgc_RGB_Color colors[CANVAS_MAX_COLORS + 1];
} cgc_Canvas;

int cgc_CreateCanvas(cgc_Canvas **can, cgc_uint16_t y_size, cgc_uint16_t x_size, cgc_uint8_t layers);
void cgc_FlattenCanvas(cgc_Canvas *c);
void cgc_DestroyCanvas(cgc_Canvas **c);

#endif