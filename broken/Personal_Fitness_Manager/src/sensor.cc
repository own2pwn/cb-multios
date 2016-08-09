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
	#include <libcgc.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include "string.h"
}

#include "sensor.h"
#include "common.h"

cgc_uint8_t cgc_FitnessSensor::m_sensorArray[];

//
// Is this Sensor MAC type valid?
// The first byte of the sensor's MAC should give the type of sensor
//
bool cgc_VerifyMacType( cgc_uint8_t type )
{
	// Real version: look in the sensor array, which is initialized during startup by the magic page cgc_GenerateTypeValues
	for( int i = 0; i < MAX_SENSOR_VALUE; i++ )
	{
		if ( type == cgc_FitnessSensor::m_sensorArray[ i ] )
			return true;
	}

	return false;
}

cgc_uint8_t cgc_GetMacType( cgc_uint8_t type )
{
	for( int i = 0; i < MAX_SENSOR_VALUE; i++ )
	{
		if ( type == cgc_FitnessSensor::m_sensorArray[ i ] )
			return i;
	}
	return 0xff;
}

//
// sensor ID, sensor MAC
//
cgc_FitnessSensor::cgc_FitnessSensor( cgc_uint16_t id, cgc_uint32_t mac, cgc_uint8_t *data, cgc_uint32_t data_len )
{
	this->m_sensorID = id;

	this->m_invalid = false;
	if ( this->cgc_SetMAC( mac ) == false )
		this->m_invalid = true;

	this->data = NULL;
	if (this->m_sensorType == BIKE)
	{
		if (data_len != 0)
		{
			this->data = new cgc_uint8_t[ data_len + 1];
			cgc_memcpy(this->data, data, data_len);
		}
	}


}

cgc_uint32_t cgc_swap_int ( cgc_uint32_t val )
{
	return ( val >> 24 ) | ( ( val << 8 ) & 0x00FF0000 ) | ( ( val >> 8 ) & 0x0000FF00 ) | ( val << 24 );
}

bool cgc_FitnessSensor::cgc_SetMAC( cgc_uint32_t mac )
{
	mac = cgc_swap_int(mac);
	cgc_SensorMacFormat *mMac = ( cgc_SensorMacFormat* )&mac;

	if ( cgc_VerifyMacType( mMac->Type ) == false )
	{
		this->m_invalid = true;
		return false;
	}
	this->m_sensorType = cgc_GetMacType( mMac->Type );
 
	this->m_sensorMAC = *mMac;
	return true;
}

cgc_uint32_t cgc_FitnessSensor::cgc_GetMacAsInt()
{
	cgc_uint32_t val = 0;

	val = this->m_sensorMAC.Type << 24;
	val += this->m_sensorMAC.Val_1 << 16;
	val += this->m_sensorMAC.Val_2 << 8;
	val += this->m_sensorMAC.Val_3 << 0;

	return val;
}

//
// Save the owner's ID with this sensor
// Allowed to have multiple owners own same sensor
// Not allowed to have the same user own multiple sensor with smae ID
//
cgc_uint8_t cgc_FitnessSensor::cgc_SetUser( cgc_uint16_t user )
{
	// if this is already registered to this user, return ERROR_REG_THIS_USER
	if ( user == m_owner )
		return ERROR_REG_THIS_USER;
	
	m_owner = user;
	return 0;
}


bool cgc_AlreadyInValues( cgc_uint8_t val )
{
	for ( int i = 0; i < MAX_SENSOR_VALUE; i++ )
	{
		if ( cgc_FitnessSensor::m_sensorArray[i] == val )
		{
			return true;
		}
	}
	return false;
}

//
// Called during init to supply values from magic page
//
bool cgc_FitnessSensor::cgc_GenerateTypeValues()
{
	// use magic page to initialize the type values for: heart, bike, band, scale, run sensors

	// Goal: Get magic values for each of the sensor values to be stored in m_sensorArray
	// Algorithm: Use magic_page values as offsets into the magic_page to select a random assortment of values

	cgc_uint8_t *magic_page = ( cgc_uint8_t* )MAGIC_PAGE;

	cgc_uint16_t curr_offset = 0;

	cgc_uint8_t a = 0;

	for ( int i = 0; i < MAX_SENSOR_VALUE; i++ )
	{
		// get the value at the address of the last value
		
		int j = 0;
		do
		{
			a = magic_page[ curr_offset + j++ ];	
		}
		while ( cgc_AlreadyInValues( a ) == true );

		cgc_FitnessSensor::m_sensorArray[ i ] = a;
		curr_offset = magic_page[ curr_offset + 1 ];
	}
	return true;
}

//
// Used to retrieve the dynamic sensor's type value
// The value was generated from cgc_GenerateTypeValues during startup
 bool cgc_FitnessSensor::cgc_GetSensorTypeValue( cgc_uint8_t lookup, cgc_uint8_t &val )
 {
  	for ( int i = 0; i < MAX_SENSOR_VALUE; i++ )
 	{
 		if ( cgc_FitnessSensor::m_sensorArray[ i ] == lookup )
 		{
 			val = i;
 			return true;
 		}	
 	}

 	return false;
 }

//1b INPUT_SENSOR_DATA
//1b sensor type
//2b sensor id
//*b sensor data
cgc_uint16_t cgc_FitnessSensor::cgc_HandleHeartSensor( cgc_uint16_t beats )
{
	// add these heartbeat to something
	// 2b: heart rate
	return beats;
}

cgc_uint16_t cgc_FitnessSensor::cgc_HandleStepSensor( cgc_uint16_t steps )
{
	return steps / 2;
}

cgc_uint16_t cgc_FitnessSensor::cgc_HandleScaleSensor( cgc_uint16_t weight )
{
	// add weight to data points
	// 2b: weight
	return weight;
}

cgc_uint16_t cgc_FitnessSensor::cgc_HandleBikeSensor( cgc_uint16_t dist )
{
	// calculate the equivalent 
	cgc_uint16_t final_value = dist / 3;

	return final_value;
}

cgc_uint16_t cgc_FitnessSensor::cgc_HandleRunSensor( cgc_uint16_t dist )
{
	return dist;
}

// test only. prints contents to screen
void cgc_FitnessSensor::cgc_Print()
{
	cgc_printf( "SENSOR ID $x / TYPE: $x / OWNER: $x MAC: $x$x$x\n", this->m_sensorID, this->m_sensorType, this->m_owner, this->m_sensorMAC.Val_1, this->m_sensorMAC.Val_2, this->m_sensorMAC.Val_3 );
}










