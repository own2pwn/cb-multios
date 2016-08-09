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

#ifndef SENSOR_H
#define SENSOR_H

#include "cutil_list.h" // custom linked list


class cgc_Sensor
{
	// create
	// destroy
	
private:
	cgc_uint16_t m_type;
	cgc_uint32_t m_address;
	cgc_uint32_t m_coefficient;
	bool m_enabled;

public:
	cgc_Sensor();
	cgc_Sensor( int, int, int, bool );
	~cgc_Sensor();

	void cgc_SetType( int val ) { m_type = val; }
	int cgc_GetType() { return m_type; }

	void cgc_SetEnabled( bool val ) { m_enabled = val; }
	bool cgc_GetEnabled() { return m_enabled; }

	CUtil::DLL_LINK( cgc_Sensor ) m_sensorLink;
};

#endif