#include "file.h"

class cgc_Picture : public cgc_File
{
private:
    struct cgc_PictureHeader
    {
        unsigned int magic;
        unsigned int data_size;
    };

    cgc_PictureHeader header;
public:
    cgc_Picture(void *header, void *data, char *_filename);
    virtual ~cgc_Picture();

    virtual unsigned int cgc_get_magic();

    static size_t cgc_find_header_size();
    static size_t cgc_find_data_size(void *header);

    void cgc_print_picture();
};
