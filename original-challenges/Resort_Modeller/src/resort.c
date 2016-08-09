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
#include <errno.h>
#include <stdint.h>
#include "libc.h"
#include "send.h"

#include "trail.h"
#include "rider.h"
#include "decider.h"
#include "resort.h"

typedef struct resort cgc_Resort;
struct resort {
	cgc_uint32_t decider_count;
	cgc_uint32_t lift_count;
	cgc_uint32_t trail_count;
	cgc_uint32_t rider_count;
	cgc_Decider **deciders;
	cgc_Lift **lifts;
	cgc_Trail **trails;
	cgc_Rider *riders;
};

#define RESORT_COUNTS_SZ 	(3 * sizeof(cgc_uint32_t))

cgc_Resort resort;

/**
 * Find trail having the given id
 *
 * @param id 	ID number
 * @return pointer to trail or NULL if not found
 */
cgc_Trail *cgc_get_trail_by_id(cgc_uint32_t id) {
	for (cgc_uint32_t idx = 0; idx < resort.trail_count && NULL != resort.trails[idx]; idx++) {
		if (id == resort.trails[idx]->id) {
			return resort.trails[idx];
		}
	}
	return NULL;
}

/**
 * Find lift having the given id
 *
 * @param id 	ID number
 * @return pointer to lift or NULL if not found
 */
cgc_Lift *cgc_get_lift_by_id(cgc_uint32_t id) {
	for (cgc_uint32_t idx = 0; idx < resort.lift_count && NULL != resort.lifts[idx]; idx++) {
		if (id == resort.lifts[idx]->id) {
			return resort.lifts[idx];
		}
	}
	return NULL;

}

/**
 * Find decider having the given id
 *
 * @param id 	ID number
 * @return pointer to decider or NULL if not found
 */
cgc_Decider *cgc_get_decider_by_id(cgc_uint32_t id) {
	for (cgc_uint32_t idx = 0; idx < resort.decider_count && NULL != resort.deciders[idx]; idx++) {
		if (id == resort.deciders[idx]->id) {
			return resort.deciders[idx];
		}
	}
	return NULL;

}

/**
 * Find the decider with the min altitude.
 *
 * @return pointer to min decider or NULL of no deciders
 */
cgc_Decider *cgc_get_decider_min(void) {
	cgc_Decider *min = NULL;

	if (0 != resort.decider_count) {
		min = resort.deciders[0];
		for (cgc_uint32_t d_idx = 1; d_idx < resort.decider_count; d_idx++) {
			if (resort.deciders[d_idx]->altitude < min->altitude) {
				min = resort.deciders[d_idx];
			}
		}
	}
	return min;
}

/**
 * Validate newly created rider
 *	- Ensure energy_level > 0
 *
 * @param r 	cgc_Rider
 * @return SUCCESS on success, else -1
 */
cgc_int32_t cgc_validate_rider(cgc_Rider *r) {
	if (0 == r->energy_level)
		return -1;

	return SUCCESS;
}

/**
 * Process input to generate riders
 *
 * @param input 	Input buffer
 * @param count 	Number of riders to generate
 * @return SUCCESS on success, else -1
 */
cgc_int32_t cgc_gen_riders(cgc_uint32_t **input, cgc_uint32_t count) {
	cgc_Rider *r;
	cgc_uint32_t *this = *input;

	while (count--) {
		if (0 > cgc_rider_new(&r, this)) {
			return -1;
		}
		if (0 > cgc_validate_rider(r)) {
			cgc_rider_destroy(&r);
			return -1;
		}

		cgc_rider_append(&resort.riders, r);
		resort.rider_count++;

		this += RIDER_IO_ELEMENTS;
	}

	*input = this;
	return SUCCESS;
}

/**
 * Validate newly created trail
 *	- Ensure no duplicate ID
 *
 * @param t 	cgc_Trail
 * @return SUCCESS on success, else -1
 */
cgc_int32_t cgc_validate_trail(cgc_Trail *t) {
	if (NULL != cgc_get_trail_by_id(t->id)) {
		return -1;
	}

	return SUCCESS;
}

/**
 * Process input to generate trails
 *
 * @param input 	Input buffer
 * @return SUCCESS on success, else -1
 */
