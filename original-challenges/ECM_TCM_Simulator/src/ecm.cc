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
extern "C"
{
#include <stdlib.h>
#include <stdio.h>
}

#include "ecm.h"

cgc_CECM::cgc_CECM( cgc_CLANBus *pLANBus, cgc_uint64_t equipmentID )
	: cgc_CSimulationComponent( pLANBus ), m_equipmentID( equipmentID )
{
	m_bRunning = false;
	m_bLastRunningState = false;
	
	m_engineRPM = 0;
	m_engineEGT = 0;
	m_engineIAT = 75;
	m_engineTemp = 75;
	m_engineTorque = 0;
	m_engineThrottlePos = 0;	// 10%
	m_engineO2_1 = 0;	// 0%
	m_engineO2_2 = 0;	// 0%
}

cgc_CECM::~cgc_CECM()
{

}

void cgc_CECM::cgc_Run( void )
{
	cgc_uint8_t destList[] = { ECM_LANBUS_ID, 0xFF };

	cgc_CLANMessage *pMsg = m_pLANBus->cgc_RecvMessage( destList, sizeof(destList) );

	for ( ; pMsg; pMsg = m_pLANBus->cgc_RecvMessage( destList, sizeof(destList), pMsg ) )
	{

		if ( pMsg->cgc_GetDestID() == ECM_LANBUS_ID )
			cgc_ProcessMessage( pMsg );
		else if ( pMsg->cgc_GetDestID() == 0xFF )
			cgc_ProcessBroadcastMessage( pMsg );
	}

	// cgc_Run engine routines
	cgc_EngineTick();
}

void cgc_CECM::cgc_ProcessBroadcastMessage( cgc_CLANMessage *pMessage )
{
	// ECM does not handle any broadcast messages
	return;
}

void cgc_CECM::cgc_ProcessMessage( cgc_CLANMessage *pMessage )
{
	cgc_uint8_t *pData = pMessage->cgc_GetMessageData();
	cgc_uint16_t dataLen = pMessage->cgc_GetMessageLen();

	cgc_uint8_t srcID = pMessage->cgc_GetSourceID();
	cgc_uint8_t destID = pMessage->cgc_GetDestID();

	// Handle ECM messages
	if ( dataLen == 0 )
		return;

	// Process messages
	cgc_CResponse oResponse;
	
	do
	{
		if ( dataLen < 3 )
		{
			// Send failure response
			cgc_uint8_t failureResponse[4];
			failureResponse[0] = FAILURE_RESPONSE_TYPE;
			*((cgc_uint16_t*)(failureResponse+1)) = 1;	
			failureResponse[3] = FAILURE_INVALID_DATA;

			oResponse.cgc_AddResponse( failureResponse, 4 );
			break;
		}

		cgc_uint8_t messageTypeID = pData[0];
		cgc_uint16_t messageLength = *((cgc_uint16_t *)(pData+1));

		if ( dataLen < (messageLength+3) )
		{
			// Send failure response
			cgc_uint8_t failureResponse[4];
			failureResponse[0] = FAILURE_RESPONSE_TYPE;
			*((cgc_uint16_t*)(failureResponse+1)) = 1;	
			failureResponse[3] = FAILURE_INVALID_DATA;

			oResponse.cgc_AddResponse( failureResponse, 4 );
			break;
		}

		// Handle a TLV message
		cgc_HandleTLVMessage( messageTypeID, messageLength, pData+3, &oResponse );

		// Continue to next TLV field
		dataLen -= (messageLength+3);
		pData += (messageLength+3);	
	}
	while ( dataLen > 0 );

	// Get response
	cgc_uint16_t responseLength = oResponse.cgc_GetResponseLength();

	if ( responseLength > 0 )
	{
		cgc_uint8_t *pResponseData = new cgc_uint8_t[responseLength];

		if ( oResponse.cgc_GetResponseData( pResponseData, responseLength ) )
			m_pLANBus->cgc_SendMessage( destID, srcID, pResponseData, responseLength );

		delete [] pResponseData;
	}
}

