#ifndef MALLOC_INTERNAL_H_
#define MALLOC_INTERNAL_H_

#define NUM_BUCKETS 13

#define OVERHEAD_BYTES 8
#define MIN_ALLOC 32
#define MAX_ALLOC 65536 /* 64 KB */
#define SPAN_SIZE 131072 /* 128 KB */

typedef struct block {
    unsigned int size;
    unsigned int free : 1;
    unsigned int mmap : 1;
} cgc_block_t;

typedef struct free_block {
    cgc_block_t hdr;
    struct free_block *next, *prev;
} cgc_free_block_t;

typedef struct {
    cgc_free_block_t *free_list[NUM_BUCKETS];    
} cgc_malloc_t;

extern cgc_malloc_t g_malloc;

#endif /* !MALLOC_INTERNAL_H_ */
