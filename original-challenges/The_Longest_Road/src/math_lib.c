/*

Author: James Nuttall (james@cromulence.co)

Copyright (c) 2015 Cromulence LLC

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


#include "math_lib.h"

// Accepts user input to seed the cgc_prng
int cgc_seed_prng()
{
	char buf[256];
	cgc_bzero(buf, sizeof(buf));
 	int status;
 	size_t bytes_received;
 	cgc_printf("Enter crypto seed\n");

 	status = cgc_receive_until(buf, '\n', 255);
	if (status < 2)
	{
		// Error on receive
		return FAIL;
	}

	// create the seed number here
	cgc_uint64_t seed = -1;
	for (int p = 0; p<status; p++)
	{
		seed = ((seed << 4) | (seed >> 60)) ^ buf[p];
	}
	cgc_sprng(seed);

	return status;
}

