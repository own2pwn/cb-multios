/*

Author: Jason Williams <jdw@cromulence.com>

Copyright (c) 2014 Cromulence LLC

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

#include "common.h"

cgc_DLQueue::cgc_DLQueue( )
    : m_pFirst( NULL ), m_pLast( NULL ), m_count( 0 )
{

}

cgc_DLQueue::~cgc_DLQueue( )
{
    cgc_DeleteAll();
}

void cgc_DLQueue::cgc_AddFirst( cgc_DLItem *pItem )
{
    if ( pItem == NULL )
        return;

    if ( m_pFirst == NULL )
    {
        pItem->m_pNext = NULL;
        pItem->m_pPrev = NULL;

        m_pFirst = pItem;
        m_pLast = pItem;
    }
    else
    {
        pItem->m_pNext = m_pFirst;
        pItem->m_pPrev = NULL;

        m_pFirst->m_pPrev = pItem;

        m_pFirst = pItem;
    }

    // Increment count
    m_count++;
}

void cgc_DLQueue::cgc_AddLast( cgc_DLItem *pItem )
{
    if ( pItem == NULL )
        return;

    if ( m_pLast == NULL )
    {
        pItem->m_pNext = NULL;
        pItem->m_pPrev = NULL;

        m_pFirst = pItem;
        m_pLast = pItem;
    }
    else
    {
        pItem->m_pNext = NULL;
        pItem->m_pPrev = m_pLast;

        m_pLast->m_pNext = pItem;

        m_pLast = pItem;
    }

    // Increment count
    m_count++;
}

void cgc_DLQueue::cgc_AddAfter( cgc_DLItem *pPrev, cgc_DLItem *pItem )
{
    if ( pPrev == NULL || pItem == NULL )
        return;

    pItem->m_pNext = pPrev->m_pNext;
    pItem->m_pPrev = pPrev;

    if ( pPrev->m_pNext )
        pPrev->m_pNext->m_pPrev = pItem;

    pPrev->m_pNext = pItem;

    if ( m_pLast == pPrev )
        m_pLast = pItem;

    // Increment count
    m_count++;
}

void cgc_DLQueue::cgc_Unlink( cgc_DLItem *pItem )
{
    if ( pItem->m_pNext )
        pItem->m_pNext->m_pPrev = pItem->m_pPrev;

    if ( pItem->m_pPrev )
        pItem->m_pPrev->m_pNext = pItem->m_pNext;

    if ( m_pFirst == pItem )
        m_pFirst = pItem->m_pNext;

    if ( m_pLast == pItem )
        m_pLast = pItem->m_pPrev;

    m_count--;
}

void cgc_DLQueue::cgc_DeleteItem( cgc_DLItem *pItem )
{
    cgc_Unlink( pItem );

    delete pItem;
}

void cgc_DLQueue::cgc_RemoveItem( cgc_DLItem *pItem )
{
    cgc_Unlink( pItem );
}

cgc_DLItem *cgc_DLQueue::cgc_RemoveFirst( void )
{
    cgc_DLItem *pItem = m_pFirst;
    cgc_Unlink( pItem );

    return (pItem);
}

cgc_DLItem *cgc_DLQueue::cgc_RemoveLast( void )
{
    cgc_DLItem *pItem = m_pLast;
    cgc_Unlink( pItem );

    return (pItem);
}

void cgc_DLQueue::cgc_DeleteAll( void )
{
    // Fast delete all items
    cgc_DLItem *pCur = m_pFirst;
    cgc_DLItem *pNext = NULL;

    for ( ; pCur; pCur = pNext )
    {
        pNext = pCur->m_pNext;

        delete pCur;
    }

    m_count = 0;
    m_pFirst = m_pLast = NULL;
}
