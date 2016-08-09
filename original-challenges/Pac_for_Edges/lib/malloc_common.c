/*
 * Copyright (c) 2015 Kaprica Security, Inc.
 *
 * Permission is hereby granted, cgc_free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "malloc_private.h"

#define ALIGNED(x,y) (((x) + (y) - 1) & ~((y) - 1))
#define DBG_ASSERT(x) do { } while (0)
#define PAGE_SIZE (4096)

#if 0
/* XXX replace with cgc allocate */
#include <sys/mman.h>
int allocate(size_t length, int is_X, void **addr)
{
    void *result;
    result = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);

    if (result == MAP_FAILED)
        return -1;

    *addr = result;
    return 0;
}

/* XXX replace with cgc deallocate */
int deallocate(void *addr, size_t length)
{
    return munmap(addr, length);
}

void _terminate(int ec)
{
    cgc_exit(ec);
}
#endif

#ifndef DISABLE_HEAP_GUARD
static void heap_error(const char *fmt, ...)
{
    cgc_va_list ap;
    va_start(ap, fmt);
    cgc_vfprintf(stderr, fmt, ap);
    va_end(ap);

    _terminate(1);
}
#endif

/* allocates a RUN_SIZE-aligned memory block and adds it to mem_map */ 
static void *cgc_run_alloc(cgc_malloc_t *heap, int type)
{
    cgc_uintptr_t addri, alignedi;
    void *addr;

    /* allocate twice the size so we can align ourselves */
    if (allocate(RUN_SIZE * 2, 0, &addr) != 0)
        return NULL;

    addri = (cgc_uintptr_t) addr;
    alignedi = ALIGNED(addri, RUN_SIZE);

    /* cgc_free the memory that is extra */
    if (addri < alignedi)
        deallocate((void *)addri, alignedi - addri);
    if (alignedi + RUN_SIZE < addri + RUN_SIZE * 2)
        deallocate((void *)(alignedi + RUN_SIZE), (addri + RUN_SIZE * 2) - (alignedi + RUN_SIZE));

    /* add run to mem_map */
    DBG_ASSERT(heap->mem_map[alignedi / RUN_SIZE] == MM_UNALLOCATED);
    heap->mem_map[alignedi / RUN_SIZE] = type;

    /* return the aligned memory block */
    return (void *)alignedi;
}

static void *cgc_tiny_alloc(cgc_malloc_t *heap, size_t n)
{
    int bin;
    cgc_malloc_tiny_free_t *hdr;

    bin = cgc_size_to_bin(n);
    hdr = &heap->free_list[bin]->tiny;

    if (hdr == NULL)
    {
        struct tiny_page *page = heap->tiny_pages[bin];
        void *ptr;

        if (page == NULL)
        {
            if (heap->tiny_run == NULL)
            {
                heap->tiny_run = cgc_run_alloc(heap, MM_TINY);
                if (heap->tiny_run == NULL)
                    return NULL;

                heap->tiny_offset = 0;
            }

            page = heap->tiny_pages[bin] = (cgc_tiny_page_t *)((cgc_uintptr_t)heap->tiny_run + heap->tiny_offset);
            page->size = n;
            page->offset = n;
            
            heap->tiny_offset += PAGE_SIZE;
            if (heap->tiny_offset == RUN_SIZE)
                heap->tiny_run = NULL;
        }

        ptr = (void *)((cgc_uintptr_t)page + page->offset);
        page->offset += n;
        if (page->offset + page->size > PAGE_SIZE)
            heap->tiny_pages[bin] = NULL;
        return ptr;
    }
    else
    {
        heap->free_list[bin] = (cgc_malloc_free_t *)hdr->next;
        return hdr;
    }
}

static void cgc_tiny_free(cgc_malloc_t *heap, void *ptr)
{
    int bin;
    cgc_tiny_page_t *page = (cgc_tiny_page_t *)((cgc_uintptr_t)ptr & ~(PAGE_SIZE - 1));
    cgc_malloc_tiny_free_t *hdr = ptr;

    bin = cgc_size_to_bin(page->size);
    hdr->next = &heap->free_list[bin]->tiny;
    heap->free_list[bin] = (cgc_malloc_free_t *)hdr;
}

