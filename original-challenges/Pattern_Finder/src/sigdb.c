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
#include "list.h"
#include "safe.h"
#include "sigdb.h"
#include "queue.h"

/*
 * Signature
 */
int cgc_InitializeSignature(cgc_signature* Signature, int Severity, cgc_trie_unit* Data, size_t DataSize, char* Path, size_t PathSize)
{
  if (!Signature || !Data || !Path)
    return -1;

  if (PathSize > MAX_PATH_SIZE - 1)
    return -1;

  if (Severity < LOW || Severity > SEVERE)
    return -1;

  Signature->Severity = Severity;

  Signature->Data = cgc_xcalloc(DataSize + 1, sizeof(cgc_trie_unit));
  cgc_memcpy(Signature->Data, Data, DataSize * sizeof(cgc_trie_unit));
  Signature->DataSize = DataSize * sizeof(cgc_trie_unit);
  cgc_fprintf(stderr, "(%s)\n", Signature->Data);

  Signature->PathSize = PathSize;
  Signature->Path = cgc_xcalloc(sizeof(char), PathSize + 1);
  cgc_memcpy(Signature->Path, Path, PathSize);

  return 0;
}

void cgc_FreeSignature(cgc_signature* Signature)
{
  if (Signature)
  {
    if (Signature->Data)
    {
      cgc_free(Signature->Data);
      Signature->Data = NULL;
    }

    if (Signature->Path)
    {
      cgc_free(Signature->Path);
      Signature->Path = NULL;
    }

    cgc_free(Signature);
  }
}

unsigned long cgc_BytesToUnsigned(unsigned char *Data, size_t DataSize)
{
  unsigned long hash = 5381;

  for (size_t Index = 0; Index < DataSize; ++Index)
  {
    hash = ((hash << 5) + hash) + Data[Index];
  }

  return hash;
}


char* cgc_SeverityString(unsigned Severity)
{
  switch (Severity)
  {
    case LOW:
      return "LOW";
    case GUARDED:
      return "GUARDED";
    case ELEVATED:
      return "ELEVATED";
    case HIGH:
      return "HIGH";
    case SEVERE:
      return "SEVERE";
    default:
      return "UNKNOWN";
  }
}

void cgc_PrintSignature(cgc_FILE* Stream, cgc_signature* Signature)
{
  cgc_fprintf(Stream, "%s - %s - %x\n",
      Signature->Path,
      cgc_SeverityString(Signature->Severity),
      cgc_BytesToUnsigned(Signature->Data, Signature->DataSize));
}

/*
 * Database
 */
int cgc_InitializeSignatureDatabase(cgc_signature_db* SignatureDatabase)
{
  if (!SignatureDatabase)
    return -1;

  SignatureDatabase->SignatureCount = 0;
  cgc_memset(SignatureDatabase->Signatures, 0, sizeof(cgc_signature *) * MAX_SIGNATURES);

  SignatureDatabase->Trie = NULL;
  SignatureDatabase->SearchMachine = cgc_xcalloc(1, sizeof(cgc_search_machine));

  return 0;
}

void cgc_FreeSignatureDatabase(cgc_signature_db* SignatureDatabase)
{
  if (SignatureDatabase)
  {
    for (size_t SignatureIndex = 0; SignatureIndex < SignatureDatabase->SignatureCount; ++SignatureIndex)
    {
      if (SignatureDatabase->Signatures[SignatureIndex])
      {
        cgc_FreeSignature(SignatureDatabase->Signatures[SignatureIndex]);
        SignatureDatabase->Signatures[SignatureIndex] = NULL;
      }
    }

    if (SignatureDatabase->Trie)
    {
      cgc_FreeTrie(SignatureDatabase->Trie);
      SignatureDatabase->Trie = NULL;
    }

    if (SignatureDatabase->SearchMachine)
    {
      cgc_FreeSearchMachine(SignatureDatabase->SearchMachine);
    }

    cgc_free(SignatureDatabase);
  }
}

int cgc_AddSignatureToSignatureDatabase(cgc_signature_db* SignatureDatabase, cgc_signature* Signature)
{
  if (SignatureDatabase->SignatureCount == MAX_SIGNATURES)
    return -1;

  SignatureDatabase->Signatures[SignatureDatabase->SignatureCount] = Signature;
  SignatureDatabase->SignatureCount += 1;
  return 0;
}