void cgc_CECM::cgc_HandleTLVMessage( cgc_uint8_t typeID, cgc_uint16_t length, cgc_uint8_t *pData, cgc_CResponse *pResponse )
{
	if ( pResponse == NULL )
		return;

	if ( pData == NULL )
		return;

	if ( length == 0 )
		return;

	// Begin parsing message
	switch( typeID )
	{
	case READ_DATA_TYPE:
		cgc_HandleReadDataMessage( pData, length, pResponse );
		break;

	case SET_DATA_TYPE:
		cgc_HandleSetDataMessage( pData, length, pResponse );
		break;

	case ACTION_DATA_TYPE:
		cgc_HandleActionMessage( pData, length, pResponse );
		break;

	case CHECK_EQUIPMENT_TYPE:
		if ( length < 8 )
		{
			// Fail
			cgc_uint8_t setResponse[4];
			setResponse[0] = CHECK_RESPONSE_TYPE;
			*((cgc_uint16_t*)(setResponse+1)) = 1;	
			setResponse[3] = CHECK_RESPONSE_INVALID_LENGTH;

			pResponse->cgc_AddResponse( setResponse, 4 );
		}
		else
		{
			cgc_uint64_t checkID = *((cgc_uint64_t*)(pData));

			cgc_uint8_t checkResponse[4];
			checkResponse[0] = CHECK_RESPONSE_TYPE;
			*((cgc_uint16_t*)(checkResponse+1)) = 1;	
			if ( checkID == m_equipmentID )
				checkResponse[3] = 1;	
			else
				checkResponse[3] = 0;

			pResponse->cgc_AddResponse( checkResponse, 4 );
		}
		break;

	default:
		// Unknown...
		return;
	}
}

void cgc_CECM::cgc_HandleReadDataMessage( cgc_uint8_t *pData, cgc_uint16_t length, cgc_CResponse *pResponse )
{
	if ( length < 1 )
		return;

	cgc_uint8_t fieldID = pData[0];

	switch( fieldID )
	{
	case READ_EGT_COMMAND:
		{
			cgc_uint8_t readResponse[6];
			readResponse[0] = READ_RESPONSE_TYPE;
			*((cgc_uint16_t*)(readResponse+1)) = 3;	
			readResponse[3] = READ_RESPONSE_EGT;
			*((cgc_uint16_t*)(readResponse+4)) = m_engineEGT;

			pResponse->cgc_AddResponse( readResponse, 6 );
		}
		break;

	case READ_RPM_COMMAND:
		{

			cgc_uint8_t readResponse[6];
			readResponse[0] = READ_RESPONSE_TYPE;
			*((cgc_uint16_t*)(readResponse+1)) = 3;	
			readResponse[3] = READ_RESPONSE_RPM;
			*((cgc_uint16_t*)(readResponse+4)) = m_engineRPM;

			pResponse->cgc_AddResponse( readResponse, 6 );
		}
		break;
	
	case READ_IAT_COMMAND:
		{

			cgc_uint8_t readResponse[6];
			readResponse[0] = READ_RESPONSE_TYPE;
			*((cgc_uint16_t*)(readResponse+1)) = 3;	
			readResponse[3] = READ_RESPONSE_IAT;
			*((cgc_uint16_t*)(readResponse+4)) = m_engineIAT;

			pResponse->cgc_AddResponse( readResponse, 6 );
		}
		break;

	case READ_O2_COMMAND:
		{
			if ( length < 2 )
			{
				// Send failure response
				cgc_uint8_t failureResponse[4];
				failureResponse[0] = FAILURE_RESPONSE_TYPE;
				*((cgc_uint16_t*)(failureResponse+1)) = 1;	
				failureResponse[3] = FAILURE_INVALID_DATA;

				pResponse->cgc_AddResponse( failureResponse, 4 );
			}
			else
			{
				cgc_uint8_t readResponse[7];
				readResponse[0] = READ_RESPONSE_TYPE;
				*((cgc_uint16_t*)(readResponse+1)) = 4;	
				readResponse[3] = READ_RESPONSE_O2;
				readResponse[4] = pData[1];

				if ( pData[1] == 0 )
					*((cgc_uint16_t*)(readResponse+5)) = m_engineO2_1;
				else
					*((cgc_uint16_t*)(readResponse+5)) = m_engineO2_2;

				pResponse->cgc_AddResponse( readResponse, 7 );
			}
		}
		break;

	case READ_TEMP_COMMAND:
		{
			cgc_uint8_t readResponse[6];
			readResponse[0] = READ_RESPONSE_TYPE;
			*((cgc_uint16_t*)(readResponse+1)) = 3;	
			readResponse[3] = READ_RESPONSE_TEMP;
			*((cgc_uint16_t*)(readResponse+4)) = m_engineTemp;

			pResponse->cgc_AddResponse( readResponse, 6 );
		}
		break;

	case READ_TORQUE_COMMAND:
		{
			cgc_uint8_t readResponse[6];
			readResponse[0] = READ_RESPONSE_TYPE;
			*((cgc_uint16_t*)(readResponse+1)) = 3;	
			readResponse[3] = READ_RESPONSE_TORQUE;
			*((cgc_uint16_t*)(readResponse+4)) = m_engineTorque;

			pResponse->cgc_AddResponse( readResponse, 6 );
		}
		break;

	case READ_THROTTLEPOS_COMMAND:
		{
			cgc_uint8_t readResponse[6];
			readResponse[0] = READ_RESPONSE_TYPE;
			*((cgc_uint16_t*)(readResponse+1)) = 3;	
			readResponse[3] = READ_RESPONSE_THROTTLEPOS;
			*((cgc_uint16_t*)(readResponse+4)) = m_engineThrottlePos;

			pResponse->cgc_AddResponse( readResponse, 6 );
		}
		break;

	default:
		{
			cgc_uint8_t readResponse[5];
			readResponse[0] = READ_RESPONSE_TYPE;
			*((cgc_uint16_t*)(readResponse+1)) = 2;	
			readResponse[3] = READ_RESPONSE_INVALID_FIELD;
			readResponse[4] = fieldID;

			pResponse->cgc_AddResponse( readResponse, 5 );
		}
		break;

	}
}

