#ifndef STDLIB_H_
#define STDLIB_H_

#include <libcgc.h>
#include <stdarg.h>
#include <stddef.h>

extern int cgc_printf(const char *fmt, ...);
extern int cgc_sprintf(char *s, const char *fmt, ...);

long cgc_strtol(const char *str, char **endptr, int base);
unsigned long cgc_strtoul(const char *str, char **endptr, int base);

extern void *cgc_malloc(size_t size);
extern void *cgc_calloc(size_t nmemb, size_t size);
extern void *cgc_realloc(void *ptr, size_t size);
extern void cgc_free(void *ptr);
extern size_t cgc_malloc_size(void *ptr);

static void cgc_exit(int ret)
{
    _terminate(ret);
}

#endif /* !STDLIB_H_ */
