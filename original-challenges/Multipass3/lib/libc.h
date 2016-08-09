/*
 * Copyright (C) Narf Industries <info@narfindustries.com>
 *
 * Permission is hereby granted, cgc_free of charge, to any person obtaining a
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

#ifndef LIBC_H
#define LIBC_H
#include <libcgc.h>
#define MAX_ALLOCATIONS 65535
#define MAX_LINE_SZ 4096

#define assert(x) \
    do { if (!(x)) { _terminate(10); } } while(0)

typedef unsigned int cgc_uint32_t;
typedef unsigned char cgc_uint8_t;
extern int cgc_transmit_all(int fd, const char *buf, const size_t size);
int cgc_recv_until(int fd, char *buf, const size_t buf_sz, const char delim);
extern size_t cgc_strlen(const char *s);

void * cgc_calloc(size_t nmemb, size_t sz);
extern void cgc_free(void *p);

void cgc_memcpy(void *d, const void *s, size_t size);
int cgc_memcmp(void *d, const void *s, size_t size);
char *cgc_strtok(char *str, const char delim);
size_t cgc_read_ascii_line(int fd, char *data, size_t len);

int cgc_buf_is_numeric(char *buf);
int cgc_atoi(char *buf);
char *cgc_strcat(char * s1, char *s2);
unsigned int cgc_pos_ceil(double num);
void cgc_malloc_reset();
char * cgc_itoaB10(int value);
#endif
