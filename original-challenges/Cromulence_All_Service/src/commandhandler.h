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
#ifndef __COMMANDHANDLER_H__
#define __COMMANDHANDLER_H__

#define MAX_COMMAND_ENTRIES     (20)
#define MAX_COMMAND_LENGTH      (128)
#define MAX_DESCRIPTION_LENGTH  (512)

typedef void (*cgc_tCmdFunction)( char *pszArgString );

typedef struct cgc_COMMAND_TABLE_ENTRY
{
    char            szCommand[128];
    char            szDescription[512];
    cgc_tCmdFunction    pCmdFunc;
} cgc_tCommandTableEntry;

class cgc_CCommandHandler
{
public:
    cgc_CCommandHandler();
    ~cgc_CCommandHandler();

    bool cgc_RegisterCommand( const char *pszCommand, const char *pszDescription, cgc_tCmdFunction funcPtr );

    cgc_tCmdFunction cgc_GetCommandFunction( const char *pszCommand );

    void cgc_Run( void );

    void cgc_ListCommands( void );

private:
    cgc_tCommandTableEntry m_cmdTable[MAX_COMMAND_ENTRIES];
    cgc_uint32_t m_cmdCount;
};

#endif // __COMMANDHANDLER_H___
