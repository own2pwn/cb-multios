#include <cstdlib.h>

extern "C"
{
    void cgc___cxa_pure_virtual()
    {
        _terminate(1);
    }
}
