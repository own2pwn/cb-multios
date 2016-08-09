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

#include "comms.h"
extern "C"
{
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <stdlib.h>
}
#include "common.h"

#define INPUT_CAP 512
#define SEQ_START 0xa5
#define SEQ_START_2 0x5a

#define BASIC_RESPONSE "0000"
#define EXT_RESPONSE "0100"
#define BASIC_EXT_RESPONSE "00000000"

cgc_CommsMessage::cgc_CommsMessage()
{

}

cgc_CommsMessage::~cgc_CommsMessage()
{
	
}

cgc_uint16_t cgc_swap( cgc_uint16_t val )
{
	return ( ( val & 0xff ) << 8 ) | ( ( val & 0xff00 ) >> 8 );
}

cgc_uint32_t cgc_swap_int ( cgc_uint32_t val )
{
	return ( val >> 24 ) | ( ( val << 8 ) & 0x00FF0000 ) | ( ( val >> 8 ) & 0x0000FF00 ) | ( val << 24 );
}

//
// Need to set: m_response, m_type, 
//
void cgc_OutgoingMessage::cgc_SendAsExtended()
{
	// 1. Construct string
	CUtil::cgc_String message;
	message = "a55a"; // flag 2B
	message += m_version; // version 2B
	message += EXT_RESPONSE; // message type 2B

	char arr[512];
	cgc_bzero(arr, 512);
	cgc_sprintf( arr, "$04x", cgc_swap( m_response ) );
	message += arr; // 2B
	cgc_bzero(arr, 512);

	cgc_sprintf( arr, "$08x", cgc_swap_int( m_message.cgc_GetLength() / 2 ) );
	message += arr; // ext message size 4B

	message += m_message;
	
	// opposite of cgc_ConvertToHexChars
	cgc_uint8_t* final_msg = cgc_ConvertBackHexChars( ( cgc_uint8_t * )message.cgc_c_str(), message.cgc_GetLength() );

	int retval = cgc_write( final_msg, message.cgc_GetLength() / 2 );

	if (retval == -1) 
		_terminate(-1);

	delete[] final_msg;
}

//
// Only need to set: m_response
// Type unused
// Extended response unused
//
void cgc_OutgoingMessage::cgc_SendAsBasic()
{
	// 1. Construct string
	CUtil::cgc_String message;
	message = "a55a"; // flag 2B
	message += m_version; // version 2B
	message += BASIC_RESPONSE; // message type 2B

	char arr[512];
	int ret = cgc_sprintf( arr, "$04x", cgc_swap ( m_response ) );
	message += arr; // 2B

	message += BASIC_EXT_RESPONSE; // ext message size 4B

	// opposite of cgc_ConvertToHexChars
	cgc_uint8_t* final_msg = cgc_ConvertBackHexChars( ( cgc_uint8_t * )message.cgc_c_str(), 24 );
	int retval = cgc_write( final_msg, 12 );

	if (retval == -1) 
		_terminate(-1);

	delete[] final_msg;
}

void cgc_CommsMessage::cgc_SetBody(CUtil::cgc_String body)
{
	m_body = body;
}

//
// return a 2B value from a cgc_String
//
cgc_uint16_t cgc_CommsMessage::cgc_GetType()
{
	// return the type of message
	cgc_uint16_t retVal = 0;

	retVal = cgc_GetByte( m_body, 2 ) << 8;
	retVal += cgc_GetByte( m_body, 0 );

	return retVal;
}

cgc_uint16_t cgc_CommsMessage::cgc_GetValue()
{

	// return the type of message
	cgc_uint16_t retVal = 0;

	retVal = cgc_GetByte( m_body, 6 ) << 8;
	retVal += cgc_GetByte( m_body, 4 );
	

	return retVal;
}

cgc_uint16_t cgc_CommsMessage::cgc_GetLenValue()
{

	// return the type of message
	cgc_uint16_t retVal = 0;

	retVal = cgc_GetByte( m_message, 6 ) << 8;
	retVal += cgc_GetByte( m_message, 4 );
	

	return retVal;
}

void cgc_OutgoingMessage::cgc_SetExtMessage( cgc_uint32_t value, cgc_uint8_t* buff )
{
	char * tmp;
	tmp = cgc_ConvertToHexChars( buff, value );

	m_message = CUtil::cgc_String( ( char* )tmp );

	delete[] tmp;
}

void cgc_OutgoingMessage::cgc_SetVersion( cgc_uint16_t new_ver )
{
	char * tmp;
	tmp = cgc_ConvertToHexChars( ( cgc_uint8_t * )&new_ver, sizeof( cgc_uint16_t ) );

	m_version = CUtil::cgc_String( ( char* )tmp );

	delete[] tmp;
}

//
// Only for Add Sensor messages 
//
cgc_uint16_t cgc_IncomingMessage::cgc_GetSensorId()
{
	cgc_uint8_t offset_sensor_id_1 = 12;
	cgc_uint8_t offset_sensor_id_2 = 14;

	// return the type of message
	cgc_uint16_t retVal = 0;

	retVal = cgc_GetByte( m_message, offset_sensor_id_2 ) << 8;
	retVal += cgc_GetByte( m_message, offset_sensor_id_1 );
	
	return retVal;
}

