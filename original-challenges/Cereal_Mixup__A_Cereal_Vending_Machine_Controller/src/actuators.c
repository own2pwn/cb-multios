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
#include "libc.h"
#include "actuators.h"

unsigned char ready = 0;
unsigned char units = 0;
unsigned char bowl_set = 0;

void cgc_init_dispenser(void) {
	bowl_set = 0;
	units = 0;
	ready = 1;
}

void cgc_dispense_bowl(void) {
	bowl_set = 1;
}

void cgc_completion_buzzer(void) {
	ready = 0;
}

// liquids
void cgc_dispense_dairy_milk(void) {
	units++;
}
void cgc_dispense_dairy_cream(void) {
	units++;
}
void cgc_dispense_water(void) {
	units++;
}
void cgc_dispense_soy_milk(void) {
	units++;
}
void cgc_dispense_almond_milk(void) {
	units++;
}

// cereals
void cgc_dispense_sugar_loops(void) {
	units++;
}
void cgc_dispense_maize_flakes(void) {
	units++;
}
void cgc_dispense_marshmallow_figuringes(void) {
	units++;
}
void cgc_dispense_chocolate_rice_pellets(void) {
	units++;
}
void cgc_dispense_oohs_of_oats(void) {
	units++;
}
void cgc_dispense_crunchy_puffs(void) {
	units++;
}
void cgc_dispense_frutiz_n_nuts(void) {
	units++;
}

// toppings
void cgc_dispense_chocolate_drops(void) {
	units++;
}
void cgc_dispense_blueberries(void) {
	units++;
}
void cgc_dispense_berry_medley(void) {
	units++;
}
void cgc_dispense_sugar_cube(void) {
	units++;
}
void cgc_dispense_strawberries(void) {
	units++;
}