cgc_int32_t cgc_gen_trails(cgc_uint32_t **input) {
	cgc_Decider *d;
	cgc_Trail *t;
	cgc_uint32_t counter;
	cgc_uint32_t *this = *input;

	for (counter = 0; counter < resort.trail_count; counter++) {
		if (0 > cgc_trail_new(&t, this)) {
			return -1;
		}
		if (0 > cgc_validate_trail(t)) {
			cgc_trail_destroy(&t, &resort.riders);
			return -1;
		}

		resort.trails[counter] = t;
		this += TRAIL_IO_ELEMENTS;

		// add this trail as an option for its start decider
		d = cgc_get_decider_by_id(t->start_decider);
		cgc_decider_add_option(d, TRAIL, (void *)t);
	}

	*input = this;
	return SUCCESS;
}

/**
 * Validate newly created lift
 *	- Ensure no duplicate ID
 *
 * @param l 	cgc_Lift
 * @return SUCCESS on success, else -1
 */
cgc_int32_t cgc_validate_lift(cgc_Lift *l) {
	if (NULL != cgc_get_lift_by_id(l->id)) {
		return -1;
	}

	return SUCCESS;
}

/**
 * Process input to generate lifts
 *
 * @param input 	Input buffer
 * @return SUCCESS on success, else -1
 */
cgc_int32_t cgc_gen_lifts(cgc_uint32_t **input) {
	cgc_Decider *d;
	cgc_Lift *l;
	cgc_uint32_t counter;
	cgc_uint32_t *this = *input;

	for (counter = 0; counter < resort.lift_count; counter++) {
		if (0 > cgc_lift_new(&l, this)) {
			return -1;
		}
		if (0 > cgc_validate_lift(l)) {
			cgc_lift_destroy(&l, &resort.riders);
			return -1;
		}

		resort.lifts[counter] = l;
		this += LIFT_IO_ELEMENTS;

		// add this list as an option for its start decider
		d = cgc_get_decider_by_id(l->start_decider);
		cgc_decider_add_option(d, LIFT, (void *)l);
	}

	*input = this;
	return SUCCESS;
}

/**
 * Validate newly created decider
 *	- Ensure no duplicate ID
 *
 * @param d 	cgc_Decider
 * @return SUCCESS on success, else -1
 */
cgc_int32_t cgc_validate_decider(cgc_Decider *d) {
	if (NULL != cgc_get_decider_by_id(d->id)) {
		return -1;
	}

	return SUCCESS;
}

/**
 * Process input to generate deciders
 *
 * @param input 	Input buffer
 * @return SUCCESS on success, else -1
 */
cgc_int32_t cgc_gen_deciders(cgc_uint32_t **input) {
	cgc_Decider *d;
	cgc_uint32_t counter;
	cgc_uint32_t *this = *input;

	for (counter = 0; counter < resort.decider_count; counter++) {
		if (0 > cgc_decider_new(&d, this)) {
			return -1;
		} 
		if (0 > cgc_validate_decider(d)) {
			cgc_decider_destroy(&d, &resort.riders);
			return -1;
		}

		resort.deciders[counter] = d;
		this += DECIDER_IO_ELEMENTS;
	}

	*input = this;
	return SUCCESS;
}

/**
 * Destroy the lifts, trails, and deciders. Move all riders to the riders list.
 */
void cgc_resort_destroy_digraph(void) {
	cgc_Decider *d;
	cgc_Lift *l;
	cgc_Trail *t;
	cgc_Rider *r;

	while (0 < resort.decider_count) {
		d = resort.deciders[--resort.decider_count];
		cgc_decider_destroy(&d, &resort.riders);
	}
	if (NULL != resort.deciders) cgc_free(resort.deciders);
	resort.deciders = NULL;

	while (0 < resort.lift_count) {
		l = resort.lifts[--resort.lift_count];
		cgc_lift_destroy(&l, &resort.riders);
	}
	if (NULL != resort.lifts) cgc_free(resort.lifts);
	resort.lifts = NULL;

	while (0 < resort.trail_count) {
		t = resort.trails[--resort.trail_count];
		cgc_trail_destroy(&t, &resort.riders);
	}
	if (NULL != resort.trails) cgc_free(resort.trails);
	resort.trails = NULL;

	r = resort.riders;
	// if not all riders have been returned to resort.riders
	//  this will go OOB
	for (cgc_uint32_t i = 0; i < resort.rider_count; i++) {
		cgc_rider_reset(r);
		r = r->next;
	}
}


