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

#ifndef FS_FILE_H_
#define FS_FILE_H_

#include "cluster.h"
#include "array.h"


#pragma pack(push, 1)
struct cgc_fs_file
{
    unsigned char name[11];
    unsigned char attrib;
    unsigned char reserved[12];
    unsigned int starting_cluster;
    unsigned int size;
};
#pragma pack(push, 1)

namespace CgFsFile
{

bool cgc_FreeEntry(const cgc_fs_file *file_metadata);
bool cgc_SkipEntry(const cgc_fs_file *file_metadata);
bool cgc_IsDotEntry(const cgc_fs_file *file_metadata);
bool cgc_IsDirectory(const cgc_fs_file *file_metadata);
bool cgc_IsFile(const cgc_fs_file *file_metadata);
bool cgc_IsSystemFile(const cgc_fs_file *file_metadata);
bool cgc_IsHiddenFile(const cgc_fs_file *file_metadata);
bool cgc_IsArchive(const cgc_fs_file *file_metadata);
bool cgc_NameEquals(const cgc_fs_file *file_metadata, const char *filename, unsigned int name_len);
void cgc_DeleteMetadataEntry(cgc_fs_file *file_metadata);

};

#endif
