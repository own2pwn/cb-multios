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
#include "trail.h"
#include "rider.h"
#include "decider.h"

static cgc_uint8_t *fp = (cgc_uint8_t *)FLAG_PAGE;
static cgc_uint16_t fp_idx = 0;

/**
 * Create and initialize a new transport option
 *
 * @param to 		Pointer to store new transport option
 * @param o_type 	Type of transport option
 * @param transport Transport
 */
void cgc_option_new(cgc_TOption **to, cgc_toption_t o_type, void *transport) {
	cgc_TOption *new = cgc_calloc(sizeof(cgc_TOption));
	MALLOC_OK(new);

	new->o_type = o_type;
	new->transport = transport;
	new->next = NULL;
	*to = new;
}

/**
 * Add new option to list of options.
 *
 * @param options 	List of options
 * @param new 		New option
 */
void cgc_option_append_new(cgc_TOption **options, cgc_TOption *new) {
	cgc_TOption *prev = *options;
	if (NULL == prev) { // this is the first one
		*options = new;
	} else { // other chairs exist
		while (NULL != prev->next) { // find last one
			prev = prev->next;
		}
		prev->next = new;
	}
}

/**
 * Remove first transport option from the list
 *
 * @param options 	List of transport options
 * @return pointer to transport option or NULL if none
 */
cgc_TOption *cgc_option_pop(cgc_TOption **options) {
	if (NULL == *options) {
		return NULL;
	}
	cgc_TOption *to = *options;
	*options = to->next;
	to->next = NULL;
	return to;
}

/**
 * Destroy a single transport option
 *
 * @param to 	Transport option
 */
void cgc_option_destroy_single(cgc_TOption **to) {

	// Do not destroy the transport itself, would cause use-after-cgc_free issues
	// Destroy them from the lift and trail master lists in the Resort.

	// switch(to->o_type) {
	// 	case LIFT:
	// 		cgc_lift_destroy((cgc_Lift **)&to->transport);
	// 		break;
	// 	case TRAIL:
	// 		cgc_trail_destroy((cgc_Trail **)&to->transport);
	// 		break;
	// 	default:
	// 		break;
	// }

	cgc_free(*to);
	*to = NULL;
}

/**
 * Destroy a list of transport options
 *
 * @param options 	List of transport options
 */
void cgc_option_destroy_list(cgc_TOption **options) {
	cgc_TOption *to = NULL;
	while (NULL != *options) {
		to = cgc_option_pop(options);
		cgc_option_destroy_single(&to);
	}
}

/**
 * Get flag page idx and update it
 *
 * @return fp idx
 */
cgc_uint16_t cgc_get_next_fp_idx(void) {
	cgc_uint16_t idx = fp_idx;
	fp_idx = (fp_idx + 1) % 4096;
	return idx;
}

/**
 * Get a random transport option
 *
 * @param d 	cgc_Decider
 * @return pointer to option or NULL if none
 */
cgc_TOption *cgc_decider_select_option(cgc_Decider *d) {
	cgc_uint8_t selection_idx = fp[cgc_get_next_fp_idx()] % d->option_count;
	cgc_TOption *to = d->t_options;
	for (cgc_uint8_t i = 0; i < selection_idx; i++) {
		to = to->next;
	}
	return to;
}

/**
 * Add a transport option to the decider
 *
 * @param d 		cgc_Decider
 * @param o_type 	Transport option type
 * @param transport Transport
 */
void cgc_decider_add_option(cgc_Decider *d, cgc_toption_t o_type, void *transport) {
	cgc_TOption *to = NULL;
	cgc_option_new(&to, o_type, transport);
	cgc_option_append_new(&d->t_options, to);
	d->option_count++;

}

/**
 * Add rider to the queue
 *
 * @param d 	cgc_Decider
 * @param r 	cgc_Rider
 */
void cgc_decider_embark(cgc_Decider *d, cgc_Rider *r) {
	cgc_rider_append(&d->queue, r);
}

/**
 * Move all riders from queue to a transport option
 *
 * @param d 	cgc_Decider
 * @return Number of riders disembarked
 */
cgc_uint32_t cgc_decider_disembark(cgc_Decider *d) {
	cgc_Rider *r = NULL;
	cgc_Trail *t = NULL;
	cgc_TOption *to = NULL;
	cgc_uint32_t count = 0;

	if (0 == d->option_count) {
		return count;
	}

	while (NULL != (r = cgc_rider_pop(&d->queue))) {
		to = cgc_decider_select_option(d);
		if (NULL == to) {
			cgc_rider_append(&d->queue, r);
			break;
		}
		switch(to->o_type) {
			case LIFT:
				if (0 == r->energy_level) {
					r->health_check(r, r->id);
					cgc_rider_append(&d->quitters, r);
				} else {
					cgc_lift_enqueue_riders((cgc_Lift *)to->transport, &r);
				}
				count++;
				break;
			case TRAIL:
				t = (cgc_Trail *)to->transport;
				if (t->difficulty > r->energy_level) {
					r->health_check(r, r->id);
					cgc_rider_append(&d->quitters, r);					
				} else {
					t->embark(t, r);
				}
				count++;
				break;
			default:
				cgc_rider_append(&d->queue, r);
				break;
		}
	}
	return count;
}

/**
 * Create and initialize a new cgc_Decider
 *
 * @param d 		Pointer to store new decider
 * @param settings 	Settings for new decider
 * @return ID of decider else -1 on error
 */
cgc_int32_t cgc_decider_new(cgc_Decider **d, cgc_uint32_t settings[2]) {

	cgc_Decider *new = cgc_calloc(sizeof(cgc_Decider));
	MALLOC_OK(new);

	new->id = settings[0];
	new->altitude = settings[1];
	new->embark = cgc_decider_embark;
	new->disembark = cgc_decider_disembark;

	if (0 == new->altitude) {
		cgc_free(new);
		return -1;
	}

	*d = new;
	return new->id;
}

/**
 * Reset the decider to the initial state
 *
 * @param d 		cgc_Decider
 * @param riders 	List to store riders into
 */
void cgc_decider_reset(cgc_Decider *d, cgc_Rider **riders) {

	// get riders out of queue
	cgc_rider_append(riders, d->queue);
	d->queue = NULL;
	// get riders out of quitters
	cgc_rider_append(riders, d->quitters);
	d->quitters = NULL;

	// The caller should reset transport options separately via the Resort lists
}

/**
 * Destroy the decider. The caller must destroy any lifts/trails assigned to this decider.
 *
 * @param d 	cgc_Decider
 */
void cgc_decider_destroy(cgc_Decider **d, cgc_Rider **riders) {
	cgc_Decider *this = *d;
	// return riders in queue
	if (NULL != this->queue) {
		cgc_rider_append(riders, this->queue);
	}
	// return riders in quitters
	if (NULL != this->quitters) {
		cgc_rider_append(riders, this->quitters);
	}

	// destroy transport options
	cgc_TOption *to = this->t_options;
	cgc_option_destroy_list(&to);

	cgc_free(*d);
	*d = NULL;
}

