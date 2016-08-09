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
#ifndef __DMA_H__
#define __DMA_H__

#include "peripheral.h"

class cgc_CMMU;

class cgc_CDMA
{
public:
	static const cgc_uint32_t MAX_DMA_WORKERS = 5;	// Max number of individual DMA workers
	static const cgc_uint32_t MAX_DMA_TRANSFER_COUNT = 4; // Max number of DMA transfers that can be serviced at a time

public:
	cgc_CDMA( );
	~cgc_CDMA( );

	void cgc_InitDMA( void );
	bool cgc_AddPeripheral( cgc_uint8_t deviceID, cgc_CPeripheral *pPeripheral );

	void cgc_ServiceDMA( cgc_CMMU *pMMU );

	bool cgc_InitReadWorker( cgc_uint8_t deviceID, cgc_uint16_t address, cgc_uint16_t length );
	bool cgc_InitWriteWorker( cgc_uint8_t deviceID, cgc_uint16_t address, cgc_uint16_t length );

private:
	typedef enum
	{
		DMA_WORKER_NONE = 0,
		DMA_WORKER_READ = 1,
		DMA_WORKER_WRITE = 2,
	} cgc_eDMAWorkerAction;

	class cgc_CDMAWorker
	{
	public:
		cgc_CDMAWorker( )
			: m_actionType( DMA_WORKER_NONE )
		{

		}

		cgc_CDMAWorker( cgc_eDMAWorkerAction actionType, cgc_uint8_t deviceID, cgc_uint16_t address, cgc_uint16_t length )
			: m_actionType( actionType ), m_deviceID( deviceID ), m_address( address ), m_length( length )
		{

		}

		~cgc_CDMAWorker( )
		{

		}

		cgc_eDMAWorkerAction cgc_GetType( void ) const { return m_actionType; };
		cgc_uint8_t cgc_GetDeviceID( void ) const { return m_deviceID; };
		cgc_uint16_t cgc_GetAddress( void ) const { return m_address; };
		cgc_uint16_t cgc_GetLength( void ) const { return m_length; };
		cgc_uint16_t cgc_GetPosition( void ) const { return m_curPos; };

		void cgc_AdvancePosition( cgc_uint16_t amount ) { m_curPos += amount; };

		void cgc_ClearWorker( void ) { m_actionType = DMA_WORKER_NONE; };

		void cgc_StartWorker( cgc_eDMAWorkerAction actionType, cgc_uint8_t deviceID, cgc_uint16_t address, cgc_uint16_t length )
		{
			m_actionType = actionType;
			m_address = address;
			m_deviceID = deviceID;
			m_length = length;

			m_curPos = 0;
		}

		bool cgc_IsWorkerAvailable( void ) const { return m_actionType != DMA_WORKER_NONE; };
		
	private:
		cgc_eDMAWorkerAction m_actionType;
		cgc_uint8_t m_deviceID;
		cgc_uint16_t m_curPos;
		cgc_uint16_t m_address;
		cgc_uint16_t m_length;
	};

	cgc_CDMAWorker m_dmaWorkers[MAX_DMA_WORKERS];
	cgc_uint8_t m_workerCount;
	cgc_uint8_t m_workerLast;

	cgc_CPeripheral *m_pPeripherals[256];
};

#endif // __DMA_H__
