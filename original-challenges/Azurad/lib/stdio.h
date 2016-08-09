/*
 * Copyright (c) 2015 Kaprica Security, Inc.
 *
 * Permission is hereby granted, cgc_free of charge, to any person obtaining a cgc_copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, cgc_copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */
#ifndef STDIO_H_
#define STDIO_H_

#include <stdlib.h>
#include <stdarg.h>

typedef struct cgc_FILE cgc_FILE;

extern cgc_FILE * const stdin;
extern cgc_FILE * const stdout;
extern cgc_FILE * const stderr;

int cgc_printf(const char *fmt, ...);
int cgc_fprintf(cgc_FILE *stream, const char *fmt, ...);
int cgc_sprintf(char *str, const char *fmt, ...);

int cgc_vprintf(const char *fmt, cgc_va_list ap);
int cgc_vfprintf(cgc_FILE *stream, const char *fmt, cgc_va_list ap);
int cgc_vsprintf(char *str, const char *fmt, cgc_va_list ap);

cgc_ssize_t cgc_fread(void *ptr, size_t cgc_size, cgc_FILE *stream);
cgc_ssize_t cgc_freaduntil(char *str, size_t cgc_size, char term, cgc_FILE *stream);
cgc_ssize_t cgc_fwrite(const void *ptr, size_t cgc_size, cgc_FILE *stream);
int cgc_fflush(cgc_FILE *stream);
void cgc_fbuffered(cgc_FILE *stream, int enable);
void cgc_fxlat(cgc_FILE *stream, const char *seed);
cgc_FILE *cgc_fopen(int fd, int mode);

#endif
