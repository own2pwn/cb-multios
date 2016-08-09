#ifndef ASCII_H
#define ASCII_H

class cgc_AsciiInterface
{
public:
    cgc_AsciiInterface() {}
    ~cgc_AsciiInterface() {}
    virtual bool cgc_is_ascii(char *_stream) = 0;
};

#endif