/**
 * Reset all stats, lifts, trails, and deciders to initial valies.
 *  Move all riders to the cgc_Resort riders list and reset their stats.
 *
 * @return SUCCESS on success, else -1
 */
cgc_int32_t cgc_reset_simulation_do(void) {
	cgc_Decider *d;
	cgc_Lift *l;
	cgc_Trail *t;
	cgc_Rider *r;
	cgc_uint32_t counter;

	counter = 0;
	while (counter < resort.decider_count) {
		d = resort.deciders[counter++];
		cgc_decider_reset(d, &resort.riders);
	}

	counter = 0;
	while (counter < resort.lift_count) {
		l = resort.lifts[counter++];
		cgc_lift_reset(l, &resort.riders);
	}

	counter = 0;
	while (counter < resort.trail_count) {
		t = resort.trails[counter++];
		cgc_trail_reset(t, &resort.riders);
	}

	r = resort.riders;
	for (cgc_uint32_t i = 0; i < resort.rider_count; i++) {
		if (NULL == r) return -1;
		cgc_rider_reset(r);
		r = r->next;
	}
	return SUCCESS;
}

/**
 * Run the simulation
 *
 * @param steps 	Number of iterations/steps to run simulation
 * @return SUCCESS on success, else -1 on err
 */
cgc_int32_t cgc_go_simul8(cgc_uint32_t steps) {
	cgc_Decider *d;
	cgc_Lift *l;
	cgc_Trail *t;
	cgc_Rider *r;
	cgc_int32_t ret = SUCCESS;
	cgc_uint32_t counter = 0;

	while (0 < steps--) {
		// deciders
		// move all riders from each decider queue onto a lift queue or trail		
		counter = 0;
		while (counter < resort.decider_count) {
			d = resort.deciders[counter++];
			d->disembark(d);
		}

		// trails
		// remove completed riders from each trail and add to decider queue/quitters
		// increment the other riders along the trail.
		counter = 0;
		while (counter < resort.trail_count) {
			t = resort.trails[counter++];
			t->step(t);
		}

		// lifts
		// remove riders from c_disembark and add to end decider queue/quitters
		// add riders to c_embark from lift queue
		// increment chairs
		counter = 0;
		while (counter < resort.lift_count) {
			l = resort.lifts[counter++];
			l->step(l);
		}
	}

	return ret;
}




/**
 * Process input to construct the resort layout; a directed graph of lifts and trails.
 *  If a resort layout already exists, it is replaced. Any existing riders will be returned
 *  to the riders list in the cgc_Resort.
 *
 * @return SUCCESS on success, else -1
 */
cgc_int32_t cgc_load_resort_digraph(void) {
	cgc_int32_t result;
	cgc_uint32_t read_sz;
	cgc_uint32_t *input;
	cgc_uint32_t *input_s;

	// if resort layout currently exists, delete it
	cgc_resort_destroy_digraph();

	// process input and create resort layout objects
	RECV(&resort, RESORT_COUNTS_SZ);

	// TODO: bounds checking on counts to prevent int overflow when * 4 (have fun :) )
	resort.deciders = cgc_calloc(resort.decider_count * sizeof(cgc_Decider *));
	MALLOC_OK(resort.deciders);

	resort.lifts = cgc_calloc(resort.lift_count * sizeof(cgc_Lift *));
	MALLOC_OK(resort.lifts);

	resort.trails = cgc_calloc(resort.trail_count * sizeof(cgc_Trail *));
	MALLOC_OK(resort.trails);

	read_sz =  resort.decider_count * DECIDER_IO_SZ;
	read_sz += resort.lift_count * LIFT_IO_SZ;
	read_sz += resort.trail_count * TRAIL_IO_SZ;

	input = cgc_calloc(read_sz);
	MALLOC_OK(input);
	input_s = input;

	RECV(input, read_sz);

	// do validation (no duplicate ID's, start/end decider id's differ, etc..)
	// gen deciders
	result = cgc_gen_deciders(&input);
	if (0 > result) return -1;

	// gen lifts
	result = cgc_gen_lifts(&input);
	if (0 > result) return -1;

	// gen trails
	result = cgc_gen_trails(&input);
	if (0 > result) return -1;

	cgc_free(input_s);
	return SUCCESS;
}