static void cgc_small_insert_free(cgc_malloc_t *heap, int bin, cgc_malloc_small_free_t *hdr)
{
#ifndef DISABLE_HEAP_GUARD
    if ((hdr->hdr.size_flags & 1) == 1)
        heap_error("BAD SMALL UNLINK: block is already cgc_free hdr=%08X\n", (cgc_uintptr_t) hdr);
#endif

    hdr->hdr.size_flags |= 1;

    /* if we insert in-order, then allocations become best-fit */
    hdr->prev = NULL;
    hdr->next = &heap->free_list[bin]->small;
    heap->free_list[bin] = (cgc_malloc_free_t *) hdr;
    if (hdr->next != NULL)
        hdr->next->prev = hdr;
}

static void cgc_small_unlink_free(cgc_malloc_t *heap, int bin, cgc_malloc_small_free_t *hdr)
{
#ifndef DISABLE_HEAP_GUARD
    if ((hdr->hdr.size_flags & 1) == 0)
        heap_error("BAD SMALL UNLINK: block is not cgc_free hdr=%08X\n", (cgc_uintptr_t) hdr);
#endif

    hdr->hdr.size_flags &= ~1;

    if (hdr->prev != NULL)
        hdr->prev->next = hdr->next;
    if (hdr->next != NULL)
        hdr->next->prev = hdr->prev;

    if (&heap->free_list[bin]->small == hdr)
        heap->free_list[bin] = (cgc_malloc_free_t *) hdr->next;

#ifndef DISABLE_HEAP_GUARD
    hdr->prev = NULL;
    hdr->next = NULL;
#endif
}

static void cgc_small_split(cgc_malloc_t *heap, cgc_malloc_small_free_t *hdr, size_t len)
{
    size_t new_len = hdr->hdr.size_flags - len;
    cgc_malloc_small_free_t *new_hdr = (cgc_malloc_small_free_t *)((cgc_uintptr_t)hdr + len), *next;

    next = (cgc_malloc_small_free_t *)((cgc_uintptr_t)hdr + hdr->hdr.size_flags);

    hdr->hdr.size_flags = len;

    new_hdr->hdr.prev_size = hdr->hdr.size_flags;
    new_hdr->hdr.size_flags = new_len;
    next->hdr.prev_size = new_len;

    cgc_small_insert_free(heap, cgc_size_to_bin(new_len), new_hdr);
}

static int cgc_small_alloc_run(cgc_malloc_t *heap)
{
    cgc_malloc_small_free_t *hdr, *footer;
    cgc_small_run_t *block;

    block = cgc_run_alloc(heap, MM_SMALL);
    if (block == NULL)
        return 1;

    /* TODO initialize block */

    hdr = (cgc_malloc_small_free_t *) ALIGNED((cgc_uintptr_t)block + sizeof(cgc_small_run_t), SMALL_SIZE);
    hdr->hdr.prev_size = 0;
    hdr->hdr.size_flags = RUN_SIZE - ((cgc_uintptr_t)hdr - (cgc_uintptr_t)block) - sizeof(cgc_malloc_small_free_t);

    footer = (cgc_malloc_small_free_t *)((cgc_uintptr_t)hdr + hdr->hdr.size_flags);
    footer->hdr.prev_size = hdr->hdr.size_flags;
    footer->hdr.size_flags = 0;
    
    cgc_small_insert_free(heap, NUM_BINS - 1, hdr);
    return 0;
}

