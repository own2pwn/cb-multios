/*

Author: James Nuttall <james@cromulence.com>

Copyright (c) 2016 Cromulence LLC

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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
}

#include "common.h"

#define SEQ_START 0xa5
#define SEQ_START_2 0x5a

#define INPUT_CAP 512

bool cgc_HexCharToInt( char c, cgc_uint8_t &outValue )
{
	if ( c >= 'A' && c <= 'F' )
		outValue = (10 + (c - 'A'));
	else if ( c >= 'a' && c <= 'f' )
		outValue = (10 + (c -'a'));
	else if ( c >= '0' && c <= '9' )
		outValue = (c - '0');
	else
		return (false);

	return (true);
}

char* cgc_ConvertToHexChars( cgc_uint8_t *pData, cgc_uint32_t dataLen )
{
	int len = dataLen * 2;
	char* str = new char[len];

	for ( cgc_uint32_t i = 0; i < dataLen; i++ )
	{
		cgc_sprintf( &str[ i * 2 ], "$x", (pData[i] >> 4) & 0xF );
		cgc_sprintf( &str[ ( i * 2 )+1 ], "$x", pData[i] & 0xF );
	}
	
	return str;
}


//
// Read the value at the offset and return as a cgc_BYTE (two hex nibbles)
//
cgc_uint16_t cgc_GetByte( CUtil::cgc_String val, int offset )
{
	cgc_uint8_t a = 0;
	cgc_uint8_t b = 0;
	cgc_uint16_t ret_value = 0;
	cgc_HexCharToInt( val.cgc_c_str()[offset], a );
	cgc_HexCharToInt( val.cgc_c_str()[offset+1], b );
	ret_value = a << 4; // move to high nibble
	ret_value += b;
	return ret_value;
}

cgc_uint8_t* cgc_ConvertBackHexChars( cgc_uint8_t *pData, cgc_uint32_t dataLen )
{
	int len = dataLen / 2;
	cgc_uint8_t* str = new cgc_uint8_t[len];

	int j = 0;

	for ( cgc_uint32_t i = 0; i < dataLen; )
	{
		cgc_uint16_t var = 0;
		cgc_uint8_t a,b;
		cgc_HexCharToInt( pData[i], a );
		cgc_HexCharToInt( pData[i+1], b );
		var = a << 4; // move to high nibble
		var += b;
		str[j++] = var;
		i += 2;
	}
	
	return str;
}

void cgc_PrintHexBytes( cgc_uint8_t *pData, cgc_uint32_t dataLen )
{
	for ( cgc_uint32_t i = 0; i < dataLen; i++ )
		cgc_printf( "$x$x", (pData[i] >> 4) & 0xF, pData[i] & 0xF );
	
	cgc_printf( "\n" );
}

