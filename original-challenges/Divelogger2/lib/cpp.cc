
extern "C"
{
#include <libcgc.h>
#include <malloc.h>
}

void *operator new( size_t alloc_size )
{
    return (void *)cgc_malloc( alloc_size );
}

void *operator new[]( size_t alloc_size )
{
    return (void *)cgc_malloc( alloc_size );
}

void operator delete( void *ptr )
{
    cgc_free( ptr );
}

void operator delete[]( void *ptr )
{
    cgc_free( ptr );
}
