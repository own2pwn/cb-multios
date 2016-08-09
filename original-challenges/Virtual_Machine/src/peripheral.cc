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
extern "C"
{
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
}

#include "peripheral.h"

cgc_CPeripheral::cgc_CPeripheral( cgc_uint8_t peripheralID )
	: m_peripheralID( peripheralID )
{

}

cgc_CPeripheral::~cgc_CPeripheral( )
{

}

cgc_CSendDevice::cgc_CSendDevice( cgc_uint8_t peripheralID )
	: cgc_CPeripheral( peripheralID )
{

}

cgc_CSendDevice::~cgc_CSendDevice( )
{

}

bool cgc_CSendDevice::cgc_Init( void )
{

	return (true);
}

bool cgc_CSendDevice::cgc_Read( cgc_uint8_t *pDest, cgc_uint16_t length )
{
	// Does nothing
	return (true);
}

bool cgc_CSendDevice::cgc_Write( cgc_uint8_t *pDest, cgc_uint16_t length )
{
	if ( !pDest )
		return (false);

	cgc_uint32_t writePos = 0;
	while ( writePos < length )
	{
		size_t tx_bytes;
		cgc_uint32_t writeRemaining = (length - writePos);

		if ( transmit( STDOUT, (const void *)(pDest + writePos), writeRemaining, &tx_bytes ) != 0 )
		      _terminate( -1 );

		if ( tx_bytes == 0 )
			_terminate( -1 );

		writePos += tx_bytes;		
	}

	return (true);
}

cgc_CReadDevice::cgc_CReadDevice( cgc_uint8_t deviceID )
	: cgc_CPeripheral( deviceID )
{

}

cgc_CReadDevice::~cgc_CReadDevice( )
{

}

bool cgc_CReadDevice::cgc_Init( void )
{

	return (true);
}

bool cgc_CReadDevice::cgc_Read( cgc_uint8_t *pDest, cgc_uint16_t length )
{
	if ( !pDest )
		return (false);

	cgc_uint32_t readPos = 0;
	while ( readPos < length )
	{
		size_t rx_bytes;
		cgc_uint32_t readRemaining = (length - readPos);

		if ( receive( STDIN, (void *)(pDest + readPos), readRemaining, &rx_bytes ) != 0 )
		      _terminate( -1 );

		if ( rx_bytes == 0 )
			_terminate( -1 );

		readPos += rx_bytes;		
	}

	return (true);
}

bool cgc_CReadDevice::cgc_Write( cgc_uint8_t *pDest, cgc_uint16_t length )
{
	// Does nothing
	return (true);
}
