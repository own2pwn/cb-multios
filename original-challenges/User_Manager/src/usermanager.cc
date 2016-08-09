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

cgc_CUserManager::cgc_CUserManager()
{
    m_sAdminPassword = "$admin$77";
}

cgc_CUserManager::~cgc_CUserManager()
{

}

void cgc_CUserManager::cgc_Run( void )
{
    // Action string
    cgc_String sAction;

    // Print banner
    cgc_printf( "User Manager Console:\n" );

    // Enter command loop
    bool bRunning = true;
    do
    {
        // Main console
        cgc_printf( "1) Create User\n" );
        cgc_printf( "2) User Login\n" );
        cgc_printf( "3) Admin Login\n" );
        cgc_printf( "4) Exit\n" );

        // Get user input
        cgc_ReadLine( sAction );

        cgc_int32_t actionIntValue;

        // Get action value as an integer
        if ( !sAction.cgc_ToInteger( actionIntValue ) )
            actionIntValue = -1;

        // Perform action
        switch ( actionIntValue )
        {
        case 1:     // Create User
            bRunning = cgc_RunCreateUser();
            break;

        case 2:     // User Login
            bRunning = cgc_RunUserLogin();
            break;

        case 3:     // Admin Login
            bRunning = cgc_RunAdminLogin();
            break;

        case 4:     // Exit
            bRunning = false;
            break;

        default:    // Invalid entry
            cgc_printf( "Invalid choice\n" );
            break;
        }
    } while ( bRunning );

    // Exit banner
    cgc_printf( "Exiting\n" );
}

bool cgc_CUserManager::cgc_RunCreateUser( void )
{
    cgc_String sUserName;
    cgc_String sPassword;

    bool bGoodUserName = true;
    do
    {
        cgc_printf( "Username: " );

        cgc_ReadLine( sUserName );

        if ( sUserName.cgc_length() > MAX_USERNAME_LENGTH )
        {
            cgc_printf( "Username too long, only @d characters allowed.\n", MAX_USERNAME_LENGTH );
            bGoodUserName = false;
        }
        else
            bGoodUserName = true;

    } while ( !bGoodUserName );

    bool bGoodPassword = false;
    do
    {
        cgc_printf( "Password: " );
        cgc_ReadLine( sPassword );

        if ( sPassword.cgc_length() > MAX_PASSWORD_LENGTH )
        {
            cgc_printf( "Password too long, only @d characters allowed.\n", MAX_PASSWORD_LENGTH );
            bGoodPassword = false;
        }
        else
            bGoodPassword = true;

    } while ( !bGoodPassword );

    if ( !m_userDB.cgc_CreateUser( sUserName, sPassword ) )
        cgc_printf( "Failed to create user -- user already exists.\n" );
    else
        cgc_printf( "User created successfully.\n" );

    // True -- indicates continue running command loop
    return (true);
}

bool cgc_CUserManager::cgc_RunUserLogin( void )
{
    cgc_String sUserName;
    cgc_String sPassword;
    cgc_String sAction;

    cgc_printf( "Enter Username: " );

    cgc_ReadLine( sUserName );

    cgc_printf( "Enter Password: " );

    cgc_ReadLine( sPassword );

    // Verify user credentials
    cgc_CUserEntry *pUserData = m_userDB.cgc_FindUserByName( sUserName );

    if ( !pUserData )
    {
        cgc_printf( "Login failed. Invalid username or password.\n" );
        return (true);
    }

    if ( pUserData->cgc_GetPassword() != sPassword )
    {
        cgc_printf( "Login failed. Invalid username or password.\n" );
        return (true);
    }

    cgc_printf( "Login success.\n" );

    bool bUserLoggedIn = true;
    do
    {
        cgc_printf( "1) Send Message\n" );
        cgc_printf( "2) Read Message\n" );
        cgc_printf( "3) List Messages\n" );
        cgc_printf( "4) Delete Message\n" );
        cgc_printf( "5) Logout\n" );
        cgc_printf( "6) Exit\n" );

        cgc_ReadLine( sAction );

        cgc_int32_t actionIntValue;

        // Get action value as an integer
        if ( !sAction.cgc_ToInteger( actionIntValue ) )
            actionIntValue = -1;

        // Perform action
        switch ( actionIntValue )
        {
        case 1: // Send message
            cgc_DoUserSendMessage( pUserData );
            break;

        case 2: // Read messages
            cgc_DoUserReadMessages( pUserData );
            break;

        case 3: // List messages
            cgc_DoUserListMessages( pUserData );
            break;

        case 4: // Delete message
            cgc_DoUserDeleteMessage( pUserData );
            break;

        case 5: // Log out this user
            bUserLoggedIn = false;
            break;

        case 6: // Exit application
            return (false);
            break;

        default:
            cgc_printf( "Invalid command\n" );
            break;
        }

    } while ( bUserLoggedIn );

    cgc_printf( "Logged out.\n" );

    // True -- indicates continue running command loop
    return (true);
}

