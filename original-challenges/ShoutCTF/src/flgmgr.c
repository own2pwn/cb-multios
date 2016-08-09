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

#include "flgmgr.h"

cgc_error_t cgc_flg_init(cgc_flgmgr_t *flg, cgc_chal_t **chals, size_t *num_chals)
{
    flg->num_submit = 0;
    flg->num_valid = 0;
    flg->sz_submit = 16;
    flg->submits = (cgc_flag_t **) cgc_malloc(16 * sizeof(cgc_flag_t *));
    flg->chals = chals;
    flg->num_chals = num_chals;
    return ERR_OK;
}

int cgc_flg_is_duplicate(cgc_flgmgr_t *flg, cgc_team_t *team, const char* flag)
{
    int i;
    for (i = 0; i < flg->num_submit; ++i)
    {
        cgc_flag_t *f = flg->submits[i];
        if (f->team == team && cgc_strcmp(f->flag, flag) == 0)
            return 1;
    }
    return 0;
}

cgc_error_t cgc_flg_add_submit(cgc_flgmgr_t *flg, cgc_team_t *team, const char* flag)
{
    cgc_error_t err = ERR_OK;
    if (!flg || !team || !flag)
        return ERR_INTERNAL;
    if (cgc_flg_is_duplicate(flg, team, flag))
        return ERR_DUPLICATE_FLAG;
    cgc_flag_t *f = (cgc_flag_t *) cgc_malloc(sizeof(cgc_flag_t));
    f->team = team;
    if ((err = cgc_flg_find_chal(flg, &f->chal, flag)) != ERR_OK)
        err = ERR_INVALID_FLAG;
    f->valid = !!(f->chal);
    f->idx = flg->num_submit;
    f->flag = cgc_strdup(flag);
    if (f->chal && f->chal->status == CSTAT_OPEN)
        f->chal->status = CSTAT_SOLVED;
    if (flg->sz_submit == flg->num_submit)
    {
        flg->submits = (cgc_flag_t **) cgc_realloc(flg->submits, 2 * flg->sz_submit * sizeof(cgc_flag_t *));
        flg->sz_submit *= 2;
    }
    flg->submits[flg->num_submit++] = f;
    return err;
}

cgc_error_t cgc_flg_get_solves(cgc_flgmgr_t *flg, cgc_flag_t ***flags, cgc_chal_t *chal, size_t *n)
{
    int i;
    size_t cnt = 0;
    if (!flg | !flags | !n)
        return ERR_INTERNAL;
    if (!chal)
    {
        *n = (*n < flg->num_submit) ? *n : flg->num_submit;
        if (*n == 0)
            *flags = NULL;
        else
            *flags = (cgc_flag_t **) cgc_malloc(*n * sizeof(cgc_flag_t *));
        for (i = 0; i < flg->num_submit && cnt < *n; ++i)
        {
            if (flg->submits[flg->num_submit - i - 1]->valid)
                (*flags)[cnt++] = flg->submits[flg->num_submit - i - 1];
        }
        *n = cnt;
    }
    else
    {
        for (i = 0; i < flg->num_submit && cnt < *n; ++i)
        {
            if (flg->submits[i]->chal == chal && flg->submits[i]->valid)
                cnt++;
        }
        *n = cnt;
        if (cnt == 0)
        {
            *flags = NULL;
            return ERR_OK;
        }
        else
            *flags = (cgc_flag_t **) cgc_malloc(cnt * sizeof(cgc_flag_t *));
        cnt = 0;
        for (i = 0; i < flg->num_submit && cnt < *n; ++i)
        {
            if (flg->submits[i]->chal == chal && flg->submits[i]->valid)
                (*flags)[cnt++] = flg->submits[i];
        }
    }
    return ERR_OK;
}

cgc_error_t cgc_flg_find_chal(cgc_flgmgr_t *flg, cgc_chal_t **chal, const char* flag)
{
    if (!flg || !chal || !flag)
        return ERR_INTERNAL;
    int i;
    for (i = 0; i < *(flg->num_chals); ++i)
    {
        if (cgc_strcmp(flg->chals[i]->flag, flag) == 0)
        {
            *chal = flg->chals[i];
            return ERR_OK;
        }
    }
    return ERR_NO_SUCH_CHAL;
}

int cgc_flg_team_did_solve(cgc_flgmgr_t *flg, cgc_team_t *team, cgc_chal_t *chal)
{
    if (!flg || !team || !chal)
        return ERR_INTERNAL;
    int i;
    for (i = 0; i < flg->num_submit; ++i)
    {
        cgc_flag_t *f = flg->submits[i];
        if (f->valid && f->team == team && f->chal == chal)
            return 1;
    }
    return 0;
}