void cgc_CECM::cgc_HandleSetDataMessage( cgc_uint8_t *pData, cgc_uint16_t length, cgc_CResponse *pResponse )
{
	if ( length < 3 )
	{
		cgc_uint8_t setResponse[4];
		setResponse[0] = SET_RESPONSE_TYPE;
		*((cgc_uint16_t*)(setResponse+1)) = 1;	
		setResponse[3] = SET_RESPONSE_INVALID_LENGTH;

		pResponse->cgc_AddResponse( setResponse, 4 );
		return;
	}

	cgc_uint8_t fieldID = pData[0];
	cgc_uint16_t setValue = *((cgc_uint16_t*)(pData+1));

	switch( fieldID )
	{
	case SET_THROTTLEPOS_COMMAND:
		{
			if ( !m_bRunning )
			{
				cgc_uint8_t setResponse[4];
				setResponse[0] = SET_RESPONSE_TYPE;
				*((cgc_uint16_t*)(setResponse+1)) = 1;	
				setResponse[3] = SET_RESPONSE_ENGINE_OFF;

				pResponse->cgc_AddResponse( setResponse, 4 );

				break;
			}

			if ( setValue > 100 )
				setValue = 100;

			if ( setValue < 10 )
				setValue = 10;

			// Set throttle position
			m_engineThrottlePos = setValue;

			cgc_uint8_t setResponse[6];
			setResponse[0] = SET_RESPONSE_TYPE;
			*((cgc_uint16_t*)(setResponse+1)) = 3;	
			setResponse[3] = SET_THROTTLEPOS_COMMAND;
			*((cgc_uint16_t*)(setResponse+4)) = setValue;

			pResponse->cgc_AddResponse( setResponse, 6 );
		}
		break;
	
	case SET_TORQUE_COMMAND:
		{
			if ( !m_bRunning )
			{
				cgc_uint8_t setResponse[4];
				setResponse[0] = SET_RESPONSE_TYPE;
				*((cgc_uint16_t*)(setResponse+1)) = 1;	
				setResponse[3] = SET_RESPONSE_ENGINE_OFF;

				pResponse->cgc_AddResponse( setResponse, 4 );

				break;
			}

			if ( setValue > ENGINE_MAX_TORQUE )
				setValue = ENGINE_MAX_TORQUE;

			// Ask engine for this much torque
			cgc_CommandTorque( setValue );

			// Response with the current torque value of the engine!
			cgc_uint8_t setResponse[6];
			setResponse[0] = SET_RESPONSE_TYPE;
			*((cgc_uint16_t*)(setResponse+1)) = 3;	
			setResponse[3] = SET_TORQUE_COMMAND;
			*((cgc_uint16_t*)(setResponse+4)) = m_engineTorque;

			pResponse->cgc_AddResponse( setResponse, 6 );
		}
		break;

	default:
		{
			cgc_uint8_t setResponse[5];
			setResponse[0] = SET_RESPONSE_TYPE;
			*((cgc_uint16_t*)(setResponse+1)) = 2;	
			setResponse[3] = SET_RESPONSE_INVALID_FIELD;
			setResponse[4] = fieldID;

			pResponse->cgc_AddResponse( setResponse, 5 );
		}
		break;
	}
}

