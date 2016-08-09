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
#include "common.h"
#include "simulation.h"
#include "ecm.h"

extern "C"
{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
}

bool cgc_CSimulationComponent::cgc_SendMessage( cgc_uint8_t srcID, cgc_uint8_t destID, cgc_uint8_t *pMessageData, cgc_uint16_t messageLen )
{
	if ( !m_pLANBus )
		return (false);

	return m_pLANBus->cgc_SendMessage( srcID, destID, pMessageData, messageLen );
}

cgc_CSimulationCommand::cgc_CSimulationCommand( cgc_uint8_t srcID, cgc_uint8_t destID, cgc_uint32_t cmdTime, cgc_uint8_t *pCmdData, cgc_uint16_t cmdLength )
	: m_srcID( srcID ), m_destID( destID ), m_runTime( cmdTime ), m_dataLen( cmdLength )
{
	m_pData = new cgc_uint8_t[cmdLength];

	cgc_memcpy( m_pData, pCmdData, cmdLength );
}

cgc_CSimulationCommand::~cgc_CSimulationCommand( ) 
{
	if ( m_pData )
		delete [] m_pData;

	m_pData = NULL;
}

void cgc_CSimulation::cgc_AddSimulationComponent( cgc_CSimulationComponent *pItem )
{
	if ( !pItem )
		return;

	m_componentList.cgc_AddLast( pItem );
}

void cgc_CSimulation::cgc_SetupSimulation( void )
{
	// Get user simulation commands
	cgc_printf( "Enter number of simulation commands: " );

	CUtil::cgc_String sLine = cgc_ReadLine( );	

	cgc_uint32_t cmd_count;

	sLine.cgc_ToInt( cmd_count );

	for ( cgc_uint32_t i = 0; i < cmd_count; i++ )
	{
		cgc_uint32_t value;

		cgc_uint8_t srcID, destID;

		cgc_printf( "Enter Source ID: " );

		sLine = cgc_ReadLine();

		sLine.cgc_ToInt( value );

		if ( value > 255 )
		{
			cgc_printf( "Invalid source ID. Setting to 255\n" );
			srcID = 255;
		}
		else
			srcID = value;

		cgc_printf( "Enter Dest ID: " );

		sLine = cgc_ReadLine();

		sLine.cgc_ToInt( value );

		if ( value > 255 )
		{
			cgc_printf( "Invalid dest ID. Setting to 255\n" );
			destID = 255;
		}
		else
			destID = value;

		cgc_printf( "Enter Command Type:\n" );
		cgc_printf( "0 - Read EGT\n" );
		cgc_printf( "1 - Read RPM\n" );
		cgc_printf( "2 - Read IAT\n" );
		cgc_printf( "3 - Read O2_1\n" );
		cgc_printf( "4 - Read O2_2\n" );
		cgc_printf( "5 - Read Engine Temp\n" );
		cgc_printf( "6 - Read Engine Torque\n" );
		cgc_printf( "7 - Start Engine\n" );
		cgc_printf( "8 - Stop Engine\n" );
		cgc_printf( "9 - Set Throttle Position\n" );
		cgc_printf( "10 - Set Engine Load\n" );
		cgc_printf( "11 - Send Binary Message\n" );
		
		cgc_uint32_t cmd_type;

		sLine = cgc_ReadLine();

		sLine.cgc_ToInt( cmd_type );

		cgc_printf( "Enter command time (in ms):\n" );
		cgc_printf( "Simulation will send this command at the specified time in the simulation (max 2 minutes of simulation time):\n" );

		cgc_uint32_t cmd_time_ms;

		sLine = cgc_ReadLine();

		sLine.cgc_ToInt( cmd_time_ms );

		switch( cmd_type )
		{
		case 0:
			// Read EGT
			cgc_ReadEGT( cmd_time_ms, srcID, destID );
			break;

		case 1:
			// Read RPM
			cgc_ReadRPM( cmd_time_ms, srcID, destID );
			break;

		case 2:
			// Read IAT
			cgc_ReadIAT( cmd_time_ms, srcID, destID );
			break;

		case 3:
			// Read 02_1
			cgc_ReadO2( cmd_time_ms, srcID, destID, 1 );
			break;

		case 4:
			// Read O2_2
			cgc_ReadO2( cmd_time_ms, srcID, destID, 2 );
	 		break;

		case 5:
			// Read Engine Temp
			cgc_ReadEngineTemp( cmd_time_ms, srcID, destID );
			break;

		case 6:
			// Read Engine Torque
			cgc_ReadEngineTorque( cmd_time_ms, srcID, destID );
			break;

		case 7:
			// Start Engine
			cgc_StartEngine( cmd_time_ms, srcID, destID );
			break;

		case 8:
			// Stop Engine
			cgc_StopEngine( cmd_time_ms, srcID, destID );
			break;

		case 9:
			// Set Throttle Position
			cgc_SetThrottlePosition( cmd_time_ms, srcID, destID );
			break;

		case 10:
			// Set Engine Load
			cgc_SetEngineLoad( cmd_time_ms, srcID, destID );
			break;

		case 11:
			// Send Binary Message
			cgc_SendBinaryMessage( cmd_time_ms, srcID, destID );
			break;	

		default:
			// Unknown
			break;
		}
	}
}

