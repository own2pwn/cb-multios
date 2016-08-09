#ifndef FILEBASE_H_
#define FILEBASE_H_

#include "filetype.h"

extern "C" {
#include <stdlib.h>
};

class cgc_File
{
private:
    cgc_size_t header_size;
    cgc_size_t data_size;
    char filename[64];
    char extension[4];
protected:
    void *raw_header;
    char *raw_data;
    void cgc_clear_data_size();
public:
    static const unsigned int MAX_HEADER_SIZE = 128;
    cgc_File(cgc_size_t _header_size, cgc_size_t _data_size, char *_filename);
    virtual ~cgc_File();

    virtual unsigned int cgc_get_magic() = 0;

    char *cgc_get_extension();
    cgc_size_t cgc_get_header_size();
    cgc_size_t cgc_get_data_size();
    const ftype cgc_tell_filetype();
};

#endif

