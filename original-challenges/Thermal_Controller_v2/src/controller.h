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

#ifndef CONTROLLER_H
#define CONTROLLER_H

extern "C"
{
	#include <libcgc.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <prng.h>
}
#include "cutil_list.h" // custom linked list
#include "cutil_string.h"
#include "common.h"
#include "sensor.h"
#include "comms.h"

#define BAD_SENSOR 6

#define MAX_PROGRAM_STEPS 10

#define VERSION 0x10a

#define MAX_SENSORS 10

#define AMBIENT_TEMP 75

class cgc_ControllerProgram
{
private:
	cgc_int32_t m_step_type;
	cgc_int32_t m_sensor_id;
	cgc_int32_t m_set_temp;
	cgc_int32_t m_duration;

public:
	cgc_int32_t cgc_getStepType() { return m_step_type; }
	cgc_int32_t cgc_getSensorId() { return m_sensor_id; }
	cgc_int32_t cgc_getSetTemp() { return m_set_temp; }
	cgc_int32_t cgc_getDuration() { return m_duration; }

	void cgc_setStepType( cgc_int32_t val ) { m_step_type = val; }
	void cgc_setSensorId( cgc_int32_t val ) { m_sensor_id = val; }
	void cgc_setSetTemp( cgc_int32_t val ) { m_set_temp = val; }
	void cgc_setDuration( cgc_int32_t val ) { m_duration = val; }
};

class cgc_Controller
{
private:
	// cgc_Sensor list vars
	CUtil::DLL_LIST( cgc_Sensor, m_sensorLink ) m_sensor_list; // use this doubly linked list
	
	cgc_uint16_t m_current_sensor_count;

	// cgc_Controller vars
	
	cgc_int32_t m_currentProgramStep;
	bool m_powerOn;
	bool m_heaterOn;
	double m_currentTemp;
	cgc_int32_t m_version;
	cgc_int32_t m_set_temp;
	bool m_smokeSensorPresent;

	cgc_Sensor m_smokeSensor;

	cgc_OutgoingMessage msg_out;
	cgc_IncomingMessage msg_in;


public:
	cgc_Controller();
	~cgc_Controller();



	cgc_OutgoingMessage* cgc_GetMsgOut() { return &msg_out; }
	cgc_IncomingMessage* cgc_GetMsgIn() { return &msg_in; }

	// Programs
	cgc_ControllerProgram m_controllerProgram[10];
	cgc_ControllerProgram m_controllerProgramBackup[10];

	void cgc_RevertProgram();
	void cgc_BackupProgram();
	void cgc_ClearBackupProgram();
	void cgc_ClearProgram();
	void cgc_PrintPrograms(); // test only

	// GET / SET
	double cgc_getCurrentTemp() { return m_currentTemp; }
	void cgc_setCurrentTemp( double val ) { m_currentTemp = val; }

	cgc_int32_t cgc_getCurrentProgramStep() { return m_currentProgramStep; }
	void cgc_setCurrentProgramStep( cgc_int32_t val ) { m_currentProgramStep = val; }
	
	bool cgc_getPowerOn() { return m_powerOn; }
	void cgc_setPowerOn( bool val );
	
	bool cgc_getHeaterOn() { return m_heaterOn; }
	void cgc_setHeaterOn( bool val ) { m_heaterOn = val; }

	cgc_int32_t cgc_getSetTemp() { return m_set_temp; }
	bool cgc_setSetTemp( cgc_int32_t val );


	// Sensors
	bool cgc_AddSensor( cgc_Sensor* );
	bool cgc_IsSensorInList ( cgc_uint16_t id );
	bool cgc_RemoveSensor( cgc_uint16_t sensor_id );
	cgc_uint16_t cgc_GetSensorCount();
	bool cgc_SetSensor( cgc_uint16_t sensor_type, bool val );

	void cgc_PrintSensorList();

	cgc_uint16_t cgc_GetVersion() { return m_version; }

	// firmware
	//  get?
	//  validate
	bool cgc_ValidateFirmware();


	// Smoke sensor
	bool cgc_IsSmokeSensorEnabled() { return m_smokeSensor.cgc_GetEnabled(); }
	void cgc_EnableSmokeSensor() { m_smokeSensor.cgc_SetEnabled( true ); }
	void cgc_DisableSmokeSensor() { m_smokeSensor.cgc_SetEnabled( false ); }

	//
	// Send a response back with the following code
	//
	void cgc_SendBasicResponse( CUtil::cgc_String code );
	void cgc_SendExtendedResponse( cgc_uint16_t code, CUtil::cgc_String desc );
};



#endif
