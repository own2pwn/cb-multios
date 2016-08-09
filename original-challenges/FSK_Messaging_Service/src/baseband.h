/*

Author: Jason Williams <jdw@cromulence.com>

Copyright (c) 2015 Cromulence LLC

Permission is hereby granted, free of charge, to any person obtaining a copy
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
#ifndef __BASEBAND_H__
#define __BASEBAND_H__

#include <stdint.h>

#define DEBUG_BASEBAND			(0)

// CDR stands for Clock Data Recovery
#define CDR_STATE_NODATA		(0)
#define CDR_STATE_PREAMBLE		(1)
#define CDR_STATE_LOCK			(2)

#define PACKET_STATE_NODATA		(0)
#define PACKET_STATE_SYNCWORD		(1)
#define PACKET_STATE_HEADER		(2)
#define PACKET_STATE_DATA		(3)
#define PACKET_STATE_CRC		(4)

// Number of preamble bits to receive before locking on the preamble (should start CDR)
#define PREAMBLE_LOCK_COUNT		(15)

#define BIT_SYNC_WORD			0xCFA3

typedef struct
{
	cgc_uint8_t cdrState;	// Clock Data Recovery state

	cgc_uint8_t lastBitValue;	// The last bit value received (0 or 1)

	double samplesPerClock;	// Number of samples per clock
	double clocksForNextSymbol;	// Number of clock cycles until sampling next symbol	
	cgc_uint32_t symbolsSinceLastTransition;	// Number of samples since last transition	
	cgc_uint32_t sampleCounter;	// Number of samples recorded between different symbols
	cgc_uint32_t preambleCounter;	// Number of preamble bits received
} cgc_tCDRState;

typedef struct
{
	cgc_uint8_t packetState;	// Packet receiver state machine

	cgc_uint8_t lastPreambleBit;// Last bit received for preamble before sync word
	cgc_uint8_t syncBitPos;	// Position in the sync word	
	
	cgc_uint8_t packetDataLen; 		// Length of the packet data
	cgc_uint8_t packetDataBytePos;	// Current position in the receiving packet data
	cgc_uint8_t packetDataBitPos;	// Current bit position in the receiving packet data
	cgc_uint8_t packetData[256];	// Packet data as being received

	cgc_uint16_t packetCRC;		// Received packet CRC
} cgc_tPacketState;

typedef struct
{
	cgc_tCDRState cdrState;
	cgc_tPacketState packetState;
} cgc_tBasebandState;

void cgc_init_baseband( cgc_tBasebandState *pState );
void cgc_reset_baseband_state( cgc_tBasebandState *pState );

void cgc_run_cdr( cgc_tBasebandState *pState, cgc_uint8_t sample_in );
void cgc_process_sample( cgc_tBasebandState *pCurState, cgc_uint8_t symbol_in ); 
void cgc_do_sample( cgc_tBasebandState *pState, cgc_uint8_t symbol_in );

#endif // __BASEBAND_H__
