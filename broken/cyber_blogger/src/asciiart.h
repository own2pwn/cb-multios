#include "file.h"
#include "ascii.h"

class cgc_AsciiArt : public cgc_File, private cgc_AsciiInterface
{
private:
    struct cgc_AsciiArtHeader
    {
        unsigned int magic;
        unsigned short line_width;
        unsigned short num_lines;
    };

    cgc_AsciiArtHeader header;
public:
    cgc_AsciiArt(void *_header, void *data, char *_filename);
    virtual ~cgc_AsciiArt();

    virtual unsigned int cgc_get_magic();
    virtual bool cgc_is_ascii(char *_stream);

    static size_t cgc_find_header_size();
    static size_t cgc_find_data_size(void *header);

    void cgc_print_asciiart();
};

