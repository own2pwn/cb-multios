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

#ifndef COMMS_H
#define COMMS_H

#include <cutil_string.h>


//
// Handles all return messages
//
class cgc_CommsMessage
{
protected:

	// cgc_uint16_t header
	// cgc_uint16_t version
	// cgc_uint8_t* body
	// cgc_uint16_t body length

	// cgc_uint16_t message type
	// cgc_uint16_t status
	// cgc_uint32_t extended body len

	// Entire message
	CUtil::cgc_String m_message;

	// Body of message
	CUtil::cgc_String m_body;

public:
	cgc_CommsMessage();
	~cgc_CommsMessage();
	
	void cgc_SetBody(CUtil::cgc_String);
	CUtil::cgc_String cgc_GetBody() { return m_body; }
	CUtil::cgc_String cgc_GetMessage() { return m_message; }
	cgc_uint16_t cgc_GetType();
	cgc_uint16_t cgc_GetValue();
	cgc_uint16_t cgc_GetLenValue();
};

class cgc_IncomingMessage : public cgc_CommsMessage
{
private:
	bool m_checksum_passed;

public:

	// read from input
	// store all variables
	void cgc_ReadInput();

	cgc_uint16_t cgc_GetSensorId();
	cgc_uint32_t cgc_GetSensorAddress();
	cgc_uint32_t cgc_GetSensorCoefficient();

	void cgc_GetProgramValues( cgc_uint32_t, cgc_uint32_t &, cgc_uint32_t &, cgc_uint32_t & );

};

class cgc_OutgoingMessage : public cgc_CommsMessage
{
private:
	CUtil::cgc_String m_version;
	cgc_uint16_t m_response;
	CUtil::cgc_String m_message;

public:

	void cgc_SetResponse( cgc_uint16_t resp ) { m_response = resp; }
	void cgc_SetVersion( cgc_uint16_t ); 
	void cgc_SetExtMessage( cgc_uint32_t, cgc_uint8_t* );

	void cgc_SendAsExtended();
	void cgc_SendAsBasic();

	void cgc_Reset() { m_version = ""; m_response = 0; }
};

#endif