//
// Only for Add Sensor messages 
//
cgc_uint32_t cgc_IncomingMessage::cgc_GetSensorAddress()
{
	cgc_uint8_t offset_sensor_address_1 = 16;
	cgc_uint8_t offset_sensor_address_2 = 18;
	cgc_uint8_t offset_sensor_address_3 = 20;
	cgc_uint8_t offset_sensor_address_4 = 22;

	// return the type of message
	cgc_uint32_t retVal = 0;

	retVal = cgc_GetByte( m_message, offset_sensor_address_4 ) << 24;
	retVal += cgc_GetByte( m_message, offset_sensor_address_3 ) << 16;
	retVal += cgc_GetByte( m_message, offset_sensor_address_2 ) << 8;
	retVal += cgc_GetByte( m_message, offset_sensor_address_1 );
	
	return retVal;
}

//
// Only for Add Sensor messages 
//
cgc_uint32_t cgc_IncomingMessage::cgc_GetSensorCoefficient()
{
	cgc_uint8_t offset_sensor_coeff_1 = 24;
	cgc_uint8_t offset_sensor_coeff_2 = 26;
	cgc_uint8_t offset_sensor_coeff_3 = 28;
	cgc_uint8_t offset_sensor_coeff_4 = 30;

	// return the type of message
	cgc_uint32_t retVal = 0;

	retVal = cgc_GetByte( m_message, offset_sensor_coeff_4 ) << 24;
	retVal += cgc_GetByte( m_message, offset_sensor_coeff_3 ) << 16;
	retVal += cgc_GetByte( m_message, offset_sensor_coeff_2 ) << 8;
	retVal += cgc_GetByte( m_message, offset_sensor_coeff_1 );
	
	return retVal;
}


void cgc_IncomingMessage::cgc_ReadInput()
{
	cgc_uint8_t input_chars[INPUT_CAP];
	unsigned char c;
	cgc_size_t num_bytes;
	cgc_size_t pos = 0;

	cgc_size_t body_start = 0;

begin:
	while ( 1 )
	{
#ifdef PATCHED_1
		pos = 0;
#endif
		// after a55a, start reading
		// read the length (which contains the checksum), then return
		// further checking happens later

		// read until first a5 is found, then start paying attention
		cgc_receive_bytes( &c, 1 );

		if ( c == SEQ_START )
		{
			// store a5
			input_chars[pos++] = c;

			cgc_receive_bytes( &c, 1 );
			
			if ( c == SEQ_START_2 )
			{
				// store 5a
				input_chars[pos++] = c;
			}
			else
				goto begin;

			// get body length
			cgc_uint16_t body_len = 0;
			
			cgc_receive_bytes( ( cgc_uint8_t* )&input_chars[pos], 2 );

			body_len = *( unsigned short * )( &input_chars[pos] );

			pos += 2;

			if ( body_len > INPUT_CAP - 4 )
				continue;

			cgc_receive_bytes( ( cgc_uint8_t* )&input_chars[pos], body_len );

			body_start = pos * 2; // account for bytes to nibbles

			pos += body_len;

			cgc_uint32_t checkval = 0;

			for ( int i = 0; i < body_len; ++i )
			{
				checkval = checkval + input_chars[i]*(i+1);
			}

			cgc_uint32_t given_checksum = *( cgc_uint32_t* )&input_chars[body_len];
			cgc_memcpy( ( unsigned char * )&given_checksum, input_chars + body_len, 4 );

			if (checkval != given_checksum)
				m_checksum_passed = false;
			else
			{
				m_checksum_passed = true;
				input_chars[pos] = '\0';
				this->m_message = CUtil::cgc_String( cgc_ConvertToHexChars( ( cgc_uint8_t * )input_chars, pos ) );
				this->m_body = CUtil::cgc_String( cgc_ConvertToHexChars( ( cgc_uint8_t * )input_chars, pos ) ).cgc_SubString( body_start, body_start + (2 * body_len ) - 8 );
			}
			return;
		}
	}
	return;
}


//
// Input: count -- number of program steps
// Output: 1st, 2nd, and 3rd values in the program
//
void cgc_IncomingMessage::cgc_GetProgramValues(cgc_uint32_t offset, cgc_uint32_t &a, cgc_uint32_t &b, cgc_uint32_t &c)
{
	int num_nibbles_to_start = 16; // 8 bytes
	int num_nibbles_for_each_program = 24; // 12 bytes

	cgc_uint8_t byte1 = offset + 0;
	cgc_uint8_t byte2 = offset + 2;
	cgc_uint8_t byte3 = offset + 4;
	cgc_uint8_t byte4 = offset + 6;

	a = cgc_GetByte( m_message, byte4 ) << 24;
	a += cgc_GetByte( m_message, byte3 ) << 16;
	a += cgc_GetByte( m_message, byte2 ) << 8;
	a += cgc_GetByte( m_message, byte1 );

	byte1 = offset + 8;
	byte2 = offset + 10;
	byte3 = offset + 12;
	byte4 = offset + 14;

	b = cgc_GetByte( m_message, byte4 ) << 24;
	b += cgc_GetByte( m_message, byte3 ) << 16;
	b += cgc_GetByte( m_message, byte2 ) << 8;
	b += cgc_GetByte( m_message, byte1 );

	byte1 = offset + 16;
	byte2 = offset + 18;
	byte3 = offset + 20;
	byte4 = offset + 22;

	c = cgc_GetByte( m_message, byte4 ) << 24;
	c += cgc_GetByte( m_message, byte3 ) << 16;
	c += cgc_GetByte( m_message, byte2 ) << 8;
	c += cgc_GetByte( m_message, byte1 );
}

