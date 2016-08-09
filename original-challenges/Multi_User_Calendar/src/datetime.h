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
#ifndef DATETIME_H_
#define DATETIME_H_

#include "bool.h"

#define MAX_DATE 10
#define MAX_TIME 5

typedef struct {
    short year;
    char month;
    char day;
} cgc_date_t;

typedef struct {
    char hour;
    char minute;
} cgc_time_t;

typedef struct {
    cgc_date_t date;
    cgc_time_t time;
} cgc_datetime_t;

typedef struct {
    cgc_datetime_t start;
    cgc_datetime_t end;
} cgc_duration_t;

cgc_bool cgc_set_date(cgc_date_t *date, char month, char day, short year);
cgc_bool cgc_set_time(cgc_time_t *time, char hour, char minute);
cgc_bool cgc_set_datetime(cgc_datetime_t *dt, cgc_date_t date, cgc_time_t time);
cgc_bool cgc_set_duration(cgc_duration_t *d, cgc_datetime_t dt1, cgc_datetime_t dt2);
int cgc_compare_date(void *_date1, void *_date2);
int cgc_compare_time(void *_time1, void *_time2);
int cgc_compare_datetime(void *_dt1, void *_dt2);
cgc_bool cgc_durations_overlap(cgc_duration_t d1, cgc_duration_t d2);
cgc_bool cgc_date_within(cgc_duration_t d, cgc_date_t date);
cgc_bool cgc_datetime_within(cgc_duration_t d, cgc_datetime_t dt);
cgc_bool cgc_strtodate(char *datestr, cgc_date_t *date);
cgc_bool cgc_strtotime(char *timestr, cgc_time_t *time);
void cgc_print_date(cgc_date_t *date);
void cgc_print_time(cgc_time_t *time);
char *cgc_get_month(cgc_date_t *date);
void cgc_get_time_str(char *time_str, cgc_time_t *time);
void cgc_get_date_str(char *date_str, cgc_date_t *date);

#endif
