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

#ifndef __FPAI_IMAGE_DATA__
#define __FPAI_IMAGE_DATA__
#include <libcgc.h>
#include <stdlib.h>

#define FPAI_MAGIC 0x55d9b6de

typedef struct cgc_fpai_image_data {
        char *buffer;
        int max;
        int cbyte;
        int cbit;
	int xlen;
	int ylen;
	int axist;
	short checksum;
} cgc_fpai_image_data, *cgc_pfpai_image_data;

int cgc_fpai_add_pixel( cgc_pfpai_image_data fid, char *image, int x, int y, char pixel );
int cgc_fpai_display_img( cgc_pfpai_image_data fid );
int cgc_fpai_read_check( cgc_pfpai_image_data fid, int bitcount );
int cgc_fpai_read_nbits( cgc_pfpai_image_data fid, int bitcount, int *value);
int cgc_fpai_read_magic( cgc_pfpai_image_data fid );
int cgc_fpai_read_xaxis( cgc_pfpai_image_data fid );
int cgc_fpai_read_yaxis( cgc_pfpai_image_data fid );
int cgc_fpai_read_axist( cgc_pfpai_image_data fid );
int cgc_fpai_read_cksum( cgc_pfpai_image_data fid, short *cksum);
int cgc_fpai_calc_cksum( cgc_pfpai_image_data fid);
int cgc_fpai_read_pixel( cgc_pfpai_image_data fid, int *x, int*y, int*pixel);

#endif
