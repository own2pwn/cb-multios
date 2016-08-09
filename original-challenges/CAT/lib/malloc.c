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
#include "list.h"
#include "malloc.h"

typedef struct node cgc_block_meta_t;

struct list allocated_list;
struct list free_list;

char init_heap_done = FALSE;
cgc_size_t remaining = 0;
cgc_block_meta_t *last = NULL;


static void cgc_set_block_size(cgc_block_meta_t *block, cgc_size_t size) {
	block->data = (void *)size;
}

static void cgc_init_heap() {
	if (FALSE == init_heap_done) {
		cgc_list_init(&allocated_list, NULL);
		cgc_list_init(&free_list, NULL);
		init_heap_done = TRUE;
	}
}

static unsigned char cgc_is_enough_room(const void *free_sz, void *new_sz) {
	if (free_sz >= new_sz) {
		return TRUE;
	}

	return FALSE;
}

void *cgc_malloc(cgc_size_t size) {

	if (0 == size) {
		return NULL;
	}

	size += sizeof(cgc_block_meta_t); // size requested + size of cgc_block_meta_t is total size required
	cgc_block_meta_t *ret = NULL;

	cgc_init_heap();

	// check for a block on the free_list of sufficient size
	ret = (cgc_block_meta_t *)cgc_list_find_node_with_data(&free_list, &cgc_is_enough_room, (void *)size);

	// found room on free_list
	if (NULL != ret) {
		cgc_list_remove_node(&free_list, ret);

	// no room on free_list
	} else {
		// is there room on the last partially-used page?
		if (size <= remaining) {
			ret = last;
			remaining -= size;
		} else { // no room anywhere, get new page(s)

			// Allocate creates new page(s), so the memory from a prevoius page where 
			// last and remaining refer should be saved to the free_list.
			//
			// Only save if that space can fit a cgc_block_meta_t + some bytes
			// else let it become dangling/unusable memory.
			if (remaining > sizeof(cgc_block_meta_t)) {
				cgc_set_block_size(last, remaining);
				cgc_list_insert_node_at_end(&free_list, last);
				last = NULL;
				remaining = 0;
			}

			if (0 != allocate(size, 0, (void **)&ret)) {
				return NULL;
			}

			remaining = PAGE_SZ - (size % PAGE_SZ);
		}

		cgc_set_block_size(ret, size);

		last = (cgc_block_meta_t *)((unsigned char *)ret + size);

	}

	// add block to allocated_list
	cgc_list_insert_node_at_end(&allocated_list, ret);

	// need casting to make the math work correctly.
	return (void *)((unsigned char*)ret + sizeof(cgc_block_meta_t));
}

void cgc_free(void *ptr) {
	// get block_meta for this memory
	ptr -= sizeof(cgc_block_meta_t);

	// rm block from allocated_list
	cgc_list_remove_node(&allocated_list, (cgc_block_meta_t *)ptr);

	// add block to free_list
	cgc_list_insert_node_at_end(&free_list, (cgc_block_meta_t *)ptr);
}

