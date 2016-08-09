#ifndef _EXTERNAL_API_H
#define _EXTERNAL_API_H

#include <libcgc.h>
#include "cgc_types.h"
#include "printf.h"

#define assert(x) \
    do { if (!(x)) { cgc_printf("fail: " #x " at %s (%u)\n", __FILE__, __LINE__); } } while(0)

char *cgc_strchr(const char *s, int c);
char *cgc_strdup(const char *src);
char *cgc_strtok(char *str, const char *sep);
int cgc_strcmp(const char *s1, const char *s2);
int cgc_strncmp(const char *s1, const char *s2, size_t n);
size_t cgc_strcspn(const char *s1, const char *s2);
size_t cgc_strlcat(char *dst, const char *src, const size_t size);
size_t cgc_strlcpy(char *dst, const char *src, const size_t size);
size_t cgc_strlen(const char *s);
size_t cgc_strspn(const char *s1, const char *s2);
void *cgc_calloc(size_t count, size_t size);
void *cgc_memset(void *b, int c, size_t cgc_len);
void *cgc_memcpy(void *dst, const void *src, size_t size);
void cgc_err(unsigned int id, char *str); // __attribute__((noreturn));
int cgc_transmit_str(int fd, const char *str);
int cgc_transmit_all(int fd, const char *str, size_t size);

#endif /* _EXTERNAL_API_H */
