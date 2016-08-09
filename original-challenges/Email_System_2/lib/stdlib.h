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
#ifndef __STDLIB_H__
#define __STDLIB_H__

#define INUSE_FLAG 1
#define FREE_FLAG 2

#include <libcgc.h>

typedef struct _heap_block_header {
	size_t remaining_size;
	struct _heap_block_header *next;
	char data[1];
} cgc_heap_block_header;


typedef struct _heap_header {
	size_t size;
	char flags;
} cgc_heap_header;

typedef struct _heap_metadata {
	size_t mem_commit;
	size_t mem_free;
	size_t mem_inuse;
	cgc_heap_block_header *blocks;
} cgc_heap_metadata;

void *cgc_calloc(size_t count, size_t size);
void cgc_free(void *ptr);
void *cgc_malloc(size_t size);



int cgc_isspace( int c );
int cgc_isdigit( int c );
int cgc_isnan( double val );
int cgc_isinf( double val );
double cgc_atof(const char *str);
int cgc_atoi(const char *str);
int cgc_islower( int c );
int cgc_isupper( int c );
int cgc_isalpha( int c );
int cgc_isalnum( int c );
int cgc_tolower( int c );
int cgc_toupper( int c );
int cgc_memcpy( void *dest, void *src, size_t n);

char *cgc_strcpy( char *dest, char *src );
char *cgc_strncpy( char *, const char *, size_t );
int cgc_putc( int );
int cgc_printf( const char *fmt, ... );
void cgc_bzero( void *, size_t );
void *cgc_memset(void *, int, size_t);
int cgc_strcmp( const char *, const char * );
int cgc_stricmp( const char *, const char * );
char *cgc_strncat( char *dest, const char *src, size_t n );
int cgc_flush_input(int);
size_t cgc_getline( char *buffer, size_t len);
size_t cgc_receive_until( char *, char, size_t );
int cgc_receive_bytes (unsigned char *buffer, size_t size);
size_t cgc_strcat( char *, char* );
size_t cgc_strlen( char * );
size_t cgc_itoa( char *, size_t, size_t );
void cgc_puts( char *t );
char *cgc_strchr(const char *, int);
char *cgc_strtok(char *, const char *);
cgc_ssize_t cgc_write( const void *, size_t );
char *cgc_strdup( char * );

#endif // __STDLIB_H__
