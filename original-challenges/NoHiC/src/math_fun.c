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
#include <libcgc.h>
#include "libc.h"
#include "math_fun.h"
#include "operation.h"

/*
 Average/Mean - The average of the numbers in the list.
 do sum / count using signed int division
 - expect int overflow/underflow
*/
void cgc_do_int_mean(cgc_list_t * num_list) {
	cgc_node_t * curr = cgc_get_first_node(num_list);

	cgc_int32_t sum = 0;
	while(curr != cgc_get_list_tail(num_list)) {
		cgc_int32_data_t * d = (cgc_int32_data_t *)curr->data;
		sum += d->val;

		curr = curr->next;
	}

	cgc_int32_t mean = 0;
	if (num_list->count > 0) {
		// yes. that's int division
		// and C's int division truncates toward 0.
		mean = sum / (cgc_int32_t)num_list->count;
	}

	SENDSI(mean);
}

/*
 Median - The median of the numbers in the list.
  if length of list is odd, it's the center item of a sorted list
  if length is even, it is the mean of the 2 center items of a sorted list
 - Expect int overflow/underflow
 -
*/
void cgc_do_int_median(cgc_list_t * num_list) {
	cgc_node_t * curr = cgc_get_first_node(num_list);

	cgc_uint32_t middle = num_list->count / 2; // should truncate toward 0
	while(curr != cgc_get_list_tail(num_list) && middle) {
		curr = curr->next;
		middle--;
	}

	cgc_int32_t median = 0;
	if (num_list->count % 2 == 0) { // if even length
		cgc_int32_t cur_val = 0;
		cgc_int32_t prev_val = 0;
		cgc_int32_data_t * d = (cgc_int32_data_t *)curr->data;
		cur_val = d->val;
		cgc_int32_data_t * d2 = (cgc_int32_data_t *)curr->prev->data;
		prev_val = d2->val;
		median = (cur_val + prev_val) / 2; // should truncate toward 0

	} else { // if odd length
		cgc_int32_data_t * d = (cgc_int32_data_t *)curr->data;
		median = d->val;
	}

	SENDSI(median);
}

/*
 Mode - The list of the most frequently occuring number(s).
 - If all numbers occur the same number of time, the list is empty
 - Else the list has one or more elements.
*/
void cgc_do_int_mode(cgc_list_t * num_list) {
	cgc_uint32_t max_count = 0;
	cgc_uint32_t num_with_max_count = 0;
	cgc_uint32_t cur_count = 0;
	cgc_int32_t cur_val = 0;

	// find max_count (max count of any number in the list)
	cgc_node_t * curr = cgc_get_first_node(num_list);
	while(curr != cgc_get_list_tail(num_list)) {
		cgc_int32_data_t * d = (cgc_int32_data_t *)curr->data;
		if (cur_val == d->val) { // another val same as cur_val
			cur_count++;
		} else { // new val
			cur_val = d->val;
			cur_count = 1;
		}

		if (cur_count > max_count) {
			max_count = cur_count;
			num_with_max_count = 1;
		} else if (cur_count == max_count) {
			num_with_max_count++;
		}

		curr = curr->next;
	}

	// no mode if all numbers have equal count or the list is empty
	if (max_count <= 1) {
		max_count = 0;
		SENDUI(max_count);
	} else { // have a mode
		SENDUI(num_with_max_count);

		// cgc_send values having max_count
		curr = cgc_get_first_node(num_list);
		while(curr != cgc_get_list_tail(num_list)) {
			cgc_int32_data_t * d = (cgc_int32_data_t *)curr->data;
			if (cur_val == d->val) { // another val same as cur_val
				cur_count++;
			} else { // new val
				cur_val = d->val;
				cur_count = 1;
			}

			if (cur_count == max_count) {
				SENDSI(cur_val);
			}

			curr = curr->next;
		}
	}
}

/*
 Calculate the difference between the min and max values in the list
 num_list is sorted ascending, so it's the difference between the last
 and first elements in the list.
*/
void cgc_do_int_range(cgc_list_t * num_list) {
	cgc_node_t * head = cgc_get_first_node(num_list);
	cgc_node_t * tail = cgc_get_last_node(num_list);

	cgc_int32_t range = 0;
	cgc_int32_data_t * d1 = (cgc_int32_data_t *)tail->data;
	range = d1->val;
	cgc_int32_data_t * d = (cgc_int32_data_t *)head->data;
	range -= d->val;

	SENDSI(range);
}