int cgc_BuildSignatureDatabaseTrie(cgc_signature_db* SignatureDatabase)
{
  if (!SignatureDatabase)
    return -1;

  cgc_AllocateAndInitializeTrieRoot(&SignatureDatabase->Trie);

  for (size_t SignatureIndex = 0; SignatureIndex < SignatureDatabase->SignatureCount; ++SignatureIndex)
  {
    cgc_InsertIntoTrie(SignatureDatabase->Trie,
        SignatureDatabase->Signatures[SignatureIndex]->Data,
        SignatureDatabase->Signatures[SignatureIndex]->DataSize);
  }

  return 0;
}

int cgc_BuildSignatureDatabaseSearchMachine(cgc_signature_db* SignatureDatabase)
{
  if (!SignatureDatabase)
    return -1;

  if (!SignatureDatabase->Trie && cgc_BuildSignatureDatabaseTrie(SignatureDatabase) != 0)
    return -1;

  cgc_InitializeSearchMachine(SignatureDatabase->SearchMachine, SignatureDatabase->Trie);

  cgc_search_machine* SM = SignatureDatabase->SearchMachine;

  // Phase 2
  cgc_queue* Queue = cgc_xcalloc(1, sizeof(cgc_queue));
  cgc_InitializeQueue(Queue);

  for (size_t AlphabetIndex = 0; AlphabetIndex < UNIT_CARDINALITY; ++AlphabetIndex)
  {
    cgc_trie* Node = SM->Goto[ROOT_IDENTIFIER][AlphabetIndex];
    //if (Node && Node->Identifier != ROOT_IDENTIFIER)
    if (!Node || Node->Identifier != ROOT_IDENTIFIER)
    {
      SM->Fail[Node->Identifier] = SM->Trie;
      cgc_Enqueue(Queue, Node);
    }
  }

  while (cgc_Peek(Queue) != NULL)
  {
    cgc_trie* R = cgc_Dequeue(Queue);

    for (size_t AlphabetIndex = 0; AlphabetIndex < UNIT_CARDINALITY; ++AlphabetIndex)
    {
      cgc_trie* S = SM->Goto[R->Identifier][AlphabetIndex];

      // Not 100% about this
      if (S)
      {
        cgc_Enqueue(Queue, S);
        cgc_trie* ST = SM->Fail[R->Identifier];

        // Not 100% about this
        while (!SM->Goto[ST->Identifier][AlphabetIndex])
        {
          ST = SM->Fail[ST->Identifier];
        }

        SM->Fail[S->Identifier] = SM->Goto[ST->Identifier][AlphabetIndex];

        // out(u) = out(u) UNION out(fail(u))
        SM->Out[S->Identifier] = cgc_UniqExtendList(SM->Out[S->Identifier], SM->Out[SM->Fail[S->Identifier]->Identifier]);
      }
    }
  }

  cgc_FreeQueue(Queue);
  Queue = NULL;
  return 0;
}

cgc_list* cgc_SearchSignatureDatabase(cgc_signature_db* SignatureDatabase, cgc_trie_unit* Data, size_t DataSize)
{
  size_t NumMatches;
  if (!SignatureDatabase || !Data || !DataSize)
   return NULL;

  if (!SignatureDatabase->SearchMachine)
    return NULL;

  cgc_match* Matches = cgc_FindMatches(SignatureDatabase->SearchMachine, Data, DataSize, &NumMatches);

  // Translate cgc_signature identifiers to signatures
  cgc_list* MatchingSignatures = NULL;
  for (size_t MatchIndex = 0; MatchIndex < NumMatches; ++MatchIndex)
  {
    cgc_list* MatchList = Matches[MatchIndex].List;
    for (; MatchList; MatchList = MatchList->Next)
    {
      for (size_t SignatureDatabaseIndex = 0; SignatureDatabaseIndex < SignatureDatabase->SignatureCount; ++SignatureDatabaseIndex)
      {
        if (cgc_memcmp(SignatureDatabase->Signatures[SignatureDatabaseIndex]->Data,
              MatchList->Value,
              SignatureDatabase->Signatures[SignatureDatabaseIndex]->DataSize) == 0)
        {
          if (!MatchingSignatures)
            cgc_AllocateAndInitializeListHead(&MatchingSignatures, SignatureDatabase->Signatures[SignatureDatabaseIndex]);
          else
            cgc_UniqAppendToList(MatchingSignatures, SignatureDatabase->Signatures[SignatureDatabaseIndex]);
        }
      }
    }
  }

  return MatchingSignatures;
}