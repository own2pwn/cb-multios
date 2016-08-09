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
#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "malloc.h"
#include "filesystem.h"

cgc_FileNode *root;
cgc_uint16_t numFiles;
cgc_uint16_t *pNextFileID = (cgc_uint16_t *)MAGIC_PAGE;


cgc_FileNode *cgc_InitializeFileSystem()
{
  root = cgc_calloc(sizeof(cgc_FileNode));
  cgc_memcpy((char *)&root->name, &"root", 4);
  root->type = FILE_DIRECTORY;
  numFiles = 1;
  return root;
}

cgc_uint16_t cgc_NextFileID()
{
  cgc_uint16_t fileID = *pNextFileID++;
  if (pNextFileID >= (cgc_uint16_t *)(MAGIC_PAGE + 4096))
  {
    pNextFileID =  (cgc_uint16_t *)MAGIC_PAGE;
  }
  return (fileID & 0xff);
}


int cgc_CreateFile(char *name, cgc_uint8_t type, cgc_uint32_t size, char *contents, cgc_FileNode *parent)
{
  int nameLength = cgc_strlen(name);
  if (nameLength == 0)
  {
    return FS_ERROR_INVALID_INPUT;
  }
  if ((parent == NULL)||(parent->type != FILE_DIRECTORY))
  {
    parent = root;
  }
  if (cgc_GetPathDepth(parent) >= MAX_FILESYSTEM_DEPTH)
  {
    return FS_ERROR_ACCESS_DENIED;
  }
  if (type >= FILE_INVALID)
  {
    return FS_ERROR_INVALID_INPUT;
  }
  if (size > MAX_FILE_SIZE)
  {
    return FS_ERROR_INVALID_INPUT;
  }
  if (numFiles >= MAX_FILES_IN_SYSTEM)
  {
    return FS_ERROR_FS_FULL;
  }
  if (cgc_strcmp(name, "upone") == 0)
  {
    return FS_ERROR_INVALID_INPUT;
  }
  if (type == FILE_DIRECTORY)
  {
    size = 0;
  }
  cgc_FileNode *newNode = cgc_calloc(sizeof(cgc_FileNode));
  cgc_memcpy(newNode->name, name, nameLength);
  newNode->type = type;
  newNode->fileID = cgc_NextFileID();
  numFiles++;
  if ((size > 0)&&(contents != NULL))
  {
    newNode->contents = cgc_calloc(size);
    cgc_memcpy(newNode->contents, contents, size);
    newNode->size = size;
  }

  cgc_FileNode *prev = NULL;
  cgc_FileNode *last = NULL;
  prev = parent->child;
  newNode->parent = parent;
  if (prev == NULL)
  {
    // No files in directory, add this file first
    parent->child = newNode;
    return FS_SUCCESS;
  }

  while (prev != NULL)
  {
    int compare = cgc_strcmp(newNode->name, prev->name);
    if (compare == 0)
    {
      cgc_DestroyNode(newNode);
      return FS_ERROR_DUPLICATE_NAME;
    }
    if (compare < 0)
    {
      // Insert here
      if (prev->prev == NULL)
      {
        // First in directory
        newNode->next = prev;
        prev->prev = newNode;
        parent->child = newNode;
        return FS_SUCCESS;
      } else {
        newNode->next = prev;
        newNode->prev = prev->prev;
        prev->prev->next = newNode;
        newNode->next->prev = newNode;
        return FS_SUCCESS;
      }
    } else {
      // Keep looking
      last = prev;
      prev = prev->next;
    }
  }
  // Got to end of directory, insert here
  last->next = newNode;
  newNode->prev = last;
  return FS_SUCCESS;
}

void cgc_DestroyNode(cgc_FileNode *node)
{
  if (node)
  {
    numFiles--;
    cgc_DestroyNode(node->child);
    cgc_DestroyNode(node->next);
    cgc_free(node->contents);
  }
}