/*
 Calculate the sum of all int values in the list
 - Expect int overflow/underflow
*/
void cgc_do_int_sum(cgc_list_t * num_list) {
	cgc_node_t * curr = cgc_get_first_node(num_list);

	cgc_int32_t sum = 0;
	while(curr != cgc_get_list_tail(num_list)) {
		cgc_int32_data_t * d = (cgc_int32_data_t *)curr->data;
		sum += d->val;

		curr = curr->next;
	}

	SENDSI(sum);
}

/*
 Calculate the product of all int values in the list
 - Expect lots of int overflow/underflow
*/
void cgc_do_int_product(cgc_list_t * num_list) {
	cgc_node_t * curr = cgc_get_first_node(num_list);

	cgc_int32_t product = 1;
	while(curr != cgc_get_list_tail(num_list)) {
		cgc_int32_data_t * d = (cgc_int32_data_t *)curr->data;
		product *= d->val;

		curr = curr->next;
	}

	SENDSI(product);
}

/*
 Calculate the min of all values in the list
*/
void cgc_do_int_min(cgc_list_t * num_list) {
	cgc_node_t * curr = cgc_get_first_node(num_list);

	cgc_int32_t min = 2147483647;
	while(curr != cgc_get_list_tail(num_list)) {
		cgc_int32_data_t * d = (cgc_int32_data_t *)curr->data;
		if (d->val < min) {
			min = d->val;
		}

		curr = curr->next;
	}

	SENDSI(min);
}

/*
 Calculate the max of all values in the list
*/
void cgc_do_int_max(cgc_list_t * num_list) {
	cgc_node_t * curr = cgc_get_first_node(num_list);

	cgc_int32_t max = -2147483646;
	while(curr != cgc_get_list_tail(num_list)) {
		cgc_int32_data_t * d = (cgc_int32_data_t *)curr->data;
		if (max < d->val) {
			max = d->val;
		}

		curr = curr->next;
	}

	SENDSI(max);	
}

/*
 The list is sorted in ascending order by default during node insertion.
 Send each value back to client in sorted ascending order.
*/
void cgc_do_int_sort(cgc_list_t * num_list) {
	cgc_node_t * curr = cgc_get_first_node(num_list);

	while(curr != cgc_get_list_tail(num_list)) {
		cgc_int32_data_t * d = (cgc_int32_data_t *)curr->data;
		SENDSI(d->val);

		curr = curr->next;
	}
}

/*
 The list is sorted in ascending order by default during node insertion.
 Send each value back to client in sorted descending order.
*/
void cgc_do_int_rsort(cgc_list_t * num_list) {
	cgc_node_t * curr = cgc_get_last_node(num_list);

	while(curr != cgc_get_list_head(num_list)) {
		cgc_int32_data_t * d = (cgc_int32_data_t *)curr->data;
		SENDSI(d->val);

		curr = curr->prev; // reverse through list
	}
}

/*
 Search the list for odd numbers. Send to the client, first the count
 of odd numbers, then each odd number.
*/
void cgc_do_int_odds(cgc_list_t * num_list) {
	cgc_node_t * curr = cgc_get_first_node(num_list);

	// determine count and cgc_send it.
	cgc_uint32_t count = 0;
	while(curr != cgc_get_list_tail(num_list)) {
		cgc_int32_data_t * d = (cgc_int32_data_t *)curr->data;
		if (d->val % 2 != 0) {
			count++;
		}

		curr = curr->next;
	}
	SENDUI(count);

	// cgc_send each odd val
	if (count > 0) {

		curr = cgc_get_first_node(num_list);
		while(curr != cgc_get_list_tail(num_list)) {
			cgc_int32_data_t * d = (cgc_int32_data_t *)curr->data;
			if (d->val % 2 != 0) {
				SENDSI(d->val);
			}

			curr = curr->next;
		}
	}
}

/*
 Search the list of even numbers. Send to the client, first the count
 of even numbers, then each even number.
*/
void cgc_do_int_evens(cgc_list_t * num_list) {
	cgc_node_t * curr = cgc_get_first_node(num_list);

	// determine count and cgc_send it.
	cgc_uint32_t count = 0;
	while(curr != cgc_get_list_tail(num_list)) {
		cgc_int32_data_t * d = (cgc_int32_data_t *)curr->data;
		if (d->val % 2 == 0) {
			count++;
		}

		curr = curr->next;
	}
	SENDUI(count);

	// cgc_send each odd val
	if (count > 0) {
		curr = cgc_get_first_node(num_list);
		while(curr != cgc_get_list_tail(num_list)) {
			cgc_int32_data_t * d = (cgc_int32_data_t *)curr->data;
			if (d->val % 2 == 0) {
				SENDSI(d->val);
			}

			curr = curr->next;
		}		
	}
}

