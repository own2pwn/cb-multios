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

#ifndef USER_H
#define USER_H

#include "error.h"

struct cgc_team_t;
typedef struct cgc_user_t {
    char nick[16];
    char pass[32];
    struct cgc_team_t *team;
} cgc_user_t;

typedef struct cgc_team_t {
    char code[33];
    char name[32];
    struct cgc_user_t** members;
    struct cgc_user_t *leader;
    cgc_size_t num_members;
    cgc_size_t sz_members;
    char *shout;
    unsigned int score;
} cgc_team_t;

/* User */
cgc_error_t cgc_user_new(cgc_user_t **user);
cgc_error_t cgc_user_set_nick(cgc_user_t *user, const char *nick);
cgc_error_t cgc_user_set_pass(cgc_user_t *user, const char *pass);
cgc_error_t cgc_user_set_team(cgc_user_t *user, cgc_team_t *team, cgc_team_t **old);

/* Team */
cgc_error_t cgc_team_new(cgc_team_t **team);
cgc_error_t cgc_team_add_member(cgc_team_t *team, cgc_user_t *user);
cgc_error_t cgc_team_remove_member(cgc_team_t *team, const char *nick);
cgc_error_t cgc_team_change_name(cgc_team_t *team, const char *name);
cgc_error_t cgc_team_set_shoutout(cgc_team_t *team, const char *shout);

#endif
