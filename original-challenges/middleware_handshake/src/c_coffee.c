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
    cgc_uint32_t K[2 * N];
} cgc_coffee_priv_t;

static inline void cgc_fE(cgc_coffee_priv_t *priv, int r, cgc_uint32_t *pL, cgc_uint32_t *pR)
{
    cgc_uint32_t L, R, tmp;
    L = *pL;
    R = *pR;

    tmp = (R << 3) ^ (R >> 4);
    tmp += R;
    tmp ^= priv->K[r * 2];
    tmp += L;
    L = R;
    R = tmp;

    tmp = (R << 3) ^ (R >> 4);
    tmp += R;
    tmp ^= priv->K[r * 2 + 1];
    tmp += L;
    L = R;
    R = tmp;

    *pL = L;
    *pR = R;
}

static inline void cgc_fD(cgc_coffee_priv_t *priv, int r, cgc_uint32_t *pL, cgc_uint32_t *pR)
{
    cgc_uint32_t L, R, tmp;
    L = *pL;
    R = *pR;

    tmp = (R << 3) ^ (R >> 4);
    tmp += R;
    tmp ^= priv->K[r * 2 + 1];
    tmp = L - tmp;
    L = R;
    R = tmp;

    tmp = (R << 3) ^ (R >> 4);
    tmp += R;
    tmp ^= priv->K[r * 2];
    tmp = L - tmp;
    L = R;
    R = tmp;

    *pL = L;
    *pR = R;
}

static int cgc_coffee_init(cgc_code_t *code, const unsigned char *k)
{
    int r;
    cgc_uint32_t D;
    const cgc_uint32_t *k32 = (const cgc_uint32_t *)k;
    cgc_coffee_priv_t *priv;

    code->priv = priv = cgc_malloc(sizeof(cgc_coffee_priv_t));
    if (priv == NULL)
        return FAILURE;

    D = 0;

    for (r = 0; r < N; r++)
    {
        priv->K[r * 2] = k32[D  % 4] + D;
        D += 0x517CC1B7;

        priv->K[r * 2 + 1] = k32[((D >> 14) ^ (D << 3)) % 4] + D;
        D += 0x517CC1B7;
    }

    return SUCCESS;
}

static void cgc_coffee_destroy(cgc_code_t *code)
{
    cgc_free(code->priv);
}

static int cgc_coffee_encode(cgc_code_t *code, unsigned char *b)
{
    cgc_uint32_t *b32 = (cgc_uint32_t *)b;
    cgc_uint32_t L, R;
    int r;
    cgc_coffee_priv_t *priv = code->priv;

    L = b32[0];
    R = b32[1];

    cgc_fE(priv, 0, &L, &R);
    cgc_fE(priv, 1, &L, &R);
    cgc_fE(priv, 2, &L, &R);
    cgc_fE(priv, 3, &L, &R);
    cgc_fE(priv, 4, &L, &R);
    cgc_fE(priv, 5, &L, &R);
    cgc_fE(priv, 6, &L, &R);
    cgc_fE(priv, 7, &L, &R);
    cgc_fE(priv, 8, &L, &R);
    cgc_fE(priv, 9, &L, &R);
    cgc_fE(priv, 10, &L, &R);
    cgc_fE(priv, 11, &L, &R);
    cgc_fE(priv, 12, &L, &R);
    cgc_fE(priv, 13, &L, &R);
    cgc_fE(priv, 14, &L, &R);
    cgc_fE(priv, 15, &L, &R);
    cgc_fE(priv, 16, &L, &R);
    cgc_fE(priv, 17, &L, &R);
    cgc_fE(priv, 18, &L, &R);
    cgc_fE(priv, 19, &L, &R);
    cgc_fE(priv, 20, &L, &R);
    cgc_fE(priv, 21, &L, &R);
    cgc_fE(priv, 22, &L, &R);
    cgc_fE(priv, 23, &L, &R);
    cgc_fE(priv, 24, &L, &R);
    cgc_fE(priv, 25, &L, &R);
    cgc_fE(priv, 26, &L, &R);
    cgc_fE(priv, 27, &L, &R);
    cgc_fE(priv, 28, &L, &R);
    cgc_fE(priv, 29, &L, &R);
    cgc_fE(priv, 30, &L, &R);
    cgc_fE(priv, 31, &L, &R);

    b32[0] = R;
    b32[1] = L;

    return SUCCESS;
}

static int cgc_coffee_decode(cgc_code_t *code, unsigned char *b)
{
    cgc_uint32_t *b32 = (cgc_uint32_t *)b;
    cgc_uint32_t L, R;
    int r;
    cgc_coffee_priv_t *priv = code->priv;

    R = b32[0];
    L = b32[1];

    cgc_fD(priv, 31, &R, &L);
    cgc_fD(priv, 30, &R, &L);
    cgc_fD(priv, 29, &R, &L);
    cgc_fD(priv, 28, &R, &L);
    cgc_fD(priv, 27, &R, &L);
    cgc_fD(priv, 26, &R, &L);
    cgc_fD(priv, 25, &R, &L);
    cgc_fD(priv, 24, &R, &L);
    cgc_fD(priv, 23, &R, &L);
    cgc_fD(priv, 22, &R, &L);
    cgc_fD(priv, 21, &R, &L);
    cgc_fD(priv, 20, &R, &L);
    cgc_fD(priv, 19, &R, &L);
    cgc_fD(priv, 18, &R, &L);
    cgc_fD(priv, 17, &R, &L);
    cgc_fD(priv, 16, &R, &L);
    cgc_fD(priv, 15, &R, &L);
    cgc_fD(priv, 14, &R, &L);
    cgc_fD(priv, 13, &R, &L);
    cgc_fD(priv, 12, &R, &L);
    cgc_fD(priv, 11, &R, &L);
    cgc_fD(priv, 10, &R, &L);
    cgc_fD(priv, 9, &R, &L);
    cgc_fD(priv, 8, &R, &L);
    cgc_fD(priv, 7, &R, &L);
    cgc_fD(priv, 6, &R, &L);
    cgc_fD(priv, 5, &R, &L);
    cgc_fD(priv, 4, &R, &L);
    cgc_fD(priv, 3, &R, &L);
    cgc_fD(priv, 2, &R, &L);
    cgc_fD(priv, 1, &R, &L);
    cgc_fD(priv, 0, &R, &L);

    b32[0] = L;
    b32[1] = R;
    return SUCCESS;
}

const cgc_code_def_t coffee_code = {
    .name = "Coffee",
    .type = C_COFFEE,
    .bsize = 64,
    .ksize = 128,
    .init = cgc_coffee_init,
    .destroy = cgc_coffee_destroy,
    .encode = cgc_coffee_encode,
    .decode = cgc_coffee_decode
};
