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
#ifndef LIFT_H
#define LIFT_H 1

#include <stdint.h>
#include "rider.h"

typedef struct chair cgc_Chair;

typedef void (*cgc_chair_embark_fn)(cgc_Chair *, cgc_Rider *);
typedef cgc_Rider *(*cgc_chair_disembark_fn)(cgc_Chair *);

struct chair {
	cgc_uint32_t id;
	cgc_uint32_t seats;					// number of seats on chair
	cgc_uint32_t empty_seats;			// number of seats that are empty
	cgc_Rider *riders;					// list of riders on chair
	cgc_chair_embark_fn embark;			// add one rider to this chair
	cgc_chair_disembark_fn disembark;	// remove one rider from this chair
	cgc_Chair *next;
};

typedef struct lift cgc_Lift;

typedef cgc_uint32_t (*cgc_lift_embark_fn)(cgc_Lift *);
typedef cgc_uint32_t (*cgc_lift_disembark_fn)(cgc_Lift *);
typedef void (*cgc_lift_step_fn)(cgc_Lift *);

struct lift {
	cgc_uint32_t id;
	cgc_uint32_t chair_count;			// number of chairs on lift
	cgc_uint32_t chair_capacity;		// number of seats on chairs in this lift
	cgc_uint32_t rider_total;			// number of riders that have embarked on this lift
	cgc_Rider *queue;					// queue of riders ready to embark on this lift
	cgc_Chair *chairs;					// list of chairs on this lift
	cgc_Chair *c_embark;				// chair onto which riders will embark
	cgc_Chair *c_disembark;				// chair from which riders will disembark
	cgc_uint32_t start_decider;			// ID of decider at start of lift
	cgc_uint32_t end_decider;			// ID of decider at end of lift
	cgc_lift_embark_fn embark;			// move rider from queue to a chair
	cgc_lift_step_fn step; 				// move rider on step through the lift
	cgc_lift_disembark_fn disembark;	// move rider from a chair to the end decider rider queue
};

#define LIFT_IO_ELEMENTS	5
#define LIFT_IO_SZ 		(LIFT_IO_ELEMENTS * sizeof(cgc_uint32_t))

cgc_int32_t cgc_lift_new(cgc_Lift **l, cgc_uint32_t settings[5]);
void cgc_lift_reset(cgc_Lift *l, cgc_Rider **riders);
void cgc_lift_enqueue_riders(cgc_Lift *l, cgc_Rider **riders);
void cgc_lift_destroy(cgc_Lift **l, cgc_Rider **riders);

#endif