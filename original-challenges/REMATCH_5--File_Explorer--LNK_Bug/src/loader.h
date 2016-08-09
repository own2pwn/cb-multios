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
#ifndef LOADER_H
#define LOADER_H

#define EXECUTABLE_FILE_MAGIC 0xC67CC76C

#define EXECUTABLE_TYPE_LIBRARY     0xe1
#define EXECUTABLE_TYPE_EXECUTABLE  0xe2

#define OPCODE_READ_MEM   0x37  
#define OPCODE_WRITE_MEM  0x38
#define OPCODE_WRITE_REG  0x39
#define OPCODE_EXIT       0x40
#define OPCODE_OUTPUT     0x41
#define OPCODE_INPUT      0x42

#pragma pack(push,1)
typedef struct ExecutableFileHeader_s {
  cgc_uint32_t  size;
  cgc_uint32_t  magic;
  cgc_uint8_t   type;
  cgc_uint16_t  functionCount;
  cgc_uint16_t  functionTableOffset;
  cgc_uint16_t  resourceCount;
  cgc_uint16_t  resourceOffset;
  cgc_uint16_t  functionsSize;
  cgc_uint16_t  functionsOffset;
} cgc_ExecutableFileHeader;
// Followed by Function Table
// Followed by Resource Entries
// FOllowed by function bodies

typedef struct FunctionTableEntry_s {
  char      name[64];
  cgc_uint16_t  index;
  cgc_uint32_t  offset; // offset within file when in file, offset in functions when in memory
} cgc_FunctionTableEntry;

typedef struct ResourceEntry_s {
  cgc_uint16_t  type;
  char      value[64];
} cgc_ResourceEntry;
#pragma pack(pop)

typedef struct ExecutableInMemory_s {
  cgc_uint8_t             type;
  cgc_uint16_t            functionCount;
  cgc_FunctionTableEntry  *functionTable;
  cgc_uint16_t            resourceCount;
  cgc_ResourceEntry       *resourceTable;
  cgc_uint8_t             *functions;
} cgc_ExecutableInMemory;

cgc_ExecutableInMemory *cgc_LoadSharedLibrary(cgc_FileNode *file); // Calls SharedLibraryMain inside
cgc_uint8_t *cgc_GetFunctionAddress(char *name, cgc_ExecutableInMemory *sl); // Get the address of the function
void cgc_FreeLibrary(cgc_ExecutableInMemory *sl);
int cgc_ExecuteFunction(cgc_uint8_t *address);
char *cgc_LookupResource(cgc_ExecutableInMemory *sl, cgc_uint16_t index);

#endif