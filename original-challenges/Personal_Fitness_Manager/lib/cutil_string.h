/*

Copyright (c) 2016 Cromulence LLC

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
#ifndef __CUTIL_STRING_H__
#define __CUTIL_STRING_H__

extern "C"
{
#include <stdint.h>
#include <libcgc.h>
#include <stddef.h>
}

#ifdef NULL
#undef NULL
#define NULL 0
#endif

#include <stdint.h>

// Cromulence Utilities Library
// cgc_String class

namespace CUtil
{
	class cgc_String
	{
	public:
		// End position marker
		const static size_t npos = 0xffffffff;

	public:
		cgc_String( );
		cgc_String( const cgc_String &str );
		cgc_String( const char *pszStr );

		~cgc_String( );

		// Operators
		bool operator==( const cgc_String &rhs ) const;
		bool operator!=( const cgc_String &rhs ) const;
		void operator=( const cgc_String &rhs );
		void operator=( const char *rhs );

		const cgc_String& operator+( const cgc_String &rhs ) const;
		cgc_String& operator+=( const cgc_String &rhs );

		// Conversion
		const char* cgc_c_str( void ) const;
		cgc_String cgc_Upper( void ) const;
		cgc_String cgc_Lower( void ) const;

		// cgc_Trim to a specific length
		cgc_String cgc_Trim( size_t length ) const;

		// Accessor method
		char operator[]( const size_t &loc ) const;

		// Get string within string from start to end positions
		cgc_String cgc_SubString( size_t startPos, size_t endPos ) const;
		
		// cgc_Trim leading spaces (cgc_TrimSpaces)	
		cgc_String cgc_TrimSpaces( void ) const;

		bool cgc_ToInt( cgc_uint32_t &value );

		size_t cgc_GetLength( void ) const;
		bool cgc_IsEmpty( void ) const;

	protected:
		void cgc_SetInternal( const cgc_String &str );
		void cgc_SetInternal( const char *pszStr );
	
	protected:
		char 	*m_pData;
		size_t	m_length;	
	};
}

#endif // __CUTIL_STRING_H__
