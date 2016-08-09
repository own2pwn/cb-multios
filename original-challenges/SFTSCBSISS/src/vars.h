/*

Author: Joe Rogers <joe@cromulence.co>

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

#define MAX_NAME_LEN (128)
#define MAX_VALUE_LEN (128)

typedef struct _vars {
	char name[MAX_NAME_LEN];
	cgc_uint8_t type;
	unsigned char value[MAX_VALUE_LEN];
	struct _vars *prev;
	struct _vars *next;
} cgc_vars_t;

#define STRING (1)
#define INT32 (2)

cgc_int32_t cgc_InitVARS(void);
cgc_int32_t cgc_DestroyVARS(void);
cgc_vars_t *cgc_SearchVARS(char *target_name);
cgc_vars_t *cgc_CreateVARSObject(char *, cgc_uint8_t, void *);
cgc_int32_t cgc_InsertVARSObject(cgc_vars_t *);
cgc_int32_t cgc_DeleteVARSObject(cgc_vars_t *);
cgc_vars_t *cgc_UpdateVARSObject(char *, cgc_uint8_t, void *);
