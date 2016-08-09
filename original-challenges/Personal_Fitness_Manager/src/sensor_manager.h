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

#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

extern "C"
{
	#include <libcgc.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include "string.h"
}
#include "cutil_list.h"
#include "common.h"
#include "sensor.h"
#include "user.h"

#define RET_USER_ADDED 0
#define RET_USER_REDUNDANT 1
#define RET_USER_FULL 2

#define MAX_SENSORS_PER_USER 3
#define MAX_USERS 7

class cgc_SensorManager
{
private:
	CUtil::DLL_LIST( cgc_FitnessSensor, m_sensorLink ) m_sensorList;

	cgc_uint16_t m_currentUser;
	CUtil::DLL_LIST( cgc_User, m_userLink ) m_userList;
	
	//
	// Is this a proper fitness sensor?
	//
	bool cgc_Validate ( cgc_FitnessSensor );

public:
	cgc_SensorManager() {}
	~cgc_SensorManager();

	// SENSOR management
	int cgc_AddSensor( cgc_FitnessSensor *sensor );
	cgc_FitnessSensor* cgc_GetSensor( cgc_uint16_t id );

	bool cgc_RemoveSensor( cgc_uint16_t id, cgc_uint16_t user );
	bool cgc_VerifySensor( cgc_uint16_t id );
	void cgc_PrintSensors(); // TEST ONLY
	cgc_uint8_t *cgc_ListSensors( cgc_uint16_t &len );
	bool cgc_ListHwIds( cgc_uint16_t &len, cgc_uint8_t* buff );


	// USER management
	cgc_uint16_t cgc_GetCurrentUser() { return m_currentUser; }
	cgc_uint8_t cgc_SetCurrentUser( cgc_uint16_t user );
	cgc_uint8_t cgc_AddUser( cgc_User* new_user );

	bool cgc_AddDistance( cgc_uint16_t new_dist, cgc_uint16_t user );
};

#endif