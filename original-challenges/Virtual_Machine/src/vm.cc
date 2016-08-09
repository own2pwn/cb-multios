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

#include "clf.h"
#include "vm.h"

cgc_CVM::cgc_CVM( void *pMagicPage )
	: m_pMagicPage( pMagicPage ), m_oCPU( pMagicPage )
{

}

cgc_CVM::~cgc_CVM( )
{

}

bool cgc_CVM::cgc_Init( cgc_CCLF *pLoader )
{
	if ( !pLoader )
		return (false);

	if ( pLoader->m_sectionCount == 0 )
		return (false);

	if ( pLoader->m_pSections == NULL )
		return (false);

	// Begin loading
	for ( cgc_uint32_t i = 0; i < pLoader->m_sectionCount; i++ )
	{
		cgc_uint16_t sectionAddress = pLoader->m_pSections[i].sectionAddress;
		cgc_uint16_t sectionSize = pLoader->m_pSections[i].sectionSize;
		cgc_uint8_t *pSectionData = pLoader->m_pSections[i].pSectionData;
		cgc_uint8_t sectionType = pLoader->m_pSections[i].sectionType;

		cgc_uint8_t mmuFlags = 0;
		if ( sectionType == cgc_CCLF::SECTION_TYPE_EXECUTE )
			mmuFlags |= MMU_PAGE_READ | MMU_PAGE_EXEC;
		else if ( sectionType == cgc_CCLF::SECTION_TYPE_DATA )
			mmuFlags |= MMU_PAGE_READ | MMU_PAGE_WRITE;
		else if ( sectionType == cgc_CCLF::SECTION_TYPE_COMMENT )
			continue;

		if ( !m_oMMU.cgc_AddMemorySection( sectionAddress, pSectionData, sectionSize, mmuFlags ) )
			return (false);
	}

	// Initialize DMA
	m_oDMA.cgc_InitDMA();

	// Initialize CPU
	m_oCPU.cgc_Init( &m_oMMU, this, &m_oDMA, pLoader->m_entryAddress );

	cgc_CSendDevice *pSendDevice = new cgc_CSendDevice( 1 );
	cgc_CReadDevice *pReadDevice = new cgc_CReadDevice( 2 );

	m_oPeripheralList.cgc_AddLast( pSendDevice );
	m_oPeripheralList.cgc_AddLast( pReadDevice );

	// Add peripherals
	m_oDMA.cgc_AddPeripheral( 1, pSendDevice );
	m_oDMA.cgc_AddPeripheral( 2, pReadDevice );

	return (true);
}

bool cgc_CVM::cgc_Run( void )
{
	m_oCPU.cgc_Run();

	if ( m_oCPU.cgc_HasException() )
		cgc_printf( "Exception: $s\n", m_oCPU.cgc_GetExceptionText().cgc_c_str() );

	cgc_printf( "Registers:\n$s\nEnd Registers\n", m_oCPU.cgc_DumpRegisters().cgc_c_str() );

	return (true);
}
