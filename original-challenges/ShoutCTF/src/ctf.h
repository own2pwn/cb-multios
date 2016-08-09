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

#ifndef CTF_H
#define CTF_H

#include "user.h"
#include "challenge.h"
#include "flgmgr.h"

#define CTF_STAT_DOWN   0
#define CTF_STAT_LIVE   1

typedef struct {
    int status;
    size_t num_chals;
    size_t num_teams;
    size_t num_users;
    size_t sz_chals;
    size_t sz_teams;
    size_t sz_users;
    cgc_chal_t **chals;
    cgc_team_t **teams;
    cgc_user_t **users;
    cgc_flgmgr_t flg;
    cgc_user_t *logged_in;
} cgc_ctf_t;

cgc_error_t cgc_ctf_init(cgc_ctf_t *ctf);

/* Feature */
cgc_error_t cgc_ctf_submit_flag(cgc_ctf_t *ctf, cgc_team_t *team, const char *flag);
cgc_error_t cgc_ctf_add_user(cgc_ctf_t *ctf, cgc_user_t *user);
cgc_error_t cgc_ctf_auth_user(cgc_ctf_t *ctf, cgc_user_t **user, const char *nick, const char *pass);
cgc_error_t cgc_ctf_add_team(cgc_ctf_t *ctf, cgc_team_t *team);
cgc_error_t cgc_ctf_add_chal(cgc_ctf_t *ctf, cgc_chal_t *chal);
cgc_error_t cgc_ctf_open_chal(cgc_ctf_t *ctf, size_t cid);

/* Ticker & Info */
cgc_error_t cgc_ctf_get_solves(cgc_ctf_t *ctf, cgc_flag_t ***solves, size_t *n);
cgc_error_t cgc_ctf_get_ranks(cgc_ctf_t *ctf, cgc_team_t ***ranks, size_t *page);
cgc_error_t cgc_ctf_get_team(cgc_ctf_t *ctf, cgc_team_t **team, size_t tid);
cgc_error_t cgc_ctf_get_chal(cgc_ctf_t *ctf, cgc_chal_t **chal, size_t cid);

#endif
