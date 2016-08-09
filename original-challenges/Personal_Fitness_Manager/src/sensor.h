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

#ifndef FITNESS_SENSOR_H
#define FITNESS_SENSOR_H

#include "cutil_list.h" // custom linked list

#define INVALID_SENSOR_TYPE 0xff

// index values into the m_sensorArray... not used directly as values
enum
{
	HEART = 0,
	STEP,
	SCALE,
	RUN,
	BIKE,
	MAX_SENSOR_VALUE

} ValidSensorTypes;

typedef struct
{
	cgc_uint8_t Type;
	cgc_uint8_t Val_1;
	cgc_uint8_t Val_2;
	cgc_uint8_t Val_3;
} cgc_SensorMacFormat;

class cgc_FitnessSensor
{

private:
	cgc_uint16_t m_sensorID;
	cgc_uint8_t m_sensorType; // pulled from MAC
	cgc_SensorMacFormat m_sensorMAC;
	cgc_uint8_t *data;
	bool m_invalid;

	cgc_uint16_t m_owner; // user this sensor is registered for


public:
	cgc_FitnessSensor() {}
	cgc_FitnessSensor( cgc_uint16_t, cgc_uint32_t, cgc_uint8_t *, cgc_uint32_t);

	cgc_uint16_t cgc_GetID() { return m_sensorID; }
	cgc_SensorMacFormat cgc_GetMAC() { return m_sensorMAC; }
	cgc_uint32_t cgc_GetMacAsInt();
	cgc_uint8_t cgc_GetType() { return m_sensorType; }

	void cgc_SetID(cgc_uint16_t id) { m_sensorID = id; }
	bool cgc_SetMAC(cgc_uint32_t mac);

	bool cgc_IsInvalid() { return m_invalid; }

	cgc_uint8_t cgc_SetUser( cgc_uint16_t user );
	cgc_uint16_t cgc_GetUser() { return m_owner; }

	cgc_uint8_t *cgc_GetData() { return data; }

	static cgc_uint8_t m_sensorArray[ MAX_SENSOR_VALUE ];
	static bool cgc_GenerateTypeValues();
	static bool cgc_GetSensorTypeValue( cgc_uint8_t lookup, cgc_uint8_t &val );

	static cgc_uint16_t cgc_HandleBikeSensor( cgc_uint16_t );
	static cgc_uint16_t cgc_HandleHeartSensor( cgc_uint16_t );
	static cgc_uint16_t cgc_HandleScaleSensor( cgc_uint16_t );
	static cgc_uint16_t cgc_HandleStepSensor( cgc_uint16_t );
	static cgc_uint16_t cgc_HandleRunSensor( cgc_uint16_t );

	
	// Test only
	void cgc_Print();

	// used for linked list in cgc_SensorManager
	CUtil::DLL_LINK( cgc_FitnessSensor ) m_sensorLink; 
};

#endif