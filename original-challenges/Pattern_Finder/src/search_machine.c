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
#include <string.h>

#include "assert.h"
#include "safe.h"
#include "search_machine.h"
#include "trie.h"

int cgc_InitializeSearchMachine(cgc_search_machine* SearchMachine, cgc_trie* Trie)
{
  SearchMachine->Trie = Trie;

  // For each keyword in the keyword tree, set out(Node-For-Keyword) = {Keyword}
  size_t TerminalCount;
  cgc_trie** Terminals = cgc_GatherTerminals(Trie, &TerminalCount);

  // Each state has an out (set of patterns recognized upon entering it
  // out :: Map StateID [String]
  SearchMachine->Out = cgc_xcalloc(cgc_GetTrieCount(), sizeof(cgc_list*));
  for (size_t TerminalIndex = 0; TerminalIndex < TerminalCount; ++TerminalIndex)
  {
    size_t DataLength;
    size_t TerminalIdentifier = Terminals[TerminalIndex]->Identifier;
    cgc_trie_unit* DataUpToNode = cgc_GetDataString(Terminals[TerminalIndex], &DataLength);
    Assert(DataUpToNode != NULL, "ISM1");
    cgc_AllocateAndInitializeListHead(&(SearchMachine->Out[TerminalIdentifier]), DataUpToNode);
  }

  SearchMachine->Fail = cgc_xcalloc(cgc_GetTrieCount(), sizeof(cgc_trie *));

  // goto :: Map (StateID, Symbol) StatePtr
  SearchMachine->Goto = cgc_xcalloc(sizeof(void *), cgc_GetTrieCount());

  // Copy over cgc_trie links into goto
  for (size_t TrieIndex = 0; TrieIndex < cgc_GetTrieCount(); TrieIndex++)
  {
    SearchMachine->Goto[TrieIndex] = cgc_xcalloc(sizeof(cgc_trie*), UNIT_CARDINALITY);
    cgc_trie* TrieToCopy = cgc_FindInTrieByIdentifier(Trie, TrieIndex);
    cgc_memcpy(SearchMachine->Goto[TrieIndex], TrieToCopy->Children, sizeof(cgc_trie*) * UNIT_CARDINALITY);
  }

  // g(ROOT, a) := ROOT forall a in SIGMA where a doesn't leave ROOT
  for (size_t AlphabetIndex = 0; AlphabetIndex < UNIT_CARDINALITY; ++AlphabetIndex)
  {
    if (Trie->Children[AlphabetIndex] == NULL)
    {
      SearchMachine->Goto[ROOT_IDENTIFIER][AlphabetIndex] = Trie;
    }
  }

  return 0;
}

void cgc_FreeSearchMachine(cgc_search_machine* SearchMachine)
{
  if (SearchMachine)
  {
    // FreeSignatureDatabase will get the Trie

    for (size_t ListIndex = 0; ListIndex < cgc_GetTrieCount(); ++ListIndex)
    {
      if (SearchMachine->Out[ListIndex])
      {
        cgc_FreeList(SearchMachine->Out[ListIndex]);
        for (size_t OtherListIndex = 0; OtherListIndex < cgc_GetTrieCount(); ++OtherListIndex)
        {
          if (OtherListIndex != ListIndex && SearchMachine->Out[OtherListIndex] == SearchMachine->Out[ListIndex])
          {
            SearchMachine->Out[OtherListIndex] = NULL;
          }
        }
        SearchMachine->Out[ListIndex] = NULL;
      }
    }

    if (SearchMachine->Fail)
    {
      cgc_free(SearchMachine->Fail);
      SearchMachine->Fail = NULL;
    }

    if (SearchMachine->Goto)
    {
      for (size_t GotoIndex = 0; GotoIndex < cgc_GetTrieCount(); ++GotoIndex)
      {
        if (SearchMachine->Goto[GotoIndex])
        {
          cgc_free(SearchMachine->Goto[GotoIndex]);
          SearchMachine->Goto[GotoIndex] = NULL;
        }
      }
    }

    cgc_free(SearchMachine);
  }
}

cgc_match* cgc_FindMatches(cgc_search_machine *SearchMachine, cgc_trie_unit* Data, size_t DataSize, size_t* NumMatches)
{
  size_t MaxMatches = 0;
  cgc_match* Matches = NULL;
  *NumMatches = 0;

  cgc_trie* Node = SearchMachine->Trie;

  for (size_t DataIndex = 0; DataIndex < DataSize; ++DataIndex)
  {
    while (SearchMachine->Goto[Node->Identifier] == NULL || SearchMachine->Goto[Node->Identifier][Data[DataIndex]] == NULL)
    {
      Node = SearchMachine->Fail[Node->Identifier];
    }

    Node = SearchMachine->Goto[Node->Identifier][Data[DataIndex]];

    if (SearchMachine->Out[Node->Identifier] != NULL)
    {
      if (*NumMatches == MaxMatches)
      {
        size_t NewMaxMatches = MaxMatches * 2 + 1;
        Assert(NewMaxMatches > MaxMatches, "MATCH");
        cgc_match* NewMatches = cgc_xcalloc(sizeof(cgc_match), NewMaxMatches);
        cgc_memcpy(NewMatches, Matches, sizeof(cgc_match) * MaxMatches);
        MaxMatches = NewMaxMatches;
        cgc_free(Matches);
        Matches = NewMatches;
      }

      Matches[*NumMatches].Point = DataIndex;
      Matches[*NumMatches].List = SearchMachine->Out[Node->Identifier];
      *NumMatches = *NumMatches + 1;
    }
  }

  return Matches;
}