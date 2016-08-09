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

cgc_CMessagePacket::cgc_CMessagePacket( cgc_uint8_t hdr, cgc_uint8_t len )
{
    m_messageHeader.hdr = hdr;
    m_messageHeader.len = len;
    m_pMessageData = new cgc_uint8_t[len];
}

cgc_CMessagePacket::~cgc_CMessagePacket( )
{
    if ( m_pMessageData )
        delete [] m_pMessageData;
}

void cgc_CMessagePacket::cgc_SetDataByteAt( cgc_uint8_t data_pos, cgc_uint8_t value )
{
    if ( !m_pMessageData )
        return;

    if ( data_pos >= m_messageHeader.len )
        return;

    m_pMessageData[data_pos] = value;
}

cgc_uint8_t cgc_CMessagePacket::cgc_WaitForPreambleLock( cgc_CDataStream *pRxStream )
{
    cgc_uint32_t lockBitCount = 0;
    cgc_uint8_t lockState = 1;

    // Wait for receiver lock on preamble
    for ( ; lockBitCount < PREAMBLE_LOCK_COUNT; )
    {
        cgc_uint8_t readBit = pRxStream->cgc_ReadBit();

        if ( lockState == 1 )
        {
            if ( readBit == 1 )
            {
                // Advance state
                lockState = 0;
                lockBitCount++;
            }
            else
            {
                // Reset
                lockState = 1;
                lockBitCount = 0;
            }
        }
        else
        {
            if ( readBit == 0 )
            {
                // Advance state
                lockState = 1;
                lockBitCount++;
            }
            else
            {
                // Reset
                lockState = 1;
                lockBitCount = 0;
            }
        }
    }

    return (lockState);
}

cgc_CMessagePacket *cgc_CMessagePacket::cgc_ParseStream( cgc_CDataStream *pRxStream )
{
    // Wait for preamble lock
    cgc_uint8_t preambleState = cgc_WaitForPreambleLock( pRxStream );

#if DEBUG_MESSAGES
    cgc_printf( "Preamble lock\n" );
#endif

    // Find synch byte
    // Stay on sequence if it is the preamble -- if it diverges it should be the byte align sequence
    cgc_uint8_t lockState = 0;
    cgc_uint8_t readBit = 0;
    cgc_uint8_t byteLockPos = 0;

    bool bByteLocked = false;
    while ( !bByteLocked )
    {
        readBit = pRxStream->cgc_ReadBit();

        if ( lockState == 0 )
        {
            // Tracking preamble
            if ( preambleState == 1 )
            {
                if ( readBit == 0 )
                {
                    // Invalid state
                    return (NULL);
                    // Expecting one -- got 0 -- puts at
                }
                else
                    preambleState = 0;
            }
            else
            {
                if ( readBit == 1 )
                {
                    // Expecting 0 got 1 -- possible byte sequence
                    lockState = 1;
                }
                else
                    preambleState = 1;
            }
        }
        else
        {
            // Find position in the byte sequence -- we may not know what position it is yet!
            if ( byteLockPos == 0 )
            {
                if ( readBit == 1 )
                {
                    // We've read two 1's
                    byteLockPos = 3;
                }
                else
                {
                    // We've read two 1's and a zero
                    byteLockPos = 4;
                }
            }
            else
            {
                if ( readBit != ((MESSAGE_BYTE_ALIGN_SEQUENCE >> (16-byteLockPos)) & 0x1) )
                    return (NULL);

                byteLockPos++;

                // Counting by index starting at 1
                if ( byteLockPos >= 17 )
                    bByteLocked = true;
            }
        }
    }

#if DEBUG_MESSAGES
    cgc_printf( "Byte lock acquired\n" );
#endif

    // Read header
    cgc_uint8_t hdr;
    cgc_uint8_t data_len;

    hdr = pRxStream->cgc_ReadByte();

    // Validate header...
    if ( (hdr & HEADER_IDENTIFIER_BITS) != HEADER_IDENTIFIER_VALUE )
        return (NULL);

    // Validate header parity bit
    cgc_uint8_t parity_check_value = 0;
    for ( cgc_uint8_t i = 0; i < 8; i++ )
    {
        if ( (1<<i) & HEADER_PARITY_BIT_MASK )
            parity_check_value += ((hdr & (1<<i)) >> i) & 0x1;
    }

    // Even or odd (even = 0, odd = 1)
    parity_check_value = parity_check_value % 2;

    cgc_uint8_t parity_value = ((hdr & (1<<HEADER_PARITY_BIT)) >> HEADER_PARITY_BIT) & 0x1;

    // Verify parity bit
    if ( parity_value != parity_check_value )
        return (NULL);

    data_len = pRxStream->cgc_ReadByte();

    // Allocate new message packet
    cgc_CMessagePacket *pNewPacket = new cgc_CMessagePacket( hdr, data_len );

    // Read in data
    cgc_uint32_t checksum32 = 0;
    cgc_uint16_t checksum16 = 0;
    for ( cgc_uint8_t data_pos = 0; data_pos < data_len; data_pos++ )
    {
        cgc_uint8_t read_byte = pRxStream->cgc_ReadByte();

        checksum32 += read_byte;
        checksum16 += read_byte;

        pNewPacket->cgc_SetDataByteAt( data_pos, read_byte );
    }

    // Now read in checksum
    if ( pNewPacket->cgc_IsLargeChecksum() )
    {
        // Read in a large checksum!
        cgc_uint32_t new_checksum = 0;

        new_checksum = (pRxStream->cgc_ReadByte() << 24);
        new_checksum |= (pRxStream->cgc_ReadByte() << 16);
        new_checksum |= (pRxStream->cgc_ReadByte() << 8);
        new_checksum |= (pRxStream->cgc_ReadByte());

        if ( checksum32 != new_checksum )
        {
            delete pNewPacket;

            return (NULL);
        }

        // Assign checksum 32-bit
        pNewPacket->cgc_SetChecksum32( new_checksum );
    }
    else
    {
        cgc_uint16_t new_checksum = 0;

        new_checksum = (pRxStream->cgc_ReadByte() << 8);
        new_checksum |= (pRxStream->cgc_ReadByte());

        if ( checksum16 != new_checksum )
        {
            delete pNewPacket;

            return (NULL);
        }

        // Assign checksum 16-bit
        pNewPacket->cgc_SetChecksum16( new_checksum );
    }

#ifdef DEBUG_MESSAGES
    cgc_printf( "Packet received\n" );
#endif

    // Return newly read packet
    return (pNewPacket);
}
