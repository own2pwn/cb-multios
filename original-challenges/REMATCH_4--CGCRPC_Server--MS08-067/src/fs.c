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
#include <libcgc.h>
#include "string.h"
#include "stdlib.h"
#include "malloc.h"
#include "stdio.h"
#include "fs.h"

cgc_fs_tree allTrees[MAX_TREES];
char serviceTypes[MAX_SERVICE_TYPES][MAX_SERVICE_NAME];

void cgc_InitializeFileSystem()
{
  // Create Service Types
  for (int i=0; i< MAX_SERVICE_TYPES; i++)
  {
    cgc_populate_random_string(serviceTypes[i], cgc_random_in_range(MAX_SERVICE_NAME/2, MAX_SERVICE_NAME));
  }

  int treeNameLen = cgc_random_in_range(MAX_TREENAME_LEN / 2, MAX_TREENAME_LEN);
  for (int i=0; i < MAX_TREES - 1; i++)
  {
    cgc_fs_tree *tree = &allTrees[i];
    tree->treeID = cgc_rand();
    cgc_populate_random_string(tree->treeName, treeNameLen - i);
    cgc_strcpy(tree->serviceType, serviceTypes[cgc_random_in_range(0, MAX_SERVICE_TYPES - 1)]);
    int numFiles = cgc_random_in_range(1, MAX_FILES_PER_TREE);
    int fileIndex = 0;
    int fileNameLen = cgc_random_in_range(MAX_FILENAME_LEN/2, MAX_FILENAME_LEN);
    while (numFiles > 0)
    {
      char filename[MAX_FILENAME_LEN];
      cgc_populate_random_string(filename, fileNameLen - numFiles);
      cgc_fs_file *file = cgc_CreateFile(tree, (cgc_uint8_t *)filename, 0, FS_MODE_CREATE);
      int length = cgc_random_in_range(10, MAX_FILESIZE);
      cgc_uint8_t *buffer = cgc_calloc(length);
      cgc_populate_random_string((char *)buffer, length);
      cgc_WriteFile(file, buffer, 0, length);
      cgc_CloseFile(file);
      cgc_free(buffer);
      tree->files[fileIndex++] = file;
      numFiles--;
    }
  }

  cgc_fs_tree *lastTree = &allTrees[MAX_TREES - 1];
  lastTree->treeID = cgc_rand();
  cgc_strcpy(lastTree->treeName, "SOMETREE");
  cgc_strcpy(lastTree->serviceType, "EYEPSEE");   
  cgc_fs_file *file = cgc_calloc(sizeof(cgc_fs_file));
  cgc_strcpy(file->filename, "NETSTUFF");
  file->fileID = cgc_rand() & 0xffff;
  lastTree->files[0] = file;
  return;

}

cgc_fs_file *cgc_FindFileByName(cgc_fs_tree *tree, cgc_uint8_t *filename)
{
  for (int i=0; i< MAX_FILES_PER_TREE; i++)
  {
    cgc_fs_file *file = tree->files[i];
    if (file != NULL)
    {
     if (cgc_strcmp(file->filename, (char *)filename) == 0)
      {
        return file;
      }
    }
  }
  return NULL;
}

cgc_fs_file *cgc_CreateFile(cgc_fs_tree *tree, cgc_uint8_t *filename, cgc_uint32_t userID, cgc_uint32_t mode)
{
  cgc_fs_file *file = cgc_FindFileByName(tree, filename);
  if (file == NULL)
  {
    if (mode != FS_MODE_CREATE)
    {
      // File doesn't exist
      return NULL;
    }
    // Create new file
    for (int i=0; i < MAX_FILES_PER_TREE; i++)
    {
      if (tree->files[i] == NULL)
      {
        // Create new file in open slot
        cgc_fs_file *newfile = cgc_calloc(sizeof(cgc_fs_file));
        newfile->fileID = cgc_rand() & 0xffff;
        int len = cgc_strlen((char *)filename);
        if (len > MAX_FILENAME_LEN)
        {
          len = MAX_FILENAME_LEN;
        }
        cgc_strncpy(newfile->filename, (char *)filename, MAX_FILENAME_LEN);
        newfile->isOpen = 1;
        tree->files[i] = newfile;
        return newfile;
      }
    }
    // No open slots
    return NULL;
  }
  
  if (file->isOpen == 1)
  {
    // File already open
    return NULL;
  }
  file->isOpen = 1;
  return file;
}

void cgc_CloseFile(cgc_fs_file *file)
{
  if (file != NULL)
  {
    file->isOpen = 0;
  }
}

int cgc_ReadFile(cgc_uint8_t *dest, cgc_fs_file *file, cgc_uint16_t offset, cgc_uint16_t length)
{
  if ((file == NULL) || (dest == NULL) || (length == 0))
  {
    return -1;
  }
  if (offset > file->numBytes)
  {
    return -1;
  }
  if (offset + length > file->numBytes)
  {
    length = file->numBytes - offset;
  }
  cgc_memcpy(dest, file->bytes + offset, length);
  return 0;
}

int cgc_WriteFile(cgc_fs_file *file, cgc_uint8_t *source, cgc_uint16_t offset, cgc_uint16_t length)
{
  if ((file == NULL) || (source == NULL) || (length == 0))
  {
    return -1;
  }
  if (offset + length > MAX_FILESIZE)
  {
    return -1;
  }
  cgc_uint8_t *oldData = file->bytes;
  file->bytes = cgc_calloc(offset + length);
  if (offset > 0)
  {
    if (oldData != NULL)
    {
      if (file->numBytes >= offset)
      {
        cgc_memcpy(file->bytes, oldData, offset);
      } else {
        cgc_memcpy(file->bytes, oldData, file->numBytes);
      }
    }
  }
  cgc_memcpy(file->bytes + offset, source, length);
  file->numBytes = offset + length;
  cgc_free(oldData);
  return 0;
}

cgc_fs_tree *cgc_FindTreeByPath(cgc_uint32_t userID, cgc_uint8_t *path, cgc_uint8_t *service)
{
  for (int i=0; i < MAX_TREES; i++)
  {
    cgc_fs_tree *tree = &allTrees[i];
    if (cgc_strcmp(tree->treeName, (char *)path) == 0)
    {
      if (cgc_strcmp(tree->serviceType, (char *)service) == 0)
      {
        return tree;
      }
    }
  }
  return NULL;
}
