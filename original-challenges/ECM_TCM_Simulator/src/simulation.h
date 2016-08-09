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
#ifndef __SIMULATION_H__
#define __SIMULATION_H__

#include <cutil_list.h>
#include "lanbus.h"

class cgc_CSimulationComponent
{
public:
	cgc_CSimulationComponent( cgc_CLANBus *pLANBus ) : m_pLANBus( pLANBus ) { };
	virtual ~cgc_CSimulationComponent( ) { };

	virtual void cgc_Run( void ) = 0;

	virtual bool cgc_SendMessage( cgc_uint8_t srcID, cgc_uint8_t destID, cgc_uint8_t *pMessageData, cgc_uint16_t messageLen );

public:
	CUtil::DLL_LINK( cgc_CSimulationComponent ) m_simulationLink;

protected:
	cgc_CLANBus *m_pLANBus;

private:
};

class cgc_CSimulationCommand
{
public:
	cgc_CSimulationCommand( cgc_uint8_t srcID, cgc_uint8_t destID, cgc_uint32_t runTime, cgc_uint8_t *pData, cgc_uint16_t dataLen );
	~cgc_CSimulationCommand( );

	cgc_uint8_t cgc_GetSrcID( void ) const { return m_srcID; };
	cgc_uint8_t cgc_GetDestID( void ) const { return m_destID; };
	cgc_uint8_t *cgc_GetData( void ) const { return m_pData; };
	cgc_uint16_t cgc_GetDataLength( void ) const { return m_dataLen; };
	cgc_uint32_t cgc_GetRunTime( void ) const { return m_runTime; };

public:
	CUtil::DLL_LINK( cgc_CSimulationCommand ) m_simulationLink;

private:
	cgc_uint8_t		m_srcID;
	cgc_uint8_t 	m_destID;
	cgc_uint32_t	m_runTime;
	cgc_uint8_t		*m_pData;
	cgc_uint16_t	m_dataLen;
};

class cgc_CSimulation
{
public:
	const static cgc_uint32_t MAX_SIMULATION_TIME_MS = (1000 * 120);	// 5 minutes
	const static cgc_uint32_t SIMULATION_TICK_COUNT_MS = (50);		// Every 50 milliseconds (or 20x per second) run a tick
public:
	cgc_CSimulation( ) { };
	~cgc_CSimulation( ) { };

	void cgc_AddSimulationComponent( cgc_CSimulationComponent *pItem );
	void cgc_SetupSimulation( void );
	void cgc_RunSimulation( void );

	cgc_CLANBus *cgc_GetLanBus( void ) { return &m_lanBus; };

private:
	bool cgc_SendComponentCommand( cgc_uint8_t srcID, cgc_uint8_t destID, cgc_uint32_t cmdTime, cgc_uint8_t *pCmdData, cgc_uint16_t cmdLength );

	void cgc_ReadEGT( cgc_uint32_t cmd_time_ms, cgc_uint8_t srcID, cgc_uint8_t destID );
	void cgc_ReadRPM( cgc_uint32_t cmd_time_ms, cgc_uint8_t srcID, cgc_uint8_t destID );
	void cgc_ReadIAT( cgc_uint32_t cmd_time_ms, cgc_uint8_t srcID, cgc_uint8_t destID );
	void cgc_ReadO2( cgc_uint32_t cmd_time_ms, cgc_uint8_t srcID, cgc_uint8_t destID, cgc_uint8_t o2_num );
	void cgc_ReadEngineTemp( cgc_uint32_t cmd_time_ms, cgc_uint8_t srcID, cgc_uint8_t destID );
	void cgc_ReadEngineTorque( cgc_uint32_t cmd_time_ms, cgc_uint8_t srcID, cgc_uint8_t destID );
	void cgc_StartEngine( cgc_uint32_t cmd_time_ms, cgc_uint8_t srcID, cgc_uint8_t destID );
	void cgc_StopEngine( cgc_uint32_t cmd_time_ms, cgc_uint8_t srcID, cgc_uint8_t destID );
	void cgc_SetThrottlePosition( cgc_uint32_t cmd_time_ms, cgc_uint8_t srcID, cgc_uint8_t destID );
	void cgc_SetEngineLoad( cgc_uint32_t cmd_time_ms, cgc_uint8_t srcID, cgc_uint8_t destID );
	void cgc_SendBinaryMessage( cgc_uint32_t cmd_time_ms, cgc_uint8_t srcID, cgc_uint8_t destID );

private:
	// LAN Bus for simulation
	cgc_CLANBus m_lanBus;

	// List of commands for simulation run -- ordered
	CUtil::DLL_LIST( cgc_CSimulationCommand, m_simulationLink ) m_commandList;

	// List of components for simulation to run
	CUtil::DLL_LIST( cgc_CSimulationComponent, m_simulationLink ) m_componentList;
};

#endif // __SIMULATION_H__
