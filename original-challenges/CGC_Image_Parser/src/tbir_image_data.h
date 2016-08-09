/*
Copyright (c) 2014 Cromulence LLC

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

#ifndef __TBIR_IMAGE_DATA__
#define __TBIR_IMAGE_DATA__

#include <libcgc.h>
#include <stdlib.h>
#include <mymath.h>

#define TBIR_MAGIC 0xb0c4df76
#define TBIR_HEADER 0x00000001
#define TBIR_FLAGS  0x00000010
#define TBIR_PIXELT 0x00000100
#define TBIR_LOADD  0x00001000
#define TBIR_PIXELS 0x00010000
#define TBIR_CHKSUM 0x00100000
#define TBIR_EOF    0x01000000

typedef struct cgc_tbir_image_data {
	char *buffer;
	int max;
	int cbyte;
	int cbit;
	int width;
	int height;
	int pixel_type;
	int load_direction;
	char *image;
	int image_length;
	int checksum;

	/// Used to determine which sections have been encountered
	/// 0x00000001 - Header parsed
	/// 0x00000010 - Flags parsed
	/// 0x00000100 - Pixel type parsed
	/// 0x00001000 - Load direction parsed
	/// 0x00010000 - Pixels parsed
	/// 0x00100000 - Checksum checked
	/// 0x01000000 - EOF reached
	int flags;
} cgc_tbir_image_data, *cgc_ptbir_image_data;

int cgc_tbir_read_check( cgc_ptbir_image_data tid, int bitcount);
int cgc_tbir_read_nbits( cgc_ptbir_image_data tid, int bitcount, int *value);
int cgc_tbir_read_magic( cgc_ptbir_image_data tid );
int cgc_tbir_read_typef( cgc_ptbir_image_data tid );
int cgc_tbir_read_pixeltype( cgc_ptbir_image_data tid );
int cgc_tbir_read_loadd( cgc_ptbir_image_data tid );
int cgc_tbir_read_flags( cgc_ptbir_image_data tid );
int cgc_tbir_read_header( cgc_ptbir_image_data tid );
int cgc_tbir_read_width( cgc_ptbir_image_data tid );
int cgc_tbir_read_height( cgc_ptbir_image_data tid);
int cgc_tbir_display_img( cgc_ptbir_image_data tid);
int cgc_tbir_read_pixels( cgc_ptbir_image_data tid);
int cgc_tbir_read_chksum( cgc_ptbir_image_data tid);
#endif
