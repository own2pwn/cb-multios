/*
 * Copyright (c) 2015 Kaprica Security, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
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

#include <ctype.h>
#include <string.h>
#include "alphabet.h"

void cgc_ftab_init1(cgc_freqtab_t *ftab)
{
    cgc_memset(ftab, 0, sizeof(cgc_freqtab_t));
}

void cgc_ftab_init(cgc_freqtab_t *ftab, const char *word)
{
    cgc_ftab_init1(ftab);
    cgc_ftab_add(ftab, word);
}

void cgc_ftab_add(cgc_freqtab_t *ftab, const char *word)
{
    unsigned int i;

    for (i = 0; word[i] != '\0'; i++)
        if (cgc_isalpha(word[i]))
            ftab->freq[cgc_toupper(word[i]) - ALPHABET_START]++;
}