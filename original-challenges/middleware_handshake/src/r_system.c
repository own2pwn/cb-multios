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
#include "common.h"
#include "rng.h"

static int cgc_system_init(cgc_rng_t *rng)
{
    rng->priv = NULL;
    return SUCCESS;
}

static int cgc_system_get_bytes(cgc_rng_t *rng, unsigned char *out, unsigned int cnt)
{
    size_t bytes;
    if (cgc_random(out, cnt, &bytes) != 0 || bytes != cnt)
        return FAILURE;
    return SUCCESS;
}

const cgc_rng_def_t system_rng = {
    .name = "System",
    .id = RNG_SYSTEM,
    .init = cgc_system_init,
    .get_bytes = cgc_system_get_bytes
};