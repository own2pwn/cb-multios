#ifndef STDLIB_H_
#define STDLIB_H_

#include <libcgc.h>
#include <stdarg.h>
#include <stddef.h>

extern int cgc_writeall(int fd, const void *buf, size_t n);
extern int cgc_readuntil(int fd, char *buf, size_t n, char delim);

extern int cgc_debug_printf(const char *fmt, ...);
extern int cgc_fdprintf(int fd, const char *fmt, ...);
extern int cgc_sprintf(char *s, const char *fmt, ...);
extern int cgc_snprintf(char *s, size_t size, const char *fmt, ...);
extern int cgc_vsnprintf(char *s, size_t size, const char *fmt, cgc_va_list ap);
#define printf(...) cgc_fdprintf(1, __VA_ARGS__)

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