static void *cgc_small_alloc(cgc_malloc_t *heap, size_t n)
{
    int i;
    size_t len;
    cgc_malloc_small_free_t *hdr;

    len = ALIGNED(n + sizeof(cgc_malloc_hdr_t), SMALL_SIZE);

    for (i = cgc_size_to_bin(len); i < NUM_BINS; i++)
    {
        for (hdr = &heap->free_list[i]->small; hdr != NULL; hdr = hdr->next)
        {
            /* first-fit (or best-fit if linked list is in-order) */
            if (hdr->hdr.size_flags >= len)
                break;
        }

        if (hdr != NULL)
            break;
    }
    
    if (i == NUM_BINS)
    {
        if (cgc_small_alloc_run(heap) != 0)
            return NULL;

        i = NUM_BINS - 1;
        hdr = &heap->free_list[i]->small;
    }

    cgc_small_unlink_free(heap, i, hdr);
    if (hdr->hdr.size_flags - len > ALIGNED(SMALL_SIZE + sizeof(cgc_malloc_hdr_t), SMALL_SIZE))
        cgc_small_split(heap, hdr, len);

    return hdr->hdr.data;
}

static void cgc_small_free(cgc_malloc_t *heap, void *ptr)
{
    cgc_uintptr_t addri = (cgc_uintptr_t) ptr;
    cgc_malloc_small_free_t *hdr = (cgc_malloc_small_free_t *) (addri - sizeof(cgc_malloc_hdr_t));
    cgc_malloc_small_free_t *prev, *next;
    cgc_small_run_t *block = (cgc_small_run_t *)(addri & ~(RUN_SIZE - 1));

    (void) block;

#ifndef DISABLE_HEAP_GUARD
    if (addri & 7)
        heap_error("BAD SMALL FREE: address is not aligned ptr=%08X\n", addri);
    if (hdr->hdr.size_flags & 1)
        heap_error("BAD SMALL FREE: double cgc_free ptr=%08x\n", addri);
    if (hdr->hdr.size_flags > LARGE_SIZE || hdr->hdr.size_flags < SMALL_SIZE)
        heap_error("BAD SMALL FREE: corrupted size ptr=%08x\n", addri);

    /* TODO: verify block */
#endif

    if (hdr->hdr.prev_size == 0)
        prev = NULL;
    else
        prev = (cgc_malloc_small_free_t *) ((cgc_uintptr_t)hdr - hdr->hdr.prev_size);
    next = (cgc_malloc_small_free_t *) ((cgc_uintptr_t)hdr + hdr->hdr.size_flags);

#ifndef DISABLE_HEAP_GUARD
    if (prev != NULL && (prev->hdr.size_flags & ~1) != hdr->hdr.prev_size)
        heap_error("BAD SMALL FREE: heap corruption prev=%08X ptr=%08X\n", (cgc_uintptr_t) prev, addri);
    if (next->hdr.prev_size != hdr->hdr.size_flags)
        heap_error("BAD SMALL FREE: heap corruption next=%08X ptr=%08X\n", (cgc_uintptr_t) next, addri);
#endif

    if (prev != NULL && (prev->hdr.size_flags & 1))
    {
        cgc_small_unlink_free(heap, cgc_size_to_bin(prev->hdr.size_flags & ~1), prev);

        /* combine blocks */
        prev->hdr.size_flags += hdr->hdr.size_flags;
        hdr = prev;

        next->hdr.prev_size = hdr->hdr.size_flags;
    }
    
    if (next->hdr.size_flags & 1)
    {
        cgc_small_unlink_free(heap, cgc_size_to_bin(next->hdr.size_flags & ~1), next);

        /* combine blocks */
        hdr->hdr.size_flags += next->hdr.size_flags;

        next = (cgc_malloc_small_free_t *) ((cgc_uintptr_t)hdr + hdr->hdr.size_flags);
        next->hdr.prev_size = hdr->hdr.size_flags;
    }

    cgc_small_insert_free(heap, cgc_size_to_bin(hdr->hdr.size_flags), hdr);
}

static void *cgc_large_alloc(cgc_malloc_t *heap, size_t n)
{
    size_t len;
    void *addr;
    cgc_malloc_hdr_t *hdr;

    /* allocate memory block with space for header */
    len = ALIGNED(n + sizeof(cgc_malloc_hdr_t), PAGE_SIZE);
    if (allocate(len, 0, &addr) != 0)
        return NULL;

    hdr = addr;
    hdr->heap = heap;
    hdr->size_flags = len;

    return hdr->data;
}

