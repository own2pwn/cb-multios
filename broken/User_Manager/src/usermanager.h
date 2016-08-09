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
#ifndef __USER_MANAGER_H__
#define __USER_MANAGER_H__

class cgc_CUserManager
{
public:
    static const cgc_uint32_t MAX_USERNAME_LENGTH = 12;
    static const cgc_uint32_t MAX_PASSWORD_LENGTH = 12;

public:
    cgc_CUserManager();
    ~cgc_CUserManager();

    void cgc_Run( void );

private:
    bool cgc_RunCreateUser( void );
    bool cgc_RunUserLogin( void );
    bool cgc_RunAdminLogin( void );

    void cgc_DoAdminListUsers( void );
    void cgc_DoAdminDeleteUser( void );
    void cgc_DoAdminChangePassword( void );

    void cgc_DoUserSendMessage( cgc_CUserEntry *pUser );
    void cgc_DoUserReadMessages( cgc_CUserEntry *pUser );
    void cgc_DoUserListMessages( cgc_CUserEntry *pUser );
    void cgc_DoUserDeleteMessage( cgc_CUserEntry *pUser );
    void cgc_DoUserChangePassword( cgc_CUserEntry *pUser );

private:
    cgc_String m_sAdminPassword;
    cgc_CUserDatabase m_userDB;
};

#endif // __USER_MANAGER_H__