bool cgc_CUserManager::cgc_RunAdminLogin( void )
{
    cgc_String sAction;
    cgc_String sAdminPassword;

    cgc_printf( "Admin Password: " );

    cgc_ReadLine( sAdminPassword );

    if ( sAdminPassword != m_sAdminPassword )
    {
        cgc_printf( "Invalid Password.\n" );
        return (true);
    }

    bool bExitAdmin = false;
    do
    {
        cgc_printf( "Admin Commands:\n" );
        cgc_printf( "1) List Users\n" );
        cgc_printf( "2) Delete User\n" );
        cgc_printf( "3) Change User Password\n" );
        cgc_printf( "4) Logout\n" );

        // Get user input
        cgc_ReadLine( sAction );

        cgc_int32_t actionIntValue;

        // Get action value as an integer
        if ( !sAction.cgc_ToInteger( actionIntValue ) )
            actionIntValue = -1;

        // Perform action
        switch ( actionIntValue )
        {
        case 1:
            cgc_DoAdminListUsers();
            break;

        case 2:
            cgc_DoAdminDeleteUser();
            break;

        case 3:
            cgc_DoAdminChangePassword();
            break;

        case 4:
            cgc_printf( "Logging out\n" );
            bExitAdmin = true;
            break;
        }
    } while ( !bExitAdmin );

    // True -- indicates continue running command loop
    return (true);
}

void cgc_CUserManager::cgc_DoAdminListUsers( void )
{
    cgc_printf( "Listing users in database:\n" );

    cgc_uint32_t userIdx;
    cgc_uint32_t userCount = m_userDB.cgc_GetUserCount();

    for ( userIdx = 0; userIdx < userCount; userIdx++ )
    {
        cgc_CUserEntry *pUserData = m_userDB.cgc_GetUserForIndex( userIdx );

        cgc_printf( "@d: @s\n", userIdx, pUserData->cgc_GetUserName().cgc_c_str() );
    }
}

void cgc_CUserManager::cgc_DoAdminDeleteUser( void )
{
    cgc_String sUserName;

    cgc_printf( "Username to delete: " );

    cgc_ReadLine( sUserName );

    if ( m_userDB.cgc_DeleteUserByName( sUserName ) )
        cgc_printf( "User deleted.\n" );
    else
        cgc_printf( "User not found, failed to delete.\n" );
}

void cgc_CUserManager::cgc_DoAdminChangePassword( void )
{
    cgc_String sUserName;
    cgc_String sPassword;

    cgc_printf( "Username to change password: " );

    cgc_ReadLine( sUserName );

    cgc_CUserEntry *pUserData = m_userDB.cgc_FindUserByName( sUserName );

    if ( pUserData == NULL )
    {
        cgc_printf( "User not found in database.\n" );
        return;
    }

    cgc_printf( "New password: " );

    cgc_ReadLine( sPassword );

    if ( sPassword.cgc_length() > MAX_PASSWORD_LENGTH )
    {
        cgc_printf( "Password too long. Failed to change password.\n" );
        return;
    }

    // Now set users password
    pUserData->cgc_SetPassword( sPassword );

    cgc_printf( "Password set for user.\n" );

    return;
}


