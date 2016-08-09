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

#ifndef FILE_H
#define FILE_H

extern "C" {
  #include <stdlib.h>
  #include <string.h>
  #include <stdint.h>
};

#include "llist.h"

class cgc_BaseFile
{
  public:
    cgc_BaseFile() {}
    virtual ~cgc_BaseFile() = 0;

    virtual int cgc_Open() = 0;
    virtual int cgc_Close() = 0;
    virtual int cgc_Read(cgc_size_t pos, cgc_size_t len, char **outBuf) = 0;
    virtual int cgc_Write(cgc_size_t pos, char *inBuf, cgc_size_t len) = 0;
    
    virtual void cgc_PrintFileInfo() = 0;
};

class cgc_File : public cgc_BaseFile
{
  public:
    enum FileType { FT_REG, FT_DIR };

  protected:
    char name[256];
    FileType type;
    cgc_size_t size;
    bool opened;
    union {
      char *content;
      cgc_List<cgc_File *> *files;
    } info;
    cgc_File *parent;

  public:
    cgc_File(const char* name, FileType type, cgc_size_t size, cgc_File* parent);
    ~cgc_File();

    const char* cgc_GetName() { return name; }
    FileType cgc_GetType() { return type; }
    cgc_size_t cgc_GetSize() { return size; }
    bool cgc_IsOpened() { return opened; }
    cgc_List<cgc_File *>* cgc_GetFiles() { return (type == FT_DIR ? info.files : 0); }
    cgc_File* cgc_GetParent() { return parent; }

    int cgc_Open();
    int cgc_Close();
    int cgc_Read(cgc_size_t pos, cgc_size_t len, char **outBuf);
    int cgc_Write(cgc_size_t pos, char *inBuf, cgc_size_t len);
    
    void cgc_PrintFileInfo();
};

#endif
