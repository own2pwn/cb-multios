#include <filaments.h>
#include <mutex.h>

void cgc_mutex_lock(cgc_mutex_t *m)
{
    while (m->locked)
    {
        // XXX cgc_filaments_switch to locker instead?
        m->waiter = cgc_filaments_current();
        cgc_filaments_yield();
    }

    m->locked = 1;
    m->locker = cgc_filaments_current();
    m->waiter = NULL;
}

void cgc_mutex_unlock(cgc_mutex_t *m)
{
    m->locked = 0;

    if (m->waiter)
        cgc_filaments_switch(m->waiter);
}

