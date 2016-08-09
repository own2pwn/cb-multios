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
#include <stdio.h>
#include <string.h>

#include "safe.h"
#include "trie.h"

static size_t TrieCount = ROOT_IDENTIFIER;

void cgc_AllocateAndInitializeTrieRoot(cgc_trie** Trie)
{
  if (!Trie)
    return;

  *Trie = cgc_xcalloc(sizeof(cgc_trie), 1);
  (*Trie)->Data = 0;
  (*Trie)->Parent = NULL;
  (*Trie)->Terminal = 0;
  (*Trie)->Identifier = TrieCount;
  TrieCount++;
}

cgc_trie* cgc_InitializeTrieChild(cgc_trie_unit Data)
{
  cgc_trie* Child;

  Child = cgc_xcalloc(sizeof(cgc_trie), 1);
  Child->Data = Data;
  Child->Terminal = 0;
  Child->Parent = NULL;
  Child->Identifier = TrieCount;
  TrieCount++;
  return Child;
}

void cgc_FreeTrie(cgc_trie* Trie)
{
  if (Trie)
  {
    for (size_t ChildIndex = 0; ChildIndex < UNIT_CARDINALITY; ++ChildIndex)
    {
      if (Trie->Children[ChildIndex])
      {
        cgc_FreeTrie(Trie->Children[ChildIndex]);
        Trie->Children[ChildIndex] = NULL;
      }
    }

    cgc_free(Trie);
  }
}

void cgc_InsertIntoTrie(cgc_trie* Trie, cgc_trie_unit* Data, size_t DataSize)
{
  if (!Trie)
    return;

  for (size_t DataIndex = 0; DataIndex < DataSize / sizeof(cgc_trie_unit); ++DataIndex)
  {
    cgc_trie* Child;

    if (!Trie->Children[Data[DataIndex]])
    {
      Child = cgc_InitializeTrieChild(Data[DataIndex]);
      Trie->Children[Data[DataIndex]] = Child;
    }
    else
    {
      Child = Trie->Children[Data[DataIndex]];
    }

    Child->Parent = Trie;
    Trie = Trie->Children[Data[DataIndex]];
  }

  Trie->Terminal = 1;
}

cgc_trie* cgc_FindInTrie(cgc_trie* Trie, cgc_trie_unit* Data, size_t DataSize)
{
  for (size_t DataIndex = 0; DataIndex < DataSize; ++DataIndex)
  {
    if (Trie->Children[Data[DataIndex]])
      Trie = Trie->Children[Data[DataIndex]];
    else
      return NULL;
  }

  return Trie->Terminal ? Trie : NULL;
}

cgc_trie* cgc_FindInTrieByIdentifier(cgc_trie* Trie, size_t Identifier)
{
  cgc_trie* Found = NULL;

  if (Trie->Identifier == Identifier)
    return Trie;

  if (!Trie)
    return NULL;

  for (size_t UnitIndex = 0; UnitIndex < UNIT_CARDINALITY; ++UnitIndex)
  {
    if (Trie->Children[UnitIndex])
    {
      Found = cgc_FindInTrieByIdentifier(Trie->Children[UnitIndex], Identifier);
      if (Found)
        return Found;
    }
  }

  return NULL;
}

size_t cgc_GetTrieCount(void)
{
  return TrieCount;
}

static int cgc__GatherTerminals(cgc_trie* Trie, cgc_trie*** Terminals, size_t* TerminalCount, size_t* TerminalMax)
{
  if (Trie->Terminal)
  {
    (*Terminals)[*TerminalCount] = Trie;
    *TerminalCount += 1;

    if (*TerminalCount == *TerminalMax)
    {
      cgc_trie** NewTerminals = cgc_xcalloc(sizeof(cgc_trie*), 2 * *TerminalMax);
      cgc_memcpy(NewTerminals, *Terminals, *TerminalMax * sizeof(cgc_trie *));
      *TerminalMax = *TerminalMax * 2;
      cgc_free(*Terminals);
      *Terminals = NewTerminals;
    }
  }

  for (size_t ChildIndex = 0; ChildIndex < UNIT_CARDINALITY; ChildIndex++)
  {
    if (Trie->Children[ChildIndex])
    {
      cgc__GatherTerminals(Trie->Children[ChildIndex], Terminals, TerminalCount, TerminalMax);
    }
  }

  return 0;
}

cgc_trie** cgc_GatherTerminals(cgc_trie* Trie, size_t* TerminalCount)
{
#define TERMINAL_START_MAX 4
  cgc_trie** Terminals = cgc_xcalloc(sizeof(cgc_trie*), TERMINAL_START_MAX);
  size_t TerminalMax = TERMINAL_START_MAX;

  *TerminalCount = 0;

  cgc__GatherTerminals(Trie, &Terminals, TerminalCount, &TerminalMax);

  return Terminals;
}

int cgc_ReverseArray(cgc_trie_unit* String, size_t ArraySize)
{
  if (!String || !ArraySize)
    return -1;

  cgc_trie_unit Temp;
  size_t Index = 0;

  while (Index < ArraySize / 2)
  {
    Temp = String[Index];
    String[Index] = String[ArraySize - Index - 1];
    String[ArraySize - Index - 1] = Temp;
    Index++;
  }

  return 0;
}

cgc_trie_unit* cgc_GetDataString(cgc_trie* Trie, size_t* DataLength)
{
  if (!Trie)
    return NULL;

  size_t StringLength = 64;
  size_t StringIndex = 0;
  cgc_trie_unit* String = cgc_xcalloc(sizeof(cgc_trie_unit), 64);

  while (Trie)
  {
    String[StringIndex++] = Trie->Data;
    if (StringIndex == StringLength)
    {
      String = cgc_realloc(String, StringLength * 2);
      StringLength *= 2;
    }

    Trie = Trie->Parent;
  }

  // -1 for NULL;
  if (cgc_ReverseArray(String, StringIndex - 1) == 0)
  {
    *DataLength = StringIndex;
    return String;
  }
  else
  {
    return NULL;
  }
}
