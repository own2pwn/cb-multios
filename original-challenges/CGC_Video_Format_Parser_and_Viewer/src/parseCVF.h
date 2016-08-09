/*
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

#ifndef __PARSE_CVF__
#define __PARSE_CVF__
#include <libcgc.h>
#include <stdlib.h>
#include <malloc.h>

#include "bitstream.h"

/**
 * Structure to store the rendering of a single cgc_frame
 **/
typedef struct cgc_frame {
	/// Height of the cgc_frame to be rendered
	unsigned int height;

	/// Width of the cgc_frame to be rendered
	unsigned int width;

	/// Pointer a character array of the rendered image. The length is equal to height*width
	char *image;
} cgc_frame, *cgc_pframe;

/**
 * Structure to store a single pixel dictionary. The length of the array is equal to the "charCount" field
 **/
typedef struct cgc_pixelDict {
	/// Number of characters available in the pixel array
	unsigned int charCount;

	/// Pointer to the array to be used with this dictionary.
	char *pixelArray;
} cgc_pixelDict, *cgc_ppixelDict;

/**
 * Main structure to store a single CVF video
 **/
typedef struct cgc_cvf {
	/// Height of the video in characters
	unsigned int height;

	/// Width of the video in characters.
	unsigned int width;

	/// Number of video frames.
	unsigned int frameCount;

	/// Buffer to store the optional name field
	char name[256];

	/// Buffer to store the optional description field
	char desc[256];

	/// Array of pixel dictionary structures used to render frames using a custom dictionary
	cgc_pixelDict pds[8];

	/// Count of frames rendered
	unsigned int rCount;

	/// Pointer to an array of rendered framed pointers. The length is determined by "frameCount"
	cgc_pframe *renderedFrames;
} cgc_cvf, *cgc_pcvf;

/// Calculates the number of bits needed for a given size.
unsigned int cgc_bitsNeeded( unsigned int length );

int cgc_parseCVFFrame( cgc_pBitStream pbs, cgc_pcvf pNewImage );
void cgc_displayCVFFrame( cgc_pcvf pNewImage );

int cgc_parseCVFPixelDict( cgc_pBitStream pbs, cgc_pcvf pNewImage, int index );

/// Parses the description section
int cgc_parseCVFDescription( cgc_pBitStream pbs, cgc_pcvf pNewImage );

/// Parses the name section.
int cgc_parseCVFName( cgc_pBitStream pbs, cgc_pcvf pNewImage );

/// Parses the header section
int cgc_parseCVFHeader( cgc_pBitStream pbs, cgc_pcvf pNewImage );

/// renders the image
void cgc_renderCVF( cgc_pBitStream pbs );

/// Initializes a new cgc_cvf structure
cgc_pcvf cgc_initCVF( void );

/// Frees the video structure
void cgc_freeCVF( cgc_pcvf cgc_cvf );

#endif
