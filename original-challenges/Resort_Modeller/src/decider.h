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
#ifndef DECIDER_H
#define DECIDER_H 1
#include <stdint.h>
#include "rider.h"
#include "lift.h"
#include "trail.h"


typedef enum {LIFT, TRAIL} cgc_toption_t;

typedef struct transport_option cgc_TOption;
struct transport_option {
	cgc_toption_t o_type;				// type of transport option
	void *transport;				// pointer to transport option
	cgc_TOption *next;
};

typedef struct decider cgc_Decider;

typedef void (*cgc_decider_embark_fn)(cgc_Decider *, cgc_Rider *);
typedef cgc_uint32_t (*cgc_decider_disembark_fn)(cgc_Decider *);

struct decider {
	cgc_uint32_t id;
	cgc_uint32_t altitude; 				// altitude of this decider
	cgc_Rider *queue;					// queue of riders on this decider ready to embark on lift or trail
	cgc_Rider *quitters;				// list of riders that are too tired to continue (0 energy level)
	cgc_uint32_t option_count;			// number of transport options
	cgc_TOption *t_options;				// list of transport options for riders (LIFT or TRAIL)
	cgc_decider_embark_fn embark;		// move rider from transport option to queue
	cgc_decider_disembark_fn disembark;	// move rider from queue to transport option
};

#define DECIDER_IO_ELEMENTS 	2
#define DECIDER_IO_SZ 	(DECIDER_IO_ELEMENTS * sizeof(cgc_uint32_t))


cgc_int32_t cgc_decider_new(cgc_Decider **d, cgc_uint32_t settings[2]);
void cgc_decider_reset(cgc_Decider *d, cgc_Rider **riders);
void cgc_decider_destroy(cgc_Decider **d, cgc_Rider **riders);
void cgc_decider_add_option(cgc_Decider *d, cgc_toption_t o_type, void *transport);


#endif