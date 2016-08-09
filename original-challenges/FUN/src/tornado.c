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
#include <stdint.h>

#define M 122
#define N 156

/* from gcc headers */
typedef long long cgc__m128i __attribute__((__vector_size__(16)));
inline cgc__m128i cgc__mm_slli_si128(cgc__m128i a, cgc_uint8_t b) {
    __asm__ volatile ( "pslldq %1, %0" : "+x" (a) : "i" (b) );
    return a;
}

inline cgc__m128i cgc__mm_srli_si128(cgc__m128i a, cgc_uint8_t b) {
    __asm__ volatile ( "psrldq %1, %0" : "+x" (a) : "i" (b) );
    return a;
}

inline cgc__m128i cgc__mm_slli_epi32(cgc__m128i a, cgc_uint8_t b) {
    __asm__ volatile ( "pslld %1, %0" : "+x" (a) : "i" (b) );
    return a;
}

inline cgc__m128i cgc__mm_srli_epi32(cgc__m128i a, cgc_uint8_t b) {
    __asm__ volatile ( "psrld %1, %0" : "+x" (a) : "i" (b) );
    return a;
}


static int initialized = 0;
static int available_bytes;
static unsigned idx;
static cgc__m128i W[N];
const static cgc__m128i MASK = {0xBFFFFFF6BFFAFFFF, 0xDDFECB7FDFFFFFEF};

void cgc_tornado_mix()
{
    int i;
    cgc__m128i g, wA, wB, wC, wD, tmp;

    // generate g
    wA = W[idx];
    wB = W[(idx + M) % N];
    wC = W[(idx + N-2) % N];
    wD = W[(idx + N-1) % N];

    wA = cgc__mm_slli_si128(wA, 1) ^ wA;
    wB = cgc__mm_srli_epi32(wB, 11) & MASK;
    wC = cgc__mm_srli_si128(wC, 1);
    wD = cgc__mm_slli_epi32(wD, 18);

    g = W[idx] ^ wA ^ wB ^ wC ^ wD;

    // shift in g
    W[0] = g;
    idx = (idx + 1) % N;

    available_bytes = N * 4;
}

void cgc_tornado_init()
{
    int i;
    cgc_uint32_t seed = 0x12345678;
    cgc_uint32_t *W32 = (cgc_uint32_t *)W;

    initialized = 1;
    available_bytes = 0;
    idx = 0;

    for (i = 0; i < N * 4; i++)
    {
        seed *= 0xdeadbeef;
        seed += 0xabcdef1c;
        W32[i] = seed;
    }

    for (i = 0; i < 1000; i++)
        cgc_tornado_mix();
}

cgc_uint32_t cgc_tornado()
{
    cgc_uint32_t result;
    if (!initialized)
        cgc_tornado_init();

    if (available_bytes < sizeof(cgc_uint32_t))
        cgc_tornado_mix();

    result = *(cgc_uint32_t *)W + N * 4 - available_bytes/4;
    available_bytes -= sizeof(cgc_uint32_t);
    return result;
}
