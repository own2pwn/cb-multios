/*

Copyright (c) 2015 Cromulence LLC

Authors: Jason Williams <jdw@cromulence.com>

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
#include <cutil_string.h>

extern "C"
{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
}

using namespace CUtil;


cgc_String::cgc_String( )
{
	m_pData = new char[1];
	m_pData[0] = '\0';

	m_length = 0;
}

cgc_String::cgc_String( const cgc_String &str )
	: m_pData( NULL ), m_length( 0 )
{
	cgc_SetInternal( str );
}

cgc_String::cgc_String( const char *pszStr )
	: m_pData( NULL ), m_length( 0 )
{
	cgc_SetInternal( pszStr );
}

cgc_String::~cgc_String( )
{
	if ( m_pData )
		delete [] m_pData;

	m_length = 0;
}
	
// Operators
bool cgc_String::operator==( const cgc_String &rhs ) const
{
	if ( m_length != rhs.m_length )
		return (false);

	return cgc_memcmp( m_pData, rhs.m_pData, m_length ) == 0;
}

bool cgc_String::operator!=( const cgc_String &rhs ) const
{
	return !(*this == rhs);
}

void cgc_String::operator=( const cgc_String &rhs )
{
	cgc_SetInternal( rhs );
}

void cgc_String::operator=( const char *rhs )
{
	cgc_SetInternal( rhs );
}

const cgc_String& cgc_String::operator+( const cgc_String &rhs ) const
{
	return cgc_String(*this) += rhs;
}

cgc_String& cgc_String::operator+=( const cgc_String &rhs )
{
	// Make a new string of this string + rhs
	size_t new_stringlen = m_length + rhs.m_length;

	char *pNewData = new char[new_stringlen+1];

	cgc_memcpy( pNewData, m_pData, m_length );
	cgc_memcpy( pNewData+m_length, rhs.m_pData, rhs.m_length );

	// Keep a null terminator at the end for easy return of cgc_c_str
	pNewData[m_length+rhs.m_length] = '\0';

	if ( m_pData )
		delete [] m_pData;

	m_pData = pNewData;
	m_length = new_stringlen;

	return *this;
}
		
// Conversion
const char* cgc_String::cgc_c_str( void ) const
{
	return (m_pData);
}

cgc_String cgc_String::cgc_Upper( void ) const
{
	cgc_String sUpper = *this;

	for ( size_t i = 0; i < m_length; i++ )
	{
		if ( cgc_islower( sUpper.m_pData[i] ) )
			sUpper.m_pData[i] = cgc_toupper( sUpper.m_pData[i] );
	}

	return (sUpper);
}

cgc_String cgc_String::cgc_Lower( void ) const
{
	cgc_String sLower = *this;

	for ( size_t i = 0; i < m_length; i++ )
	{
		if ( cgc_isupper( sLower.m_pData[i] ) )
			sLower.m_pData[i] = cgc_tolower( sLower.m_pData[i] );
	}

	return (sLower);
}

char cgc_String::operator[]( const size_t &loc ) const
{
	if ( cgc_IsEmpty() )
		return '\0';

	if ( loc >= m_length )
		return '\0';

	return m_pData[loc];
}

cgc_String cgc_String::cgc_Trim( size_t length ) const
{
	return (cgc_SubString( 0, length ));
}

cgc_String cgc_String::cgc_SubString( size_t startPos, size_t endPos ) const
{
	if ( endPos > m_length )
		endPos = m_length;

	if ( startPos >= m_length || startPos >= endPos )
	       return cgc_String("");

	size_t new_length = endPos - startPos;
	char *pszNewStr = new char[new_length+1];

	size_t destPos = 0;
	for ( size_t srcPos = startPos; srcPos < endPos; srcPos++ )
		pszNewStr[destPos++] = m_pData[srcPos];

	pszNewStr[destPos] = '\0';
	
	return cgc_String(pszNewStr);	
}

cgc_String cgc_String::cgc_TrimSpaces( void ) const
{
	size_t pos = 0;
	for ( ; pos < m_length; pos++ )
	{
		if ( m_pData[pos] != ' ' )
			break;
	}

	return (cgc_SubString( pos, npos ));
}

bool cgc_String::cgc_ToInt( cgc_uint32_t &value )
{
	if ( !cgc_IsEmpty() )
	{
		value = cgc_atoi( m_pData );
		return (true);
	}
	else
		return (false);
}

size_t cgc_String::cgc_GetLength( void ) const
{
	return (m_length);
}

bool cgc_String::cgc_IsEmpty( void ) const
{
	return (m_length == 0);
}

void cgc_String::cgc_SetInternal( const char *pszStr )
{
	if ( m_pData )
		delete [] m_pData;

	if ( pszStr == NULL )
	{
		m_pData = new char[1];
		m_pData[0] = '\0';

		m_length = 0;
		return;
	}

	m_length = cgc_strlen( pszStr );

	m_pData = new char[m_length+1];

	cgc_memcpy( m_pData, pszStr, m_length );
	m_pData[m_length] = '\0';
}

void cgc_String::cgc_SetInternal( const cgc_String& str )
{
	if ( m_pData )
		delete [] m_pData;

	m_length = str.m_length;

	m_pData = new char[m_length+1];

	cgc_memcpy( m_pData, str.m_pData, m_length );
	m_pData[m_length] = '\0';
}
