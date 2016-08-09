/*
 * Copyright (c) 2015 Kaprica Security, Inc.
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
#include "picture.h"

extern "C" {
#include <ctype.h>
};

cgc_Picture::cgc_Picture(void *_header, void *data, char *_filename)
: cgc_File(sizeof(cgc_PictureHeader), ((cgc_PictureHeader *)_header)->data_size, _filename)
{
    cgc_memcpy(&header, _header, sizeof(cgc_PictureHeader));
    raw_header = (void *)&header;
    raw_data = new char[cgc_get_data_size() + 1];
    cgc_memcpy(raw_data, data, cgc_get_data_size());
    raw_data[cgc_get_data_size()] = '\0';
}

cgc_Picture::~cgc_Picture()
{
    if (raw_data)
        delete[] raw_data;
    cgc_clear_data_size();
}

unsigned int cgc_Picture::cgc_get_magic()
{
    return header.magic;
}

size_t cgc_Picture::cgc_find_header_size()
{
    return sizeof(cgc_PictureHeader);
}

size_t cgc_Picture::cgc_find_data_size(void *header)
{
    return ((cgc_PictureHeader *)header)->data_size;
}

void cgc_Picture::cgc_print_picture()
{
    for (size_t i = 0; i < cgc_get_data_size(); i++) {
        if (cgc_isprint(raw_data[i]) || raw_data[i]=='\n' || raw_data[i]=='\t')
            printf("%c", raw_data[i]);
        else
            printf(" ");
    }
    printf("\n");
}
