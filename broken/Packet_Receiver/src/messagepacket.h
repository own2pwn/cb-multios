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
#ifndef __MESSAGE_PACKET_H__
#define __MESSAGE_PACKET_H__

class cgc_CMessagePacket
{
public:
    static const cgc_uint8_t FRAGMENTED_BIT = 0x0;         // Indicates fragmented packet
    static const cgc_uint8_t LAST_FRAGMENT_BIT = 0x1;      // Indicates last fragment of fragmented packets
    static const cgc_uint8_t LARGE_CHECKSUM_BIT = 0x2;     // Indicates large checksum (32-bit instead of 16-bit)

    static const cgc_uint8_t HEADER_IDENTIFIER_BITS = 0xF0;       // Validates the packet header
    static const cgc_uint8_t HEADER_IDENTIFIER_VALUE = 0xB0;      // Validates the packet header
    static const cgc_uint8_t HEADER_PARITY_BIT = 0x3;             // Validates the packet header
    static const cgc_uint8_t HEADER_PARITY_BIT_MASK = 0xF7;       // Mask for parity bit analysis

    static const cgc_uint16_t MESSAGE_BYTE_ALIGN_SEQUENCE = 0xC3AC;    // 1100 0011 1010 1100
    static const cgc_uint16_t PREAMBLE_LOCK_COUNT = 15;                // Lock after 15 out of 32 bits have been received

public:
    cgc_CMessagePacket( cgc_uint8_t hdr, cgc_uint8_t len );
    ~cgc_CMessagePacket( );

    static cgc_CMessagePacket *cgc_ParseStream( cgc_CDataStream *pRxData );

    const cgc_uint8_t *cgc_GetData( void )
    {
        if ( cgc_IsFragmented() && m_messageHeader.len >= 2 )
            return (m_pMessageData+2);
        else
            return (m_pMessageData);
    }

    cgc_uint8_t cgc_GetDataLength( void ) const
    {
        if ( cgc_IsFragmented() )
        {
            if ( m_messageHeader.len >= 2 )
                return (m_messageHeader.len - 2);
            else
                return (0);
        }
        else
            return (m_messageHeader.len);
    }

    cgc_uint16_t cgc_GetSequenceNumber( void ) const
    {
        if ( cgc_IsFragmented() && m_messageHeader.len >= 2 )
            return (*((cgc_uint16_t *)m_pMessageData) >> 4);
        else
            return (0);
    }

    cgc_uint8_t cgc_GetFragmentNumber( void ) const
    {
        if ( cgc_IsFragmented() && m_messageHeader.len >= 2 )
            return (cgc_uint8_t)(*((cgc_uint16_t *)m_pMessageData) & 0xF);
        else
            return (0);
    }

    cgc_uint8_t cgc_GetHeader( void ) const
    {
        return (m_messageHeader.hdr);
    }

    bool cgc_IsFragmented( void ) const
    {
        if ( m_messageHeader.hdr & (1<<FRAGMENTED_BIT) )
            return (true);
        else
            return (false);
    }

    bool cgc_IsLastFragment( void ) const
    {
        if ( m_messageHeader.hdr & (1<<LAST_FRAGMENT_BIT) )
            return (true);
        else
            return (false);
    }

    bool cgc_IsLargeChecksum( void ) const
    {
        if ( m_messageHeader.hdr & (1<<LARGE_CHECKSUM_BIT) )
            return (true);
        else
            return (false);
    }

private:
    static cgc_uint8_t cgc_WaitForPreambleLock( cgc_CDataStream *pRxStream );

    void cgc_SetDataByteAt( cgc_uint8_t data_pos, cgc_uint8_t value );

    void cgc_SetChecksum32( cgc_uint32_t new_checksum )
    {
        m_messageHeader.check.checksum32 = new_checksum;
    }

    void cgc_SetChecksum16( cgc_uint16_t new_checksum )
    {
        m_messageHeader.check.checksum16 = new_checksum;
    }

private:
    typedef struct
    {
        cgc_uint8_t hdr;
        cgc_uint8_t len;
        union
        {
            cgc_uint32_t checksum32;
            cgc_uint16_t checksum16;
        } check;
    } cgc_tMessageHeader;

    cgc_tMessageHeader m_messageHeader;
    cgc_uint8_t *m_pMessageData;
};

#endif // __MESSAGE_PACKET_H__