/**
 * A group of riders is loaded into the model.
 *
 * We do not check for duplicate rider id's. It doesn't hurt the simulation.
 * It just gives the application (caller/CRS) confusing results because they
 * will not be able to tell which rider_N had which results if there is more
 * than 1 rider_N.
 *
 * @return SUCCESS on success, else -1
 */
cgc_int32_t cgc_load_rider_group(void) {
	cgc_int32_t result;
	cgc_uint32_t read_sz;
	cgc_uint32_t *input;
	cgc_uint32_t *input_s;
	cgc_uint32_t count;

	// get count and then read in rider data
	RECV(&count, sizeof(cgc_uint32_t));

	read_sz = count * RIDER_IO_SZ;
	input = cgc_calloc(read_sz);
	MALLOC_OK(input);
	input_s = input;

	RECV(input, read_sz);

	result = cgc_gen_riders(&input, count);
	if (0 > result) return -1;

	cgc_free(input_s);
	return SUCCESS;
}

/**
 * A single rider is loaded into the model.
 *
 * We do not check for duplicate rider id's. It doesn't hurt the simulation.
 * It just gives the application (caller/CRS) confusing results because they
 * will not be able to tell which rider_N had which results if there is more
 * than 1 rider_N.
 *
 * @return SUCCESS on success, else -1
 */
cgc_int32_t cgc_load_rider_single(void) {
	cgc_int32_t result;
	cgc_uint32_t read_sz;
	cgc_uint32_t *input;
	cgc_uint32_t *input_s;
	cgc_uint32_t count;

	// get count and then read in rider data
	RECV(&count, sizeof(cgc_uint32_t));
	if (count != 1) return -1;

	// count is 1, so read in rider data
	read_sz = count * RIDER_IO_SZ;
	input = cgc_calloc(read_sz);
	MALLOC_OK(input);
	input_s = input;

	RECV(input, read_sz);

	result = cgc_gen_riders(&input, count);
	if (0 > result) return -1;

	cgc_free(input_s);
	return SUCCESS;
}

/**
 * All riders are unloaded from the cgc_Resort riders list (i.e. deleted).
 *  The simulation is reset.
 *
 * @return SUCCESS on success, else -1
 */
cgc_int32_t cgc_unload_riders(void) {
	cgc_int32_t ret;
	// reset the simulation to move all riders to the cgc_Resort riders list
	ret = cgc_reset_simulation_do();

	// delete all riders
	cgc_rider_destroy_list(&resort.riders);
	resort.rider_count = 0;

	return ret;
}

/**
 * Start the simulation and run it for the specified number of steps.
 *  All riders will start via the queue of the lowest altitude decider.
 *
 * @return SUCCESS on success, else -1
 */
cgc_int32_t cgc_start_simulation(void) {
	cgc_uint32_t steps;
	cgc_Decider *min_decider;
	cgc_int32_t ret;
	cgc_Rider *r;

	// read # of steps
	RECV(&steps, sizeof(cgc_uint32_t));
	if (0 == steps) return -1;

	// get min_decider
	min_decider = cgc_get_decider_min();
	if (NULL == min_decider) return -1; // no deciders defined

	if (0 == resort.rider_count) return -1; // no riders defined

	// move riders from resort.riders to min_decider's queue
	while (NULL != resort.riders) {
		r = cgc_rider_pop(&resort.riders);
		min_decider->embark(min_decider, r);
	}

	// run simulation that iterates through each decider, lift, trail to move riders along
	ret = cgc_go_simul8(steps);

	return 0;

}

/**
 * Reset all stats. Move all Riders to the riders list in the cgc_Resort.
 * Then reset all lifts, trails, and deciders to initial values.
 *
 * @return SUCCESS on success, else -1
 */
cgc_int32_t cgc_reset_simulation(void) {
	// reset the simulation to move all riders to the cgc_Resort riders list
	return cgc_reset_simulation_do();
}

