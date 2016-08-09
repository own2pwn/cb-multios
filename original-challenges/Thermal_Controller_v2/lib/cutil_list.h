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
#ifndef __CUTIL_LIST_H__
#define __CUTIL_LIST_H__

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
// Doubly linked list class

namespace CUtil
{
	// Easy means of declaring linked list data structures
#define DLL_LINK( T ) cgc_DoubleLink<T>
#define DLL_LIST( T, link ) cgc_DoubleListDeclare<T, offsetof( T, link )>
#define DLL_PTR( T ) cgc_DoubleList<T> *

	template<class T>
	class cgc_DoubleLink
	{
	public:
		cgc_DoubleLink();
		~cgc_DoubleLink();

		cgc_DoubleLink *cgc_GetNext( void ) { return m_pNext; };
		cgc_DoubleLink *cgc_GetPrev( void ) { return m_pPrev; };

		const cgc_DoubleLink* cgc_GetNext( void ) const { return m_pNext; };
		const cgc_DoubleLink* cgc_GetPrev( void ) const { return m_pPrev; };

		void cgc_Unlink( void );

	private:
		template<class U> friend class cgc_DoubleList;

		cgc_DoubleLink<T> *m_pNext;
		cgc_DoubleLink<T> *m_pPrev;
	};

	template<class T>
	class cgc_DoubleList
	{
	public:
		cgc_DoubleList();
		~cgc_DoubleList();

		void cgc_AddFirst( T *pItem );
		void cgc_AddLast( T *pItem );

		void cgc_AddAfter( T *pBefore, T *pItem );

		T *cgc_GetFirst( void );
		T *cgc_GetLast( void );

		T *cgc_RemoveFirst( void );
		T *cgc_RemoveLast( void );

		T *cgc_GetNext( T *pCur );
		T *cgc_GetPrev( T *pPrev );

		bool cgc_IsEmpty();

		// Delets all items (calls delete operator)
		void cgc_DeleteAll( void );

		// Clears the list items -- unlinks them but does not delete the elements!
		void cgc_ClearAll( void );

	private:
		cgc_DoubleList( cgc_uint32_t offset );

		template<class U, cgc_uint32_t linkOffset> friend class cgc_DoubleListDeclare;

		cgc_uint32_t m_listLinkOffset;	// Contains the offset for the list link member variable in the item class

		cgc_DoubleLink<T> m_first;
		cgc_DoubleLink<T> m_last;
	};

	template<class T, cgc_uint32_t linkOffset>
	class cgc_DoubleListDeclare : public cgc_DoubleList<T>
	{
	public:
		cgc_DoubleListDeclare();
	};

	// Declare a double list class with the appropriate link offset
	template<class T, cgc_uint32_t linkOffset>
	cgc_DoubleListDeclare<T, linkOffset>::cgc_DoubleListDeclare()
		: cgc_DoubleList<T>( linkOffset )
	{

	}

	template<class T>
	cgc_DoubleLink<T>::cgc_DoubleLink( )
	        : m_pNext( NULL ), m_pPrev( NULL )
	{

	}

	template<class T>
	cgc_DoubleLink<T>::~cgc_DoubleLink()
	{
		cgc_Unlink();
	}

	template<class T>
	void cgc_DoubleLink<T>::cgc_Unlink( void )
	{
	        if ( m_pPrev )
	                m_pPrev->m_pNext = m_pNext;

	        if ( m_pNext )
	                m_pNext->m_pPrev = m_pPrev;

		m_pNext = NULL;
		m_pPrev = NULL;	
	}

	template<class T>
	cgc_DoubleList<T>::cgc_DoubleList()
		: m_listLinkOffset( -1 )
	{
		m_first.m_pNext = &m_last;
		m_first.m_pPrev = NULL;

		m_last.m_pPrev = &m_first;
		m_last.m_pNext = NULL;
	}

	template<class T>
	cgc_DoubleList<T>::cgc_DoubleList( cgc_uint32_t offset )
		: m_listLinkOffset( offset )
	{
		m_first.m_pNext = &m_last;
		m_first.m_pPrev = NULL;

		m_last.m_pPrev = &m_first;
		m_last.m_pNext = NULL;
	}

	template<class T>
	cgc_DoubleList<T>::~cgc_DoubleList()
	{
	        cgc_DeleteAll();
	}

	template<class T>
	bool cgc_DoubleList<T>::cgc_IsEmpty( void )
	{
		return (m_first.m_pNext == &m_last);
	}

