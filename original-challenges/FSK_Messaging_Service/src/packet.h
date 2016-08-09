/*

Author: Jason Williams <jdw@cromulence.com>

Copyright (c) 2015 Cromulence LLC

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
#ifndef __PACKET_H__
#define __PACKET_H__

#include <stdint.h>

#define MAX_PACKET_LENGTH		(48)
#define MAX_USERNAME_LENGTH		(8)

#define DEBUG_PACKET			(0)

typedef void (*cgc_fpPacketTypeHandler)( cgc_uint8_t *pPacketData, cgc_uint32_t dataLen );

typedef struct
{
	cgc_uint8_t packetType;
	cgc_fpPacketTypeHandler fpHandler;		
} cgc_tPacketTypeHandler;

typedef struct
{
	cgc_tPacketTypeHandler *pHandlers;
	cgc_uint8_t handlerCount;
} cgc_tPacketHandler;

typedef struct PACKET_INFO_STRUCT
{
	cgc_uint8_t packetData[MAX_PACKET_LENGTH];
	cgc_uint32_t dataLen;
	struct PACKET_INFO_STRUCT *pNextPacket;
	cgc_fpPacketTypeHandler fpHandler;		
	cgc_uint8_t packetType;
} cgc_tSinglePacketData;

typedef struct
{
	cgc_tSinglePacketData *pPacketList;
	cgc_uint32_t packetCount;
	cgc_uint32_t byteCount;
	cgc_uint32_t invalidPacketCount;
} cgc_tPacketData;

void cgc_init_packet_handler( void );
void cgc_receive_packet( cgc_uint8_t *pData, cgc_uint8_t dataLen, cgc_uint16_t packetCRC );
void cgc_add_new_packet( cgc_uint8_t packetType, cgc_fpPacketTypeHandler fpHandler, cgc_uint8_t *pData, cgc_uint8_t dataLen );
void cgc_destroy_packet_handler( void );

void cgc_display_packets( void );

void cgc_HandleBroadcastPacket( cgc_uint8_t *pData, cgc_uint32_t dataLen );
void cgc_HandleChannelPacket( cgc_uint8_t *pData, cgc_uint32_t dataLen );
void cgc_HandlePrivatePacket( cgc_uint8_t *pData, cgc_uint32_t dataLen );
void cgc_HandleConnectPacket( cgc_uint8_t *pData, cgc_uint32_t dataLen );
void cgc_HandleDisconnectPacket( cgc_uint8_t *pData, cgc_uint32_t dataLen );

#endif // __PACKET_H__