/**
 * Request the counts for how many total riders have boarded each lift.
 *
 * @return SUCCESS on success, else -1
 */
cgc_int32_t cgc_lift_stats(void) {
	cgc_uint32_t *stats;
	size_t stats_sz;
	cgc_Lift *l;

	if (0 == resort.lift_count) return -1;

	stats_sz = resort.lift_count * 2 * sizeof(cgc_uint32_t); // id and rider_total
	stats = cgc_calloc(stats_sz);
	MALLOC_OK(stats);

	// examine lifts and store stats
	for (cgc_uint32_t s_idx = 0, l_idx = 0; l_idx < resort.lift_count; s_idx += 2, l_idx++) {
		l = resort.lifts[l_idx];
		stats[s_idx] 		= l->id;
		stats[s_idx + 1]	= l->rider_total;
	}

	cgc_send((char *)stats, stats_sz);

	cgc_free(stats);

	return SUCCESS;
}

/**
 * Request the counts for how many total riders have completed each trail.
 *
 * @return SUCCESS on success, else -1
 */
cgc_int32_t cgc_trail_stats(void) {
	cgc_uint32_t *stats;
	size_t stats_sz;
	cgc_Trail *t;

	if (0 == resort.trail_count) return -1;

	stats_sz = resort.trail_count * 2 * sizeof(cgc_uint32_t); // id and rider_total
	stats = cgc_calloc(stats_sz);
	MALLOC_OK(stats);

	// examine trails and store stats
	for (cgc_uint32_t s_idx = 0, t_idx = 0; t_idx < resort.trail_count; s_idx += 2, t_idx++) {
		t = resort.trails[t_idx];
		stats[s_idx] 		= t->id;
		stats[s_idx + 1]	= t->rider_total;
	}

	cgc_send((char *)stats, stats_sz);

	cgc_free(stats);

	return SUCCESS;
}

/**
 * Request the counts for the number of trails each rider has completed and their energy level.
 *
 * @return SUCCESS on success, else -1
 */
cgc_int32_t cgc_rider_stats(void) {
	cgc_Decider *d;
	cgc_Lift *l;
	cgc_Trail *t;
	cgc_Chair *c;
	cgc_Rider *r;
	cgc_uint32_t *stats;
	size_t stats_sz;
	cgc_uint32_t s_idx = 0;
	cgc_uint32_t counter = 0;

	if (0 == resort.rider_count) return -1;

	stats_sz = resort.rider_count * 2 * sizeof(cgc_uint32_t); // id, energy_level
	stats = cgc_calloc(stats_sz);
	MALLOC_OK(stats);

	// process deciders
	counter = 0;
	while (counter < resort.decider_count) {
		d = resort.deciders[counter++];
		// process queue
		r = d->queue;
		while (NULL != r) {
			stats[s_idx++] = r->id;
			stats[s_idx++] = r->energy_level;
			r = r->next;
		}

		// process quitters
		r = d->quitters;
		while (NULL != r) {
			stats[s_idx++] = r->id;
			stats[s_idx++] = r->energy_level;
			r = r->next;
		}
	}

	// process lifts
	counter = 0;
	while (counter < resort.lift_count) {
		l = resort.lifts[counter++];
		
		// process queue
		r = l->queue;
		while (NULL != r) {
			stats[s_idx++] = r->id;
			stats[s_idx++] = r->energy_level;
			r = r->next;
		}

		// process chairs
		c = l->chairs;
		while (NULL != c) {
			// process riders on this chair
			r = c->riders;
			while (NULL != r) {
				stats[s_idx++] = r->id;
				stats[s_idx++] = r->energy_level;
				r = r->next;
			}
			c = c->next;
		}

	}

	// process trails
	counter = 0;
	while (counter < resort.trail_count) {
		t = resort.trails[counter++];
		
		// process riders
		r = t->riders;
		while (NULL != r) {
			stats[s_idx++] = r->id;
			stats[s_idx++] = r->energy_level;
			r = r->next;
		}
	}

	r = resort.riders;
	while (NULL != r) {
		stats[s_idx++] = r->id;
		stats[s_idx++] = r->energy_level;
		r = r->next;
	}

	cgc_send((char *)stats, stats_sz);

	cgc_free(stats);

	return SUCCESS;
}


