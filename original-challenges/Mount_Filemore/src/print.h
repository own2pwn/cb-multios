/*
 * Copyright (c) 2016 Kaprica Security, Inc.
 *
 * Permission is hereby granted, cgc_free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
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

#ifndef PRINT_H_
#define PRINT_H_

#include "bhdr.h"
#include "fs_info_sector.h"
#include "array.h"
#include "cluster.h"
#include "fs_file.h"

#define PRINT_ARR_CHARS(_array, _size) \
    do { \
        unsigned char * __array = (unsigned char *)(_array); \
        for (int _i=0; _i < (_size); _i++) \
        { \
            cgc_printf("" ESC "c", (__array)[_i]); \
        } \
        cgc_printf(NL); \
    } while (0)

#define PRINT_ARR_CHARS_ERR(_array, _size) \
    do { \
        unsigned char * __array = (unsigned char *)(_array); \
        for (int _i=0; _i < (_size); _i++) \
        { \
            cgc_fprintf(stderr, ESC "c", (__array)[_i]); \
        } \
        cgc_fprintf(stderr, NL); \
    } while (0)

#define PRINT_ARR_BYTES(_array, _size) \
    do { \
        unsigned char * __array = (unsigned char *)(_array); \
        for (int _i=0; _i < (_size); _i++) \
        { \
            cgc_printf("\\x" ESC "02x", (__array)[_i]); \
        } \
        cgc_printf(NL); \
    } while (0)


#define PRINT_ARR_BYTES_ERR(_array, _size) \
    do { \
        unsigned char * __array = (unsigned char *)(_array); \
        for (int _i=0; _i < (_size); _i++) \
        { \
            cgc_fprintf(stderr, "\\x" ESC "02x", (__array)[_i]); \
        } \
        cgc_fprintf(stderr, NL); \
    } while (0)

#define PRINT_ARR_HEXDUMP(_array, _size) \
    do { \
        unsigned char * __array = (unsigned char *)(_array); \
        for (int _i=0; _i < (_size); _i+=16) \
        { \
            for (int _j=0; _j < 16 && _i + _j < (_size); _j++) \
                cgc_printf("\\x" ESC "02x", (__array)[_i + _j]); \
            cgc_printf("   |   "); \
            for (int _j=0; _j < 16 && _i + _j < (_size); _j++) \
            { \
                if ((__array)[_i + _j] == '\n') \
                    cgc_printf("\\n"); \
                else if ((__array)[_i + _j] == '\r') \
                    cgc_printf("\\r"); \
                else if ((__array)[_i + _j] == '\t') \
                    cgc_printf("\\t"); \
                else \
                    cgc_printf(ESC "c", (__array)[_i + _j]); \
            } \
            cgc_printf(NL); \
        } \
        cgc_printf(NL); \
    } while (0)


void cgc_debug_bhdr(const cgc_bhdr &header);
void cgc_debug_fs_info(const cgc_fs_info_sector &fs_info);
void cgc_debug_file_metadata(const cgc_fs_file *file_meta);
void cgc_debug_file(cgc_Array<cgc_cluster_data> file_clusters);

#endif
