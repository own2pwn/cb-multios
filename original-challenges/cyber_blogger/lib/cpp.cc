extern "C" {
    #include <libcgc.h>
    #include <stdlib.h>
};

extern void terminate(int);

extern "C" void __cxa_pure_virtual()
{
}

void *operator new(size_t size)
{
    return cgc_malloc(size);
}

void *operator new[](size_t size)
{
    return ::operator new(size);
}

void operator delete(void *p)
{
    cgc_free(p);
}

void operator delete[](void *p)
{
    ::operator delete(p);
}

