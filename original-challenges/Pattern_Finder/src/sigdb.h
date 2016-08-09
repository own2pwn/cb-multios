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
#ifndef __SIGDB_H__
#define __SIGDB_H__

#include "search_machine.h"
#include "stdio.h"
#include "trie.h"

#define MAX_PATH_SIZE 0x40

typedef struct cgc_signature cgc_signature;
struct cgc_signature
{
  enum
  {
    LOW = 1,
    GUARDED,
    ELEVATED,
    HIGH,
    SEVERE
  } Severity;

  char* Path;
  size_t PathSize;

  cgc_trie_unit* Data;
  size_t DataSize;
};

#define MAX_SIGNATURES 0x1000
typedef struct cgc_signature_db cgc_signature_db;
struct cgc_signature_db
{
  size_t SignatureCount;
  cgc_signature* Signatures[MAX_SIGNATURES];
  cgc_trie* Trie;
  cgc_search_machine* SearchMachine;
};

int cgc_InitializeSignatureDatabase(cgc_signature_db* SignatureDatabase);
int cgc_InitializeSignature(cgc_signature* Signature, int Severity, cgc_trie_unit* Data, size_t DataSize, char* Path, size_t PathSize);
void cgc_FreeSignature(cgc_signature* Signature);
char* cgc_SeverityString(unsigned Severity);
void cgc_PrintSignature(cgc_FILE* Stream, cgc_signature* Signature);
unsigned long cgc_BytesToUnsigned(unsigned char *Data, size_t DataSize);
void cgc_FreeSignatureDatabase(cgc_signature_db* SignatureDatabase);
int cgc_AddSignatureToSignatureDatabase(cgc_signature_db* SignatureDatabase, cgc_signature* Signature);
int cgc_BuildSignatureDatabaseSearchMachine(cgc_signature_db* SignatureDatabase);
cgc_list* cgc_SearchSignatureDatabase(cgc_signature_db* SignatureDatabase, cgc_trie_unit* Data, size_t DataSize);

#endif /* __SIGDB_H__ */
