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
#include "common.h"

cgc_CDataStream::cgc_CDataStream()
    : m_bitpos( 0 ), m_bytesRxCount( 0 )
{

}

cgc_CDataStream::~cgc_CDataStream()
{

}

void cgc_CDataStream::cgc_Setup( cgc_int32_t socketNumber )
{
    m_socketNumber = socketNumber;
    m_bitpos = 0;
    m_buffer = 0;
}

cgc_uint8_t cgc_CDataStream::cgc_ReadBit( void )
{
    cgc_uint8_t value = 0;

    if ( m_bitpos >= 8 )
        cgc_RefillBuffer();

    value = (m_buffer & (1<<(7-m_bitpos))) >> (7-m_bitpos);

    m_bitpos++;

    return (value);
}

cgc_uint8_t cgc_CDataStream::cgc_ReadByte( void )
{
    cgc_uint8_t value = 0;
    cgc_uint8_t idx;

    for ( idx = 0; idx < 8; idx++ )
        value |= (cgc_ReadBit() << (7-idx));

    return (value);
}

void cgc_CDataStream::cgc_RefillBuffer( void )
{
    cgc_int32_t retvalue;
    cgc_uint8_t value;
    cgc_size_t rx_count = 1;

    if ( m_bytesRxCount >= MAX_BYTES_RECEIVED )
        _terminate( 0 );

    if  ( (retvalue = receive( m_socketNumber, &value, 1, &rx_count )) != 0 )
        _terminate( -1 );

    if ( rx_count != 1 )
        _terminate( -1 );

    m_bytesRxCount++;

    m_buffer = value;
    m_bitpos = 0;
}
