/*

Author: Bryce Kerley <bk@cromulence.co>

Copyright (c) 2016 Cromulence LLC

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#include <libcgc.h>
#include <stdlib.h>
#include <stdio.h>

#include "monte.h"
#include "block.h"

#define PI 3.14126535
#define CHECKS 0x1000
#define COUNTER_MAX 0x100

cgc_uint8* random_page = (cgc_uint8*)RANDOM_PAGE_ADDRESS;

cgc_float64 kajigger;
cgc_uint64 counter;

cgc_float64 splatter = 128.0;
cgc_float64 scaler = 1.0;

cgc_float64 cgc_monte_gen();
cgc_float64 cgc_kajigger_from_random_int(cgc_uint64 random_int);

void cgc_monte_initialize() {
  cgc_uint64* random_uint_page = (cgc_uint64*) random_page;
  kajigger = cgc_kajigger_from_random_int(random_uint_page[0]);
  counter = 0;
}

cgc_float64 cgc_kajigger_from_random_int(cgc_uint64 random_int) {
  cgc_float64 cast = (cgc_float64) random_int;
  cgc_float64 sine = sin(cast);
  cgc_uint64  sine_bytes = *((cgc_uint64*)&sine);
  cgc_uint64 new_sine_bytes = sine_bytes & 0xffffffffffff0000;
  return *((cgc_float64*)&new_sine_bytes);
}

void cgc_monte_adjust(cgc_float64 new_splatter, cgc_float64 new_scaler) {
  splatter = new_splatter;
  scaler = new_scaler;
}

cgc_float64 cgc_monte_gen_step(cgc_float64 kj, cgc_uint64 ctr, cgc_float64 spl, cgc_float64 scl) {
  cgc_float64 line = tan(sin(kj + (spl * ctr))) * (2.0 / PI);
  return (line + 1) * scl;
}

cgc_float64 cgc_monte_gen() {
  cgc_float64 result = cgc_monte_gen_step(kajigger, counter, splatter, scaler);
  counter++;
  if (counter >= COUNTER_MAX) counter = 0;

  return result;
}

cgc_uint8 cgc_monte_happy() {
  cgc_uint64 in_circle = 0;

  for (int i = 0; i < CHECKS; i++) {
    cgc_float64 x = cgc_monte_gen();
    cgc_float64 y = cgc_monte_gen();

    cgc_float64 dist = sqrt((x * x) + (y * y));

    if (dist <= 1) in_circle++;
  }

  cgc_float64 ratio = ((cgc_float64) in_circle) / ((cgc_float64)CHECKS);

  cgc_float64 diff = fabs((4 * ratio) - PI);

  cgc_float64 doubler_size = 50 * log10(1.0 / diff);
  /* cgc_printf("scl %f rat %f dif %f dbl %f\n", scaler, ratio, diff, doubler_size); */
  if (doubler_size > 254.0) return 0xff;
  if (doubler_size < 1.0) return 0;

  return (cgc_uint8) doubler_size;
}

cgc_uint8 cgc_check_random_params(cgc_float64 check_kj,
                          cgc_uint64 check_counter,
                          cgc_float64 spl,
                          cgc_float64 scl) {
  cgc_float64 existing_kajigger = kajigger;
  kajigger = check_kj;
  cgc_uint64 existing_counter = counter;
  counter = check_counter;

  cgc_float64 existing_splatter = splatter;
  cgc_float64 existing_scaler = scaler;

  splatter = spl;
  scaler = scl;
  cgc_uint8 size = cgc_monte_happy();

  counter = existing_counter;
  kajigger = existing_kajigger;
  splatter = existing_splatter;
  scaler = existing_scaler;

  return size;
}
