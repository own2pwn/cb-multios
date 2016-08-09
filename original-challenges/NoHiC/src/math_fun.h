/*
 * Copyright (C) Narf Industries <info@narfindustries.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef MATH_FUN_H
#define MATH_FUN_H
#include "libc.h"

void cgc_do_int_mean(cgc_list_t * num_list);
void cgc_do_int_median(cgc_list_t * num_list);
void cgc_do_int_mode(cgc_list_t * num_list);
void cgc_do_int_range(cgc_list_t * num_list);
void cgc_do_int_sum(cgc_list_t * num_list);
void cgc_do_int_product(cgc_list_t * num_list);
void cgc_do_int_min(cgc_list_t * num_list);
void cgc_do_int_max(cgc_list_t * num_list);
void cgc_do_int_sort(cgc_list_t * num_list);
void cgc_do_int_rsort(cgc_list_t * num_list);
void cgc_do_int_odds(cgc_list_t * num_list);
void cgc_do_int_evens(cgc_list_t * num_list);

void cgc_do_dbl_mean(cgc_list_t * num_list);
void cgc_do_dbl_median(cgc_list_t * num_list);
void cgc_do_dbl_mode(cgc_list_t * num_list);
void cgc_do_dbl_range(cgc_list_t * num_list);
void cgc_do_dbl_sum(cgc_list_t * num_list);
void cgc_do_dbl_product(cgc_list_t * num_list);
void cgc_do_dbl_min(cgc_list_t * num_list);
void cgc_do_dbl_max(cgc_list_t * num_list);
void cgc_do_dbl_sort(cgc_list_t * num_list);
void cgc_do_dbl_rsort(cgc_list_t * num_list);
void cgc_do_dbl_odds(cgc_list_t * num_list);
void cgc_do_dbl_evens(cgc_list_t * num_list);

#endif