cgc_FileNode *cgc_FindFile(char *name, cgc_FileNode *parent)
{
  if ((parent == NULL)||(name == NULL))
  {
    return NULL;
  }
  cgc_FileNode *file = parent->child;
  while (file != NULL)
  {
    int compare = cgc_strcmp(name, file->name);
    if (compare == 0)
    {
      return file;
    }
    if (compare < 0)
    {
      break;
    }
    file = file->next;
  }
  return NULL;
}

cgc_FileNode *cgc_FindFileAbsolute(char *name)
{
  if (name == NULL)
  {
    return NULL;
  }
  char folder[65];
  char *namePtr = name;
  char *sepPtr = name;
  cgc_FileNode *cwd = root;
  while (*namePtr != '\0')
  {
    sepPtr = cgc_strchr(namePtr, '%');
    if (sepPtr == NULL)
    {
      break;
    }
    cgc_memset(folder, '\0', 65);
    cgc_memcpy(folder, namePtr, (sepPtr - namePtr));
    namePtr = sepPtr + 1;
    cwd = cgc_FindFile(folder, cwd);
    if (cwd == NULL)
    {
      return NULL;
    }
  }
  return cgc_FindFile(namePtr, cwd);
}




int cgc_DeleteFile(char *name, cgc_FileNode *parent)
{
  cgc_FileNode *file = cgc_FindFile(name, parent);
  if (file == NULL)
  {
    return FS_ERROR_FILE_NOT_FOUND;
  }
  if (file == root)
  {
    return FS_ERROR_ACCESS_DENIED;
  }
  if (parent->child == file)
  {
    parent->child = file->next;
  }
  if (file->prev)
  { 
    file->prev->next = file->next;
  }
  if (file->next != NULL)
  {
    file->next->prev = file->prev;
  }
  file->prev = NULL;
  file->next = NULL;
  cgc_DestroyNode(file);
  return FS_SUCCESS;
}

cgc_uint8_t cgc_GetFileType(cgc_FileNode *file)
{
  if (file == NULL)
  {
    return 0;
  }
  return file->type;
}

cgc_uint32_t cgc_GetFileSize(cgc_FileNode *file)
{
  if (file == NULL)
  {
    return 0;
  }
  return file->size;
}

cgc_uint32_t cgc_GetFileID(cgc_FileNode *file)
{
  if (file == NULL)
  {
    return 0;
  }
  return file->fileID;
}

char *cgc_GetFilePath(cgc_FileNode *file)
{
  if (file == NULL)
  {
    return NULL;
  }
  if (file == root)
  {
    return NULL;
  }
  int pathLength = 4 + 1;
  cgc_FileNode *parent = file->parent;
  while (parent != root)
  {
    pathLength += cgc_strlen(parent->name) + 1;
    parent = parent->parent;
  }
  char *path = cgc_calloc(pathLength + 1);
  parent = file->parent;
  char *pathPtr = path + pathLength;
  *--pathPtr = '%';
  while (parent != root)
  {
    pathPtr -= cgc_strlen(parent->name);
    cgc_memcpy(pathPtr, parent->name, cgc_strlen(parent->name));
    pathPtr--;
    *pathPtr = '%';
    parent = parent->parent;
  }
  cgc_memcpy(path, &"root", 4);
  return path;
}

int cgc_GetPathDepth(cgc_FileNode *file)
{
  int depth = 0;
  cgc_FileNode *parent = file;
  while ((parent != root)&&(parent != NULL))
  {
    depth++;
    parent = parent->parent;
  }
  return depth;
}

char *cgc_ReadFile(cgc_FileNode *file)
{
  
  char *returnData = NULL;
  if (file == NULL)
  {
    return NULL;
  }
  if (file->size > 0)
  {
    returnData = cgc_calloc(file->size + 1);
    cgc_memcpy(returnData, file->contents, file->size);
  }
  return returnData;
}

char *cgc_GetFileName(cgc_FileNode *file)
{
  return file->name;
}

cgc_FileNode *cgc_GetParent(cgc_FileNode *file)
{
  if (file == root)
  {
    return root;
  }
  return file->parent;
}


