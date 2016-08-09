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

#ifndef __3dc_h
#define __3dc_h

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <input.h>
#include <string.h>
#include <math.h>
#include <prng.h>

#define MAGIC_PAGE      0x4347C000
#define MAGIC_PAGE_SIZE 4096

#define MAX_PIXELS      (MAGIC_PAGE_SIZE / sizeof(cgc_t3DCPixel))
#define MAX_FILE_SIZE   (MAX_PIXELS) * sizeof(cgc_t3DCPixel)

#define MIN(a,b)        ((a > b) ? a : b)
#define MAX(a,b)        ((a < b) ? a : b)

#pragma pack (1)
typedef struct {
    cgc_int16_t x;
    cgc_int16_t y;
    cgc_int16_t z;
    cgc_uint8_t r;
    cgc_uint8_t g;
    cgc_uint8_t b;
    cgc_uint8_t a;
} cgc_t3DCPixel;

typedef void (*cgc_task_callback)(cgc_t3DCPixel*,cgc_int16_t);

// Data functions
void cgc_Push(cgc_t3DCPixel **px_list, cgc_t3DCPixel *px);
cgc_t3DCPixel* cgc_Pop(cgc_t3DCPixel **px_list);
void cgc_RunTask(cgc_t3DCPixel **px_list, cgc_task_callback pFunc, cgc_int16_t fptr_arg);
void cgc_ReadFile(cgc_t3DCPixel **px_list);

void cgc_NewFile(cgc_t3DCPixel **px_list, char *buf);

void cgc_ShowPixel(cgc_t3DCPixel *px);

void cgc_CheckFile(cgc_t3DCPixel **px, cgc_int16_t num);

// Image functions
void cgc_RotateX(cgc_t3DCPixel *px, cgc_int16_t rotate_val);
void cgc_RotateY(cgc_t3DCPixel *px, cgc_int16_t rotate_val);
void cgc_RotateZ(cgc_t3DCPixel *px, cgc_int16_t rotate_val);

void cgc_SkewX(cgc_t3DCPixel *px, cgc_int16_t skew_val);
void cgc_SkewY(cgc_t3DCPixel *px, cgc_int16_t skew_val);
void cgc_SkewZ(cgc_t3DCPixel *px, cgc_int16_t skew_val);

void cgc_Scale(cgc_t3DCPixel *px, cgc_int16_t scale_val);

void cgc_Brightness(cgc_t3DCPixel *px, cgc_int16_t bright_val);

void cgc_Opacity(cgc_t3DCPixel *px, cgc_int16_t opacity_val);

// math helpers
cgc_int32_t cgc_multiply(double a, double b);
cgc_int16_t cgc_divide(double a, double b);
cgc_int16_t cgc_degree_to_radian(cgc_int16_t a);

double cgc_cosine(cgc_int16_t a);
double cgc_sine(cgc_int16_t a);

#endif
