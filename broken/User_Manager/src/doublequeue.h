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
#ifndef __DOUBLE_QUEUE_H__
#define __DOUBLE_QUEUE_H__

class cgc_CDoubleItemLink;

class cgc_CDoubleItemList
{
public:
    cgc_CDoubleItemList();
    ~cgc_CDoubleItemList();

    void cgc_DeleteAll();

    cgc_CDoubleItemLink *cgc_GetFirst( void )
    {
        return (m_pFirst);
    }

    cgc_CDoubleItemLink *cgc_GetLast( void )
    {
        return (m_pLast);
    }

    cgc_CDoubleItemLink *cgc_GetNext( cgc_CDoubleItemLink *pItem );
    cgc_CDoubleItemLink *cgc_GetPrev( cgc_CDoubleItemLink *pItem );

    cgc_CDoubleItemLink *cgc_RemoveFirst( void );
    cgc_CDoubleItemLink *cgc_RemoveLast( void );

    cgc_CDoubleItemLink *cgc_RemoveItem( cgc_CDoubleItemLink *pItem );

    cgc_CDoubleItemLink *cgc_AddFirst( cgc_CDoubleItemLink *pItem );
    cgc_CDoubleItemLink *cgc_AddAfter( cgc_CDoubleItemLink *pPrev, cgc_CDoubleItemLink *pItem );
    cgc_CDoubleItemLink *cgc_AddLast( cgc_CDoubleItemLink *pItem );

protected:
    cgc_CDoubleItemLink *m_pFirst;
    cgc_CDoubleItemLink *m_pLast;
};

class cgc_CDoubleItemLink
{
friend class cgc_CDoubleItemList;

public:
    cgc_CDoubleItemLink();
    ~cgc_CDoubleItemLink();

    void cgc_Unlink( void );

private:
    void cgc_AddList( cgc_CDoubleItemList *pList )
    {
        m_pList = pList;
    }

    void cgc_ClearList( void )
    {
        m_pList = NULL;
    }

private:
    cgc_CDoubleItemLink *m_pNext;
    cgc_CDoubleItemLink *m_pPrev;
    cgc_CDoubleItemList *m_pList;
};

class cgc_CDoubleQueue : public cgc_CDoubleItemList
{
public:
    cgc_CDoubleQueue();
    ~cgc_CDoubleQueue();

    cgc_uint32_t cgc_GetCount( void ) const
    {
        return (m_itemCount);
    }

    cgc_CDoubleItemLink *cgc_PopFront( void )
    {
        cgc_CDoubleItemLink *pItem;

        if ( (pItem = cgc_RemoveFirst()) )
            m_itemCount--;

        return (pItem);
    }

    cgc_CDoubleItemLink *cgc_PopBack( void )
    {
        cgc_CDoubleItemLink *pItem;

        if ( (pItem = cgc_RemoveLast()) )
            m_itemCount--;

        return (pItem);
    }

    void cgc_PushFront( cgc_CDoubleItemLink *pItem )
    {
        if ( cgc_AddFirst( pItem ) )
            m_itemCount++;
    }

    void cgc_PushBack( cgc_CDoubleItemLink *pItem )
    {
        if ( cgc_AddLast( pItem ) )
            m_itemCount++;
    }

    cgc_CDoubleItemLink *cgc_GetFront( void )
    {
        return (m_pFirst);
    }

    cgc_CDoubleItemLink *cgc_GetBack( void )
    {
        return (m_pLast);
    }

    bool cgc_DeleteItem( cgc_CDoubleItemLink *pItem )
    {
        cgc_CDoubleItemLink *pRemovedItem = cgc_CDoubleItemList::cgc_RemoveItem( pItem );

        if ( pRemovedItem )
            m_itemCount--;
        else
            return (false);

        delete pItem;

        return (true);
    }
private:
    cgc_uint32_t m_itemCount;
};

#endif // __DOUBLE_QUEUE_H__
