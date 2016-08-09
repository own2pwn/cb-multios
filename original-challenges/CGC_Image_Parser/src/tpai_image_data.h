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

#ifndef __TPAI_IMAGE_DATA__
#define __TPAI_IMAGE_DATA__

#include <libcgc.h>
#include <stdlib.h>

#define TPAI_MAGIC 0xcb590f31

typedef struct cgc_tpai_image_data {
	char *buffer;
	int max;
	int cbyte;
	int cbit;
	int width;
	int height;
	int load_direction;
	int checksum;
} cgc_tpai_image_data, *cgc_ptpai_image_data;

int cgc_tpai_calc_checksum( cgc_ptpai_image_data tid );
int cgc_tpai_read_nbits( cgc_ptpai_image_data tid, int bitcount, int *value );
int cgc_tpai_read_check( cgc_ptpai_image_data tid, int bitcount );
int cgc_tpai_read_magic( cgc_ptpai_image_data tid );
int cgc_tpai_read_width( cgc_ptpai_image_data tid );
int cgc_tpai_read_height( cgc_ptpai_image_data tid );
int cgc_tpai_read_loadd( cgc_ptpai_image_data tid, unsigned int *dir);
int cgc_tpai_skip_rsrvd( cgc_ptpai_image_data tid );
int cgc_tpai_display_image( cgc_ptpai_image_data tid );
int cgc_tpai_read_pixel( cgc_ptpai_image_data tid, unsigned int *pixel);

#endif
