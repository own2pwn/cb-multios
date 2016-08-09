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
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "malloc.h"
#include "filesystem.h"
#include "loader.h"

cgc_ExecutableInMemory *cgc_LoadSharedLibrary(cgc_FileNode *file)
{
  //Read data from file and verify
  if (file == NULL)
  {
    return NULL;
  }
  cgc_ExecutableInMemory *sl = cgc_calloc(sizeof(cgc_ExecutableInMemory));

  cgc_ExecutableFileHeader *header = (cgc_ExecutableFileHeader *)file->contents;
  if (header->size != file->size)
  {
    cgc_debug_print("Invalid file header: size $d != $d\n", header->size, file->size);
    return NULL;
  }
  if (header->magic != EXECUTABLE_FILE_MAGIC)
  {
    cgc_debug_print("Invalid file header: magic\n");
    return NULL;
  }
  if ((header->type != EXECUTABLE_TYPE_LIBRARY)&&(header->type != EXECUTABLE_TYPE_EXECUTABLE))
  {
    cgc_debug_print("Invalid file header: type\n");
    return NULL;
  }
  if (header->functionCount == 0)
  {
    cgc_debug_print("Invalid file header: functions\n");
    return NULL;
  }
  if (header->functionTableOffset != sizeof(cgc_ExecutableFileHeader))
  {
    cgc_debug_print("Invalid file header: function table offset\n");
    return NULL;
  }
  if (header->resourceCount * sizeof(cgc_ResourceEntry) > file->size - sizeof(cgc_ExecutableFileHeader) - header->functionCount * sizeof(cgc_FunctionTableEntry))
  {
    cgc_debug_print("Invalid file header: resource size\n");
    return NULL;
  }
  if (header->resourceOffset > file->size - header->resourceCount * sizeof(cgc_ResourceEntry))
  {
    cgc_debug_print("Invalid file header: resource offset\n");
    return NULL;
  }
  if (header->functionsSize > file->size - sizeof(cgc_ExecutableFileHeader) - header->functionCount * sizeof(cgc_FunctionTableEntry) - header->resourceCount * sizeof(cgc_ResourceEntry))
  {
    cgc_debug_print("Invalid file header: functions size\n");
    return NULL;
  }
  if (header->functionsOffset > file->size - header->functionsSize)
  {
    cgc_debug_print("Invalid file header: functions size\n");
    return NULL;
  }
  sl->type = header->type;
  sl->functionCount = header->functionCount;
  sl->functionTable = cgc_calloc(header->functionCount * sizeof(cgc_FunctionTableEntry));
  cgc_memcpy(sl->functionTable, file->contents + header->functionTableOffset, header->functionCount * sizeof(cgc_FunctionTableEntry));
  for (int i = 0; i < sl->functionCount; i++)
  {
    sl->functionTable[i].offset -= header->functionsOffset;
  }
  sl->resourceCount = header->resourceCount;
  sl->resourceTable = cgc_calloc(header->resourceCount * sizeof(cgc_ResourceEntry));
  cgc_memcpy(sl->resourceTable, file->contents + header->resourceOffset, header->resourceCount * sizeof(cgc_ResourceEntry));
  sl->functions = cgc_calloc(header->functionsSize);
  cgc_memcpy(sl->functions, file->contents + header->functionsOffset, header->functionsSize);

  cgc_uint8_t *mainAddress = cgc_GetFunctionAddress("SharedLibraryMain", sl);
  cgc_ExecuteFunction(mainAddress);
  return sl;
}

cgc_uint8_t *cgc_GetFunctionAddress(char *name, cgc_ExecutableInMemory *sl)
{
  if (sl == NULL)
  {
    return NULL;
  }
  for(int i = 0; i < sl->functionCount; i++)
  {
    if (cgc_strcmp(name, sl->functionTable[i].name) == 0)
    {
      return sl->functions + sl->functionTable[i].offset;
    }
  }
  return NULL;
}

void cgc_FreeLibrary(cgc_ExecutableInMemory *sl)
{
  if (sl)
  {
    cgc_free(sl->functions);
    cgc_free(sl);
  }
}

int cgc_ExecuteFunction(cgc_uint8_t *address)
{
  if (address == NULL)
  {
    return -1;
  }
  cgc_uint32_t regA = 0;
  char opcode = *address++;
  while (opcode != OPCODE_EXIT)
  {
    switch(opcode)
    {
      case OPCODE_READ_MEM:
      {
        cgc_uint32_t *source = *(cgc_uint32_t **)address; 
        address += 4;
        regA = *source;
        break;
      }
      case OPCODE_WRITE_MEM:
      {
        cgc_uint32_t *dest = *(cgc_uint32_t **)address; 
        address += 4;
        cgc_uint32_t value = *(cgc_uint32_t *)address;
        address += 4;
        *dest = value;
        break;
      }
      case OPCODE_WRITE_REG:
      {
        cgc_uint32_t *dest = *(cgc_uint32_t **)address;
        address += 4;
        *dest = regA;
        break;
      }
      case OPCODE_OUTPUT:
      {
        cgc_transmit_fixed((cgc_uint8_t *)&regA, 4);
        break;
      }
      default:
        cgc_debug_print("Invalid Instruction\n");
        _terminate(-2);
    }
    opcode = *address++;
  }
  return regA;
}

char *cgc_LookupResource(cgc_ExecutableInMemory *sl, cgc_uint16_t index)
{
  if (sl == NULL)
  {
    return NULL;
  }
  if (index >= sl->resourceCount)
  {
    cgc_debug_print("Invalid Resource index $d\n", index);
    return NULL;
  }
  return sl->resourceTable[index].value;
}