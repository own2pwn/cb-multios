#include <stdio.h>
#include "readuntil.h"

int cgc_read_until(int fd, char *buf, cgc_size_t len, char delim)
{
    cgc_fflush(stdout);
    return cgc_freaduntil(buf, len, delim, stdin);
}

int cgc_read_n(int fd, char *buf, cgc_size_t len)
{
    cgc_fflush(stdout);
    return cgc_fread(buf, len, stdin);
}
