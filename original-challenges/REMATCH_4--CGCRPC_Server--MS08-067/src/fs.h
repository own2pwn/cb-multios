/*

Author: Debbie Nuttall <debbie@cromulence.com>

Copyright (c) 2016 Cromulence LLC

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
#ifndef FILE_H
#define FILE_H

#include "stdint.h"

#define MAX_TREES 10
#define MAX_FILES_PER_TREE 10

// Error codes
#define FS_ACCESS_DENIED  0x10
#define FS_FILE_NOT_FOUND 0x20

// File modes
#define FS_MODE_CREATE    0x31
#define FS_MODE_READ      0x32
#define FS_MODE_OVERWRITE 0x33
#define FS_MODE_WRITE     0x34

#define MAX_FILENAME_LEN  128
#define MAX_TREENAME_LEN  128
#define MAX_SERVICE_NAME  64
#define MAX_SERVICE_TYPES 5
#define MAX_FILESIZE      1024

typedef struct file_s {
  cgc_uint16_t fileID;
  char filename[MAX_FILENAME_LEN];
  cgc_uint16_t numBytes;
  cgc_uint8_t isOpen;
  cgc_uint8_t *bytes;
} cgc_fs_file;

typedef struct tree_s {
  cgc_uint32_t treeID; 
  char treeName[MAX_TREENAME_LEN];
  char serviceType[MAX_SERVICE_NAME];
  cgc_fs_file *files[MAX_FILES_PER_TREE];
} cgc_fs_tree;

extern cgc_fs_tree allTrees[MAX_TREES];
extern char serviceTypes[MAX_SERVICE_TYPES][MAX_SERVICE_NAME];

void cgc_InitializeFileSystem();
cgc_fs_file *cgc_CreateFile(cgc_fs_tree *tree, cgc_uint8_t *filename, cgc_uint32_t userID, cgc_uint32_t mode);
void cgc_CloseFile(cgc_fs_file *file);
cgc_fs_file *cgc_FindFileByName(cgc_fs_tree *tree, cgc_uint8_t *filename);
int cgc_ReadFile(cgc_uint8_t *dest, cgc_fs_file *file, cgc_uint16_t offset, cgc_uint16_t length);
int cgc_WriteFile(cgc_fs_file *file, cgc_uint8_t *source, cgc_uint16_t offset, cgc_uint16_t length);
cgc_fs_tree *cgc_FindTreeByPath(cgc_uint32_t userID, cgc_uint8_t *path, cgc_uint8_t *service);

#endif



