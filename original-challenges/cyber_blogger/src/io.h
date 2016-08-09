#ifndef IO_H_
#define IO_H_

#include "file.h"

extern "C" {
#include <stdlib.h>
};

namespace IO {
    static const unsigned int MAX_IO_SIZE = 128;

    char *cgc_buf();
    bool cgc_readline(cgc_size_t max_size=MAX_IO_SIZE);
    int cgc_readnum(cgc_size_t max_size=MAX_IO_SIZE);
    char *cgc_iotextdup(cgc_size_t max_size=MAX_IO_SIZE);
    bool cgc_readnbytes(cgc_size_t nbytes, char *data);
    cgc_File *cgc_upload_file();
}

#endif
