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
#ifndef __DOUBLE_LIST_H__
#define __DOUBLE_LIST_H__

class cgc_CDoubleList;

class cgc_CDoubleListElement
{
public:
    cgc_CDoubleListElement( ) : m_pNext( NULL ), m_pPrev( NULL ) { };
    ~cgc_CDoubleListElement( ) { };

    cgc_CDoubleListElement *cgc_GetNext( void ) const { return m_pNext; };
    cgc_CDoubleListElement *cgc_GetPrev( void ) const { return m_pPrev; };

public:
    friend class cgc_CDoubleList;

private:
    cgc_CDoubleListElement *m_pNext;
    cgc_CDoubleListElement *m_pPrev;
};

class cgc_CDoubleList
{
public:
    cgc_CDoubleList( );
    ~cgc_CDoubleList( );

    void cgc_DeleteAll( void );

    void cgc_InsertHead( cgc_CDoubleListElement *pItem );
    void cgc_InsertTail( cgc_CDoubleListElement *pItem );
    void cgc_InsertAfter( cgc_CDoubleListElement *pPrev, cgc_CDoubleListElement *pItem );

    cgc_CDoubleListElement *cgc_GetFirst( void ) const { return m_pFirst; };
    cgc_CDoubleListElement *cgc_GetLast( void ) const { return m_pFirst; };

    void cgc_Unlink( cgc_CDoubleListElement *pItem );

private:
    cgc_CDoubleListElement *m_pFirst;
    cgc_CDoubleListElement *m_pLast;
};

#endif // __DOUBLE_LIST_H__
