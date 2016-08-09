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
 
#ifndef ZOOM_H
#define ZOOM_H
#include <libcgc.h>



typedef struct zoom_filter {



	unsigned char * data;
	unsigned int num_elements;
	unsigned int num_bits;
	unsigned int hashes;
	size_t data_len;
	unsigned int inserted_elements;



} cgc_zoom_filter_t;

typedef struct hash_pair {
	unsigned int h1;
	unsigned int h2;

} cgc_hash_pair_t;



// return null pointer on failure
cgc_zoom_filter_t *cgc_new_zoom_filter(int num_entries);
cgc_hash_pair_t * cgc_hash_pair_buf(char *input, size_t buf_sz);
void cgc_zoom_add(cgc_hash_pair_t * hp, cgc_zoom_filter_t *zf);
int cgc_zoom_check(cgc_hash_pair_t *hp, cgc_zoom_filter_t *zf);
void cgc_zoom_free(cgc_zoom_filter_t *zf);
unsigned int cgc_get_sample_n_bits(size_t sz);
size_t cgc_n_elements_from_sampsz(size_t sz);
unsigned int cgc_crazy_hash(const char * key, size_t sz, unsigned int seed);
cgc_zoom_filter_t * cgc_load_zoom_filter(char *fromwire, unsigned int num_bits);

#endif