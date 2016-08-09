#ifndef ENDIAN_H_
#define ENDIAN_H_

#include <stdint.h>

#define betoh32 cgc_swap32
#define htobe32 cgc_swap32
#define betoh16 cgc_swap16
#define htobe16 cgc_swap16

static inline cgc_uint16_t cgc_swap16(cgc_uint16_t x)
{
    return (x >> 8) | (x << 8);
}

static inline cgc_uint32_t cgc_swap32(cgc_uint32_t x)
{
    return (cgc_swap16(x) << 16) | (cgc_swap16(x >> 16));
}

#endif
