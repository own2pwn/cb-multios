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

#include "file_manager.h"

cgc_FileManager::cgc_FileManager()
{
  numFiles = 0;
  numOpenedFiles = 0;
  rootDir = new cgc_File("/", cgc_File::FT_DIR, 0, 0);
  cgc_memset(openedFiles, 0, sizeof(openedFiles));
  cwd = rootDir;
}

cgc_FileManager::~cgc_FileManager()
{
}

cgc_File* cgc_FileManager::cgc_GetFile(const char* name)
{
  int i;
  cgc_List<cgc_File *> *files = cwd->cgc_GetFiles();
  if (!name || cgc_strcmp(name, ".") == 0)
    return cwd;
  if (cgc_strcmp(name, "..") == 0)
    return (cwd == rootDir) ? cwd : cwd->cgc_GetParent();
  if (!files)
    return 0;
  for (i = 0; i < files->cgc_length(); ++i)
  {
    cgc_File *file = files->cgc_get(i);
    if (cgc_strcmp(file->cgc_GetName(), name) == 0)
      return file;
  }
  return 0;
}

void cgc_FileManager::cgc_PrintFile(const char* name)
{
  int i;
  cgc_File *file = cgc_GetFile(name);
  if (!file)
    return;
  if (file->cgc_GetType() == cgc_File::FT_DIR)
  {
    cgc_List<cgc_File *> *files = file->cgc_GetFiles();
    printf("%d entries\n", files->cgc_length() + 2);
    printf("<DIR> D 0 .\n<DIR> D 0 ..\n");
    for (i = 0; files && i < files->cgc_length(); ++i)
    {
      cgc_File *f = files->cgc_get(i);
      f->cgc_PrintFileInfo();
    }
  }
  else
    file->cgc_PrintFileInfo();
}

int cgc_FileManager::cgc_CreateFile(const char* name)
{
  int i;
  if (numFiles == MAX_NUM_FILES)
    return -1;
  cgc_List<cgc_File *> *files = cwd->cgc_GetFiles();
  if (!files)
    return -2;
  if (cgc_strlen(name) > 255)
    return -4;
  if (!cgc_strcmp(name, ".") || !cgc_strcmp(name, "..") || !cgc_strcmp(name, "/"))
    return -3;
  for (i = 0; i < files->cgc_length(); ++i)
  {
    cgc_File *file = files->cgc_get(i);
    if (cgc_strcmp(file->cgc_GetName(), name) == 0)
      return -3;
  }
  cgc_File *file = new cgc_File(name, cgc_File::FT_REG, 0, cwd);
  files->cgc_add(file);
  numFiles++;
  return 0;
}

int cgc_FileManager::cgc_CreateDirectory(const char* name)
{
  int i;
  if (numFiles == MAX_NUM_FILES)
    return -1;
  cgc_List<cgc_File *> *files = cwd->cgc_GetFiles();
  if (!files)
    return -2;
  if (cgc_strlen(name) > 255)
    return -4;
  if (!cgc_strcmp(name, ".") || !cgc_strcmp(name, "..") || !cgc_strcmp(name, "/"))
    return -3;
  for (i = 0; i < files->cgc_length(); ++i)
  {
    cgc_File *file = files->cgc_get(i);
    if (cgc_strcmp(file->cgc_GetName(), name) == 0)
      return -3;
  }
  cgc_File *file = new cgc_File(name, cgc_File::FT_DIR, 0, cwd);
  files->cgc_add(file);
  numFiles++;
  return 0;
}

int cgc_FileManager::cgc_OpenFile(const char* name)
{
  int i;
  if (numOpenedFiles == MAX_NUM_OPENED_FILES)
    return -1;
  cgc_List<cgc_File *> *files = cwd->cgc_GetFiles();
  if (!files)
    return -2;
  if (!cgc_strcmp(name, ".") || !cgc_strcmp(name, "..") || !cgc_strcmp(name, "/"))
    return -3;
  for (i = 0; i < files->cgc_length(); ++i)
  {
    cgc_File *file = files->cgc_get(i);
    if (cgc_strcmp(file->cgc_GetName(), name) == 0)
    {
      if (file->cgc_GetType() == cgc_File::FT_DIR)
        return -3;
      if (file->cgc_IsOpened())
        return -4;
      int j;
      for (j = 0; j < MAX_NUM_OPENED_FILES; ++j)
        if (openedFiles[j] == 0)
          break;
      file->cgc_Open();
      openedFiles[j] = file;
      numOpenedFiles++;
      return j;
    }
  }
  return -5;
}

