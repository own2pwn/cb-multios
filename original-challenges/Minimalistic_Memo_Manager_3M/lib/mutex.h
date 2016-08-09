#ifndef MUTEX_H_
#define MUTEX_H_

#include <filaments.h>

typedef struct {
    unsigned int locked;
    cgc_fib_t *locker;
    cgc_fib_t *waiter;
} cgc_mutex_t;

static inline void cgc_mutex_init(cgc_mutex_t *m)
{
    m->locked = 0;
    m->locker = 0;
    m->waiter = 0;
}

void cgc_mutex_lock(cgc_mutex_t *mutex);
void cgc_mutex_unlock(cgc_mutex_t *mutex);

#endif
