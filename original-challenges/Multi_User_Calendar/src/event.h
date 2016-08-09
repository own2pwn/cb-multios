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
#ifndef EVENT_H_
#define EVENT_H_

#include "user.h"
#include "datetime.h"

#define MAX_EVENT_NAME 128
#define MAX_EVENT_DESC 256

typedef struct event cgc_event_t;
typedef struct event_list cgc_event_list_t;

struct user;
struct user_list;
struct event {
    //required fields
    int id;
    struct user *owner;
    char name[MAX_EVENT_NAME];
    char desc[MAX_EVENT_DESC];
    cgc_duration_t duration;

    //Metadata
    cgc_bool is_all_day;
};

struct event_list {
    cgc_event_list_t *next;
    cgc_event_t *event;
};

cgc_event_t *cgc_create_event(struct user *owner, char *name, char *desc, cgc_duration_t *duration, cgc_bool is_all_day);
cgc_bool cgc_delete_event(cgc_event_t **event);
int cgc_compare_events(void *_event1, void *_event2);
int cgc_compare_event_dates(void *_event1, void *_event2);
cgc_event_t *cgc_find_event_from_list(char *buf, size_t buflen, int *recv_status, cgc_event_list_t *list);
void cgc_print_event(cgc_event_t *event);

#endif