int cgc_FileManager::cgc_CloseFile(unsigned int fileno)
{
  if (fileno >= MAX_NUM_OPENED_FILES)
    return -1;
  cgc_File *file = openedFiles[fileno];
  if (!file)
    return -2;
  if (file->cgc_GetType() == cgc_File::FT_DIR)
    return -3;
  numOpenedFiles--;
  openedFiles[fileno] = 0;
  return file->cgc_Close();
}

int cgc_FileManager::cgc_CloseAll()
{
  int i;
  for (i = 0; i < MAX_NUM_OPENED_FILES; ++i)
  {
    if(openedFiles[i])
      cgc_CloseFile(i);
  }
  return 0;
}

int cgc_FileManager::cgc_ReadFile(unsigned int fileno, cgc_size_t pos, cgc_size_t len, char** outBuf)
{
  if (fileno >= MAX_NUM_OPENED_FILES)
    return -1;
  cgc_File *file = openedFiles[fileno];
  if (!file)
    return -2;
  return file->cgc_Read(pos, len, outBuf);
}

int cgc_FileManager::cgc_ModifyFile(unsigned int fileno, cgc_size_t pos, char *inBuf, cgc_size_t len)
{
  if (fileno >= MAX_NUM_OPENED_FILES)
    return -1;
  cgc_File *file = openedFiles[fileno];
  if (!file)
    return -2;
  return file->cgc_Write(pos, inBuf, len);
}

int cgc_FileManager::cgc_DeleteFile(const char* name)
{
  int i;
  cgc_List<cgc_File *> *files = cwd->cgc_GetFiles();
  if (!files)
    return -1;
  if (!cgc_strcmp(name, ".") || !cgc_strcmp(name, "..") || !cgc_strcmp(name, "/"))
    return -2;
  for (i = 0; i < files->cgc_length(); ++i)
  {
    cgc_File *file = files->cgc_get(i);
    if (cgc_strcmp(file->cgc_GetName(), name) == 0)
    {
      if (file->cgc_GetType() == cgc_File::FT_REG)
      {
#if PATCHED
        int j;
        for (j = 0; j < MAX_NUM_OPENED_FILES; ++j)
        {
          if (file == openedFiles[j])
          {
            cgc_CloseFile(j);
            break;
          }
        }
#endif
        files->cgc_remove(file);
        delete file;
        numFiles--;
        return 0;
      }
      else
        return -4;
    }
  }
  return -3;
}

void cgc_FileManager::cgc_DeleteDirectoryHelper(cgc_File *dir)
{
  int i;
  cgc_File *file;
  cgc_List<cgc_File *> *files = dir->cgc_GetFiles();
  while (files->cgc_length())
  {
    file = files->cgc_get(0);
    if (file->cgc_GetType() == cgc_File::FT_REG)
    {
      for (i = 0; i < MAX_NUM_OPENED_FILES; ++i)
      {
        if (file == openedFiles[i])
        {
          cgc_CloseFile(i);
          break;
        }
      }
    }
    else
    {
      cgc_DeleteDirectoryHelper(file);
    }
    files->cgc_remove(file);
    numFiles--;
    delete file;
  }
}

int cgc_FileManager::cgc_DeleteDirectory(const char* name)
{
  int i;
  bool found = false;
  cgc_File *file;
  cgc_List<cgc_File *> *files = cwd->cgc_GetFiles();
  if (!files)
    return -1;
  if (!cgc_strcmp(name, ".") || !cgc_strcmp(name, "..") || !cgc_strcmp(name, "/"))
    return -2;
  for (i = 0; i < files->cgc_length(); ++i)
  {
    file = files->cgc_get(i);
    if (cgc_strcmp(file->cgc_GetName(), name) == 0)
    {
      if (file->cgc_GetType() == cgc_File::FT_REG)
        return -3;
      found = true;
      break;
    }
  }
  if (!found)
    return -4;
  cgc_DeleteDirectoryHelper(file);
  files->cgc_remove(file);
  delete file;
  return 0;
}

int cgc_FileManager::cgc_ChangeDirectory(const char* name)
{
  int i;
  cgc_List<cgc_File *> *files = cwd->cgc_GetFiles();
  if (!files)
    return -1;
  if (!name || cgc_strcmp(name, "/") == 0)
  {
    cwd = rootDir;
    return 0;
  }
  if (cgc_strcmp(name, ".") == 0)
    return 0;
  if (cgc_strcmp(name, "..") == 0)
  {
    if (cwd != rootDir)
      cwd = cwd->cgc_GetParent();
    return 0;
  }
  for (i = 0; i < files->cgc_length(); ++i)
  {
    cgc_File *file = files->cgc_get(i);
    if (cgc_strcmp(file->cgc_GetName(), name) == 0)
    {
      if (file->cgc_GetType() != cgc_File::FT_DIR)
        return -2;
      cwd = file;
      return 0;
    }
  }
  return -3;
}
