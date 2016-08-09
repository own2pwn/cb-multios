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
#include "asciiart.h"

extern "C" {
#include <ctype.h>
#include <string.h>
};

cgc_AsciiArt::cgc_AsciiArt(void *_header, void *data, char *_filename)
: cgc_File(sizeof(cgc_AsciiArtHeader), cgc_find_data_size(_header), _filename)
{
    cgc_memcpy(&header, _header, sizeof(cgc_AsciiArtHeader));
    raw_header = (void *)&header;

    raw_data = new char[cgc_get_data_size() + 1];
    cgc_memcpy(raw_data, data, cgc_get_data_size());
    raw_data[cgc_get_data_size()] = '\0';

    if (!cgc_is_ascii((char *) data)) {
        header.line_width = 0;
        header.num_lines = 0;

        if (raw_data)
            delete[] (raw_data);
        raw_data = NULL;
        cgc_clear_data_size();
    }
}

cgc_AsciiArt::~cgc_AsciiArt()
{
    if (raw_data)
        delete[] raw_data;
    raw_data = NULL;
    cgc_clear_data_size();
}

unsigned int cgc_AsciiArt::cgc_get_magic()
{
    return header.magic;
}

bool cgc_AsciiArt::cgc_is_ascii(char *stream)
{
    char *line = NULL;
    char *_stream = new char[cgc_strlen(stream) + 1];
    cgc_strcpy(_stream, stream);
    int num_lines = 0;
    cgc_size_t len, i;
    while ((line = cgc_strsep(&_stream, "\n")) && num_lines < header.num_lines) {
        len = cgc_strlen(line) + 1;
        if (len != header.line_width) {
            return false;
        } else {
            num_lines++;
        }

        for (i = 0; i < len - 1; i++) {
            if (!cgc_isprint(line[i])) {
                delete[] _stream;
                return false;
            }
        }
    }

    delete[] _stream;
    if (num_lines != header.num_lines)
        return false;

    return true;
}

cgc_size_t cgc_AsciiArt::cgc_find_header_size()
{
    return sizeof(cgc_AsciiArtHeader);
}

cgc_size_t cgc_AsciiArt::cgc_find_data_size(void *header)
{
    cgc_AsciiArtHeader *pheader = (cgc_AsciiArtHeader *)header;
    return (cgc_size_t)(pheader->line_width) * pheader->num_lines;
}

void cgc_AsciiArt::cgc_print_asciiart()
{
    if (raw_data && cgc_get_data_size()) {
        if(!cgc_is_ascii(raw_data)) {
            printf("-------------------------------\n");
            printf("--Ascii Art file is corrupted--\n");
            printf("-------------------------------\n");
            return;
        } else {
            printf("%s\n", raw_data);
        }
    }
}

