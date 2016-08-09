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
#ifndef __RESPONSE_H__
#define __RESPONSE_H__

#include <cutil_list.h>

extern "C"
{
#include <stdio.h>
#include <stdlib.h>
}

class cgc_CResponse
{
public:
	cgc_CResponse( );
	~cgc_CResponse( );

	cgc_uint16_t cgc_GetResponseLength( void ) const { return m_responseLen; };

	cgc_uint16_t cgc_AddResponse( cgc_uint8_t *pData, cgc_uint16_t dataLen );
	cgc_uint16_t cgc_GetResponseData( cgc_uint8_t *pDest, cgc_uint16_t destLen );

	void cgc_ClearResponse( void );

private:
	class cgc_CResponseElement
	{
	public:
		cgc_CResponseElement( cgc_uint8_t *pData, cgc_uint16_t dataLen )
		{
			m_pData = new cgc_uint8_t[dataLen];

			cgc_memcpy( m_pData, pData, dataLen );
			m_dataLen = dataLen;
		}

		~cgc_CResponseElement( )
		{
			if ( m_pData )
				delete [] m_pData;
		}

		cgc_uint8_t *cgc_GetData( void ) const { return m_pData; };
		cgc_uint16_t cgc_GetDataLength( void ) const { return m_dataLen; };

	public:
		CUtil::DLL_LINK( cgc_CResponseElement ) m_responseLink;

	private:
		cgc_uint8_t *m_pData;
		cgc_uint16_t m_dataLen;

	};

private:
	CUtil::DLL_LIST( cgc_CResponseElement, m_responseLink ) m_responseList;
	cgc_uint16_t m_responseLen;	
};

#endif // __RESPONSE_H__
