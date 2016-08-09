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
#ifndef __ECM_H__
#define __ECM_H__

#include "simulation.h"
#include "lanbus.h"
#include "response.h"

class cgc_CECM : public cgc_CSimulationComponent
{
public:
	// Broadcast message type
	const static cgc_uint8_t ENGINE_STATE_CHANGE = 0x1;

	const static cgc_uint8_t ECM_LANBUS_ID = 0x13;

	const static cgc_uint8_t READ_DATA_TYPE = 0x1;
	const static cgc_uint8_t SET_DATA_TYPE = 0x2;
	const static cgc_uint8_t ACTION_DATA_TYPE = 0x3;
	const static cgc_uint8_t CHECK_EQUIPMENT_TYPE = 0x4;

	const static cgc_uint8_t READ_EGT_COMMAND = 0x1;
	const static cgc_uint8_t READ_RPM_COMMAND = 0x2;
	const static cgc_uint8_t READ_IAT_COMMAND = 0x3;
	const static cgc_uint8_t READ_O2_COMMAND = 0x4;
	const static cgc_uint8_t READ_TEMP_COMMAND = 0x5;
	const static cgc_uint8_t READ_TORQUE_COMMAND = 0x6;
	const static cgc_uint8_t READ_THROTTLEPOS_COMMAND = 0x7;

	const static cgc_uint8_t SET_THROTTLEPOS_COMMAND = 0x1;
	const static cgc_uint8_t SET_TORQUE_COMMAND = 0x2;

	const static cgc_uint8_t SET_RESPONSE_INVALID_FIELD = 0xFD;
	const static cgc_uint8_t SET_RESPONSE_ENGINE_OFF = 0xFE;
	const static cgc_uint8_t SET_RESPONSE_INVALID_LENGTH = 0xFF;

	const static cgc_uint8_t START_ENGINE_COMMAND = 0x1;
	const static cgc_uint8_t STOP_ENGINE_COMMAND = 0x2;

	const static cgc_uint8_t READ_RESPONSE_TYPE = 0x1;
	const static cgc_uint8_t SET_RESPONSE_TYPE = 0x2;
	const static cgc_uint8_t ACTION_RESPONSE_TYPE = 0x3;
	const static cgc_uint8_t CHECK_RESPONSE_TYPE = 0x4;
	const static cgc_uint8_t FAILURE_RESPONSE_TYPE = 0x5;

	const static cgc_uint8_t READ_RESPONSE_EGT = 0x1;
	const static cgc_uint8_t READ_RESPONSE_RPM = 0x2;
	const static cgc_uint8_t READ_RESPONSE_IAT = 0x3;
	const static cgc_uint8_t READ_RESPONSE_O2 = 0x4;
	const static cgc_uint8_t READ_RESPONSE_TEMP = 0x5;
	const static cgc_uint8_t READ_RESPONSE_TORQUE = 0x6;
	const static cgc_uint8_t READ_RESPONSE_THROTTLEPOS = 0x7;
	const static cgc_uint8_t READ_RESPONSE_INVALID_FIELD = 0xFF;

	const static cgc_uint8_t ACTION_ENGINE_OFF = 0x0;
	const static cgc_uint8_t ACTION_ENGINE_ON = 0x1;

	const static cgc_uint8_t CHECK_RESPONSE_INVALID_LENGTH = 0xFF;

	const static cgc_uint8_t FAILURE_INVALID_DATA = 0x1;

	const static cgc_uint16_t ENGINE_MAX_TORQUE = 400;	// 400 lb-ft

public:
	cgc_CECM( cgc_CLANBus *pLANBus, cgc_uint64_t equipmentID );
	~cgc_CECM();

	void cgc_Run( void );	// Runs simulation

	void cgc_ProcessMessage( cgc_CLANMessage *pMessage );
	void cgc_ProcessBroadcastMessage( cgc_CLANMessage *pMessage );

	void cgc_HandleTLVMessage( cgc_uint8_t typeID, cgc_uint16_t length, cgc_uint8_t *pData, cgc_CResponse *pResponse );
	
	void cgc_HandleReadDataMessage( cgc_uint8_t *pData, cgc_uint16_t length, cgc_CResponse *pResponse );
	void cgc_HandleSetDataMessage( cgc_uint8_t *pData, cgc_uint16_t length, cgc_CResponse *pResponse );
	void cgc_HandleActionMessage( cgc_uint8_t *pData, cgc_uint16_t length, cgc_CResponse *pResponse );

	bool cgc_IsEngineRunning( void ) const { return m_bRunning; };
	cgc_uint16_t cgc_GetEngineRPM( void ) const { return m_engineRPM; };
	cgc_uint16_t cgc_GetEngineEGT( void ) const { return m_engineEGT; };
	cgc_uint16_t cgc_GetEngineIAT( void ) const { return m_engineIAT; };
	cgc_uint16_t cgc_GetEngineO2_1( void ) const { return m_engineO2_1; };
	cgc_uint16_t cgc_GetEngineO2_2( void ) const { return m_engineO2_2; };
	cgc_uint16_t cgc_GetEngineTemp( void ) const { return m_engineTemp; };	
	cgc_uint16_t cgc_GetEngineTorque( void ) const { return m_engineTorque; };
	cgc_uint16_t cgc_GetThrottlePos( void ) const { return m_engineThrottlePos; };

	void cgc_StartEngine( cgc_CResponse *pResponse );
	void cgc_StopEngine( cgc_CResponse *pResponse );

	cgc_uint64_t cgc_GetEquipmentID( void ) const { return m_equipmentID; };

private:
	void cgc_CommandTorque( cgc_uint16_t newTorqueValue );
	void cgc_EngineTick( void );

	cgc_uint16_t m_engineRPM;		// Engine RPM
	cgc_uint16_t m_engineEGT;		// Engine Exhaust Gas Temperature
	cgc_uint16_t m_engineIAT;		// Engine Incoming Air Temperature
	cgc_uint16_t m_engineO2_1;		// Engine O2 sensor 1
	cgc_uint16_t m_engineO2_2;		// Engine O2 sensor 2
	cgc_uint16_t m_engineTemp; 		// Engine Temperature
	cgc_uint16_t m_engineThrottlePos;	// ENgine Throttle Position
	cgc_uint16_t m_engineTorque;	// Engine calculated torque
	bool m_bRunning;		// Engine running (true or false)
	bool m_bLastRunningState;	// Engine last state... for turn on sequencing

	cgc_uint64_t m_equipmentID;		// Equipment Identifier
};

#endif // __ECM_H__
