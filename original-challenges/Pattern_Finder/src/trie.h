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
#ifndef __TRIE_H__
#define __TRIE_H__

#include <stdint.h>
#include <stdlib.h>

typedef unsigned char cgc_trie_unit;
#ifndef cgc_bool
typedef unsigned char cgc_bool;
#endif

#define UNIT_CARDINALITY 256

typedef struct cgc_trie cgc_trie;
struct cgc_trie
{
  cgc_trie* Parent;
  cgc_trie* Children[UNIT_CARDINALITY];
  cgc_trie_unit Data;
  cgc_bool Terminal;
  size_t Identifier;
};

#define ROOT_IDENTIFIER 0
void cgc_AllocateAndInitializeTrieRoot(cgc_trie** Trie);
void cgc_FreeTrie(cgc_trie* Trie);
void cgc_InsertIntoTrie(cgc_trie* Trie, cgc_trie_unit* Data, size_t DataSize);
cgc_trie_unit* cgc_GetDataString(cgc_trie* Trie, size_t* DataLength);
cgc_trie** cgc_GatherTerminals(cgc_trie* Trie, size_t* TerminalCount);
cgc_trie* cgc_FindInTrie(cgc_trie* Trie, cgc_trie_unit* Data, size_t DataSize);
cgc_trie* cgc_FindInTrieByIdentifier(cgc_trie* Trie, size_t Identifier);
size_t cgc_GetTrieCount(void);
int cgc_TestTrie(void);

#endif /* __TRIE_H__ */