void cgc_CSimulation::cgc_RunSimulation( void )
{
	cgc_printf( "Running simulation:\n" );

	if ( m_componentList.cgc_IsEmpty() )
	{
		cgc_printf( "0 components for simulation.\n" );
		return;
	}	

	cgc_uint32_t lastTick = 0;

	cgc_CSimulationCommand *pCommand;

	if ( (pCommand = m_commandList.cgc_GetLast()) )
	{
		lastTick = pCommand->cgc_GetRunTime() / SIMULATION_TICK_COUNT_MS;

		lastTick++;
	}
	else
	{
		cgc_printf( "0 commands for simulation.\n" );
		return;
	}

	// cgc_Run simulation ticks
	cgc_uint32_t nextTick = SIMULATION_TICK_COUNT_MS;

	// Reset pCommand
	pCommand = m_commandList.cgc_GetFirst();

	for ( cgc_uint32_t curTick = 0; curTick < lastTick; curTick++ )
	{
		// Only display the tick counter if an event has occurred
		bool bDidDisplayTick = false;


		// Send commands first...
		while ( pCommand && pCommand->cgc_GetRunTime() < nextTick )
		{
			if ( !bDidDisplayTick )
			{
				cgc_printf( "[TICK][$d]:\n", curTick );
				
				bDidDisplayTick = true;
			}

			cgc_printf( "[SENDCMD][TIME=$d][LENGTH=$d][SRC=$d][DEST=$d]\n", pCommand->cgc_GetRunTime(), pCommand->cgc_GetDataLength(), pCommand->cgc_GetSrcID(), pCommand->cgc_GetDestID() );
			m_lanBus.cgc_InjectSimulationMessage( pCommand->cgc_GetSrcID(), pCommand->cgc_GetDestID(), pCommand->cgc_GetData(), pCommand->cgc_GetDataLength() );			

			pCommand = m_commandList.cgc_GetNext( pCommand );
		}

		// Now run the tick for every component
		for ( cgc_CSimulationComponent *pComponent = m_componentList.cgc_GetFirst(); pComponent; pComponent = m_componentList.cgc_GetNext( pComponent ) )
			pComponent->cgc_Run();

		// Move messages
		m_lanBus.cgc_NextTick();
		
		// Get any messages destined back to us!
		cgc_uint8_t destList[] = { 0xFF, 0x0 };
		
		cgc_CLANMessage *pMsg = m_lanBus.cgc_RecvMessage( destList, sizeof(destList) );

		for ( ; pMsg; pMsg = m_lanBus.cgc_RecvMessage( destList, sizeof(destList), pMsg ) )
		{
			if ( !bDidDisplayTick )
			{
				cgc_printf( "[TICK][$d]:\n", curTick );
				
				bDidDisplayTick = true;
			}

			cgc_printf( "[MESSAGE][SRC=$d][DEST=$d][LENGTH=$d]\n", pMsg->cgc_GetSourceID(), pMsg->cgc_GetDestID(), pMsg->cgc_GetMessageLen() );

			cgc_PrintHexBytes( pMsg->cgc_GetMessageData(), pMsg->cgc_GetMessageLen() );
		}

		nextTick += SIMULATION_TICK_COUNT_MS;
	}

	// Destroy simulation components
	m_commandList.cgc_DeleteAll();
	m_componentList.cgc_DeleteAll();

	cgc_printf( "Simulation complete.\n" );
}

