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

#include "file.h"

cgc_BaseFile::~cgc_BaseFile() {}

cgc_File::cgc_File(const char* name, FileType type, cgc_size_t size, cgc_File* parent)
{
  cgc_strcpy(this->name, name);
  this->type = type;
  this->size = size;
  this->parent = parent;
  this->opened = false;
  this->info.content = 0;
  if (this->type == FT_DIR)
    this->info.files = new cgc_List<cgc_File *>();
}

cgc_File::~cgc_File()
{
  if (type == FT_REG && info.content)
    cgc_free(info.content);
  else if (type == FT_DIR && info.files)
    delete info.files;
}

int cgc_File::cgc_Open()
{
  if (opened)
    return 1;
  opened = true;
  return 0;
}

int cgc_File::cgc_Close()
{
  if (!opened)
    return 1;
  opened = false;
  return 0;
}

int cgc_File::cgc_Read(cgc_size_t pos, cgc_size_t len, char **outBuf)
{
  if (type != FT_REG)
    return -10;
  if (!outBuf)
    return -20;
  if (!opened)
    return -30;
  if (size == 0)
    return -40;
  if (pos >= size)
    return -50;
  if (len == 0)
    len = size;
  len = (len > size - pos) ? (size - pos) : len;
  *outBuf = (char *) cgc_calloc(len, sizeof(char));
  if (!*outBuf)
    return -20;
  cgc_memcpy(*outBuf, &info.content[pos], len);
  return len;
}

int cgc_File::cgc_Write(cgc_size_t pos, char *inBuf, cgc_size_t len)
{
  if (type != FT_REG)
    return -10;
  if (!inBuf && len > 0)
    return -20;
  if (!opened)
    return -30;
  if (len == 0)
    return 0;
  if (pos >= size || pos + len > size)
  {
    info.content = (char *) cgc_realloc(info.content, pos + len);
    if (info.content == NULL)
    {
      size = 0;
      return -40;
    }
    size = pos + len;
  }
  cgc_memcpy(&info.content[pos], inBuf, len);
  return len;
}

void cgc_File::cgc_PrintFileInfo()
{
  if (type == FT_REG)
  {
    printf("<FILE> ");
    printf("%s ", (opened ? "O" : "C"));
  }
  else
  {
    printf("<DIR> D ");
  }
  printf("%d ", size);
  printf("%s", name);
  printf("\n");
}