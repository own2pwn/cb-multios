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
#include <stdint.h>
#include "codes.h"

#define N 32

typedef struct {
    cgc_uint16_t K[N+8];
} cgc_faith_priv_t;

static inline cgc_uint32_t cgc_S(cgc_uint8_t x, cgc_uint8_t y, cgc_uint8_t a)
{
    cgc_uint8_t t = x + y + a;
    return (t << 2) | (t >> 6);
}

static inline void cgc_f(cgc_faith_priv_t *priv, int r, cgc_uint32_t *pL, cgc_uint32_t *pR)
{
    cgc_uint8_t a0, a1, a2, a3, b0, b1, f0, f1, f2, f3;
    cgc_uint32_t L, R, tmp;
    L = *pL;
    R = *pR;
    tmp = R;

    a0 = R >> 24;
    a1 = R >> 16;
    a2 = R >> 8;
    a3 = R;
    b0 = priv->K[r] >> 8;
    b1 = priv->K[r];

    f1 = a1 ^ b0;
    f2 = a2 ^ b1;
    f1 ^= a0;
    f2 ^= a3;
    f1 = cgc_S(f1, f2, 0);
    f2 = cgc_S(f2, f1, 1);
    f0 = cgc_S(a0, f1, 1);
    f3 = cgc_S(a3, f2, 0);

    R = L ^ ((f0 << 24) | (f1 << 16) | (f2 << 8) | (f3));
    L = tmp;

    *pL = L;
    *pR = R;
}

static inline cgc_uint32_t cgc_fK(cgc_uint32_t a, cgc_uint32_t b)
{
    cgc_uint8_t a0, a1, a2, a3, b0, b1, b2, b3, f0, f1, f2, f3;

    a0 = a >> 24;
    a1 = a >> 16;
    a2 = a >>  8;
    a3 = a;
    b0 = b >> 24;
    b1 = b >> 16;
    b2 = b >>  8;
    b3 = b;

    f1 = a1 ^ a0;
    f2 = a2 ^ a3;
    f1 = cgc_S(f1, f2 ^ b0, 1);
    f2 = cgc_S(f2, f1 ^ b1, 0);
    f0 = cgc_S(a0, f1 ^ b2, 0);
    f3 = cgc_S(a3, f2 ^ b3, 1);

    return (f0 << 24) | (f1 << 16) | (f2 << 8) | f3;
}

static int cgc_faith_init(cgc_code_t *code, const unsigned char *k)
{
    int r;
    cgc_uint32_t R1, R2, L1, L2, A, B, D;
    const cgc_uint32_t *k32 = (const cgc_uint32_t *)k;
    cgc_faith_priv_t *priv;

    code->priv = priv = cgc_malloc(sizeof(cgc_faith_priv_t));
    if (priv == NULL)
        return FAILURE;

    L1 = k32[0];
    L2 = k32[1];
    R1 = k32[2];
    R2 = k32[3];

    A = L1;
    B = L2;
    D = 0;

    for (r = 0; r < N/2 + 4; r++)
    {
        cgc_uint32_t Q, tmp;
        if ((r % 3) == 0)
            Q = R1 ^ R2;
        else if ((r % 3) == 1)
            Q = R1;
        else
            Q = R2;
        tmp = cgc_fK(A, (B ^ D) ^ Q);

        priv->K[r * 2] = tmp >> 16;
        priv->K[r * 2 + 1] = tmp;

        D = A;
        A = B;
        B = tmp;
    }

    return SUCCESS;
}

static void cgc_faith_destroy(cgc_code_t *code)
{
    cgc_free(code->priv);
}