/*
 Average/Mean - The average of the numbers in the list.
 = sum / count using floating point division
*/
void cgc_do_dbl_mean(cgc_list_t * num_list) {
	cgc_node_t * curr = cgc_get_first_node(num_list);

	double sum = 0.0;
	while(curr != cgc_get_list_tail(num_list)) {
		cgc_dbl64_data_t * d = (cgc_dbl64_data_t *)curr->data;
		sum += d->val;

		curr = curr->next;
	}

	double mean = 0.0;
	if (num_list->count > 0) {
		mean = sum / num_list->count;
	}

	SENDD(mean);
}

/*
 Median - The median of the numbers in the list.
 if length of list is odd, it's the center item of a sorted list
 if length is even, it is the mean of the 2 center items of a sorted list
*/
void cgc_do_dbl_median(cgc_list_t * num_list) {
	cgc_node_t * curr = cgc_get_first_node(num_list);

	cgc_uint32_t middle = num_list->count / 2; // should truncate toward 0
	while(curr != cgc_get_list_tail(num_list) && middle) {
		curr = curr->next;
		middle--;
	}

	double median = 0.0;
	if (num_list->count % 2 == 0) { // if even length
		double cur_val = 0.0;
		double prev_val = 0.0;
		cgc_dbl64_data_t * d = (cgc_dbl64_data_t *)curr->data;
		cur_val = d->val;
		cgc_dbl64_data_t * d2 = (cgc_dbl64_data_t *)curr->prev->data;
		prev_val = d2->val;
		median = (cur_val + prev_val) / 2.0;

	} else { // if odd length
		cgc_dbl64_data_t * d = (cgc_dbl64_data_t *)curr->data;
		median = d->val;
	}

	SENDD(median);	
}

/*
 Mode - The list of the most frequently occuring number(s).
 - If all numbers occur the same number of time, the list is empty
 - Else the list has one or more elements.
*/
void cgc_do_dbl_mode(cgc_list_t * num_list) {
	cgc_uint32_t max_count = 0;
	cgc_uint32_t num_with_max_count = 0;
	cgc_uint32_t cur_count = 0;
	double cur_val = 0.0;

	// find max_count (max count of any number in the list)
	cgc_node_t * curr = cgc_get_first_node(num_list);
	while(curr != cgc_get_list_tail(num_list)) {
		cgc_dbl64_data_t * d = (cgc_dbl64_data_t *)curr->data;
		if (cur_val == d->val) { // another val same as cur_val
			cur_count++;
		} else { // new val
			cur_val = d->val;
			cur_count = 1;
		}

		if (cur_count > max_count) {
			max_count = cur_count;
			num_with_max_count = 1;
		} else if (cur_count == max_count) {
			num_with_max_count++;
		}

		curr = curr->next;
	}

	// no mode if all numbers have equal count or the list is empty
	if (max_count <= 1) {
		max_count = 0;
		SENDUI(max_count);
	} else { // have a mode
		SENDUI(num_with_max_count);

		// cgc_send values having max_count
		curr = cgc_get_first_node(num_list);
		while(curr != cgc_get_list_tail(num_list)) {
			cgc_dbl64_data_t * d = (cgc_dbl64_data_t *)curr->data;
			// yes, this is equality comparison of floats.
			if (cur_val == d->val) { // another val same as cur_val
				cur_count++;
			} else { // new val
				cur_val = d->val;
				cur_count = 1;
			}

			if (cur_count == max_count) {
				SENDD(cur_val);
			}

			curr = curr->next;
		}
	}
}

/*
 Calculate the difference between the min and max values in the list
 num_list is sorted ascending, so it's the difference between the last
 and first elements in the list.
*/
void cgc_do_dbl_range(cgc_list_t * num_list) {
	cgc_node_t * head = cgc_get_first_node(num_list);
	cgc_node_t * tail = cgc_get_last_node(num_list);

	double range = 0.0;
	cgc_dbl64_data_t * d1 = (cgc_dbl64_data_t *)tail->data;
	range = d1->val;
	cgc_dbl64_data_t * d = (cgc_dbl64_data_t *)head->data;
	range -= d->val;

	SENDD(range);
}

/*
 Sum of values in the list
*/
void cgc_do_dbl_sum(cgc_list_t * num_list) {
	cgc_node_t * curr = cgc_get_first_node(num_list);

	double sum = 0.0;
	while(curr != cgc_get_list_tail(num_list)) {
		cgc_dbl64_data_t * d = (cgc_dbl64_data_t *)curr->data;
		sum += d->val;

		curr = curr->next;
	}

	SENDD(sum);
}

