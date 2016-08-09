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

#ifndef FLAG_H
#define FLAG_H

#include "user.h"
#include "error.h"
#include "challenge.h"

typedef struct {
    cgc_team_t *team;
    cgc_chal_t *chal;
    char *flag;
    int valid;
    int idx;
    char *shout;
} cgc_flag_t;

typedef struct {
    cgc_size_t num_submit;
    cgc_size_t num_valid;
    cgc_size_t sz_submit;
    cgc_flag_t **submits;
    cgc_chal_t **chals;
    cgc_size_t *num_chals;
} cgc_flgmgr_t;

cgc_error_t cgc_flg_init(cgc_flgmgr_t *flg, cgc_chal_t **chals, cgc_size_t *num_chals);
cgc_error_t cgc_flg_add_submit(cgc_flgmgr_t *flg, cgc_team_t *team, const char* flag);
cgc_error_t cgc_flg_get_solves(cgc_flgmgr_t *flg, cgc_flag_t ***flags, cgc_chal_t *chal, cgc_size_t *n);
cgc_error_t cgc_flg_find_chal(cgc_flgmgr_t *flg, cgc_chal_t **chal, const char* flag);
int cgc_flg_team_did_solve(cgc_flgmgr_t *flg, cgc_team_t *team, cgc_chal_t *chal);

#endif