static int cgc_faith_encode(cgc_code_t *code, unsigned char *b)
{
    cgc_uint32_t *b32 = (cgc_uint32_t *)b;
    cgc_uint32_t L, R;
    int r;
    cgc_faith_priv_t *priv = code->priv;

    L = b32[0];
    R = b32[1];

    L ^= (priv->K[N] << 16) | priv->K[N+1];
    R ^= (priv->K[N+2] << 16) | priv->K[N+3];
    R ^= L;

    cgc_f(priv, 0, &L, &R);
    cgc_f(priv, 1, &L, &R);
    cgc_f(priv, 2, &L, &R);
    cgc_f(priv, 3, &L, &R);
    cgc_f(priv, 4, &L, &R);
    cgc_f(priv, 5, &L, &R);
    cgc_f(priv, 6, &L, &R);
    cgc_f(priv, 7, &L, &R);
    cgc_f(priv, 8, &L, &R);
    cgc_f(priv, 9, &L, &R);
    cgc_f(priv, 10, &L, &R);
    cgc_f(priv, 11, &L, &R);
    cgc_f(priv, 12, &L, &R);
    cgc_f(priv, 13, &L, &R);
    cgc_f(priv, 14, &L, &R);
    cgc_f(priv, 15, &L, &R);
    cgc_f(priv, 16, &L, &R);
    cgc_f(priv, 17, &L, &R);
    cgc_f(priv, 18, &L, &R);
    cgc_f(priv, 19, &L, &R);
    cgc_f(priv, 20, &L, &R);
    cgc_f(priv, 21, &L, &R);
    cgc_f(priv, 22, &L, &R);
    cgc_f(priv, 23, &L, &R);
    cgc_f(priv, 24, &L, &R);
    cgc_f(priv, 25, &L, &R);
    cgc_f(priv, 26, &L, &R);
    cgc_f(priv, 27, &L, &R);
    cgc_f(priv, 28, &L, &R);
    cgc_f(priv, 29, &L, &R);
    cgc_f(priv, 30, &L, &R);
    cgc_f(priv, 31, &L, &R);

    L ^= R;
    R ^= (priv->K[N+4] << 16) | priv->K[N+5];
    L ^= (priv->K[N+6] << 16) | priv->K[N+7];

    b32[0] = R;
    b32[1] = L;
    return SUCCESS;
}

static int cgc_faith_decode(cgc_code_t *code, unsigned char *b)
{
    cgc_uint32_t *b32 = (cgc_uint32_t *)b;
    cgc_uint32_t L, R;
    int r;
    cgc_faith_priv_t *priv = code->priv;

    R = b32[0];
    L = b32[1];

    R ^= (priv->K[N+4] << 16) | priv->K[N+5];
    L ^= (priv->K[N+6] << 16) | priv->K[N+7];
    L ^= R;

    cgc_f(priv, 31, &R, &L);
    cgc_f(priv, 30, &R, &L);
    cgc_f(priv, 29, &R, &L);
    cgc_f(priv, 28, &R, &L);
    cgc_f(priv, 27, &R, &L);
    cgc_f(priv, 26, &R, &L);
    cgc_f(priv, 25, &R, &L);
    cgc_f(priv, 24, &R, &L);
    cgc_f(priv, 23, &R, &L);
    cgc_f(priv, 22, &R, &L);
    cgc_f(priv, 21, &R, &L);
    cgc_f(priv, 20, &R, &L);
    cgc_f(priv, 19, &R, &L);
    cgc_f(priv, 18, &R, &L);
    cgc_f(priv, 17, &R, &L);
    cgc_f(priv, 16, &R, &L);
    cgc_f(priv, 15, &R, &L);
    cgc_f(priv, 14, &R, &L);
    cgc_f(priv, 13, &R, &L);
    cgc_f(priv, 12, &R, &L);
    cgc_f(priv, 11, &R, &L);
    cgc_f(priv, 10, &R, &L);
    cgc_f(priv, 9, &R, &L);
    cgc_f(priv, 8, &R, &L);
    cgc_f(priv, 7, &R, &L);
    cgc_f(priv, 6, &R, &L);
    cgc_f(priv, 5, &R, &L);
    cgc_f(priv, 4, &R, &L);
    cgc_f(priv, 3, &R, &L);
    cgc_f(priv, 2, &R, &L);
    cgc_f(priv, 1, &R, &L);
    cgc_f(priv, 0, &R, &L);

    R ^= L;
    L ^= (priv->K[N] << 16) | priv->K[N+1];
    R ^= (priv->K[N+2] << 16) | priv->K[N+3];

    b32[0] = L;
    b32[1] = R;
    return SUCCESS;
}

const cgc_code_def_t faith_code = {
    .name = "FAITH",
    .type = C_FAITH,
    .bsize = 64,
    .ksize = 128,
    .init = cgc_faith_init,
    .destroy = cgc_faith_destroy,
    .decode = cgc_faith_decode,
    .encode = cgc_faith_encode
};
