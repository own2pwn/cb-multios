#ifndef STDDEF_H_
#define STDDEF_H_

#include <libcgc.h>

typedef long int cgc_ptrdiff_t;

#ifndef _LIBCGC_H
    #undef NULL
    #define NULL 0
#endif

#define offsetof(TYPE, MEMBER) __builtin_offsetof (TYPE, MEMBER)

#endif /* !STDDEF_H_ */
