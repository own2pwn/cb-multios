#ifndef WRAPPER_H
#define WRAPPER_H

#ifdef FILAMENTS
    /* syscalls with wrappers */
    #define transmit cgc___filaments_transmit
    #define receive cgc___filaments_receive
    #define fdwait cgc___filaments_fdwait
    #define allocate cgc___filaments_allocate
    #define deallocate cgc___filaments_deallocate
    #define random cgc___filaments_random
#endif

#endif /* WRAPPER_H */
