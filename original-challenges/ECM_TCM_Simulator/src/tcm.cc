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
#include <stdio.h>
#include <stdlib.h>
}
#include "tcm.h"

cgc_CTCM::cgc_CTCM( cgc_CLANBus *pLANBus, cgc_uint64_t equipmentID )
	: cgc_CSimulationComponent( pLANBus ), m_equipmentID( equipmentID )
{
	// Off by default
	m_bEnableTC = false;

	// Engine off by default
	m_bEngineRunning = false;

	m_wheelSpeed[0] = 0;
	m_wheelSpeed[1] = 0;
	m_wheelSpeed[2] = 0;
	m_wheelSpeed[3] = 0;
}

cgc_CTCM::~cgc_CTCM()
{

}

void cgc_CTCM::cgc_Run( void )
{
	cgc_uint8_t destList[] = { TCM_LANBUS_ID, 0xFF };

	cgc_CLANMessage *pMsg = m_pLANBus->cgc_RecvMessage( destList, sizeof(destList) );

	for ( ; pMsg; pMsg = m_pLANBus->cgc_RecvMessage( destList, sizeof(destList), pMsg ) )
	{
		if ( pMsg->cgc_GetDestID() == TCM_LANBUS_ID )
			cgc_ProcessMessage( pMsg );
		else if ( pMsg->cgc_GetDestID() == 0xFF )
			cgc_ProcessBroadcastMessage( pMsg );
	}

	// cgc_Run TCM management
	cgc_TCMTick();
}

void cgc_CTCM::cgc_ProcessBroadcastMessage( cgc_CLANMessage *pMessage )
{
	// Handle broadcast messages from ECM
	if ( pMessage->cgc_GetSourceID() == ECM_LANBUS_ID )
	{
		cgc_uint8_t *pData = pMessage->cgc_GetMessageData();
		cgc_uint16_t dataLen = pMessage->cgc_GetMessageLen();

		do
		{
			if ( dataLen < 3 )
				break;

			cgc_uint8_t messageType = pData[0];
			cgc_uint16_t messageLen = *((cgc_uint16_t*)(pData+1));
			cgc_uint8_t *pMsgData = pData+3;
			
			if ( dataLen < (messageLen+3) )
				break;

			if ( messageType == 0x1 && messageLen == 1 )
			{
				// State change -- engine state
				if ( pMsgData[0] == 0x1 )
					m_bEngineRunning = true;
				else if ( pMsgData[0] == 0x0 )
					m_bEngineRunning = false;
			}

			pData += (messageLen+3);
			dataLen -= (messageLen+3);

		} while ( dataLen > 0 );
	}
}

void cgc_CTCM::cgc_ProcessMessage( cgc_CLANMessage *pMessage )
{
	cgc_uint8_t *pData = pMessage->cgc_GetMessageData();
	cgc_uint16_t dataLen = pMessage->cgc_GetMessageLen();

	cgc_uint8_t srcID = pMessage->cgc_GetSourceID();
	cgc_uint8_t destID = pMessage->cgc_GetDestID();

	// Handle messages
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

	// Get response data
	cgc_uint16_t responseLength = oResponse.cgc_GetResponseLength();

	if ( responseLength > 0 )
	{
		cgc_uint8_t *pResponseData = new cgc_uint8_t[responseLength];

		if ( oResponse.cgc_GetResponseData( pResponseData, responseLength ) )
			m_pLANBus->cgc_SendMessage( destID, srcID, pResponseData, responseLength );
	}
}

void cgc_CTCM::cgc_HandleTLVMessage( cgc_uint8_t typeID, cgc_uint16_t length, cgc_uint8_t *pData, cgc_CResponse *pResponse )
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

void cgc_CTCM::cgc_HandleReadDataMessage( cgc_uint8_t *pData, cgc_uint16_t length, cgc_CResponse *pResponse )
{
	if ( length <= 1 )
		return;

	cgc_uint8_t fieldID = pData[0];

	switch( fieldID )
	{
	case READ_WHEELSPEED_COMMAND:
		{
			if ( !m_bEnableTC )
			{
				cgc_uint8_t readFailureResponse[6];
				readFailureResponse[0] = READ_RESPONSE_TYPE;
				*((cgc_uint16_t*)(readFailureResponse+1)) = 1;
				readFailureResponse[3] = READ_FAILURE_TC_OFF_RESPONSE;

				pResponse->cgc_AddResponse( readFailureResponse, 4 );
				return;	
			}

			if ( length < 2 )
				return;

			cgc_uint8_t wheelCount = pData[1];

			if ( wheelCount > 4 )
				return;

			if ( length < (wheelCount+2) )
				return;

			cgc_uint16_t resultList[4];

			for ( cgc_uint8_t pos = 0; pos < wheelCount; pos++ )
			{
				// BUG:: Allow them to access up to 512-8 bytes past the array
#ifdef PATCHED_1
				if ( pData[pos+2] >= 4 )
				       return;	
#endif
				resultList[pos] = m_wheelSpeed[pData[pos+2]];	
			}

			// Send result...
			cgc_uint8_t readWheelSpeedResponse[20];
			readWheelSpeedResponse[0] = READ_RESPONSE_TYPE;
			*((cgc_uint16_t*)(readWheelSpeedResponse+1)) = (2+(sizeof(cgc_uint16_t)*wheelCount));
			readWheelSpeedResponse[3] = READ_WHEELSPEED_RESPONSE;
			readWheelSpeedResponse[4] = wheelCount;
			cgc_memcpy( readWheelSpeedResponse+5, resultList, sizeof(cgc_uint16_t)*wheelCount );

			pResponse->cgc_AddResponse( readWheelSpeedResponse, 5+(wheelCount*sizeof(cgc_uint16_t)) );
		}	
		break;

	default:
		break;
	}
}

