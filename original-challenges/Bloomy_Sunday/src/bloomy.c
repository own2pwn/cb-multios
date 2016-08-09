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

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "bloomy.h"

cgc_bloomy_t* cgc_bloomy_new(size_t size, cgc_hash_t hash1, cgc_hash_t hash2, cgc_hash_t hash3)
{
  cgc_bloomy_t *bloomy;

  if (size < 1)
    return NULL;

  bloomy = (cgc_bloomy_t *) cgc_malloc(sizeof(cgc_bloomy_t));
  if (bloomy == NULL)
    goto fail;
  cgc_memset(bloomy, 0, sizeof(cgc_bloomy_t));
  bloomy->size = size;

  bloomy->bits = (cgc_uint8_t *) cgc_malloc(size);
  if (bloomy->bits == NULL)
    goto fail;
  cgc_memset(bloomy->bits, 0, size);

  bloomy->hashes[0] = hash1;
  bloomy->hashes[1] = hash2;
  bloomy->hashes[2] = hash3;

  return bloomy;

fail:
  cgc_bloomy_free(bloomy);
  return NULL;
}

void cgc_bloomy_free(cgc_bloomy_t *bloomy)
{
  if (bloomy)
  {
    if (bloomy->bits)
      cgc_free(bloomy->bits);
    cgc_free(bloomy);
  }
}

int cgc_bloomy_check(cgc_bloomy_t *bloomy, const char *buf)
{
  cgc_uint8_t bit;
  unsigned int i, n;
  if (bloomy == NULL || bloomy->bits == NULL)
    return -1;
  for (i = 0; i < sizeof(bloomy->hashes) / sizeof(cgc_hash_t); ++i)
  {
    if (bloomy->hashes[i])
    {
      n = (bloomy->hashes[i](buf, cgc_strlen(buf)) % bloomy->size);
      bit = (bloomy->bits[n/8] & (1 << (n%8)));
      if (!bit)
        return 0;
    }
  }
  return 1;
}

void cgc_bloomy_add(cgc_bloomy_t *bloomy, const char *buf)
{
  cgc_uint8_t bit;
  unsigned int i, n;

  for (i = 0; i < sizeof(bloomy->hashes) / sizeof(cgc_hash_t); ++i)
  {
    if (bloomy->hashes[i])
    {
      n = (bloomy->hashes[i](buf, cgc_strlen(buf)) % bloomy->size);
      bloomy->bits[n/8] |= (1 << (n%8));
    }
  }
}
