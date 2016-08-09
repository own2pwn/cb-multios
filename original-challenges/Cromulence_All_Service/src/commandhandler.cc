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

cgc_CCommandHandler::cgc_CCommandHandler()
    : m_cmdCount( 0 )
{
    cgc_memset( m_cmdTable, 0, sizeof(cgc_tCommandTableEntry) * MAX_COMMAND_ENTRIES );
}

cgc_CCommandHandler::~cgc_CCommandHandler()
{

}

bool cgc_CCommandHandler::cgc_RegisterCommand( const char *pszCommand, const char *pszDescription, cgc_tCmdFunction pFunc )
{
    if ( m_cmdCount == MAX_COMMAND_ENTRIES )
        return (false);

    if ( !pszCommand )
        return (false);

    if ( !pFunc )
        return (false);

    cgc_strncpy( m_cmdTable[m_cmdCount].szCommand, pszCommand, MAX_COMMAND_LENGTH );
    cgc_strncpy( m_cmdTable[m_cmdCount].szDescription, pszDescription, MAX_DESCRIPTION_LENGTH );

    m_cmdTable[m_cmdCount].pCmdFunc = pFunc;

    // Update command table count
    m_cmdCount++;

    return (true);
}

cgc_tCmdFunction cgc_CCommandHandler::cgc_GetCommandFunction( const char *pszCommand )
{
    for ( cgc_uint32_t i = 0; i < m_cmdCount; i++ )
    {
        if ( cgc_stricmp( m_cmdTable[i].szCommand, pszCommand ) == 0 )
            return (m_cmdTable[i].pCmdFunc);
    }

    return (NULL);
}

void cgc_CCommandHandler::cgc_Run( void )
{
    char szLine[1024];

    for (;;)
    {
        // Prompt
        cgc_printf( ": " );

        // Get command line
        cgc_getline( szLine, 1024 );

        char *szToken = cgc_strtok( szLine, " " );

        if ( szToken == NULL )
        {
            cgc_printf( "Unknown command.\n" );
            continue;
        }

        if ( cgc_stricmp( szToken, "exit" ) == 0 )
            break;

        if ( cgc_stricmp( szToken, "?" ) == 0 )
        {
            cgc_ListCommands();
            continue;
        }

        // Search for appropriate command
        cgc_tCmdFunction pCmdFunc = cgc_GetCommandFunction( szToken );

        if ( pCmdFunc == NULL )
        {
            cgc_printf( "Unknown command.\n" );
            continue;
        }

        (*pCmdFunc)( cgc_strtok( NULL, "" ) );
    }
}

void cgc_CCommandHandler::cgc_ListCommands( void )
{
    cgc_printf( "Available commands:\n" );
    for ( cgc_uint32_t i = 0; i < m_cmdCount; i++ )
    {
        cgc_printf( "@s - @s\n", m_cmdTable[i].szCommand, m_cmdTable[i].szDescription );
    }
}
