#ifndef IO_H_
#define IO_H_

#include "file.h"

extern "C" {
#include <stdlib.h>
};

namespace IO {
    static const unsigned int MAX_IO_SIZE = 128;

    char *cgc_buf();
    bool cgc_readline(size_t max_size=MAX_IO_SIZE);
    int cgc_readnum(size_t max_size=MAX_IO_SIZE);
    char *cgc_iotextdup(size_t max_size=MAX_IO_SIZE);
    bool cgc_readnbytes(size_t nbytes, char *data);
    cgc_File *cgc_upload_file();
}

#endif
