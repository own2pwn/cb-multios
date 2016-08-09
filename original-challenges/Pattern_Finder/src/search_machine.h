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
#ifndef __SEARCH_MACHINE_H__
#define __SEARCH_MACHINE_H__

#include "list.h"
#include "trie.h"

typedef struct cgc_search_machine cgc_search_machine;
struct cgc_search_machine
{
  // Root of cgc_trie we built form signatures
  cgc_trie* Trie;

  // Out :: Map TrieIdentifier [Trie]
  cgc_list** Out;

  // Out :: Map TrieIdentifier Trie
  cgc_trie** Fail;

  // Goto :: Map (TrieIdentifier, Char)  Trie
  cgc_trie*** Goto;
};

typedef struct cgc_match cgc_match;
struct cgc_match
{
  // Location in search input where pattern matches
  cgc_size_t Point;

  // List of matching strings
  cgc_list* List;
};

int cgc_InitializeSearchMachine(cgc_search_machine* SearchMachine, cgc_trie* Trie);
void cgc_FreeSearchMachine(cgc_search_machine* SearchMachine);
cgc_match* cgc_FindMatches(cgc_search_machine *SearchMachine, cgc_trie_unit* Data, cgc_size_t DataSize, cgc_size_t* NumMatches);

#endif /* __SEARCH_MACHINE_H */