void cgc_CUserManager::cgc_DoUserSendMessage( cgc_CUserEntry *pUser )
{
    cgc_String sUserName;
    cgc_String sMessage;

    cgc_printf( "Username to send to: " );

    cgc_ReadLine( sUserName );

    cgc_CUserEntry *pUserData = m_userDB.cgc_FindUserByName( sUserName );

    if ( pUserData == NULL )
    {
        cgc_printf( "User not found.\n" );
        return;
    }

    cgc_printf( "Enter Message: " );

    cgc_ReadLine( sMessage );

    if ( sMessage.cgc_length() > 128 )
    {
        cgc_printf( "Message too long, truncating.\n" );
        sMessage.cgc_Trim( 128 );
    }

    if ( !pUserData->cgc_AddMessage( new cgc_CUserMessage( pUser->cgc_GetUserName(), sMessage ) ) )
        cgc_printf( "Message not sent.\n" );
    else
        cgc_printf( "Message sent.\n" );
}

void cgc_CUserManager::cgc_DoUserReadMessages( cgc_CUserEntry *pUser )
{
    if ( pUser->cgc_GetMessageCount() == 0 )
    {
        cgc_printf( "No messages.\n" );
        return;
    }

    // Print new messages
    if ( pUser->cgc_GetUnreadMessageCount() > 0 )
    {
        cgc_CUserMessage *pCur;

        cgc_printf( "@d unread messages available. Last unread message:\n", pUser->cgc_GetUnreadMessageCount() );

        // Display last unread message
        pCur = pUser->cgc_GetLastUnreadMessage();

        cgc_printf( "From: @s\n", pCur->cgc_GetFrom().cgc_c_str() );
        cgc_printf( "Message: @s\n", pCur->cgc_GetMessage().cgc_c_str() );

        pUser->cgc_ClearUnreadMessages( );
    }

    cgc_printf( "@d total messages available, enter number to read: ", pUser->cgc_GetMessageCount() );

    cgc_String sAction;

    cgc_int32_t messageNumber;

    cgc_ReadLine( sAction );

    if ( !sAction.cgc_ToInteger( messageNumber ) )
    {
        cgc_printf( "Invalid message number.\n" );
        return;
    }

    if ( messageNumber < 0 || messageNumber >= pUser->cgc_GetMessageCount() )
    {
        cgc_printf( "Invalid message number.\n" );
        return;
    }

    cgc_CUserMessage *pCurMessage = pUser->cgc_GetMessageByIndex( messageNumber );

    if ( !pCurMessage )
        return;

    cgc_printf( "Message @d:\n", messageNumber );
    cgc_printf( "From: @s\n", pCurMessage->cgc_GetFrom().cgc_c_str() );
    cgc_printf( "Message: @s\n", pCurMessage->cgc_GetMessage().cgc_c_str() );

    return;
}

void cgc_CUserManager::cgc_DoUserListMessages( cgc_CUserEntry *pUser )
{
    if ( pUser->cgc_GetMessageCount() == 0 )
    {
        cgc_printf( "No messages.\n" );
        return;
    }

    cgc_printf( "Listing @d messages:\n", pUser->cgc_GetMessageCount() );

    cgc_uint32_t idx = 0;
    for ( cgc_CUserMessage *pCur = pUser->cgc_GetFirstMessage(); pCur; pCur = pUser->cgc_GetNextMessage( pCur ) )
    {
        cgc_printf( "Message @d:\n", idx );
        cgc_printf( "From: @s\n", pCur->cgc_GetFrom().cgc_c_str() );
        cgc_printf( "Message: @s\n", pCur->cgc_GetMessage().cgc_c_str() );

        idx++;
    }
}

void cgc_CUserManager::cgc_DoUserDeleteMessage( cgc_CUserEntry *pUser )
{
    if ( pUser->cgc_GetMessageCount() == 0 )
    {
        cgc_printf( "No messages to delete.\n" );
        return;
    }

    cgc_printf( "@d messages available. Enter number to delete: ", pUser->cgc_GetMessageCount() );

    cgc_String sAction;

    cgc_int32_t messageNumber;

    cgc_ReadLine( sAction );

    if ( !sAction.cgc_ToInteger( messageNumber ) )
    {
        cgc_printf( "Invalid message number.\n" );
        return;
    }

    if ( messageNumber < 0 || messageNumber >= pUser->cgc_GetMessageCount() )
    {
        cgc_printf( "Invalid message number.\n" );
        return;
    }

    cgc_CUserMessage *pCurMessage = pUser->cgc_GetMessageByIndex( messageNumber );

    if ( !pUser->cgc_DeleteMessage( pCurMessage ) )
        cgc_printf( "Failed to delete message.\n" );
    else
        cgc_printf( "Message deleted.\n" );
}
