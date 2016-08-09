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
#ifndef __WORKER_PRNG_H__
#define __WORKER_PRNG_H__

// Needlessly complex PRNG that does unnecessary work (memory allocations/transformations/etc.)
#define PRNG_MATRIX_MAX_SIZE	(4096)
#define DEFAULT_MATRIX_SIZE	(512)

#define MAGIC_PAGE_MASK		(0xA5C1B301)

#define MAGIC_PAGE_SIZE_DWORD	(1024)

class cgc_CPRNG
{
public:
	cgc_CPRNG( cgc_uint32_t *pMagicPage, cgc_uint32_t magicPageByteSize );
	~cgc_CPRNG( );

	cgc_uint32_t cgc_GetRandomU32( void );
	cgc_uint16_t cgc_GetRandomU16( void );
	cgc_uint8_t cgc_GetRandomU8( void );
	
	cgc_uint32_t cgc_GetRandomRange( cgc_uint32_t start, cgc_uint32_t end );

private:
	cgc_uint32_t cgc_GetRandomInternal( void );

	void cgc_GenerateMatrix( void );	

private:
	cgc_uint32_t m_matrixPosition;
	cgc_uint32_t m_matrixSize;
	cgc_uint32_t *m_pGeneratorMatrix;
	
	cgc_uint32_t m_magicPagePosition;
	cgc_uint32_t m_magicPageSize;	
	cgc_uint32_t *m_pMagicPage;
};

#endif // __WORKER_PRNG_H__
