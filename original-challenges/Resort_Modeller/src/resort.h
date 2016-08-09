/*
 * Copyright (C) Narf Industries <info@narfindustries.com>
 *
 * Permission is hereby granted, cgc_free of charge, to any person obtaining a
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
#ifndef RESORT_H
#define RESORT_H 1

#include <stdint.h>
#include "decider.h"

cgc_Decider *cgc_get_decider_by_id(cgc_uint32_t id);

cgc_int32_t cgc_load_resort_digraph(void);
cgc_int32_t cgc_load_rider_group(void);
cgc_int32_t cgc_load_rider_single(void);
cgc_int32_t cgc_unload_riders(void);
cgc_int32_t cgc_start_simulation(void);
cgc_int32_t cgc_reset_simulation(void);
cgc_int32_t cgc_lift_stats(void);
cgc_int32_t cgc_trail_stats(void);
cgc_int32_t cgc_rider_stats(void);




#endif



