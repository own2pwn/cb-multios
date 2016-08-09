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
#ifndef __USER_DATABASE_H__
#define __USER_DATABASE_H__

class cgc_CUserEntry
{
public:
    cgc_CUserEntry( const cgc_String &sUserName, const cgc_String &sPassword );
    ~cgc_CUserEntry( );

    const cgc_String &cgc_GetPassword( void ) const
    {
        return m_sPassword;
    }

    const cgc_String &cgc_GetUserName( void ) const
    {
        return m_sUserName;
    }

    void cgc_SetPassword( cgc_String sPassword )
    {
        m_sPassword = sPassword;
    }

    bool cgc_AddMessage( cgc_CUserMessage *pNewMessage );

    bool cgc_DeleteMessage( cgc_CUserMessage *pMessage );

    cgc_uint32_t cgc_GetMessageCount( void );
    cgc_uint32_t cgc_GetUnreadMessageCount( void );

    cgc_CUserMessage *cgc_GetFirstMessage( void );
    cgc_CUserMessage *cgc_GetNextMessage( cgc_CUserMessage *pCur );

    cgc_CUserMessage *cgc_GetFirstUnreadMessage( void );
    cgc_CUserMessage *cgc_GetLastUnreadMessage( void );
    cgc_CUserMessage *cgc_GetNextUnreadMessage( cgc_CUserMessage *pCur );

    cgc_CUserMessage *cgc_GetMessageByIndex( cgc_uint32_t index );

    void cgc_ClearUnreadMessages( void );

private:
    cgc_String m_sUserName;
    cgc_String m_sPassword;

    cgc_CUserMessage *m_pLastUnreadMessage;
    cgc_CDoubleQueue m_oMessageQueue;
};

class cgc_CUserDatabase
{
public:
    static const cgc_uint32_t MAX_USER_ENTRIES = 64;

public:
    cgc_CUserDatabase();
    ~cgc_CUserDatabase();

    bool cgc_CreateUser( const cgc_String &sUserName, const cgc_String &sPassword );

    cgc_CUserEntry *cgc_FindUserByName( const cgc_String &sUserName ) const;
    cgc_CUserEntry *cgc_GetUserForIndex( cgc_uint32_t idx ) const;

    bool cgc_DeleteUserByName( const cgc_String &sUserName );

    cgc_uint32_t cgc_GetUserCount( void ) const;


private:
    cgc_CUserEntry *m_userTable[MAX_USER_ENTRIES];
};

#endif // __USER_DATABASE_H__
