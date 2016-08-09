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

#include <stdint.h>
#include <string.h>

// Forward declarations
class cgc_CommandHandler
{
public:
    cgc_CommandHandler();
    ~cgc_CommandHandler();

    void cgc_ParserLoop( void );

private:
    bool cgc_CmdChangeDiverInfo( void );
    bool cgc_CmdLogNewDive( void );
    bool cgc_CmdDownloadDiveData( void );
    bool cgc_CmdEditDives( void );
    bool cgc_CmdPrintDiveLogs( void );
    bool cgc_CmdRemoveDives( void );
    bool cgc_CmdDiverStatistics( void );
    bool cgc_CmdExitApplication( void );

    void cgc_PrintDiverInfo( void );

private:
    typedef bool (cgc_CommandHandler::*cgc_pCmdFptr)( void );

    typedef struct COMMAND_FUNCTION
    {
        char szCommand[128];
        char szDescription[128];
        cgc_pCmdFptr pCmdFunc;
    } cgc_tCommandFunction;

    cgc_DLQueue m_diveQueue;
    cgc_DiverInfo m_oDiver;
    bool m_bDiverInitialized;

private:
    void cgc_PrintCommandTable( cgc_tCommandFunction * );
    void cgc_PrintDiveList( void );
};

#endif // __COMMANDHANDLER_H__
