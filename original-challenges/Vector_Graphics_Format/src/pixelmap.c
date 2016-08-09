/*

Author: Jason Williams <jdw@cromulence.com>

Copyright (c) 2014 Cromulence LLC

Permission is hereby granted, cgc_free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/
#include <stdlib.h>
#include <libcgc.h>
#include "pixelmap.h"

#define PMP_HEADER_FILE_TYPE            (0x4D50)
#define PMP_HEADER_COMPRESSION_NONE     (0)
#define PMP_HEADER_BITCOUNT_24BIT_RGB   (24)

#define PMP_FILE_HEADER_SIZE       (14)
#define PMP_INFO_HEADER_SIZE       (40)


typedef struct __attribute__ ((__packed__)) tagPixelmapFileHeader
{
    cgc_uint16_t    pmType;
    cgc_uint32_t    pmSize;
    cgc_uint16_t    pmReserved1;
    cgc_uint16_t    pmReserved2;
    cgc_uint32_t    pmOffBits;
} cgc_tPixelmapFileHeader;


typedef struct __attribute__ ((__packed__)) tagPixelmapInfoHeader
{
    cgc_uint32_t    piSize;
    cgc_uint32_t    piWidth;
    cgc_uint32_t    piHeight;
    cgc_uint16_t    piPlanes;
    cgc_uint16_t    piBitCount;
    cgc_uint32_t    piCompression;
    cgc_uint32_t    piSizeImage;
    cgc_uint32_t    piXPelsPerMeter;
    cgc_uint32_t    piYPelsPerMeter;
    cgc_uint32_t    piClrUsed;
    cgc_uint32_t    piClrImportant;
} cgc_tPixelmapInfoHeader;

// Binary writing helpers
cgc_uint32_t cgc_pm_write_u32( cgc_uint8_t *pData, cgc_uint32_t *pFilePos, cgc_uint32_t value )
{
    *((cgc_uint32_t*)(pData+(*pFilePos))) = value;
    (*pFilePos) += sizeof(value);

    return (sizeof(value));
}

cgc_uint32_t cgc_pm_write_u16( cgc_uint8_t *pData, cgc_uint32_t *pFilePos, cgc_uint16_t value )
{
    *((cgc_uint16_t*)(pData+(*pFilePos))) = value;
    (*pFilePos) += sizeof(value);

    return (sizeof(value));
}

cgc_uint32_t cgc_pm_write_u8( cgc_uint8_t *pData, cgc_uint32_t *pFilePos, cgc_uint8_t value )
{
    *((cgc_uint8_t*)(pData+(*pFilePos))) = value;
    (*pFilePos) += sizeof(value);

    return (sizeof(value));
}

cgc_int32_t cgc_pixelmap_write_file( cgc_uint8_t **pFileData, cgc_uint32_t *pFileSize, cgc_uint8_t *rgb_data, cgc_uint32_t width, cgc_uint32_t height )
{
    cgc_uint32_t pmImageSize;
    cgc_uint32_t row, col;
    cgc_tPixelmapFileHeader oFileHeader;
    cgc_tPixelmapInfoHeader oInfoHeader;

    if ( width == 0 || height == 0 )
        return -1;

    if ( rgb_data == NULL )
        return -1;

    if ( PMP_FILE_HEADER_SIZE != sizeof(cgc_tPixelmapFileHeader) )
        return -1;

    if ( PMP_INFO_HEADER_SIZE != sizeof(cgc_tPixelmapInfoHeader) )
        return -1;

    oFileHeader.pmOffBits = (PMP_FILE_HEADER_SIZE + PMP_INFO_HEADER_SIZE);
    oFileHeader.pmType = PMP_HEADER_FILE_TYPE;
    oFileHeader.pmReserved1 = 0;
    oFileHeader.pmReserved2 = 0;

    // Calculate overall image size for RGB
    cgc_uint32_t col_size;

    col_size = (width * 3);

    if ( col_size % 4 != 0 )
        col_size = (col_size & ~0x3) + 4;

    pmImageSize = (col_size * height);

    oFileHeader.pmSize = pmImageSize + PMP_FILE_HEADER_SIZE + PMP_INFO_HEADER_SIZE;

    if ( oFileHeader.pmSize % 4 != 0 )
        oFileHeader.pmSize = (oFileHeader.pmSize & ~0x3) + 4;

    oInfoHeader.piHeight = height;
    oInfoHeader.piWidth = width;

    oInfoHeader.piSize = PMP_INFO_HEADER_SIZE;
    oInfoHeader.piBitCount = PMP_HEADER_BITCOUNT_24BIT_RGB;
    oInfoHeader.piCompression = PMP_HEADER_COMPRESSION_NONE;

    oInfoHeader.piPlanes = 1;
    oInfoHeader.piClrUsed = 0;
    oInfoHeader.piClrImportant = 0;
    oInfoHeader.piXPelsPerMeter = 2835; // 72 DPI
    oInfoHeader.piYPelsPerMeter = 2835; // 72 DPI

    oInfoHeader.piSizeImage = pmImageSize;

    // Allocate file data
    (*pFileData) = cgc_malloc( oFileHeader.pmSize );
    (*pFileSize) = oFileHeader.pmSize;

    cgc_uint8_t *pData = (*pFileData);
    cgc_uint32_t filePos = 0;

    // Write out file data...
    cgc_memcpy( (pData+filePos), &oFileHeader, sizeof(oFileHeader) );
    filePos += sizeof(oFileHeader);

    cgc_memcpy( (pData+filePos), &oInfoHeader, sizeof(oInfoHeader) );
    filePos += sizeof(oInfoHeader);

    for ( row = height; row > 0; row-- )
    {
        cgc_uint32_t col_byte_count = 0;
        for ( col = 0; col < width; col++ )
        {
            cgc_uint8_t red, green, blue;
            cgc_uint32_t data_offset = ((row-1) * (width*3));

            red = rgb_data[data_offset + (col*3)+2];
            green = rgb_data[data_offset + (col*3)+1];
            blue = rgb_data[data_offset + (col*3)+0];

            cgc_pm_write_u8( pData, &filePos, red );
            cgc_pm_write_u8( pData, &filePos, green );
            cgc_pm_write_u8( pData, &filePos, blue );

            col_byte_count += 3;
        }

        // Pad to 4-bytes
        while ( col_byte_count % 4 != 0 )
        {
            pData[filePos++] = 0x0;
            col_byte_count++;
        }
    }


    // Generate file data
    return (filePos);
}

cgc_int32_t cgc_pixelmap_get_size( cgc_uint32_t width, cgc_uint32_t height )
{
    cgc_int32_t pmp_size = PMP_FILE_HEADER_SIZE + PMP_INFO_HEADER_SIZE;
    cgc_int32_t col_size = (width * 3);

    // Add padding to 4-byte boundaries
    if ( col_size % 4 )
        col_size = (col_size & ~0x3) + 4;

    pmp_size = pmp_size + (col_size * height);

    return (pmp_size);
}