bool cgc_CSimulation::cgc_SendComponentCommand( cgc_uint8_t srcID, cgc_uint8_t destID, cgc_uint32_t cmdTime, cgc_uint8_t *pCmdData, cgc_uint16_t cmdLength )
{
	if ( pCmdData == NULL )
		return (false);

	if ( cmdLength == 0 )
		return (false);

	if ( cmdTime > MAX_SIMULATION_TIME_MS )
		return (false);

	// Order by command time
	cgc_CSimulationCommand *pBefore = NULL;
	cgc_CSimulationCommand *pCur = NULL;

	for ( pCur = m_commandList.cgc_GetFirst(); pCur; pCur = m_commandList.cgc_GetNext( pCur ) )
	{
		if ( pCur->cgc_GetRunTime() > cmdTime )
			break;

		pBefore = pCur;
	}	

	if ( pBefore == NULL )
	{
		// Add to the front...
		m_commandList.cgc_AddFirst( new cgc_CSimulationCommand( srcID, destID, cmdTime, pCmdData, cmdLength ) );		
	}
	else if ( pCur == NULL )
	{
		// Add at the end...
		m_commandList.cgc_AddLast( new cgc_CSimulationCommand( srcID, destID, cmdTime, pCmdData, cmdLength ) );
	}
	else
	{
		// Add after
		m_commandList.cgc_AddAfter( pBefore, new cgc_CSimulationCommand( srcID, destID, cmdTime, pCmdData, cmdLength ) );
	}

	// Successfully added
	return (true);
}

void cgc_CSimulation::cgc_ReadEGT( cgc_uint32_t cmd_time_ms, cgc_uint8_t srcID, cgc_uint8_t destID )
{
	// Force dest ID
	destID = cgc_CECM::ECM_LANBUS_ID;

	cgc_uint8_t egtCommand[4];
	egtCommand[0] = cgc_CECM::READ_DATA_TYPE;
	*((cgc_uint16_t*)(egtCommand+1)) = 1;
	egtCommand[3] = cgc_CECM::READ_EGT_COMMAND;
	
	cgc_SendComponentCommand( srcID, destID, cmd_time_ms, egtCommand, 4 );
}

void cgc_CSimulation::cgc_ReadRPM( cgc_uint32_t cmd_time_ms, cgc_uint8_t srcID, cgc_uint8_t destID )
{
	// Force dest ID
	destID = cgc_CECM::ECM_LANBUS_ID;
	
	cgc_uint8_t egtCommand[4];
	egtCommand[0] = cgc_CECM::READ_DATA_TYPE;
	*((cgc_uint16_t*)(egtCommand+1)) = 1;
	egtCommand[3] = cgc_CECM::READ_RPM_COMMAND;

	cgc_SendComponentCommand( srcID, destID, cmd_time_ms, egtCommand, 4 );
}

