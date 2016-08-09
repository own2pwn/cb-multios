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
#ifndef __DLQUEUE_H__
#define __DLQUEUE_H__

class cgc_DLQueue;

class cgc_DLItem
{
public:
    cgc_DLItem( ) : m_pNext( NULL ), m_pPrev( NULL ) { };
    ~cgc_DLItem( ) { };

    cgc_DLItem *cgc_GetNext( void ) { return m_pNext; };
    cgc_DLItem *cgc_GetPrev( void ) { return m_pPrev; };

    friend class cgc_DLQueue;

private:
    cgc_DLItem *m_pNext;
    cgc_DLItem *m_pPrev;
};

class cgc_DLQueue
{
public:
    cgc_DLQueue( );
    ~cgc_DLQueue( );

    void cgc_DeleteAll( void );

    void cgc_AddFirst( cgc_DLItem *pItem );
    void cgc_AddLast( cgc_DLItem *pItem );
    void cgc_AddAfter( cgc_DLItem *pPrev, cgc_DLItem *pItem );

    void cgc_Unlink( cgc_DLItem *pItem );

    void cgc_DeleteItem( cgc_DLItem *pItem );
    void cgc_RemoveItem( cgc_DLItem *pItem );
    cgc_DLItem *cgc_RemoveFirst( void );
    cgc_DLItem *cgc_RemoveLast( void );

    cgc_DLItem *cgc_GetFirst( void ) { return m_pFirst; };
    cgc_DLItem *cgc_GetLast( void ) { return m_pLast; };

    cgc_size_t cgc_GetCount( void ) { return m_count; };

private:
    cgc_DLItem *m_pFirst;
    cgc_DLItem *m_pLast;
    cgc_size_t m_count;
};

#endif // __DLQUEUE_H___
