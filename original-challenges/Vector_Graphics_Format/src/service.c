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
#include <libcgc.h>
#include <stdlib.h>
#include "io.h"
#include "vgf.h"
#include "pixelmap.h"

#define INPUT_FILE_SIZE_MAXIMUM     (20000)

int main(void)
{
    cgc_setup_io( STDIN, STDOUT );

    cgc_tVGFParsedFile *pFile;

#ifdef PATCHED
    cgc_uint16_t fileSize = cgc_read_u16();
#else
    cgc_int16_t fileSize = cgc_read_u16();
#endif

    if ( fileSize > INPUT_FILE_SIZE_MAXIMUM )
        _terminate( -3 );

    // Read in file data
    cgc_uint8_t *pFileData = (cgc_uint8_t*)cgc_malloc( INPUT_FILE_SIZE_MAXIMUM );

    // Read loop for file data
    cgc_uint32_t readCount = 0;
    for ( readCount = 0; readCount < fileSize; readCount++ )
        pFileData[readCount] = cgc_read_u8();

    if ( readCount != fileSize )
        _terminate( -3 );

    if ( cgc_vgf_parse_data( pFileData, fileSize, &pFile ) != VGF_PARSE_SUCCESS )
        _terminate( -3 );

    // Free memory
    cgc_free( pFileData );

    cgc_int32_t renderSize;
    cgc_uint8_t *pRenderData;

    if ( (renderSize = cgc_vgf_get_render_size( pFile )) <= 0 )
        _terminate( -3 );

    // Allocate render buffer
    cgc_uint32_t destLen = renderSize;
    pRenderData = (cgc_uint8_t*)cgc_malloc( destLen );

    if ( cgc_vgf_render_file( pFile, pRenderData, &destLen ) != 0 )
        _terminate( -3 );

    cgc_uint32_t pmSize;
    cgc_uint8_t *pPMFileData;

    if ( cgc_pixelmap_write_file( &pPMFileData, &pmSize, pRenderData, cgc_vgf_get_width( pFile ), cgc_vgf_get_height( pFile ) ) <= 0 )
        _terminate( -3 );

    // Free the render buffer
    cgc_free( pRenderData );

    // Send the complete file
    size_t tx_bytes = pmSize;
    size_t writeCur;
    for ( writeCur = 0; writeCur < tx_bytes; writeCur++ )
        cgc_write_u8( pPMFileData[writeCur] );

    cgc_free( pPMFileData );

    cgc_vgf_destroy_file( pFile );

    return 0;
}
