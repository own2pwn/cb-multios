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
#include <string.h>
}

#include "response.h"


cgc_CResponse::cgc_CResponse( )
	: m_responseLen( 0 )
{

}

cgc_CResponse::~cgc_CResponse( )
{

}

cgc_uint16_t cgc_CResponse::cgc_AddResponse( cgc_uint8_t *pData, cgc_uint16_t dataLen )
{
	if ( pData == NULL || dataLen == 0 )
		return 0;

	m_responseList.cgc_AddLast( new cgc_CResponseElement( pData, dataLen ) );

	m_responseLen += dataLen;

	return (dataLen);
}

cgc_uint16_t cgc_CResponse::cgc_GetResponseData( cgc_uint8_t *pDest, cgc_uint16_t destLen )
{
	cgc_uint16_t pos = 0;

	if ( pDest == NULL )
		return (0);

	if ( destLen < cgc_GetResponseLength() )
		return (0);

	for ( cgc_CResponseElement *pCur = m_responseList.cgc_GetFirst(); pCur; pCur = m_responseList.cgc_GetNext( pCur ) )
	{
		cgc_memcpy( pDest+pos, pCur->cgc_GetData(), pCur->cgc_GetDataLength() );
		pos += pCur->cgc_GetDataLength();
	}

	return (pos);
}

void cgc_CResponse::cgc_ClearResponse( void )
{
	m_responseList.cgc_DeleteAll();
	m_responseLen = 0;
}