/*
 Calculate the product of all double values in the list
 - Expect some -inf/+inf
*/
void cgc_do_dbl_product(cgc_list_t * num_list) {
	cgc_node_t * curr = cgc_get_first_node(num_list);

	double product = 1.0;
	while(curr != cgc_get_list_tail(num_list)) {
		cgc_dbl64_data_t * d = (cgc_dbl64_data_t *)curr->data;
		product *= d->val;

		curr = curr->next;
	}

	SENDD(product);
}

/*
 Calculate the min of all values in the list
*/
void cgc_do_dbl_min(cgc_list_t * num_list) {
	cgc_node_t * curr = cgc_get_first_node(num_list);

	double min = 1.7e308;
	while(curr != cgc_get_list_tail(num_list)) {
		cgc_dbl64_data_t * d = (cgc_dbl64_data_t *)curr->data;
		if (d->val < min) {
			min = d->val;
		}

		curr = curr->next;
	}

	SENDD(min);
}

/*
 Calculate the max of all values in the list
*/
void cgc_do_dbl_max(cgc_list_t * num_list) {
	cgc_node_t * curr = cgc_get_first_node(num_list);

	double max = -1.7e308;
	while(curr != cgc_get_list_tail(num_list)) {
		cgc_dbl64_data_t * d = (cgc_dbl64_data_t *)curr->data;
		if (max < d->val) {
			max = d->val;
		}

		curr = curr->next;
	}

	SENDD(max);	
}

/*
 The list is sorted in ascending order by default during node insertion.
 Send each value back to client in sorted ascending order.
*/
void cgc_do_dbl_sort(cgc_list_t * num_list) {
	cgc_node_t * curr = cgc_get_first_node(num_list);

	while(curr != cgc_get_list_tail(num_list)) {
		cgc_dbl64_data_t * d = (cgc_dbl64_data_t *)curr->data;
		SENDD(d->val);

		curr = curr->next;
	}
}

/*
 The list is sorted in ascending order by default during node insertion.
 Send each value back to client in sorted descending order.
*/
void cgc_do_dbl_rsort(cgc_list_t * num_list) {
	cgc_node_t * curr = cgc_get_last_node(num_list);

	while(curr != cgc_get_list_head(num_list)) {
		cgc_dbl64_data_t * d = (cgc_dbl64_data_t *)curr->data;
		SENDD(d->val);

		curr = curr->prev; // reverse through list
	}
}

/*
 Search the list for odd numbers. Send to the client, first the count
 of odd numbers, then each odd number.
 - In the land of fairies and Oz, even floats can be odd/even!
*/
void cgc_do_dbl_odds(cgc_list_t * num_list) {
	cgc_node_t * curr = cgc_get_first_node(num_list);

	// determine count and cgc_send it.
	cgc_uint32_t count = 0;
	cgc_int32_t temp = 0;
	while(curr != cgc_get_list_tail(num_list)) {
		cgc_dbl64_data_t * d = (cgc_dbl64_data_t *)curr->data;
		temp = (cgc_int32_t)d->val;
		if (temp % 2 != 0) {
			count++;
		}

		curr = curr->next;
	}
	SENDUI(count);

	// cgc_send each odd val
	if (count > 0) {

		curr = cgc_get_first_node(num_list);
		while(curr != cgc_get_list_tail(num_list)) {
			cgc_dbl64_data_t * d = (cgc_dbl64_data_t *)curr->data;
			temp = (cgc_int32_t)d->val;
			if (temp % 2 != 0) {
				SENDD(d->val);
			}

			curr = curr->next;
		}
	}
}

/*
 Search the list of even numbers. Send to the client, first the count
 of even numbers, then each even number.
 - yes, we have a fairy infestation!
*/
void cgc_do_dbl_evens(cgc_list_t * num_list) {
	cgc_node_t * curr = cgc_get_first_node(num_list);

	// determine count and cgc_send it.
	cgc_uint32_t count = 0;
	cgc_int32_t temp = 0;
	while(curr != cgc_get_list_tail(num_list)) {
		cgc_dbl64_data_t * d = (cgc_dbl64_data_t *)curr->data;
		temp = (cgc_int32_t)d->val;
		if (temp % 2 == 0) {
			count++;
		}

		curr = curr->next;
	}
	SENDUI(count);

	// cgc_send each odd val
	if (count > 0) {
		curr = cgc_get_first_node(num_list);
		while(curr != cgc_get_list_tail(num_list)) {
			cgc_dbl64_data_t * d = (cgc_dbl64_data_t *)curr->data;
			temp = (cgc_int32_t)d->val;
			if (temp % 2 == 0) {
				SENDD(d->val);
			}

			curr = curr->next;
		}
	}
}
