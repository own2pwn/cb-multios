/*

Author: Jason Williams

Copyright (c) 2015 Cromulence LLC

Permission is hereby granted, cgc_free of charge, to any person obtaining a copy
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
#include <libcgc.h>
#include <stdint.h>
#include <stdlib.h>
}

#include "workerprng.h"

#ifdef NULL
#undef NULL
#define NULL (0)
#endif

cgc_CPRNG::cgc_CPRNG( cgc_uint32_t *pMagicPage, cgc_uint32_t magicPageByteSize )
{
	m_pMagicPage = pMagicPage;

	m_magicPageSize = (magicPageByteSize / sizeof(cgc_uint32_t));

	m_magicPagePosition = 0;

	m_matrixPosition = 0;
	m_matrixSize = 0;
	m_pGeneratorMatrix = NULL;
}

cgc_CPRNG::~cgc_CPRNG( )
{
	if ( m_pGeneratorMatrix )
		delete m_pGeneratorMatrix;
}

cgc_uint32_t cgc_CPRNG::cgc_GetRandomU32( void )
{
	return (cgc_GetRandomInternal());
}

cgc_uint16_t cgc_CPRNG::cgc_GetRandomU16( void )
{
	return (cgc_uint16_t)(cgc_GetRandomInternal() & 0xFFFF);
}

cgc_uint8_t cgc_CPRNG::cgc_GetRandomU8( void )
{
	return (cgc_uint8_t)(cgc_GetRandomInternal() & 0xFF);
}
	
cgc_uint32_t cgc_CPRNG::cgc_GetRandomRange( cgc_uint32_t start, cgc_uint32_t end )
{
	if ( start > end )
	{
		cgc_uint32_t temp = end;
		end = start;
		start = temp;
	}

	cgc_uint32_t delta = (end - start);

	if ( delta == 0 )
		return (start);

	// Not exactly uniform distribution but good enough!
	return start + cgc_GetRandomInternal()%(delta+1);	
}

cgc_uint32_t cgc_CPRNG::cgc_GetRandomInternal( void )
{
	if ( !m_pGeneratorMatrix )
		cgc_GenerateMatrix();

	if ( m_matrixPosition >= m_matrixSize )
		cgc_GenerateMatrix();
	
	cgc_uint32_t newValue = m_pGeneratorMatrix[m_matrixPosition++];

	return (newValue);
}

void cgc_CPRNG::cgc_GenerateMatrix( void )
{
	if ( !m_pGeneratorMatrix )
	{
		m_pGeneratorMatrix = new cgc_uint32_t[DEFAULT_MATRIX_SIZE];

		// Initialize
		for ( cgc_uint32_t i = 0; i < DEFAULT_MATRIX_SIZE; i++ )
			m_pGeneratorMatrix[i] = i;

		m_matrixSize = DEFAULT_MATRIX_SIZE;
		m_matrixPosition = 0;
	}

	cgc_uint32_t newMatrixSize = m_matrixSize + DEFAULT_MATRIX_SIZE;
	if ( newMatrixSize > PRNG_MATRIX_MAX_SIZE )
		newMatrixSize = PRNG_MATRIX_MAX_SIZE;

	cgc_uint32_t *pNewMatrix = new cgc_uint32_t[newMatrixSize];

	cgc_uint32_t generatorPos = 0;
	cgc_uint32_t magicPageLast = 0;
	for ( cgc_uint32_t i = 0; i < newMatrixSize; i++ )
	{
		pNewMatrix[i] = m_pMagicPage[m_magicPagePosition++] & MAGIC_PAGE_MASK;

		pNewMatrix[i] = pNewMatrix[i] ^ (magicPageLast+(i+1));

		if ( m_magicPagePosition >= m_magicPageSize )
			m_magicPagePosition = 0;

		pNewMatrix[i] = pNewMatrix[i] ^ m_pGeneratorMatrix[generatorPos];
		magicPageLast = pNewMatrix[i];

		generatorPos++;
		if ( generatorPos > m_matrixSize )
			generatorPos = 0;
	}

	delete m_pGeneratorMatrix;
	m_pGeneratorMatrix = pNewMatrix;

	m_matrixSize = newMatrixSize;
	m_matrixPosition = 0;
}

