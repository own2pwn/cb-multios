#ifndef CGC_MALLOC_H
#define CGC_MALLOC_H

void *cgc_calloc(size_t nmemb, size_t size);
void  cgc_free(void *ptr);
void *cgc_malloc(size_t size);
void *cgc_realloc(void *ptr, size_t size);
#endif
