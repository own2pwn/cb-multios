/*

Author: Debbie Nuttall <debbie@cromulence.com>

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

#include <libcgc.h>
#include "libc.h"

void print(char *buf)
{
   size_t len = cgc_strlen(buf);
   size_t tx = 0;
   size_t nbytes;
   while (tx < len) {
#ifdef _WIN32
   	int res = transmit(STDOUT, (char *)buf + tx, len - tx, &nbytes);
#else
   	int res = transmit(STDOUT, (void *)buf + tx, len - tx, &nbytes);
#endif
   	if (res != 0 || nbytes == 0) {
   	   _terminate(res);
   	}
   	tx += nbytes;
   }
}

// Returns 1 if the strings are equal, otherwise 0
int equals(char *one, char *two)
{
	int i = 0;
	while((one[i]==two[i]) && (one[i]!='\0')) i++;
	return (one[i] == two[i]);
}

// Copy a string from src to dst. Copy ends when null byte reached
// in src string OR when length bytes have been copied to dst.
void cgc_strncpy(char *dst, char *src, size_t length)
{
	size_t pos;
	for(pos = 0; pos<length && src[pos]!='\0'; pos++)
	{
		dst[pos] = src[pos];
	}
	if (pos < length)
		dst[pos] = '\0';
}

