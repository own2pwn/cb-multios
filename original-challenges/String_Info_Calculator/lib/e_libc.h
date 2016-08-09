/*
 * Copyright (C) Chris Eagle
 * Copyright (C) Narf Industries <info@narfindustries.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

// this is the libc from EAGLE_00005/pov_1/
// the only change I've made is to define cgc_fprintf() in e_libc.c
// this is only used when debugging.
#ifndef E_LIBC_H
#define E_LIBC_H

// cgc_size_t cgc_strlen(const char *str);
// char *cgc_strcpy(char *dst, const char *src);
// int cgc_strcmp(const char *s1, const char *s2);
// int cgc_memcmp(const char *s1, const char *s2, unsigned int len);
// char *cgc_memcpy(char *s1, const char *s2, unsigned int len);
// char *cgc_strchr(const char *s, int c);
// int cgc_printf(const char *format, ...);

// int cgc_toupper(int c);
// int cgc_tolower(int c);

// int cgc_transmit_all(int fd, const void *buf, const cgc_size_t size);
// int cgc_receive_all(int fd, char *buf, const cgc_size_t size);

#define EOF                  -1

#ifndef NULL
#define NULL ((void*)0)
#endif

#define _FILE_STATE_OPEN  1
#define _FILE_STATE_ERROR 2
#define _FILE_STATE_EOF   4
#define _FILE_HAVE_LAST   8

struct _FILE;
typedef struct _FILE cgc_FILE;

extern cgc_FILE *stdin;
extern cgc_FILE *stdout;
extern cgc_FILE *stderr;

// int  cgc_fgetc(cgc_FILE *);
// int  cgc_getc(cgc_FILE *);
// int  cgc_getchar(void);

// char *cgc_fgets(char *, int, cgc_FILE *);
// int cgc_fread(void *, cgc_size_t, cgc_size_t, cgc_FILE *);

// int ferror(cgc_FILE *stream);
// int feof(cgc_FILE *stream);

int cgc_fprintf(cgc_FILE * stream, const char *format, ...);

#endif
