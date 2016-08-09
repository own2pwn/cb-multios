
extern "C"
{
#include <libcgc.h>
#include <stdlib.h>
}

void call_inits( void )
{
    size_t asize, n;
    cgc_printf("ctors called");
}
