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

cgc_CUserEntry::cgc_CUserEntry( const cgc_String &sUserName, const cgc_String &sPassword )
    : m_pLastUnreadMessage( NULL ), m_sUserName( sUserName ), m_sPassword( sPassword )
{

}

cgc_CUserEntry::~cgc_CUserEntry( )
{

}

bool cgc_CUserEntry::cgc_DeleteMessage( cgc_CUserMessage *pCur )
{
    if ( !pCur )
        return (false);

#if PATCHED
    if ( m_pLastUnreadMessage == pCur )
        m_pLastUnreadMessage = (cgc_CUserMessage *)m_oMessageQueue.cgc_GetPrev( pCur );

    m_oMessageQueue.cgc_DeleteItem( pCur );

#else
    // BUG:: Update the last unread message if we need to
    m_oMessageQueue.cgc_DeleteItem( pCur );
#endif

    return (true);
}

cgc_CUserMessage *cgc_CUserEntry::cgc_GetFirstMessage( void )
{
    return (cgc_CUserMessage *)(m_oMessageQueue.cgc_GetFirst());
}

cgc_CUserMessage *cgc_CUserEntry::cgc_GetNextMessage( cgc_CUserMessage *pCur )
{
    if ( !pCur )
        return (NULL);

    return (cgc_CUserMessage *)(m_oMessageQueue.cgc_GetNext( pCur ));
}

cgc_CUserMessage *cgc_CUserEntry::cgc_GetFirstUnreadMessage( void )
{
    if ( m_pLastUnreadMessage == NULL )
        return (NULL);

    return (cgc_CUserMessage *)(m_oMessageQueue.cgc_GetFirst());
}

cgc_CUserMessage *cgc_CUserEntry::cgc_GetLastUnreadMessage( void )
{
    return (m_pLastUnreadMessage);
}

cgc_CUserMessage *cgc_CUserEntry::cgc_GetNextUnreadMessage( cgc_CUserMessage *pCur )
{
    if ( m_pLastUnreadMessage == NULL )
        return (NULL);

    if ( pCur == m_pLastUnreadMessage )
        return (NULL);

    return (cgc_CUserMessage *)(m_oMessageQueue.cgc_GetNext( pCur ));
}

void cgc_CUserEntry::cgc_ClearUnreadMessages( void )
{
    m_pLastUnreadMessage = NULL;
}

bool cgc_CUserEntry::cgc_AddMessage( cgc_CUserMessage *pNewMessage )
{
    if ( !pNewMessage )
        return (false);

    if ( m_pLastUnreadMessage == NULL )
        m_pLastUnreadMessage = pNewMessage;

    m_oMessageQueue.cgc_PushFront( pNewMessage );

    return (true);
}

cgc_uint32_t cgc_CUserEntry::cgc_GetMessageCount( void )
{
    return (m_oMessageQueue.cgc_GetCount());
}

cgc_uint32_t cgc_CUserEntry::cgc_GetUnreadMessageCount( void )
{
    if ( m_pLastUnreadMessage == NULL )
        return (0);

    cgc_uint32_t msgCount = 0;

    for ( cgc_CUserMessage *pCur = (cgc_CUserMessage *)m_oMessageQueue.cgc_GetFirst(); pCur; pCur = (cgc_CUserMessage *)m_oMessageQueue.cgc_GetNext( pCur ) )
    {
        msgCount++;

        if ( pCur == m_pLastUnreadMessage )
            break;
    }

    return (msgCount);
}

cgc_CUserMessage *cgc_CUserEntry::cgc_GetMessageByIndex( cgc_uint32_t idx )
{
    cgc_uint32_t cur_idx = 0;

    for ( cgc_CUserMessage *pCur = (cgc_CUserMessage *)m_oMessageQueue.cgc_GetFirst(); pCur; pCur = (cgc_CUserMessage *)m_oMessageQueue.cgc_GetNext( pCur ) )
    {
        if ( cur_idx == idx )
            return (pCur);

        cur_idx++;
    }

    return (NULL);
}

cgc_CUserDatabase::cgc_CUserDatabase()
{
    for ( cgc_uint32_t idx = 0; idx < MAX_USER_ENTRIES; idx++ )
    {
        m_userTable[idx] = NULL;
    }
}

cgc_CUserDatabase::~cgc_CUserDatabase()
{
    for ( cgc_uint32_t idx = 0; idx < MAX_USER_ENTRIES; idx++ )
    {
        if ( m_userTable[idx] )
            delete m_userTable[idx];
    }
}

bool cgc_CUserDatabase::cgc_CreateUser( const cgc_String &sUserName, const cgc_String &sUserPassword )
{
    // Do we have another user???
    if ( cgc_FindUserByName( sUserName ) )
        return (false);

    if ( cgc_GetUserCount() >= MAX_USER_ENTRIES )
        return (false);

    // Add User
    for ( cgc_uint32_t idx = 0; idx < MAX_USER_ENTRIES; idx++ )
    {
        if ( m_userTable[idx] == NULL )
        {
            m_userTable[idx] = new cgc_CUserEntry( sUserName, sUserPassword );

            return (true);
        }
    }

    return (false);
}

cgc_CUserEntry *cgc_CUserDatabase::cgc_FindUserByName( const cgc_String &sUserName ) const
{
    for ( cgc_uint32_t idx = 0; idx < MAX_USER_ENTRIES; idx++ )
    {
        if ( m_userTable[idx] )
        {
            if ( m_userTable[idx]->cgc_GetUserName() == sUserName )
                return (m_userTable[idx]);
        }
    }

    return (NULL);
}

cgc_CUserEntry *cgc_CUserDatabase::cgc_GetUserForIndex( cgc_uint32_t idx ) const
{
    cgc_uint32_t cur_idx = 0;
    for ( cgc_uint32_t cur = 0; cur < MAX_USER_ENTRIES; cur++ )
    {
        if ( m_userTable[cur] )
        {
            if ( idx == cur_idx )
                return (m_userTable[cur]);

            cur_idx++;
        }
    }

    return (NULL);
}

bool cgc_CUserDatabase::cgc_DeleteUserByName( const cgc_String &sUserName )
{
    cgc_uint32_t idx;

    for ( idx = 0; idx < MAX_USER_ENTRIES; idx++ )
    {
        if ( m_userTable[idx] )
        {
            if ( m_userTable[idx]->cgc_GetUserName() == sUserName )
            {
                delete m_userTable[idx];
                m_userTable[idx] = NULL;

                return (true);
            }
        }
    }

    return (false);
}

cgc_uint32_t cgc_CUserDatabase::cgc_GetUserCount( void ) const
{
    cgc_uint32_t userCount = 0;
    for ( cgc_uint32_t idx = 0; idx < MAX_USER_ENTRIES; idx++ )
    {
        if ( m_userTable[idx] )
            userCount++;
    }

    return (userCount);
}