void cgc_CECM::cgc_HandleActionMessage( cgc_uint8_t *pData, cgc_uint16_t length, cgc_CResponse *pResponse )
{
	if ( length < 1 )
		return;

	cgc_uint8_t fieldID = pData[0];

	switch( fieldID )
	{
	case START_ENGINE_COMMAND:
		{	
			if ( !m_bRunning )
				cgc_StartEngine( pResponse );
		
			cgc_uint8_t actionResponse[4];
			actionResponse[0] = ACTION_RESPONSE_TYPE;
			*((cgc_uint16_t*)(actionResponse+1)) = 1;	
			actionResponse[3] = ACTION_ENGINE_ON;

			pResponse->cgc_AddResponse( actionResponse, 4 );
		}

		break;

	case STOP_ENGINE_COMMAND:
		{
			if ( m_bRunning )
				cgc_StopEngine( pResponse );

			cgc_uint8_t actionResponse[4];
			actionResponse[0] = ACTION_RESPONSE_TYPE;
			*((cgc_uint16_t*)(actionResponse+1)) = 1;	
			actionResponse[3] = ACTION_ENGINE_OFF;

			pResponse->cgc_AddResponse( actionResponse, 4 );
		}
		break;
	}
}

void cgc_CECM::cgc_StartEngine( cgc_CResponse *pResponse )
{
	if ( m_bRunning )
		return;

	m_bRunning = true;

	m_engineRPM = 700;
	m_engineEGT = 90;
	m_engineIAT = 75;
	m_engineTemp = 90;
	m_engineTorque = 0;
	m_engineThrottlePos = 10;	// 10%
	m_engineO2_1 = 17;	// 17%
	m_engineO2_2 = 17;	// 17%
}

void cgc_CECM::cgc_StopEngine( cgc_CResponse *pResponse )
{
	if ( !m_bRunning )
		return;

	m_bRunning = false;

	m_engineRPM = 0;
	m_engineEGT = 0;
	m_engineIAT = 75;
	m_engineTemp = 75;
	m_engineTorque = 0;
	m_engineThrottlePos = 0;	// 10%
	m_engineO2_1 = 0;	// 0%
	m_engineO2_2 = 0;	// 0%
}

void cgc_CECM::cgc_CommandTorque( cgc_uint16_t newTorque )
{
	cgc_uint16_t deltaTorque;
	bool bTorqueDecrease = false;

	if ( newTorque < m_engineTorque )
	{
		deltaTorque = m_engineTorque - newTorque;
		bTorqueDecrease = true;
	}
	else
	{
		deltaTorque = newTorque - m_engineTorque;
	}

	if ( bTorqueDecrease )
		m_engineRPM -= (deltaTorque * 10);
	else
		m_engineRPM += (deltaTorque * 10);

	m_engineTorque = newTorque;
}

void cgc_CECM::cgc_EngineTick( void )
{
	// Check engine state
	if ( m_bLastRunningState != m_bRunning )
	{	
		cgc_uint8_t ecmBroadcastMessage[5];

		ecmBroadcastMessage[0] = ENGINE_STATE_CHANGE;
		*((cgc_uint16_t*)(ecmBroadcastMessage+1)) = 1;

		if ( m_bRunning )
			ecmBroadcastMessage[3] = 1;
		else
			ecmBroadcastMessage[3] = 0;

		// Inform everyone the engine is on
		m_pLANBus->cgc_SendMessage( ECM_LANBUS_ID, 0xFF, ecmBroadcastMessage, 4 );
	}

	// Record last state
	m_bLastRunningState = m_bRunning;
}
