/*
 * Copyright (c) 2016 Kaprica Security, Inc.
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
#include <cstring.h>

cgc_List::cgc_List(void)
{
  cap = MAX_WORKERS;
  size = 0;
}

bool cgc_List::cgc_Append(cgc_Support* s)
{
  if (size == cap)
    return false;

  data[size++] = s;
  return true;
}

cgc_Support* cgc_List::cgc_Get(cgc_uint32_t idx)
{
  if (idx >= size)
  {
    return nullptr;
  }

  return data[idx];
}

cgc_Support* cgc_List::cgc_Remove(cgc_uint32_t idx)
{
  if (idx >= size)
  {
    return nullptr;
  }

  cgc_Support *support = data[idx];
  if (idx < size - 1)
  {
    cgc_memmove(&(data[idx]), &(data[idx + 1]), (size - 1 - idx) * sizeof(idx));
  }

  data[size - 1] = nullptr;
  size--;
  return support;
}

cgc_uint32_t cgc_List::cgc_Length()
{
    return size;
}

