/*

Author: Jason Williams <jdw@cromulence.com>

Copyright (c) 2014 Cromulence LLC

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
#ifndef __VGF_H__
#define __VGF_H__

#include <stdint.h>

#define VGF_MAGIC       (0x78330909)
#define VGF_VERSION     (0x0001)

#define     VGF_OBJECT_RECT         (0)
#define     VGF_OBJECT_TRIANGLE     (1)
#define     VGF_OBJECT_LINE         (2)
#define     VGF_OBJECT_CIRCLE       (3)
#define     VGF_OBJECT_END          (100)

#define     VGF_OBJECT_SETTINGS_NOFILL      (0)
#define     VGF_OBJECT_SETTINGS_FILL        (1)

#define     VGF_ERROR_INVALID_FILE      (-1)
#define     VGF_ERROR_INVALID_VERSION   (-2)
#define     VGF_PARSE_SUCCESS           (0)

// VGF maximums
#define VGF_MAX_X       (512)
#define VGF_MAX_Y       (512)

#define VGF_MAX_LAYER   (6)

#define VGF_MAX_COLOR   (200)

// Main file format header
struct VGF_HEADER
{
    cgc_uint32_t    vgfMagic;
    cgc_uint16_t    vgfVersion;
    cgc_uint16_t    vgfHeight;      // Height in pixels
    cgc_uint16_t    vgfWidth;       // Width in pixels
    cgc_uint8_t     vgfLayerCount;  // Number of layers to render on
} __attribute__ ((__packed__));

typedef struct VGF_HEADER cgc_tVGFHeader;

// VGF files are composed of a collection of VGF objects
struct VGF_OBJECT_HEADER
{
    cgc_uint8_t     object_type;
    cgc_uint8_t     object_layer;
    cgc_uint8_t     object_color_index;     // Index to the objects color
    cgc_uint8_t     object_settings;
} __attribute__ ((__packed__));

typedef struct VGF_OBJECT_HEADER cgc_tVGFObjectHeader;

// The VGF color table
struct VGF_COLOR_TABLE
{
    cgc_uint8_t     color_count;
} __attribute__ ((__packed__));

typedef struct VGF_COLOR_TABLE cgc_tVGFColorTable;

// A single color reference in the color index table
struct VGF_COLOR_HEADER
{
    cgc_uint8_t    red;
    cgc_uint8_t    green;
    cgc_uint8_t    blue;
} __attribute__ ((__packed__));

typedef struct VGF_COLOR_HEADER cgc_tVGFColorHeader;

// A VGF circle object
struct VGF_DRAW_CIRCLE
{
    cgc_uint16_t    x_pos;
    cgc_uint16_t    y_pos;
    cgc_uint16_t    radius;
} __attribute__ ((__packed__));

typedef struct VGF_DRAW_CIRCLE cgc_tVGFDrawCircle;

// A VGF rectangle object
struct VGF_DRAW_RECT
{
    cgc_uint16_t    x_start;
    cgc_uint16_t    y_start;
    cgc_uint16_t    x_len;
    cgc_uint16_t    y_len;
} __attribute__ ((__packed__));

typedef struct VGF_DRAW_RECT cgc_tVGFDrawRect;

// A VGF triangle object
struct VGF_DRAW_TRIANGLE
{
    cgc_uint16_t    x_pos1;
    cgc_uint16_t    y_pos1;
    cgc_uint16_t    x_pos2;
    cgc_uint16_t    y_pos2;
    cgc_uint16_t    x_pos3;
    cgc_uint16_t    y_pos3;
} __attribute__ ((__packed__));

typedef struct VGF_DRAW_TRIANGLE cgc_tVGFDrawTriangle;

// A VGF line object
struct VGF_DRAW_LINE
{
    cgc_uint16_t    x_start;
    cgc_uint16_t    y_start;
    cgc_uint16_t    x_end;
    cgc_uint16_t    y_end;
} __attribute__ ((__packed__));

typedef struct VGF_DRAW_LINE cgc_tVGFDrawLine;

typedef struct VGF_OBJECT_TABLE
{
    cgc_tVGFObjectHeader obj_header;
    union
    {
        cgc_tVGFDrawLine        obj_line;
        cgc_tVGFDrawTriangle    obj_triangle;
        cgc_tVGFDrawRect        obj_rect;
        cgc_tVGFDrawCircle      obj_circle;
    } obj_data;

    // Next in the list
    struct VGF_OBJECT_TABLE *pNext;
} cgc_tVGFObjectTable;

typedef struct VGF_PARSED_FILE
{
    cgc_tVGFHeader file_header;
    cgc_tVGFObjectTable *pObjectList;

    cgc_uint8_t color_count;
    cgc_tVGFColorHeader *pColorTable;

} cgc_tVGFParsedFile;

cgc_uint16_t cgc_vgf_get_width( cgc_tVGFParsedFile *pFile );
cgc_uint16_t cgc_vgf_get_height( cgc_tVGFParsedFile *pFile );
cgc_int32_t cgc_vgf_parse_data( cgc_uint8_t *pData, cgc_uint32_t dataLen, cgc_tVGFParsedFile **pParsedFile );
void cgc_vgf_destroy_file( cgc_tVGFParsedFile *pFile );

cgc_int32_t cgc_vgf_get_render_size( cgc_tVGFParsedFile *pFile );
cgc_int32_t cgc_vgf_render_file( cgc_tVGFParsedFile *pFile, cgc_uint8_t *pDest, cgc_uint32_t *pDestLen );

void cgc_vgf_render_rect( cgc_tVGFDrawRect *pRectData, cgc_uint16_t *layer_data, cgc_uint8_t settings, cgc_uint8_t color_index, cgc_uint16_t pixel_width, cgc_uint16_t pixel_height );
void cgc_vgf_render_line( cgc_tVGFDrawLine *pLineData, cgc_uint16_t *layer_data, cgc_uint8_t settings, cgc_uint8_t color_index, cgc_uint16_t pixel_width, cgc_uint16_t pixel_height );
void cgc_vgf_render_triangle( cgc_tVGFDrawTriangle *pTriangleData, cgc_uint16_t *layer_data, cgc_uint8_t settings, cgc_uint8_t color_index, cgc_uint16_t pixel_width, cgc_uint16_t pixel_height );
void cgc_vgf_render_circle( cgc_tVGFDrawCircle *pCircleData, cgc_uint16_t *layer_data, cgc_uint8_t settings, cgc_uint8_t color_index, cgc_uint16_t pixel_wieth, cgc_uint16_t pixel_height );

#endif // __VGF_H___
