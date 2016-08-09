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

#ifdef NULL
#undef NULL
#define NULL (0)
#endif

cgc_CCLF::cgc_CCLF( )
	: m_pSections( NULL ), m_sectionCount( 0 )
{

}

cgc_CCLF::~cgc_CCLF( )
{
	if ( m_pSections )
		delete [] m_pSections;

	m_pSections = NULL;
	m_sectionCount = 0;
}

bool cgc_CCLF::cgc_LoadFile( cgc_uint8_t *pFileData, cgc_uint32_t fileSize )
{
	if ( !pFileData )
		return (false);

	if ( fileSize > MAX_LOADER_FILE_SIZE )
		return (false);

	if ( fileSize == 0 )
		return (false);

	if ( fileSize < 8 )
		return (false);

	cgc_uint32_t fileHeader = *((cgc_uint32_t*)pFileData);

	if ( fileHeader != CLF_HEADER_MAGIC )
		return (false);

	cgc_uint32_t fileHeaderSize = *((cgc_uint32_t*)(pFileData+4));

	if ( fileSize != fileHeaderSize )
		return (false);

	if ( fileSize < 12 )
		return (false);

	// Read number of sections
	cgc_uint16_t sectionCount = *((cgc_uint16_t*)(pFileData+8));
	cgc_uint16_t entryAddress = *((cgc_uint16_t*)(pFileData+10));

	// Check section count
	if ( sectionCount > CLF_MAX_SECTION_COUNT )
		return (false);

	if ( (fileSize-12) < (sizeof(cgc_tSectionFileHeader) * sectionCount) )
		return (false);


	cgc_uint8_t i;
	for ( i = 0; i < sectionCount; i++ )
	{
		cgc_tSectionFileHeader *pSectionHeader = (cgc_tSectionFileHeader *)((pFileData+12)+(sizeof(cgc_tSectionFileHeader)*i));
	
		if ( pSectionHeader->sectionType != SECTION_TYPE_EXECUTE && pSectionHeader->sectionType != SECTION_TYPE_DATA && pSectionHeader->sectionType != SECTION_TYPE_COMMENT )
			return (false);	// Invalid section type

		// Check file size
		if ( pSectionHeader->sectionSize+pSectionHeader->fileOffset > fileSize )
			return (false);

		// Check address
		cgc_uint32_t sectionAddress = pSectionHeader->sectionAddress;
		sectionAddress += (cgc_uint32_t)pSectionHeader->sectionSize;	

		if ( sectionAddress > 0x10000 )
			return (false);

		// OK good section 
	}

	// Section good!!!
	// Begin loading sections
	m_entryAddress = entryAddress;
	m_sectionCount = sectionCount;

	if ( m_pSections )
		delete [] m_pSections;

	m_pSections = new cgc_tSectionData[m_sectionCount];

	for ( i = 0; i < sectionCount; i++ )
	{
		cgc_tSectionFileHeader *pSectionHeader = (cgc_tSectionFileHeader *)((pFileData+12)+(sizeof(cgc_tSectionFileHeader)*i));

		m_pSections[i].sectionType = pSectionHeader->sectionType;
		m_pSections[i].sectionSize = pSectionHeader->sectionSize;
		m_pSections[i].sectionAddress = pSectionHeader->sectionAddress;

		// Now load file data
		m_pSections[i].pSectionData = new cgc_uint8_t[pSectionHeader->sectionSize];

		// Copy in data
		cgc_memcpy( m_pSections[i].pSectionData, pFileData+pSectionHeader->fileOffset, pSectionHeader->sectionSize );		
	}

	return (true);
}