void cgc_CSimulation::cgc_ReadIAT( cgc_uint32_t cmd_time_ms, cgc_uint8_t srcID, cgc_uint8_t destID )
{
	// Force dest ID
	destID = cgc_CECM::ECM_LANBUS_ID;
	
	cgc_uint8_t egtCommand[4];
	egtCommand[0] = cgc_CECM::READ_DATA_TYPE;
	*((cgc_uint16_t*)(egtCommand+1)) = 1;	
	egtCommand[3] = cgc_CECM::READ_IAT_COMMAND;

	cgc_SendComponentCommand( srcID, destID, cmd_time_ms, egtCommand, 4 );
}

void cgc_CSimulation::cgc_ReadO2( cgc_uint32_t cmd_time_ms, cgc_uint8_t srcID, cgc_uint8_t destID, cgc_uint8_t o2_num )
{
	// Force dest ID
	destID = cgc_CECM::ECM_LANBUS_ID;
	
	cgc_uint8_t egtCommand[5];
	egtCommand[0] = cgc_CECM::READ_DATA_TYPE;
	*((cgc_uint16_t*)(egtCommand+1)) = 2;	
	egtCommand[3] = cgc_CECM::READ_O2_COMMAND;
	egtCommand[4] = o2_num;

	cgc_SendComponentCommand( srcID, destID, cmd_time_ms, egtCommand, 5 );
}

void cgc_CSimulation::cgc_ReadEngineTemp( cgc_uint32_t cmd_time_ms, cgc_uint8_t srcID, cgc_uint8_t destID )
{
	// Force dest ID
	destID = cgc_CECM::ECM_LANBUS_ID;
	
	cgc_uint8_t egtCommand[4];
	egtCommand[0] = cgc_CECM::READ_DATA_TYPE;
	*((cgc_uint16_t*)(egtCommand+1)) = 1;	
	egtCommand[3] = cgc_CECM::READ_TEMP_COMMAND;

	cgc_SendComponentCommand( srcID, destID, cmd_time_ms, egtCommand, 4 );
}

void cgc_CSimulation::cgc_ReadEngineTorque( cgc_uint32_t cmd_time_ms, cgc_uint8_t srcID, cgc_uint8_t destID )
{
	// Force dest ID
	destID = cgc_CECM::ECM_LANBUS_ID;
	
	cgc_uint8_t egtCommand[4];
	egtCommand[0] = cgc_CECM::READ_DATA_TYPE;
	*((cgc_uint16_t*)(egtCommand+1)) = 1;	
	egtCommand[3] = cgc_CECM::READ_TORQUE_COMMAND;

	cgc_SendComponentCommand( srcID, destID, cmd_time_ms, egtCommand, 4 );
}

void cgc_CSimulation::cgc_StartEngine( cgc_uint32_t cmd_time_ms, cgc_uint8_t srcID, cgc_uint8_t destID )
{
	// Force dest ID
	destID = cgc_CECM::ECM_LANBUS_ID;
	
	cgc_uint8_t egtCommand[4];
	egtCommand[0] = cgc_CECM::ACTION_DATA_TYPE;
	*((cgc_uint16_t*)(egtCommand+1)) = 1;	
	egtCommand[3] = cgc_CECM::START_ENGINE_COMMAND;

	cgc_SendComponentCommand( srcID, destID, cmd_time_ms, egtCommand, 4 );
}

void cgc_CSimulation::cgc_StopEngine( cgc_uint32_t cmd_time_ms, cgc_uint8_t srcID, cgc_uint8_t destID )
{
	// Force dest ID
	destID = cgc_CECM::ECM_LANBUS_ID;
	
	cgc_uint8_t egtCommand[4];
	egtCommand[0] = cgc_CECM::ACTION_DATA_TYPE;
	*((cgc_uint16_t*)(egtCommand+1)) = 1;	
	egtCommand[3] = cgc_CECM::STOP_ENGINE_COMMAND;

	cgc_SendComponentCommand( srcID, destID, cmd_time_ms, egtCommand, 4 );
}

