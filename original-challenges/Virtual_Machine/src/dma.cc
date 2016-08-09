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
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
}

#include "dma.h"
#include "mmu.h"

cgc_CDMA::cgc_CDMA( )
{
	for ( cgc_uint32_t i = 0; i < 256; i++ )
		m_pPeripherals[i] = NULL;
}

cgc_CDMA::~cgc_CDMA( )
{

}

void cgc_CDMA::cgc_InitDMA( void )
{
	for ( cgc_uint32_t i = 0; i < MAX_DMA_WORKERS; i++ )
		m_dmaWorkers[i].cgc_ClearWorker();

	m_workerLast = 0;
	m_workerCount = 0;
}

bool cgc_CDMA::cgc_AddPeripheral( cgc_uint8_t deviceID, cgc_CPeripheral *pPeripheral )
{
	if ( !pPeripheral )
		return (false);

	if ( m_pPeripherals[deviceID] )
		return (false);

	m_pPeripherals[deviceID] = pPeripheral;

	return (true);
}

bool cgc_CDMA::cgc_InitReadWorker( cgc_uint8_t deviceID, cgc_uint16_t address, cgc_uint16_t length  )
{
	cgc_uint32_t workerCur = m_workerCount++;

#ifdef PATCHED_1
	if ( workerCur >= MAX_DMA_WORKERS )
#else
	if ( workerCur > MAX_DMA_WORKERS )
#endif
		return (false);

	m_dmaWorkers[workerCur].cgc_StartWorker( DMA_WORKER_READ, deviceID, address, length );

	return (true);
}

bool cgc_CDMA::cgc_InitWriteWorker( cgc_uint8_t deviceID, cgc_uint16_t address, cgc_uint16_t length )
{
	cgc_uint32_t workerCur = m_workerCount++;

#ifdef PATCHED_1
	if ( workerCur >= MAX_DMA_WORKERS )
#else
	if ( workerCur > MAX_DMA_WORKERS )
#endif
		return (false);

	m_dmaWorkers[workerCur].cgc_StartWorker( DMA_WORKER_WRITE, deviceID, address, length );

	return (true);
}

void cgc_CDMA::cgc_ServiceDMA( cgc_CMMU *pMMU )
{
	if ( !pMMU )
		return;

	cgc_uint8_t dmaTransferCount = 0;

	for ( dmaTransferCount = 0; dmaTransferCount < m_workerCount; dmaTransferCount++ )
	{
		// Only allow a maximum number of transfers
		if ( dmaTransferCount >= MAX_DMA_TRANSFER_COUNT )
			break;

		// Round robin each DMA worker
		cgc_uint8_t workerCur = m_workerLast;

		// Find a worker to service
		bool bWorkersAvailable = false;

		cgc_uint32_t tryCount = 0;

		for ( tryCount = 0; tryCount < MAX_DMA_WORKERS; tryCount++ )
		{
			if ( workerCur >= MAX_DMA_WORKERS )
				workerCur = 0;

			if ( m_dmaWorkers[workerCur].cgc_IsWorkerAvailable() )
			{
				bWorkersAvailable = true;
				break;
			}

			workerCur++;
		}

		if ( bWorkersAvailable )
		{
			cgc_uint8_t deviceID = m_dmaWorkers[workerCur].cgc_GetDeviceID();
			cgc_uint16_t address = m_dmaWorkers[workerCur].cgc_GetAddress();
			cgc_uint16_t length = m_dmaWorkers[workerCur].cgc_GetLength();
			cgc_uint16_t position = m_dmaWorkers[workerCur].cgc_GetPosition();
			cgc_eDMAWorkerAction actionType = m_dmaWorkers[workerCur].cgc_GetType();

			cgc_uint32_t dmaAmount = (length - position);
			if ( dmaAmount > 4 )
				dmaAmount = 4;

			if ( dmaAmount == 0 )
			{
				// Remove worker
				m_workerCount--;

				// End transfer
				m_dmaWorkers[workerCur].cgc_ClearWorker();
			}
			else if ( m_pPeripherals[deviceID] )
			{
				cgc_uint8_t readValue[4];

				if ( actionType == DMA_WORKER_READ )
				{

					if ( m_pPeripherals[deviceID]->cgc_Read( readValue, dmaAmount ) )
						pMMU->cgc_WriteDMA( address+position, readValue, dmaAmount );
				}
				else
				{
					if ( pMMU->cgc_ReadDMA( address+position, readValue, dmaAmount ) )
						m_pPeripherals[deviceID]->cgc_Write( readValue, dmaAmount );
				}

				if ( position+dmaAmount >= length )
				{
					// Remove worker
					m_workerCount--;

					// End transfer
					m_dmaWorkers[workerCur].cgc_ClearWorker();
				}
				else
					m_dmaWorkers[workerCur].cgc_AdvancePosition( dmaAmount );
			}
			else
			{
				m_dmaWorkers[workerCur].cgc_ClearWorker();
				m_workerCount--;
			}

			// Advance to next worker	
			m_workerLast = workerCur++;
		}
		else
		{
			m_workerLast = 0;
			break;
		}
	}
}
