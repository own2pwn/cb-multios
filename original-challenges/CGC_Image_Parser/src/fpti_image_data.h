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

#ifndef __FPTI_IMAGE_DATA__
#define __FPTI_IMAGE_DATA__
#include <libcgc.h>
#include <mymath.h>
#include <stdlib.h>

#define FPTI_MAGIC 0x24c7ee85

/**
 * Structure to hold the fpti image data.
 * It points to the current byte/bit to be read
 **/
typedef struct cgc_fpti_image_data {
        char *buffer;
        int max;
        int cbyte;
        int cbit;
	char pixel;
} cgc_fpti_image_data, *cgc_pfpti_image_data;

int cgc_fpti_add_pixel( cgc_pfpti_image_data fid, int x, int y, char *image, int xlen, int ylen, int at);
int cgc_fpti_display_img( cgc_pfpti_image_data fid );
int cgc_fpti_read_magic( cgc_pfpti_image_data fid );
int cgc_fpti_read_xaxis( cgc_pfpti_image_data fid );
int cgc_fpti_read_yaxis( cgc_pfpti_image_data fid );
int cgc_fpti_read_axist( cgc_pfpti_image_data fid );
int cgc_fpti_read_ptype( cgc_pfpti_image_data fid, int *ptype);
int cgc_fpti_read_pixel( cgc_pfpti_image_data fid, int *x, int *y);
int cgc_fpti_read_check( cgc_pfpti_image_data fid, int bitcount);
int cgc_fpti_read_nbits( cgc_pfpti_image_data fid, int bitcount, int *value);

#endif