void cgc_CSimulation::cgc_SetThrottlePosition( cgc_uint32_t cmd_time_ms, cgc_uint8_t srcID, cgc_uint8_t destID )
{
	cgc_printf( "Enter throttle position: " );
	cgc_uint32_t value;

	CUtil::cgc_String sLine = cgc_ReadLine();

	sLine.cgc_ToInt( value );
	
	// Force dest ID
	destID = cgc_CECM::ECM_LANBUS_ID;

	cgc_uint8_t egtCommand[6];
	egtCommand[0] = cgc_CECM::SET_DATA_TYPE;
	*((cgc_uint16_t*)(egtCommand+1)) = 3;	
	egtCommand[3] = cgc_CECM::SET_THROTTLEPOS_COMMAND;
	*((cgc_uint16_t*)(egtCommand+4)) = (cgc_uint16_t)value;

	cgc_SendComponentCommand( srcID, destID, cmd_time_ms, egtCommand, 6 );
}

void cgc_CSimulation::cgc_SetEngineLoad( cgc_uint32_t cmd_time_ms, cgc_uint8_t srcID, cgc_uint8_t destID )
{
	cgc_printf( "Set Engine Load: " );
	cgc_uint32_t value;

	CUtil::cgc_String sLine = cgc_ReadLine();

	sLine.cgc_ToInt( value );

	// Force dest ID
	destID = cgc_CECM::ECM_LANBUS_ID;
	
	cgc_uint8_t egtCommand[6];
	egtCommand[0] = cgc_CECM::SET_DATA_TYPE;
	*((cgc_uint16_t*)(egtCommand+1)) = 3;	
	egtCommand[3] = cgc_CECM::SET_TORQUE_COMMAND;
	*((cgc_uint16_t*)(egtCommand+4)) = (cgc_uint16_t)value;

	cgc_SendComponentCommand( srcID, destID, cmd_time_ms, egtCommand, 6 );
}

void cgc_CSimulation::cgc_SendBinaryMessage( cgc_uint32_t cmd_time_ms, cgc_uint8_t srcID, cgc_uint8_t destID )
{
	cgc_printf( "Enter Binary Message (in hex): " );

	CUtil::cgc_String sLine = cgc_ReadLine();

	if ( sLine.cgc_IsEmpty() )
	{
		cgc_printf( "Message data empty. Not sending.\n" );
		return;
	}

	if ( sLine.cgc_GetLength() % 2 != 0 )
	{
		cgc_printf( "Invalid hex message, must be 2-byte aligned.\n" );
		return;
	}

	cgc_uint32_t messageLength = (sLine.cgc_GetLength() / 2);
	
	if ( messageLength > 1024 )
	{
		cgc_printf( "Invalid hex message, max message length exceeded.\n" );
		return;
	}	
	
	cgc_uint8_t *pMessageData = new cgc_uint8_t[messageLength];

	cgc_memset( pMessageData, 0, messageLength );

	cgc_uint32_t binaryPos = 0;
	for ( size_t pos = 0; pos < sLine.cgc_GetLength(); )
	{
		cgc_uint8_t temp;

		if ( !cgc_HexCharToInt( sLine[pos], temp ) )
		{
			cgc_printf( "Invalid character at position $d, not a hex character -- discarding message.\n", pos );
			
			// Memory leak here -- leave it	
			return;
		}

		pos++;
		pMessageData[binaryPos] = (temp & 0xF) << 4;

		if ( !cgc_HexCharToInt( sLine[pos], temp ) )
		{
			cgc_printf( "Invalid character at position $d, not a hex character -- discarding message.\n", pos );
			
			// Memory leak here -- leave it	
			return;
		}

		pos++;
		pMessageData[binaryPos] |= (temp & 0xF);	

		binaryPos++;
	}

	// Send custom component command	
	cgc_SendComponentCommand( srcID, destID, cmd_time_ms, pMessageData, binaryPos );

	// Free temporary message
	delete [] pMessageData;
}
