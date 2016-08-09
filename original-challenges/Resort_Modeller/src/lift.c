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

#include <libcgc.h>
#include <stdint.h>
#include "libc.h"
#include "lift.h"
#include "rider.h"
#include "decider.h"
#include "resort.h"


/**
 * Add one rider to the chair
 *
 * @param c 	cgc_Chair
 * @param r 	cgc_Rider
 */
void cgc_chair_embark_rider(cgc_Chair *c, cgc_Rider *r) {
	cgc_rider_append(&c->riders, r);
	c->empty_seats--;
}

/**
 * Remove one rider from the chair
 *
 * @param c 	cgc_Chair
 * @return pointer to cgc_Rider or NULL if chair is empty
 */
cgc_Rider *cgc_chair_disembark_rider(cgc_Chair *c) {
	cgc_Rider *r = NULL;
	r = cgc_rider_pop(&c->riders);
	if (NULL != r) {
		c->empty_seats++;
	}
	return r;
}

/**
 * Create and initialize a chair using the given settings
 *
 * @param c 		Pointer to assign new chair
 * @param id		ID for this chair
 * @param seatas 	Number of seats on this chair
 * @return SUCCESS on success, else -1
 */
cgc_int32_t cgc_chair_new(cgc_Chair **c, cgc_uint32_t id, cgc_uint32_t seats) {
	if (0 == seats) return -1;

	cgc_Chair *new = cgc_calloc(sizeof(cgc_Chair));
	MALLOC_OK(new);

	new->id = id;
	new->seats = seats;
	new->empty_seats = seats;
	new->embark = cgc_chair_embark_rider;
	new->disembark = cgc_chair_disembark_rider;
	*c = new;

	return SUCCESS;
}

/**
 * Reset chair to initial state.
 *
 * @param c 		cgc_Chair to reset
 * @param riders 	List of riders to store riders removed from chair
 */
void cgc_chair_reset(cgc_Chair *c, cgc_Rider **riders) {
	cgc_Rider *r = NULL;
	while (NULL != (r = c->disembark(c))) {
		cgc_rider_append(riders, r);
	}
}

/**
 * Reset all chair in a list.
 *
 * @param chairs 	List of chairs
 * @param riders 	List of riders to store riders removed from chair list
 */
void cgc_chair_reset_list(cgc_Chair *chairs, cgc_Rider **riders) {
	cgc_Chair *c = chairs;
	while (NULL != c) {
		cgc_chair_reset(c, riders);
		c = c->next;
	}
}

/**
 * Add new chair to list of chairs.
 *
 * @param chairs 	List of chairs
 * @param new 		New chair
 * @return pointer to new chair.
 */
cgc_Chair *cgc_chair_append_new(cgc_Chair **chairs, cgc_Chair *new) {
	cgc_Chair *prev = *chairs;
	if (NULL == prev) { // this is the first chair
		*chairs = new;
	} else { // other chairs exist
		while (NULL != prev->next) { // find last chair
			prev = prev->next;
		}
		prev->next = new;
	}

	return new;
}

/**
 * Remove chair from a list of chairs
 *
 * @param chairs 	List of chairs
 * @return pointer to chair or NULL if list is empty
 */
cgc_Chair *cgc_chair_pop(cgc_Chair **chairs) {
	if (NULL == *chairs) {
		return NULL;
	}
	cgc_Chair *c = *chairs;
	*chairs = c->next;
	c->next = NULL;
	return c;
}

/**
 * Destroy one chair
 *
 * @param c 	cgc_Chair
 */
void cgc_chair_destroy_single(cgc_Chair **c, cgc_Rider **riders) {
	cgc_chair_reset(*c, riders);
	cgc_free(*c);
	*c = NULL;
}

/**
 * Destroy all chairs in a list
 *
 * @param chairs 	List of chairs
 */
void cgc_chair_destroy_list(cgc_Chair **chairs, cgc_Rider **riders) {
	cgc_Chair *c = NULL;
	while (NULL != *chairs) {
		c = cgc_chair_pop(chairs);
		cgc_chair_destroy_single(&c, riders);
	}
}

/**
 * Add riders to the lift queue
 *
 * @param l 		cgc_Lift
 * @param riders 	List of riders
 */
void cgc_lift_enqueue_riders(cgc_Lift *l, cgc_Rider **riders) {
	cgc_Rider *r = NULL;
	while (NULL != (r = cgc_rider_pop(riders))) {
		cgc_rider_append(&l->queue, r);
	}
}

/**
 * Remove riders from the lift queue and embark onto the next chair.
 * 	If enough riders in queue, fill the chair to capacity.
 *
 * @param l 	cgc_Lift
 * @return Number of riders that were embarked.
 */