	template<class T>
	void cgc_DoubleList<T>::cgc_AddFirst( T *pItem )
	{
	        if ( !pItem )
	        	return;

		cgc_DoubleLink<T> *pItemLink = (cgc_DoubleLink<T> *)((cgc_uint8_t*)pItem + m_listLinkOffset);

		pItemLink->m_pNext = m_first.m_pNext;
		pItemLink->m_pPrev = &m_first;
	
		m_first.m_pNext->m_pPrev = pItemLink;	
		m_first.m_pNext = pItemLink;
	}

	template<class T>
	void cgc_DoubleList<T>::cgc_AddLast( T *pItem )
	{
		if ( !pItem )
	        	return;

		cgc_DoubleLink<T> *pItemLink = (cgc_DoubleLink<T> *)((cgc_uint8_t*)pItem + m_listLinkOffset);

		pItemLink->m_pNext = &m_last;
		pItemLink->m_pPrev = m_last.m_pPrev;

		m_last.m_pPrev->m_pNext = pItemLink;
		m_last.m_pPrev = pItemLink;
	}

	template<class T>
	void cgc_DoubleList<T>::cgc_AddAfter( T *pBefore, T *pItem )
	{
		if ( !pItem )
			return;

		if ( !pBefore )
		{
			cgc_AddFirst( pItem );
			return;
		}

		cgc_DoubleLink<T> *pBeforeLink = (cgc_DoubleLink<T> *)((cgc_uint8_t*)pBefore + m_listLinkOffset);
		cgc_DoubleLink<T> *pItemLink = (cgc_DoubleLink<T> *)((cgc_uint8_t*)pItem + m_listLinkOffset);

		pItemLink->m_pNext = pBeforeLink->m_pNext;
		pItemLink->m_pPrev = pBeforeLink;

		if ( pBeforeLink->m_pNext )
			pBeforeLink->m_pNext->m_pPrev = pItemLink;

		pBeforeLink->m_pNext = pItemLink;
	}

	template<class T>
	T *cgc_DoubleList<T>::cgc_GetFirst( void )
	{
		if ( m_first.m_pNext == &m_last )
			return (NULL);
		else
			return (T*)((cgc_uint8_t*)m_first.m_pNext - m_listLinkOffset);
	}

	template<class T>
	T *cgc_DoubleList<T>::cgc_GetLast( void )
	{
		if ( m_last.m_pPrev == &m_first )
			return (NULL);
		else
			return (T*)((cgc_uint8_t*)m_last.m_pPrev - m_listLinkOffset);
	}

	template<class T>
	T *cgc_DoubleList<T>::cgc_GetNext( T *pCur )
	{
		if ( pCur == NULL )
			return (NULL);

		cgc_DoubleLink<T> *pItemLink = (cgc_DoubleLink<T> *)((cgc_uint8_t*)pCur + m_listLinkOffset);
		cgc_DoubleLink<T> *pLink = pItemLink->cgc_GetNext();

		if ( pLink == &m_last )
			return (NULL);
		else
			return (T*)((cgc_uint8_t*)pLink - m_listLinkOffset);
	}

	template<class T>
	T *cgc_DoubleList<T>::cgc_GetPrev( T *pCur )
	{
		if ( pCur == NULL )
			return (NULL);

		cgc_DoubleLink<T> *pItemLink = (cgc_DoubleLink<T> *)((cgc_uint8_t*)pCur + m_listLinkOffset);
		cgc_DoubleLink<T> *pLink = pItemLink->cgc_GetPrev();

		if ( pLink == &m_first )
			return (NULL);
		else
			return (T*)((cgc_uint8_t*)pLink - m_listLinkOffset);
	}

	template<class T>
	T *cgc_DoubleList<T>::cgc_RemoveFirst( void )
	{
		if ( m_first.m_pNext == &m_last )
			return (NULL);
		else
		{
			T *pItem = (T*)((cgc_uint8_t*)m_first.m_pNext - m_listLinkOffset);

			m_first.m_pNext->cgc_Unlink();

			return (pItem);
		}
	}

	template<class T>
	T *cgc_DoubleList<T>::cgc_RemoveLast( void )
	{
		if ( m_last.m_pPrev == &m_first )
			return (NULL);
		else
		{
			T *pItem = (T*)((cgc_uint8_t*)m_last.m_pPrev - m_listLinkOffset);

			m_last.m_pPrev->cgc_Unlink();

			return (pItem);
		}
	}
				
	//         // Delets all items (calls delete operator)
	template<class T>
	void cgc_DoubleList<T>::cgc_DeleteAll( void )
	{
		while ( !cgc_IsEmpty() )
			delete cgc_GetFirst();
	}

	// Clears the list items -- unlinks them but does not delete the elements!
	template<class T>
	void cgc_DoubleList<T>::cgc_ClearAll( void )
	{
		while ( !cgc_IsEmpty() )
			m_first.m_pNext->cgc_Unlink();
	}
}

#endif // __CUTIL_LIST_H__
