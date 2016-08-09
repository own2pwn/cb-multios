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

#include "controller.h"

//#define RESPONSE_HEADER "5AA5"
#define VERSION 0x10a
#define BASIC_RESPONSE "00"
#define BASIC_RESPONSE_CODE "00"
#define EXTENDED_RESPONSE "01"

#define ON 1
#define OFF 0



cgc_Controller::cgc_Controller() 
{
	m_current_sensor_count = 0;
	m_version = VERSION;
	m_currentProgramStep = -1;
	m_powerOn = false;
	m_heaterOn = false;
	m_currentTemp = AMBIENT_TEMP;
	m_set_temp = 0;
	m_smokeSensorPresent = false;

	cgc_ClearBackupProgram();
	cgc_ClearProgram();
}

cgc_Controller::~cgc_Controller() 
{
	m_sensor_list.cgc_DeleteAll();
}

//
// Add a sensor to the sensor list
//
bool cgc_Controller::cgc_AddSensor( cgc_Sensor *s ) 
{
	// already full?
	if ( m_current_sensor_count >= 10 )
		return false; 

	m_sensor_list.cgc_AddLast( s );

	m_current_sensor_count += 1;

	return true;
}

bool cgc_Controller::cgc_SetSensor( cgc_uint16_t sensor_type, bool val )
{
	for ( cgc_Sensor *pCur = m_sensor_list.cgc_GetFirst(); pCur; pCur = m_sensor_list.cgc_GetNext( pCur ) )
	{
		if ( pCur->cgc_GetType() == sensor_type )
		{
			pCur->cgc_SetEnabled( ( bool )val );

			return true;
		}
	}
	return false;
}

//
// Remove a sensor to the sensor list
//
bool cgc_Controller::cgc_RemoveSensor( cgc_uint16_t id )
{
	for ( cgc_Sensor *pCur = m_sensor_list.cgc_GetFirst(); pCur; pCur = m_sensor_list.cgc_GetNext( pCur ) )
	{
		if ( pCur->cgc_GetType() == id )
		{
			delete pCur;

			m_current_sensor_count -= 1;
			
			return true;
		}
	}
	return false;
}

cgc_uint16_t cgc_Controller::cgc_GetSensorCount()
{
	return m_current_sensor_count;
}

void cgc_Controller::cgc_PrintSensorList()
{
	cgc_printf( "sl:");
	for ( cgc_Sensor *pCur = m_sensor_list.cgc_GetFirst(); pCur; pCur = m_sensor_list.cgc_GetNext( pCur ) )
	{
		cgc_printf( "$d ", pCur->cgc_GetType() );
	}
	cgc_printf("\n");
}

//
// Set power to on or off
// When system is turned off, heater turns off and programmer resets
//
void cgc_Controller::cgc_setPowerOn( bool val )
{
	m_powerOn = val;
	if ( val == false )
	{
		m_heaterOn = false;
		m_currentProgramStep = -1;
	}
}

//
// Set temperature, as long is it's in valid range (175 - 350)
//
bool cgc_Controller::cgc_setSetTemp( cgc_int32_t val )
{
	if ( val > 350 || val < 175 )
		return false;
	else
		m_set_temp = val;

	return true;
}

// TODO: search list and return true if found
bool cgc_Controller::cgc_IsSensorInList( cgc_uint16_t id )
{
	for ( cgc_Sensor *pCur = m_sensor_list.cgc_GetFirst(); pCur; pCur = m_sensor_list.cgc_GetNext( pCur ) )
	{
		if ( pCur->cgc_GetType() == id )
		{
			return true;
		}
	}
	return false;
}

//
// Back up current program to temp storage
//
void cgc_Controller::cgc_BackupProgram()
{
	for ( int i = 0; i < 10; i++ )
	{
		m_controllerProgramBackup[i] = m_controllerProgram[i];
	}
}

//
// Overwite current programs with backup
//
void cgc_Controller::cgc_RevertProgram()
{
	for ( int i = 0; i < 10; i++ )
	{
		m_controllerProgram[i] = m_controllerProgramBackup[i];
	}
}

void cgc_Controller::cgc_ClearBackupProgram()
{
	for ( int i = 0; i < 10; i++ )
	{
		m_controllerProgramBackup[i].cgc_setStepType( 0 );
		m_controllerProgramBackup[i].cgc_setSensorId( 0 );
		m_controllerProgramBackup[i].cgc_setSetTemp( 0 );
		m_controllerProgramBackup[i].cgc_setDuration( 0 );
	}
}

void cgc_Controller::cgc_ClearProgram()
{
	for ( int i = 0; i < 10; i++ )
	{
		m_controllerProgram[i].cgc_setStepType( 0 );
		m_controllerProgram[i].cgc_setSensorId( 0 );
		m_controllerProgram[i].cgc_setSetTemp( 0 );
		m_controllerProgram[i].cgc_setDuration( 0 );
	}
}

// test only
void cgc_Controller::cgc_PrintPrograms()
{
	for ( int i = 0; i < 10; i++ )
	{
		cgc_printf("Program: $x $x $x $x\n", m_controllerProgram[i].cgc_getStepType(), m_controllerProgram[i].cgc_getSensorId(), m_controllerProgram[i].cgc_getSetTemp(), m_controllerProgram[i].cgc_getDuration() );
	}
}