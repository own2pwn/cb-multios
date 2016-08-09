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
#ifndef __CUTIL_VECTOR_H__
#define __CUTIL_VECTOR_H__

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

// Cromulence Utilities Library
// Vector utility class
namespace CUtil
{
#define VECTOR( T ) Vector<T>

	template<class T>
	class Vector
	{
	public:
		static const cgc_uint32_t VECTOR_BASE_ALLOC_SIZE = 10;
		static const cgc_uint32_t VECTOR_ADD_ALLOC_SIZE = 10;

	public:
		Vector();
		Vector( cgc_uint32_t allocSize );
		Vector( cgc_uint32_t allocSize, cgc_uint32_t growSize );
		~Vector();

		cgc_uint32_t cgc_GetSize( void ) const { return m_vectorSize; };
		cgc_uint32_t cgc_GetAllocSize( void ) const { return m_allocSize; };

		T cgc_GetAt( cgc_uint32_t pos );
		void cgc_AppendLast( T );

		bool cgc_IsEmpty( void );

	private:
		void cgc_GrowVector( void );

		T *m_pContainer;

		cgc_uint32_t m_vectorSize;
		cgc_uint32_t m_allocSize;
		cgc_uint32_t m_growSize;
	};

	template<class T>
	Vector<T>::Vector( )
		: m_allocSize( VECTOR_BASE_ALLOC_SIZE ), m_growSize( VECTOR_ADD_ALLOC_SIZE ), m_vectorSize( 0 )	
	{
		m_pContainer = new T[ VECTOR_BASE_ALLOC_SIZE ];
	}

	template<class T>
	Vector<T>::Vector( cgc_uint32_t allocSize )
		: m_allocSize( allocSize ), m_growSize( VECTOR_ADD_ALLOC_SIZE ), m_vectorSize( 0 )
	{
		m_pContainer = new T[ allocSize ];
	}

	template<class T>
	Vector<T>::Vector( cgc_uint32_t allocSize, cgc_uint32_t growSize )
		: m_allocSize( allocSize ), m_growSize( growSize ), m_vectorSize( 0 )
	{
		m_pContainer = new T[ allocSize ];
	}

	template<class T>
	Vector<T>::~Vector( )
	{
		if ( m_pContainer )
			delete [] m_pContainer;

		m_allocSize = 0;
		m_vectorSize = 0;
	}	

	template<class T>
	T Vector<T>::cgc_GetAt( cgc_uint32_t pos )
	{
		if ( pos < m_vectorSize )
			return (m_pContainer[ pos ]);
		else
			return T(0); // TODO: BAD VECTOR
	}

	template<class T>
	void Vector<T>::cgc_AppendLast( T item )
	{
		if ( m_vectorSize >= m_allocSize )
		{
			// Grow vector
			cgc_GrowVector();
		}

		m_pContainer[ m_vectorSize ] = item;

		m_vectorSize++;
	}

	template<class T>
	void Vector<T>::cgc_GrowVector( void )
	{
		T *pTemp = new T[ m_vectorSize ];

		cgc_memcpy( pTemp, m_pContainer, (m_vectorSize * sizeof(T)) );

		delete [] m_pContainer;

		m_pContainer = new T[ m_vectorSize + m_growSize ];

		m_allocSize += m_growSize;

		cgc_memcpy( m_pContainer, pTemp, (m_vectorSize * sizeof(T)) );

		delete [] pTemp;
	}

	template<class T>
	bool Vector<T>::cgc_IsEmpty( void )
	{
		return (m_vectorSize == 0);
	}
}

#endif // __CUTIL_VECTOR_H__
