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
#include "list.h"
#include "safe.h"

void cgc_AllocateAndInitializeListHead(cgc_list** List, void* Value)
{
  if (!List)
    return;

  *List = cgc_xcalloc(1, sizeof(cgc_list));
  (*List)->Value = Value;
}

void cgc_FreeList(cgc_list* List)
{
  if (List)
  {
    if (List->Next)
    {
      cgc_FreeList(List->Next);
      List->Next = NULL;
    }

    cgc_free(List);
  }
}

cgc_size_t cgc_LenList(cgc_list* List)
{
  cgc_size_t Len = 0;
  for (; List; Len++)
    List = List->Next;

  return Len;
}

void cgc_AppendToList(cgc_list* List, void* Value)
{
  if (!List)
    return;

  while (List->Next)
    List = List->Next;

  List->Next = cgc_xcalloc(1, sizeof(cgc_list));
  List->Next->Value = Value;
}

void cgc_UniqAppendToList(cgc_list* List, void* Value)
{
  while (List->Next)
  {
    if (List->Value == Value)
      return;
    else
      List = List->Next;
  }

  if (List->Value != Value)
    cgc_AppendToList(List, Value);
}

cgc_list* cgc_UniqExtendList(cgc_list* L1, cgc_list* L2)
{
  cgc_list* Ret;
  cgc_list* L1Walker;
  if (!L1 && L2)
  {
    return L2;
  }
  else if (L1 && !L2)
  {
    return L1;
  }
  else if (L1 && L2)
  {
    Ret = L1;

    while (L2)
    {
      L1Walker = L1;
      while (L1Walker)
      {
        if (L1Walker->Value == L2->Value)
        {
          break;
        }
        else
        {
          // Check next value in L1
          L1Walker = L1Walker->Next;
        }
      }

      // Didn't find L2 value in L1.
      if (!L1Walker)
      {
        cgc_AppendToList(L1, L2->Value);
      }

      // Move onto next value in L2 that we want to add.
      L2 = L2->Next;
    }

    return Ret;
  }
  else
  {
    return NULL;
  }
}

int cgc_CheckForCycle(cgc_list* List)
{
  if (!List)
    return 0;

  cgc_list* Speedy = List->Next;

  while (Speedy && List)
  {
    if (Speedy == List)
      return 1;

    if (Speedy->Next == NULL)
      break;

    Speedy = Speedy->Next->Next;
    List = List->Next;
  }

  return 0;
}

void cgc_PrintList(cgc_FILE* Stream, cgc_list *List)
{
  while (List)
  {
    if (List->Next)
      cgc_fprintf(Stream, "%x:%s ", List, List->Value);
    else
      cgc_fprintf(Stream, "%x:%s\n", List, List->Value);

    List = List->Next;
  }
}