cgc_uint32_t cgc_lift_embark_riders(cgc_Lift *l) {
	cgc_uint32_t count = 0;
	cgc_Rider *r = NULL;
	cgc_Chair *c = l->c_embark;
	while (0 != c->empty_seats) {
		r = cgc_rider_pop(&l->queue);
		if (NULL == r) {
			break;
		}
		c->embark(c, r);
		l->rider_total++;
		count++;
	}

	if (NULL == c->next) { // last chair in list
		l->c_embark = l->chairs;
	} else {
		l->c_embark = c->next;
	}
	return count;
}

/**
 * Move all riders one step further through the lift
 *
 * @param l 	cgc_Lift
 */
void cgc_lift_one_step(cgc_Lift *l) {
	// unload c_disembark chair and increment to the next chair
	l->disembark(l);

	// load c_embark char and increment to the next chair
	l->embark(l);
}

/**
 * Disembark riders from the chair and add to the lift's end decider queue.
 *  All riders on chair are disembarked.
 *
 * @param l 	cgc_Lift
 * @return Number of riders that were disembarked.
 */
cgc_uint32_t cgc_lift_disembark_riders(cgc_Lift *l) {
	cgc_uint32_t count = 0;

	cgc_Decider *d = cgc_get_decider_by_id(l->end_decider);
	if (NULL == d) {
		return count;
	}
	cgc_Rider *r = NULL;
	cgc_Chair *c = l->c_disembark;
	while (c->empty_seats < c->seats) {
		r = c->disembark(c);
		if (NULL == r) {
			break;
		}
		d->embark(d, r);
		count++;
	}

	if (NULL == c->next) { // last chair in list
		l->c_disembark = l->chairs;
	} else {
		l->c_disembark = c->next;
	}

	return count;
}

/**
 * Create and initialize a new lift
 *
 * @param l 		Pointer to store lift
 * @param settings 	Array of lift settings
 * @return lift's ID on success, else -1
 */
cgc_int32_t cgc_lift_new(cgc_Lift **l, cgc_uint32_t settings[5]) {

	cgc_Lift *new = cgc_calloc(sizeof(cgc_Lift));
	MALLOC_OK(new);

	new->id = settings[0];
	new->start_decider = settings[1];
	new->end_decider = settings[2];
	new->chair_count = settings[3];
	new->chair_capacity = settings[4];
	new->embark = cgc_lift_embark_riders;
	new->step = cgc_lift_one_step;
	new->disembark = cgc_lift_disembark_riders;

	if ((new->chair_count == 0) ||										// at least 1 chair
		(new->start_decider == new->end_decider) || 					// different start/end
		((new->chair_capacity != 2) && (new->chair_capacity != 4))) { 	// valid chair size/qty
		cgc_free(new);
		return -1;
	}

	cgc_Chair *c_new;
	// gen chairs
	// set embark and disembark chairs
	for (cgc_uint32_t i = 0; i < new->chair_count; i++) {
		cgc_chair_new(&c_new, i, new->chair_capacity);
		cgc_chair_append_new(&new->chairs, c_new);
		if (0 == i) { // first chair
			new->c_embark = c_new;
		}
		if (new->chair_count/2 == i) { // middle chair
			new->c_disembark = c_new;
		}
	}

	*l = new;
	return new->id;
}

/**
 * Reset lift to initial state (no riders on chairs or in queue).
 *
 * @param l 		cgc_Lift
 * @param riders 	List of riders to store riders removed from lift.
 */
void cgc_lift_reset(cgc_Lift *l, cgc_Rider **riders) {

	// empty all chairs
	cgc_chair_reset_list(l->chairs, riders);

	// remove riders from queue
	cgc_rider_append(riders, l->queue);
	l->queue = NULL;

	l->rider_total = 0;
	l->c_embark = l->chairs;
	l->c_disembark = l->chairs;
	for (cgc_uint32_t i = 0; i < l->chair_count; i++) {
		if (l->chair_count/2 == i) { // middle chair
			break;
		}
		l->c_disembark = l->c_disembark->next;
	}
}

/**
 * Destroy a lift and remove all riders from it
 *
 * @param l 		cgc_Lift
 * @param riders 	List to store riders into
 */
void cgc_lift_destroy(cgc_Lift **l, cgc_Rider **riders) {
	cgc_Lift *this = *l;
	// destroy all chairs and get the riders from them
	cgc_chair_destroy_list(&this->chairs, riders);
	// return all riders in queue
	cgc_rider_append(riders, this->queue);
	cgc_free(*l);
	*l = NULL;
}

