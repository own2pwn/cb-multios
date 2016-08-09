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
#ifndef __DATA_STREAM_H__
#define __DATA_STREAM_H__

// Used to receive incoming data (bit stream)
class cgc_CDataStream
{
public:
    cgc_CDataStream();
    ~cgc_CDataStream();

    void cgc_Setup( cgc_int32_t socketNumber );

    cgc_uint8_t cgc_ReadBit( void );
    cgc_uint8_t cgc_ReadByte( void );

    cgc_uint32_t cgc_BytesReceived( void ) const
    {
        return (m_bytesRxCount);
    }

private:
    void cgc_RefillBuffer( void );

private:
    cgc_uint32_t m_bytesRxCount;
    cgc_int32_t m_socketNumber;

    cgc_uint8_t m_bitpos;
    cgc_uint8_t m_buffer;
};

#endif // __DATA_STREAM_H__
