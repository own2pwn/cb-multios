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
#include "io.h"
#include "picture.h"
#include "asciiart.h"
#include "filetype.h"

extern "C" {
#include <libcgc.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
}

static char iobuf[IO::MAX_IO_SIZE];

char *IO::cgc_buf()
{
    return iobuf;
}

bool IO::cgc_readline(cgc_size_t max_size)
{
    cgc_size_t rx, i = 0;
    if (max_size > MAX_IO_SIZE || max_size < 2)
        return false;

    while (receive(STDIN, &iobuf[i], 1, &rx) == 0 && rx == 1 && i < max_size) {
        if(iobuf[i] == '\n') {
            iobuf[i] = '\0';
            break;
        }

        if(!cgc_isprint(iobuf[i]))
            return false;

        i++;
    }

    if (i == max_size || i == 0)
        return false;

    return true;
}

int IO::cgc_readnum(cgc_size_t max_size)
{
    cgc_size_t rx, i = 0;
    max_size = max_size ? max_size : MAX_IO_SIZE;

    while (receive(STDIN, &iobuf[i], 1, &rx) == 0 && rx == 1 && i < max_size) {
        if(iobuf[i] < '0' || iobuf[i] > '9') {
            iobuf[i] = '\0';
            break;
        }

        i++;
    }

    if (i == max_size || i == 0)
        return 0;

    return cgc_strtol(&iobuf[0], NULL, 10);
}

char *IO::cgc_iotextdup(cgc_size_t max_size)
{
    cgc_size_t rx, i = 0, multiplier = 0;
    cgc_size_t tilde_count = 0; //``` = cgc_exit str
    char *strbuf;
    if (max_size < 3)
        return NULL;

    strbuf = new char[MAX_IO_SIZE];
    if (!strbuf)
        return NULL;

#ifdef PATCHED
    while (i < max_size && receive(STDIN, &strbuf[i], 1, &rx) == 0 && rx == 1) {
#else
    while (receive(STDIN, &strbuf[i], 1, &rx) == 0 && rx == 1 && i < max_size) {
#endif
        if (tilde_count == 3)
            break;

        tilde_count = strbuf[i++] == '`' ? tilde_count+1 : 0;
        if (i == (MAX_IO_SIZE << multiplier)) {
            char *temp = new char[MAX_IO_SIZE << ++multiplier];
            if (!temp) {
                delete[] strbuf;
                return NULL;
            }

            cgc_memcpy(temp, strbuf, MAX_IO_SIZE << multiplier);
            delete[] strbuf;
            strbuf = temp;
        }
    }

    if (tilde_count != 3 && strbuf[i] != '\n') {
        delete[] strbuf;
        return NULL;
    }

    strbuf[i-3] = '\0';
    return strbuf;
}

bool IO::cgc_readnbytes(cgc_size_t nbytes, char *data)
{
    cgc_size_t rx, i = 0;
    if (!nbytes)
        return false;

    while (receive(STDIN, &data[i], 1, &rx) == 0 && rx == 1 && ++i < nbytes);
    if (i != nbytes)
        return false;

    return true;
}

cgc_File *IO::cgc_upload_file()
{
    cgc_size_t rx, i = 0;
    char file_name[64];
    char header[cgc_File::MAX_HEADER_SIZE];
    char *data = NULL;
    unsigned int file_type, magic;
    unsigned int data_size = 0;
    const filetype *ft = NULL;
    cgc_File *new_file = NULL;


    cgc_memset(file_name, 0, sizeof(file_name));
    printf("Enter Filename: ");
    if (IO::cgc_readline()) {
        if (cgc_strlen(IO::cgc_buf()) > 2 && cgc_strlen(IO::cgc_buf()) < 64) {
            cgc_memcpy(file_name, IO::cgc_buf(), cgc_strlen(IO::cgc_buf()));
        }
        else {
            cgc_memcpy(file_name, IO::cgc_buf(), sizeof(file_name) - 1);
        }
    }

    printf("--Begin Uploading cgc_File--\n");
    if (!IO::cgc_readnbytes(sizeof(unsigned int), header)) {
        printf("Could not read magic num\n");
        return NULL;
    }

    magic = ((unsigned int *)header)[0];

    ft = cgc_get_filetype_m(magic);
    if (!ft) {
        printf("Unsupported file\n");
        return NULL;
    }

    if (ft->type == ASCIIART) {
        if (!IO::cgc_readnbytes(cgc_AsciiArt::cgc_find_header_size() - sizeof(magic), &header[sizeof(magic)])){
            printf("Could not read header\n");
            return NULL;
        }

        data_size = cgc_AsciiArt::cgc_find_data_size(header);
        data = new char[data_size];
        if (!IO::cgc_readnbytes(data_size, data)) {
            printf("Could not read data\n");
            delete[] data;
            return NULL;
        }

        new_file = new cgc_AsciiArt(header, data, file_name);
        if (!new_file->cgc_get_data_size()) {
            printf("Malformed Data\n");
            delete[] data;
            delete new_file;
            return NULL;
        }

        return new_file;
    } else if (ft->type == PICTURE) {
        if (!IO::cgc_readnbytes(cgc_Picture::cgc_find_header_size() - sizeof(magic), &header[sizeof(magic)])){
            printf("Could not read header\n");
            return NULL;
        }

        data_size = cgc_Picture::cgc_find_data_size(header);

        data = new char[data_size];
        if (!IO::cgc_readnbytes(data_size, data)) {
            printf("Could not read data\n");
            delete[] data;
            return NULL;
        }

        new_file = new cgc_Picture(header, data, file_name);
        if (!new_file->cgc_get_data_size()) {
            printf("Malformed Data\n");
            delete[] data;
            delete new_file;
            return NULL;
        }

        delete[] data;
        return new_file;
    } else {
        printf("Unsupported Upload Type\n");
        return NULL;
    }
}