static void cgc_large_free(cgc_malloc_t *heap, void *ptr)
{
    cgc_uintptr_t addri = (cgc_uintptr_t) ptr;
    cgc_malloc_hdr_t *hdr;

    (void) addri;

#ifndef DISABLE_HEAP_GUARD
    if ((addri & (PAGE_SIZE - 1)) != sizeof(cgc_malloc_hdr_t))
        heap_error("BAD FREE: large address is not aligned ptr=%08X\n", addri);
#endif

    hdr = (cgc_malloc_hdr_t *)(ptr - sizeof(cgc_malloc_hdr_t));

#ifndef DISABLE_HEAP_GUARD
    if (hdr->heap != heap)
        heap_error("BAD FREE: wrong heap address ptr=%08X (%08X != %08X)\n", addri, hdr->heap, heap);
    if (hdr->size_flags < LARGE_SIZE ||
          hdr->size_flags > (MAX_SIZE + PAGE_SIZE) ||
          hdr->size_flags & (PAGE_SIZE - 1))
        heap_error("BAD FREE: wrong block size ptr=%08X (%08X)\n", addri, hdr->size_flags);
#endif

    deallocate(hdr, hdr->size_flags);
}

void *cgc_malloc_alloc(cgc_malloc_t *heap, size_t n)
{
    void *ptr;

    if (n > MAX_SIZE)
        return NULL;

    n = ALIGNED(n, 4);

    if (n < TINY_SIZE)
        ptr = cgc_tiny_alloc(heap, TINY_SIZE);
    else if (n < SMALL_SIZE)
        ptr = cgc_tiny_alloc(heap, n);
    else if (n < LARGE_SIZE)
        ptr = cgc_small_alloc(heap, n);
    else
        ptr = cgc_large_alloc(heap, n);

    return ptr;
}

void cgc_malloc_free(cgc_malloc_t *heap, void *ptr)
{
    int type = heap->mem_map[(cgc_uintptr_t)ptr / RUN_SIZE];

    if (ptr == NULL)
        return;

    if (type == MM_UNALLOCATED)
    {
        /* either large or bad address */
        cgc_large_free(heap, ptr);
    }
    else if (type == MM_SMALL)
    {
        cgc_small_free(heap, ptr);
    }
    else if (type == MM_TINY)
    {
        cgc_tiny_free(heap, ptr);
    }
    else
    {
        DBG_ASSERT(0 && "Bad memory type");
    }
}

/* returns size available to user */
size_t cgc_malloc_size(cgc_malloc_t *heap, void *ptr)
{
    int type = heap->mem_map[(cgc_uintptr_t)ptr / RUN_SIZE];

    if (type == MM_SMALL || type == MM_UNALLOCATED)
    {
        cgc_malloc_hdr_t *hdr = (cgc_malloc_hdr_t *)((cgc_uintptr_t)ptr - sizeof(cgc_malloc_hdr_t));
        return hdr->size_flags - sizeof(cgc_malloc_hdr_t);
    }
    else if (type == MM_TINY)
    {
        cgc_tiny_page_t *page = (cgc_tiny_page_t *)((cgc_uintptr_t)ptr & ~(PAGE_SIZE - 1));
        return page->size;
    }
    else
    {
        DBG_ASSERT(0 && "Bad memory type");
        return 0;
    }
}

void *cgc_malloc_realloc(cgc_malloc_t *heap, void *ptr, size_t n)
{
    void *new_ptr;

    if (n > MAX_SIZE)
        return NULL;

    if (n == 0)
    {
        cgc_malloc_free(heap, ptr);
        return NULL;
    }

    if (ptr == NULL)
        return cgc_malloc_alloc(heap, n);

    if (cgc_malloc_size(heap, ptr) >= n)
        return ptr;

    new_ptr = cgc_malloc_alloc(heap, n);
    if (new_ptr == NULL)
        return NULL;

    cgc_memcpy(new_ptr, ptr, cgc_malloc_size(heap, ptr));

    cgc_malloc_free(heap, ptr);
    return new_ptr;
}
