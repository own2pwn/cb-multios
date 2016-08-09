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
#ifndef PAINT_H
#define PAINT_H
#include "vgf.h"
#include "canvas.h"

cgc_RGB_Color *cgc_GetColor(cgc_Canvas *c, cgc_uint16_t y, cgc_uint16_t x, cgc_uint8_t layer);
int cgc_GetColorIndex(cgc_Canvas *c, cgc_uint16_t y, cgc_uint16_t x, cgc_uint8_t layer);
void cgc_SetColor(cgc_Canvas *c, cgc_uint16_t y, cgc_uint16_t x, cgc_uint8_t layer, cgc_uint8_t color);
void cgc_PaintTriangle(cgc_Canvas *c, cgc_uint8_t layer, cgc_uint8_t color, cgc_uint8_t fill, cgc_VGF_Triangle *triangle);
void cgc_ConnectPoints(cgc_Canvas *c, cgc_uint8_t layer, cgc_uint8_t color, cgc_uint16_t x_start, cgc_uint16_t y_start, cgc_uint16_t x_end, cgc_uint16_t y_end);
void cgc_PaintRectangle(cgc_Canvas *c, cgc_uint8_t layer, cgc_uint8_t color, cgc_uint8_t fill, cgc_VGF_Rectangle *rectangle);
void cgc_PaintSquare(cgc_Canvas *c, cgc_uint8_t layer, cgc_uint8_t color, cgc_uint8_t fill, cgc_VGF_Square *square);
void cgc_PaintSpray(cgc_Canvas *c, cgc_uint8_t layer, cgc_uint8_t color, cgc_uint8_t fill, cgc_VGF_Spray *spray);
void cgc_PaintLine(cgc_Canvas *c, cgc_uint8_t layer, cgc_uint8_t color, cgc_uint8_t fill, cgc_VGF_Line *line);
void cgc_PaintCircle(cgc_Canvas *c, cgc_uint8_t layer, cgc_uint8_t color, cgc_uint8_t fill, cgc_VGF_Circle *circle);

#endif