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

cgc_CDoubleItemList::cgc_CDoubleItemList( )
    : m_pFirst( NULL ), m_pLast( NULL )
{

}

cgc_CDoubleItemList::~cgc_CDoubleItemList( )
{
    cgc_DeleteAll();
}

void cgc_CDoubleItemList::cgc_DeleteAll( void )
{
    cgc_CDoubleItemLink *pNext;
    cgc_CDoubleItemLink *pCur;

    for ( pCur = m_pFirst; pCur; pCur = pNext )
    {
        pNext = pCur->m_pNext;

        // Delete entry
        delete pCur;

        pCur = pNext;
    }
}

cgc_CDoubleItemLink *cgc_CDoubleItemList::cgc_GetNext( cgc_CDoubleItemLink *pItem )
{
    if ( pItem->m_pList != this )
        return (NULL);

    return (pItem->m_pNext);
}

cgc_CDoubleItemLink *cgc_CDoubleItemList::cgc_GetPrev( cgc_CDoubleItemLink *pItem )
{
    if ( pItem->m_pList != this )
        return (NULL);

    return (pItem->m_pPrev);
}

cgc_CDoubleItemLink *cgc_CDoubleItemList::cgc_RemoveFirst( void )
{
    cgc_CDoubleItemLink *pItem = m_pFirst;

    cgc_RemoveItem( pItem );

    return (pItem);
}

cgc_CDoubleItemLink *cgc_CDoubleItemList::cgc_RemoveLast( void )
{
    cgc_CDoubleItemLink *pItem = m_pLast;

    cgc_RemoveItem( pItem );

    return (pItem);
}

cgc_CDoubleItemLink *cgc_CDoubleItemList::cgc_RemoveItem( cgc_CDoubleItemLink *pItem )
{
    // Check if item is in list...
    if ( pItem->m_pList != this )
        return (NULL);

    if ( pItem->m_pNext )
        pItem->m_pNext->m_pPrev = pItem->m_pPrev;

    if ( pItem->m_pPrev )
        pItem->m_pPrev->m_pNext = pItem->m_pNext;

    if ( pItem == m_pLast )
        m_pLast = pItem->m_pPrev;

    if ( pItem == m_pFirst )
        m_pFirst = pItem->m_pNext;

    // Remove our list pointer
    pItem->cgc_ClearList();

    return (pItem);
}

cgc_CDoubleItemLink *cgc_CDoubleItemList::cgc_AddFirst( cgc_CDoubleItemLink *pItem )
{
    if ( pItem == NULL )
        return (NULL);

    if ( pItem->m_pList != NULL )
        return (NULL);  // Already in a list!

    if ( m_pFirst == NULL )
    {
        // Empty list
        pItem->m_pPrev = NULL;
        pItem->m_pNext = NULL;

        m_pFirst = m_pLast = pItem;
    }
    else
    {
        pItem->m_pNext = m_pFirst;

        pItem->m_pPrev = NULL;
        m_pFirst->m_pPrev = pItem;

        m_pFirst = pItem;
    }

    // Item tracks what list it is in
    pItem->cgc_AddList( this );

    return (pItem);
}

cgc_CDoubleItemLink *cgc_CDoubleItemList::cgc_AddAfter( cgc_CDoubleItemLink *pPrev, cgc_CDoubleItemLink *pItem )
{
    // SANITY check
    if ( pItem == NULL )
        return (NULL);

    if ( pItem->m_pList != NULL )
        return (NULL);

    if ( pPrev == NULL )
        return (cgc_AddFirst( pItem ));

    if ( pPrev->m_pList != this )
        return (NULL);

    pItem->m_pNext = pPrev->m_pNext;
    pItem->m_pPrev = pPrev;

    if ( pPrev->m_pNext )
        pPrev->m_pNext->m_pPrev = pItem;

    pPrev->m_pNext = pItem;

    if ( m_pLast == pPrev )
        m_pLast = pItem;

    // Item tracks what list it is in
    pItem->cgc_AddList( this );

    return (pItem);
}

cgc_CDoubleItemLink *cgc_CDoubleItemList::cgc_AddLast( cgc_CDoubleItemLink *pItem )
{
    // SANITY check
    if ( pItem == NULL )
        return (NULL);

    if ( pItem->m_pList != NULL )
        return (NULL);  // Already in a list!

    if ( m_pLast == NULL )
    {
        // Empty list
        pItem->m_pNext = NULL;
        pItem->m_pPrev = NULL;

        m_pFirst = m_pLast = pItem;
    }
    else
    {
        pItem->m_pNext = NULL;
        pItem->m_pPrev = m_pLast;

        m_pLast->m_pNext = pItem;

        m_pLast = pItem;
    }

    // Item tracks what list it is in
    pItem->cgc_AddList( this );

    return (pItem);
}

cgc_CDoubleItemLink::cgc_CDoubleItemLink( )
    : m_pNext( NULL ), m_pPrev( NULL ), m_pList( NULL )
{

}

cgc_CDoubleItemLink::~cgc_CDoubleItemLink( )
{
    cgc_Unlink();
}

void cgc_CDoubleItemLink::cgc_Unlink( void )
{
    if ( m_pList == NULL )
        return;

    m_pList->cgc_RemoveItem( this );
}

cgc_CDoubleQueue::cgc_CDoubleQueue()
    : m_itemCount( 0 ), cgc_CDoubleItemList()
{

}

cgc_CDoubleQueue::~cgc_CDoubleQueue()
{

}