void cgc_CTCM::cgc_HandleSetDataMessage( cgc_uint8_t *pData, cgc_uint16_t length, cgc_CResponse *pResponse )
{
	if ( length <= 1 )
		return;

	cgc_uint8_t fieldID = pData[0];

	switch( fieldID )
	{
	case SET_WHEELSPEED_COMMAND:
		{
			if ( !m_bEnableTC )
			{
				cgc_uint8_t setFailureResponse[6];
				setFailureResponse[0] = SET_RESPONSE_TYPE;
				*((cgc_uint16_t*)(setFailureResponse+1)) = 1;
				setFailureResponse[3] = SET_FAILURE_TC_OFF_RESPONSE;

				pResponse->cgc_AddResponse( setFailureResponse, 4 );
				return;	
			}

			if ( length < 2 )
				return;

			cgc_uint8_t wheelCount = pData[1];

			if ( wheelCount > 4 )
				return;

			if ( length < ((wheelCount+2)+(wheelCount*3)) )
				return;

			for ( cgc_uint8_t pos = 0; pos < wheelCount; pos++ )
			{
				if ( pData[pos+2] >= 4 )
				       return;	
				
				m_wheelSpeed[pData[pos+2]] = ((cgc_uint16_t*)(pData+wheelCount+2))[pos];
			}

			// Send result...
			cgc_uint8_t setWheelSpeedResponse[20];
			setWheelSpeedResponse[0] = SET_RESPONSE_TYPE;
			*((cgc_uint16_t*)(setWheelSpeedResponse+1)) = 2+(3*wheelCount);
			setWheelSpeedResponse[3] = SET_WHEELSPEED_RESPONSE;
			setWheelSpeedResponse[4] = wheelCount;
			cgc_memcpy( setWheelSpeedResponse+5, pData+2, 3*wheelCount );
			
			pResponse->cgc_AddResponse( setWheelSpeedResponse, 5+(wheelCount*3) );
		}
		break;
	}
}

void cgc_CTCM::cgc_HandleActionMessage( cgc_uint8_t *pData, cgc_uint16_t length, cgc_CResponse *pResponse )
{
	if ( length < 1 )
		return;

	cgc_uint8_t fieldID = pData[0];

	if ( !m_bEngineRunning )
		return;

	switch( fieldID )
	{
	case TC_ON_COMMAND:
		{
			// Check state
			if ( m_bEnableTC )
			{
				// Already on -- do nothing
			}

			cgc_uint8_t actionResponse[6];
			actionResponse[0] = ACTION_RESPONSE_TYPE;
			*((cgc_uint16_t*)(actionResponse+1)) = 2;
			actionResponse[3] = ACTION_TC_ENABLE_RESPONSE;
			actionResponse[4] = 1;
				
			pResponse->cgc_AddResponse( actionResponse, 5 );
		
			m_bEnableTC = true;
		}
		break;

	case TC_OFF_COMMAND:
		{
			// Check state
			if ( !m_bEnableTC )
			{
				// Already off -- do nothing
			}

			cgc_uint8_t actionResponse[6];
			actionResponse[0] = ACTION_RESPONSE_TYPE;
			*((cgc_uint16_t*)(actionResponse+1)) = 2;
			actionResponse[3] = ACTION_TC_ENABLE_RESPONSE;
			actionResponse[4] = 0;
				
			pResponse->cgc_AddResponse( actionResponse, 5 );
		
			m_bEnableTC = false;
		}
		break;
	}
}

void cgc_CTCM::cgc_TCMTick( void )
{
	if ( m_bEnableTC && !m_bEngineRunning )
	{
		// TCM is on and engine stopped running -- shut off
		m_bEnableTC = false;
	}

	if ( !m_bEnableTC )
	{
		m_wheelSpeed[0] = 0;
		m_wheelSpeed[1] = 0;
		m_wheelSpeed[2] = 0;
		m_wheelSpeed[3] = 0;
	}
	else
	{
		m_wheelSpeed[0] = 300;
		m_wheelSpeed[1] = 300;
		m_wheelSpeed[2] = 300;
		m_wheelSpeed[3] = 300;
	}
}
