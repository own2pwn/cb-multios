/*

Author: John Berry <hj@cromulence.co>

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

#ifndef __STRING_H__
#define __STRING_H__

#include <libcgc.h>
#include <stdlib.h>

/**
 * Structure to maintain the buffer and index
 **/
typedef struct cgc_string {
        /// Pointer to the data
        char *buffer;

        /// Total length of the cgc_string
        int maxlength;

        /// Current index being examined
        int index;
} cgc_string, *cgc_pstring;

void cgc_freeString(cgc_pstring str );
int cgc_skipAlpha( cgc_pstring str );
int cgc_incChar( cgc_pstring st );
cgc_pstring cgc_initString( char *data );
int cgc_skipWhiteSpace( cgc_pstring st );
int cgc_atChar( cgc_pstring st, char c );
int cgc_skipTo( cgc_pstring st, char c );
int cgc_skipLength( cgc_pstring st, int count );
int cgc_skipToNonAlphaNum( cgc_pstring str );
int cgc_skipToNonAlphaNumSpace( cgc_pstring str );
char *cgc_copyData( cgc_pstring str, int start, int end );
int cgc_skipFloat( cgc_pstring str );
int cgc_skipInt( cgc_pstring str );
int cgc_getIndex( cgc_pstring str, int *outIndex );
int cgc_skipUrl( cgc_pstring str );

#endif
