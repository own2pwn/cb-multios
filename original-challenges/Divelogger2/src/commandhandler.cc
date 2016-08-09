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
/*
extern "C" {
#include <libcgc.h>
#include <stdlib.h>
}
*/

#include "common.h"

cgc_CommandHandler::cgc_CommandHandler() :
    m_bDiverInitialized( false )
{

}

cgc_CommandHandler::~cgc_CommandHandler()
{

}

bool cgc_CommandHandler::cgc_CmdChangeDiverInfo( void )
{
    cgc_printf( "\n" );

    if ( m_bDiverInitialized )
    {
        cgc_String sFirstName;
        cgc_String sLastName;
        cgc_String sStreet;
        cgc_String sCity;
        cgc_String sState;
        cgc_String sZipCode;
        cgc_String sPhoneNumber;
        cgc_String sPadiNumber;
        cgc_String sPadiDate;

        cgc_printf( "First Name (@s): ", m_oDiver.cgc_GetFirstName().cgc_c_str() );
        cgc_ReadLine( sFirstName );
        sFirstName.cgc_Trim( 20 );

        cgc_printf( "Last Name (@s): ", m_oDiver.cgc_GetLastName().cgc_c_str() );
        cgc_ReadLine( sLastName );
        sLastName.cgc_Trim( 20 );

        cgc_printf( "Street (@s): ", m_oDiver.cgc_GetStreet().cgc_c_str() );
        cgc_ReadLine( sStreet );
        sStreet.cgc_Trim( 29 );

        cgc_printf( "City (@s): ", m_oDiver.cgc_GetCity().cgc_c_str() );
        cgc_ReadLine( sCity );
        sCity.cgc_Trim( 19 );

        cgc_printf( "State (@s): ", m_oDiver.cgc_GetState().cgc_c_str() );
        cgc_ReadLine( sState );
        sState.cgc_Trim( 2 );

        cgc_printf( "Zip Code (@s): ", m_oDiver.cgc_GetZipCode().cgc_c_str() );
        cgc_ReadLine( sZipCode );
        sZipCode.cgc_Trim( 10 );

        cgc_printf( "Phone Number (@s): ", m_oDiver.cgc_GetPhoneNumber().cgc_c_str() );
        cgc_ReadLine( sPhoneNumber );
        sPhoneNumber.cgc_Trim( 15 );

        cgc_printf( "PADI Diver Number (@s): ", m_oDiver.cgc_GetPadiNumber().cgc_c_str() );
        cgc_ReadLine( sPadiNumber );
        sPadiNumber.cgc_Trim( 19 );

        cgc_printf( "PADI Cert cgc_Date (@s): ", m_oDiver.cgc_GetPadiDate().cgc_c_str() );
        cgc_ReadLine( sPadiDate );
        sPadiDate.cgc_Trim( 10 );

        // Only change them if they entered information
        if ( sFirstName != "" )
            m_oDiver.cgc_SetFirstName( sFirstName );

        if ( sLastName != "" )
            m_oDiver.cgc_SetLastName( sLastName );

        if ( sStreet != "" )
            m_oDiver.cgc_SetStreet( sStreet );

        if ( sCity != "" )
            m_oDiver.cgc_SetCity( sCity );

        if ( sState != "" )
            m_oDiver.cgc_SetState( sState );

        if ( sZipCode != "" )
            m_oDiver.cgc_SetZipCode( sZipCode );

        if ( sPhoneNumber != "" )
            m_oDiver.cgc_SetPhoneNumber( sPhoneNumber );

        if ( sPadiNumber != "" )
            m_oDiver.cgc_SetPadiNumber( sPadiNumber );

        if ( sPadiDate != "" )
            m_oDiver.cgc_SetPadiDate( sPadiDate );
    }
    else
    {
        cgc_String sFirstName;
        cgc_String sLastName;
        cgc_String sStreet;
        cgc_String sCity;
        cgc_String sState;
        cgc_String sZipCode;
        cgc_String sPhoneNumber;
        cgc_String sPadiNumber;
        cgc_String sPadiDate;

        cgc_printf( "First Name: " );
        cgc_ReadLine( sFirstName );
        sFirstName.cgc_Trim( 20 );

        cgc_printf( "Last Name: " );
        cgc_ReadLine( sLastName );
        sLastName.cgc_Trim( 20 );

        cgc_printf( "Street: " );
        cgc_ReadLine( sStreet );
        sStreet.cgc_Trim( 29 );

        cgc_printf( "City: " );
        cgc_ReadLine( sCity );
        sCity.cgc_Trim( 19 );

        cgc_printf( "State: " );
        cgc_ReadLine( sState );
        sState.cgc_Trim( 2 );

        cgc_printf( "Zip Code: " );
        cgc_ReadLine( sZipCode );
        sZipCode.cgc_Trim( 10 );

        cgc_printf( "Phone Number: " );
        cgc_ReadLine( sPhoneNumber );
        sPhoneNumber.cgc_Trim( 15 );

        cgc_printf( "PADI Diver Number: " );
        cgc_ReadLine( sPadiNumber );
        sPadiNumber.cgc_Trim( 19 );

        cgc_printf( "PADI Cert cgc_Date: " );
        cgc_ReadLine( sPadiDate );
        sPadiDate.cgc_Trim( 10 );

        m_oDiver.cgc_SetFirstName( sFirstName );
        m_oDiver.cgc_SetLastName( sLastName );
        m_oDiver.cgc_SetStreet( sStreet );
        m_oDiver.cgc_SetCity( sCity );
        m_oDiver.cgc_SetState( sState );
        m_oDiver.cgc_SetZipCode( sZipCode );
        m_oDiver.cgc_SetPhoneNumber( sPhoneNumber );
        m_oDiver.cgc_SetPadiNumber( sPadiNumber );
        m_oDiver.cgc_SetPadiDate( sPadiDate );
    }

    m_bDiverInitialized = true;

    cgc_PrintDiverInfo();

    return (false);
}

bool cgc_CommandHandler::cgc_CmdLogNewDive( void )
{
    cgc_String sDiveSite;
    cgc_String sDate;
    cgc_String sTime;
    cgc_String sLocation;
    cgc_String sMaxDepth;
    cgc_String sAvgDepth;
    cgc_String sDiveDuration;
    cgc_String sO2Percentage;
    cgc_String sPressureIn;
    cgc_String sPressureOut;

    // Capture dive information
    cgc_printf( "\n" );

    cgc_printf( "cgc_Dive Site: " );
    cgc_ReadLine( sDiveSite );
    sDiveSite.cgc_Trim( 25 );

    cgc_printf( "cgc_Date: " );
    cgc_ReadLine( sDate );
    sDate.cgc_Trim( 10 );

    cgc_printf( "cgc_Time: " );
    cgc_ReadLine( sTime );
    sTime.cgc_Trim( 8 );

    cgc_printf( "Location (area/city): " );
    cgc_ReadLine( sLocation );
    sLocation.cgc_Trim( 25 );

    cgc_printf( "Max Depth in ft: " );
    cgc_ReadLine( sMaxDepth );

    cgc_printf( "Avg Depth in ft: " );
    cgc_ReadLine( sAvgDepth );

    cgc_printf( "cgc_Dive Duration (mins): " );
    cgc_ReadLine( sDiveDuration );

    cgc_printf( "O2 Percentage: " );
    cgc_ReadLine( sO2Percentage );

    cgc_printf( "Pressure In (psi): " );
    cgc_ReadLine( sPressureIn );

    cgc_printf( "Pressure Out (psi): " );
    cgc_ReadLine( sPressureOut );

    // Add new dive to dive table -- parse data formats
    cgc_Dive *pNewDive = new cgc_Dive();

    cgc_Date oDiveDate;
    oDiveDate.cgc_SetDate( sDate );

    cgc_Time oDiveTime;
    oDiveTime.cgc_SetTime( sTime );

    pNewDive->cgc_SetDate( oDiveDate );
    pNewDive->cgc_SetTime( oDiveTime );
    pNewDive->cgc_SetDiveSite( sDiveSite );
    pNewDive->cgc_SetLocation( sLocation );
    pNewDive->cgc_SetMaxDepth( cgc_StringToInt( sMaxDepth ) );
    pNewDive->cgc_SetAvgDepth( cgc_StringToInt( sAvgDepth ) );
    pNewDive->cgc_SetDuration( cgc_StringToInt( sDiveDuration ) );
    pNewDive->cgc_SetO2Percentage( cgc_StringToInt( sO2Percentage ) );
    pNewDive->cgc_SetStartPressure( cgc_StringToInt( sPressureIn ) );
    pNewDive->cgc_SetEndPressure( cgc_StringToInt( sPressureOut ) );

    m_diveQueue.cgc_AddLast( pNewDive );

    return (false);
}

bool cgc_CommandHandler::cgc_CmdDownloadDiveData( void )
{
    // Get download data (cgc_timeval and depth as uint32 values)
    cgc_printf( "\n" );

    cgc_Dive *pNewDive = new cgc_Dive();

    // Begin reading samples
    bool bDone = false;
    cgc_uint32_t cgc_timeval, depth;

    // Always get first sample
    cgc_ReadUint32( cgc_timeval );
    cgc_ReadUint32( depth );
    pNewDive->cgc_AddDiveBin( cgc_timeval, depth );

    // Get remaining samples (exit on cgc_timeval of 0)
    for (;;)
    {
        cgc_ReadUint32( cgc_timeval );

        if ( cgc_timeval == 0 )
            break;

        cgc_ReadUint32( depth );
        pNewDive->cgc_AddDiveBin( cgc_timeval, depth );
    }

    // Collect information about the dives
    cgc_uint32_t binIndex;

    cgc_uint32_t maxDepth = 0;
    cgc_uint32_t depthSum = 0;
    for ( binIndex = 0; binIndex < pNewDive->cgc_GetBinCount(); binIndex++ )
    {
        if ( pNewDive->cgc_GetDiveBin( binIndex ).cgc_GetDepth() > maxDepth )
            maxDepth = pNewDive->cgc_GetDiveBin( binIndex ).cgc_GetDepth();

        depthSum += pNewDive->cgc_GetDiveBin( binIndex ).cgc_GetDepth();
    }

    cgc_uint32_t depthAvg = (depthSum / pNewDive->cgc_GetBinCount());

    // Get start time
    cgc_uint32_t startTime = pNewDive->cgc_GetDiveBin( 0 ).cgc_GetTimeVal();
    cgc_uint32_t endTime = pNewDive->cgc_GetDiveBin( pNewDive->cgc_GetBinCount()-1 ).cgc_GetTimeVal();

    cgc_uint32_t diveDurationInSeconds = endTime - startTime;
    cgc_uint32_t diveDurationInMinutes = (diveDurationInSeconds / 60);

    cgc_Date oDate;
    cgc_Time oTime;

    oDate.cgc_FromUnixTimeval( startTime );
    oTime.cgc_FromUnixTimeval( startTime );

    // Start collecting the rest of the dive information
    cgc_String sDiveSite;
    cgc_String sDate;
    cgc_String sTime;
    cgc_String sLocation;
    cgc_String sMaxDepth;
    cgc_String sAvgDepth;
    cgc_String sDiveDuration;
    cgc_String sO2Percentage;
    cgc_String sPressureIn;
    cgc_String sPressureOut;

    // Capture dive information
    cgc_printf( "cgc_Dive Site: " );
    cgc_ReadLine( sDiveSite );
    sDiveSite.cgc_Trim( 25 );

    cgc_printf( "cgc_Date (@s): ", oDate.cgc_GetString().cgc_c_str() );
    cgc_ReadLine( sDate );
    sDate.cgc_Trim( 10 );

    cgc_printf( "cgc_Time (@s): ", oTime.cgc_GetString().cgc_c_str() );
    cgc_ReadLine( sTime );
    sTime.cgc_Trim( 8 );

    cgc_printf( "Location (area/city): " );
    cgc_ReadLine( sLocation );
    sLocation.cgc_Trim( 25 );

    cgc_printf( "Max Depth in ft (@d): ", maxDepth );
    cgc_ReadLine( sMaxDepth );

    cgc_printf( "Avg Depth in ft (@d): ", depthAvg );
    cgc_ReadLine( sAvgDepth );

    cgc_printf( "cgc_Dive Duration (mins) (@d): ", diveDurationInMinutes );
    cgc_ReadLine( sDiveDuration );

    cgc_printf( "O2 Percentage: " );
    cgc_ReadLine( sO2Percentage );

    cgc_printf( "Pressure In (psi): " );
    cgc_ReadLine( sPressureIn );

    cgc_printf( "Pressure Out (psi): " );
    cgc_ReadLine( sPressureOut );

    // Update fields as necessary
    if ( sDate != "" )
        oDate.cgc_SetDate( sDate );

    if ( sTime != "" )
        oTime.cgc_SetTime( sTime );

    if ( sMaxDepth != "" )
        maxDepth = cgc_StringToInt( sMaxDepth );

    if ( sAvgDepth != "" )
        depthAvg = cgc_StringToInt( sAvgDepth );

    if ( sDiveDuration != "" )
        diveDurationInMinutes = cgc_StringToInt( sDiveDuration );

    // Add data
    pNewDive->cgc_SetDate( oDate );
    pNewDive->cgc_SetTime( oTime );
    pNewDive->cgc_SetDiveSite( sDiveSite );
    pNewDive->cgc_SetLocation( sLocation );
    pNewDive->cgc_SetMaxDepth( maxDepth );
    pNewDive->cgc_SetAvgDepth( depthAvg );
    pNewDive->cgc_SetDuration( diveDurationInMinutes );
    pNewDive->cgc_SetO2Percentage( cgc_StringToInt( sO2Percentage ) );
    pNewDive->cgc_SetStartPressure( cgc_StringToInt( sPressureIn ) );
    pNewDive->cgc_SetEndPressure( cgc_StringToInt( sPressureOut ) );

    m_diveQueue.cgc_AddLast( pNewDive );

    return (false);
}

bool cgc_CommandHandler::cgc_CmdEditDives( void )
{
    if ( m_diveQueue.cgc_GetCount() == 0 )
    {
        cgc_printf( "\n\nDive Log is cgc_empty\n" );
        return (false);
    }

    cgc_PrintDiveList();

    cgc_printf( "\nEnter cgc_Dive # to edit: " );

    cgc_String sDiveNumber;

    cgc_ReadLine( sDiveNumber );

    cgc_uint32_t diveNumber = cgc_StringToInt( sDiveNumber );

    if ( diveNumber == 0 || diveNumber > m_diveQueue.cgc_GetCount() )
    {
        cgc_printf( "Invalid dive number entered\n" );
        return (false);
    }

    cgc_Dive *pCur;
    cgc_uint32_t curPos = 1;
    for ( pCur = (cgc_Dive *)m_diveQueue.cgc_GetFirst(); pCur; pCur = (cgc_Dive *)pCur->cgc_GetNext() )
    {
        if ( diveNumber == curPos )
            break;

        curPos++;
    }

    if ( pCur == NULL )
    {
        cgc_printf( "Invalid dive number entered\n" );
        return (false);
    }

    cgc_String sDiveSite;
    cgc_String sDate;
    cgc_String sTime;
    cgc_String sLocation;
    cgc_String sMaxDepth;
    cgc_String sAvgDepth;
    cgc_String sDiveDuration;
    cgc_String sO2Percentage;
    cgc_String sPressureIn;
    cgc_String sPressureOut;

    // Capture dive information
    cgc_printf( "Editing dive number @d\n", diveNumber );

    cgc_printf( "cgc_Dive Site (@s): ", pCur->cgc_GetDiveSite().cgc_c_str() );
    cgc_ReadLine( sDiveSite );
    sDiveSite.cgc_Trim( 25 );

    cgc_printf( "cgc_Date (@s): ", pCur->cgc_GetDate().cgc_GetString().cgc_c_str() );
    cgc_ReadLine( sDate );
    sDate.cgc_Trim( 10 );

    cgc_printf( "cgc_Time (@s): ", pCur->cgc_GetTime().cgc_GetString().cgc_c_str() );
    cgc_ReadLine( sTime );
    sTime.cgc_Trim( 8 );

    cgc_printf( "Location (area/city) (@s): ", pCur->cgc_GetLocation().cgc_c_str() );
    cgc_ReadLine( sLocation );
    sLocation.cgc_Trim( 25 );

    cgc_printf( "Max Depth in ft (@d): ", pCur->cgc_GetMaxDepth() );
    cgc_ReadLine( sMaxDepth );

    cgc_printf( "Avg Depth in ft (@d): ", pCur->cgc_GetAvgDepth() );
    cgc_ReadLine( sAvgDepth );

    cgc_printf( "cgc_Dive Duration (mins) (@d): ", pCur->cgc_GetDuration() );
    cgc_ReadLine( sDiveDuration );

    cgc_printf( "O2 Percentage (@d): ", pCur->cgc_GetO2Percentage() );
    cgc_ReadLine( sO2Percentage );

    cgc_printf( "Pressure In (psi) (@d): ", pCur->cgc_GetStartPressure() );
    cgc_ReadLine( sPressureIn );

    cgc_printf( "Pressure Out (psi) (@d): ", pCur->cgc_GetEndPressure() );
    cgc_ReadLine( sPressureOut );

    cgc_Date oDiveDate;
    oDiveDate.cgc_SetDate( sDate );

    cgc_Time oDiveTime;
    oDiveTime.cgc_SetTime( sTime );

    pCur->cgc_SetDate( oDiveDate );
    pCur->cgc_SetTime( oDiveTime );
    pCur->cgc_SetDiveSite( sDiveSite );
    pCur->cgc_SetLocation( sLocation );
    pCur->cgc_SetMaxDepth( cgc_StringToInt( sMaxDepth ) );
    pCur->cgc_SetAvgDepth( cgc_StringToInt( sAvgDepth ) );
    pCur->cgc_SetDuration( cgc_StringToInt( sDiveDuration ) );
    pCur->cgc_SetO2Percentage( cgc_StringToInt( sO2Percentage ) );
    pCur->cgc_SetStartPressure( cgc_StringToInt( sPressureIn ) );
    pCur->cgc_SetEndPressure( cgc_StringToInt( sPressureOut ) );

    return (false);
}

bool cgc_CommandHandler::cgc_CmdPrintDiveLogs( void )
{
    if ( m_diveQueue.cgc_GetCount() == 0 )
    {
        cgc_printf( "\n\nDive Log is cgc_empty\n" );
        return (false);
    }

    cgc_PrintDiveList();

    cgc_printf( "\nEnter cgc_Dive # to display: " );

    cgc_String sDiveNumber;

    cgc_ReadLine( sDiveNumber );

    cgc_uint32_t diveNumber = cgc_StringToInt( sDiveNumber );

    if ( diveNumber == 0 || diveNumber > m_diveQueue.cgc_GetCount() )
    {
        cgc_printf( "Invalid dive number entered\n" );
        return (false);
    }

    cgc_Dive *pCur;
    cgc_uint32_t curPos = 1;
    for ( pCur = (cgc_Dive *)m_diveQueue.cgc_GetFirst(); pCur; pCur = (cgc_Dive *)pCur->cgc_GetNext() )
    {
        if ( diveNumber == curPos )
            break;

        curPos++;
    }

    if ( pCur == NULL )
    {
        cgc_printf( "Invalid dive number entered\n" );
        return (false);
    }

    cgc_printf( "\n" );
    cgc_printf( "          cgc_Date: @s\n", pCur->cgc_GetDate().cgc_GetString().cgc_c_str() );
    cgc_printf( "          cgc_Time: @s\n", pCur->cgc_GetTime().cgc_GetString().cgc_c_str() );
    cgc_printf( "     cgc_Dive Site: @s\n", pCur->cgc_GetDiveSite().cgc_c_str() );
    cgc_printf( "      Location: @s\n", pCur->cgc_GetLocation().cgc_c_str() );
    cgc_printf( "     Max Depth: @d\n", pCur->cgc_GetMaxDepth() );
    cgc_printf( "     Avg Depth: @d\n", pCur->cgc_GetAvgDepth() );
    cgc_printf( "      Duration: @d\n", pCur->cgc_GetDuration() );
    cgc_printf( "    O2 Percent: @d\n", pCur->cgc_GetO2Percentage() );
    cgc_printf( "Start Pressure: @d\n", pCur->cgc_GetStartPressure() );
    cgc_printf( "  End Pressure: @d\n", pCur->cgc_GetEndPressure() );
    cgc_printf( "     Bin Count: @d\n", pCur->cgc_GetBinCount() );
    cgc_printf( "\n" );

    return (false);
}

bool cgc_CommandHandler::cgc_CmdRemoveDives( void )
{
    if ( m_diveQueue.cgc_GetCount() == 0 )
    {
        cgc_printf( "\n\nDive Log is cgc_empty\n" );
        return (false);
    }

    cgc_PrintDiveList();

    cgc_printf( "\nEnter cgc_Dive # to delete or blank to abort: " );

    cgc_String sDiveNumber;

    // Read input
    cgc_ReadLine( sDiveNumber );

    // Check for abort
    if ( sDiveNumber == "" )
    {
        // abort
        return (false);
    }

    cgc_uint32_t diveNumber = cgc_StringToInt( sDiveNumber );

    if ( diveNumber == 0 || diveNumber > m_diveQueue.cgc_GetCount() )
    {
        cgc_printf( "Invalid dive number entered\n" );
        return (false);
    }

    cgc_Dive *pCur;
    cgc_uint32_t curPos = 1;
    for ( pCur = (cgc_Dive *)m_diveQueue.cgc_GetFirst(); pCur; pCur = (cgc_Dive *)pCur->cgc_GetNext() )
    {
        if ( diveNumber == curPos )
            break;

        curPos++;
    }

    if ( pCur == NULL )
    {
        cgc_printf( "Invalid dive number entered\n" );
        return (false);
    }

    // Remove the item from the dive queue
    m_diveQueue.cgc_RemoveItem( pCur );

    // Delete it
    delete pCur;

    return (false);
}

bool cgc_CommandHandler::cgc_CmdDiverStatistics( void )
{
    if ( m_diveQueue.cgc_GetCount() == 0 )
    {
        cgc_printf( "\n\nNo dives are logged\n" );
        return (false);
    }

    cgc_uint32_t depthTotal = 0;
    cgc_uint32_t lengthTotal = 0;

    cgc_Dive *pCur;

    for ( pCur = (cgc_Dive *)m_diveQueue.cgc_GetFirst(); pCur; pCur = (cgc_Dive *)pCur->cgc_GetNext() )
    {
        depthTotal += pCur->cgc_GetMaxDepth();
        lengthTotal += pCur->cgc_GetDuration();
    }

    cgc_printf( "\n\n" );
    cgc_printf( "Dives logged: @d\n", m_diveQueue.cgc_GetCount() );
    cgc_printf( "Average Max Depth: @d\n", (depthTotal / m_diveQueue.cgc_GetCount()) );
    cgc_printf( "Average cgc_Dive Length: @d\n", (lengthTotal / m_diveQueue.cgc_GetCount()) );

    return (false);
}

bool cgc_CommandHandler::cgc_CmdExitApplication( void )
{
    // Return true to exit application
    return (true);
}

void cgc_CommandHandler::cgc_PrintDiverInfo( void )
{
    cgc_printf( "\n" );
    cgc_printf( "     Name: @s @s\n", m_oDiver.cgc_GetFirstName().cgc_c_str(), m_oDiver.cgc_GetLastName().cgc_c_str() );
    cgc_printf( "  Address: @s\n", m_oDiver.cgc_GetStreet().cgc_c_str() );
    cgc_printf( "     City: @s\n", m_oDiver.cgc_GetCity().cgc_c_str() );
    cgc_printf( "    State: @s\n", m_oDiver.cgc_GetState().cgc_c_str() );
    cgc_printf( " Zip Code: @s\n", m_oDiver.cgc_GetZipCode().cgc_c_str() );
    cgc_printf( "    Phone: @s\n", m_oDiver.cgc_GetPhoneNumber().cgc_c_str() );
    cgc_printf( " PADI Num: @s\n", m_oDiver.cgc_GetPadiNumber().cgc_c_str() );
    cgc_printf( "Cert cgc_Date: @s\n", m_oDiver.cgc_GetPadiDate().cgc_c_str() );
    cgc_printf( "\n" );
}

void cgc_CommandHandler::cgc_PrintCommandTable( cgc_tCommandFunction *pCommandTable )
{
    cgc_printf( "\n" );
    for ( cgc_uint32_t i = 0; pCommandTable[i].pCmdFunc != NULL; i++ )
        cgc_printf( "@s - @s\n", pCommandTable[i].szCommand, pCommandTable[i].szDescription );

    cgc_printf( ":" );
}

void cgc_CommandHandler::cgc_ParserLoop( void )
{
    cgc_tCommandFunction oCmdTable[] = {
        { "C", "Change Diver Info", &cgc_CommandHandler::cgc_CmdChangeDiverInfo },
        { "L", "Log a New cgc_Dive", &cgc_CommandHandler::cgc_CmdLogNewDive },
        { "D", "Download cgc_Dive Data", &cgc_CommandHandler::cgc_CmdDownloadDiveData },
        { "E", "Edit Dives", &cgc_CommandHandler::cgc_CmdEditDives },
        { "P", "Print cgc_Dive Logs", &cgc_CommandHandler::cgc_CmdPrintDiveLogs },
        { "R", "Remove Dives", &cgc_CommandHandler::cgc_CmdRemoveDives },
        { "S", "Diver Statistics", &cgc_CommandHandler::cgc_CmdDiverStatistics },
        { "X", "Exit Application", &cgc_CommandHandler::cgc_CmdExitApplication },
        { "", "", NULL }
    };

    bool bDone = false;
    while ( !bDone )
    {
        // Print table
        cgc_PrintCommandTable( oCmdTable );

        // Read a line
        cgc_String inputLine;
        cgc_ReadLine( inputLine );

        // Parse command
        for ( cgc_uint32_t i = 0; oCmdTable[i].pCmdFunc != NULL; i++ )
        {
            if ( inputLine.cgc_lower() == cgc_String( oCmdTable[i].szCommand ).cgc_lower() )
            {
                if ( (this->*(oCmdTable[i].pCmdFunc))() )
                    bDone = true;

                break;
            }
        }
    }
}

void cgc_CommandHandler::cgc_PrintDiveList( void )
{
    cgc_Dive *pCur;
    cgc_printf( "\n\n" );
    cgc_printf( "cgc_Dive# cgc_Date       cgc_Time     cgc_Dive Site                 Location                 \n" );

    cgc_uint32_t curPos = 1;
    for ( pCur = (cgc_Dive *)m_diveQueue.cgc_GetFirst(); pCur; pCur = (cgc_Dive *)pCur->cgc_GetNext() )
    {
        cgc_printf( "@4d: @-10s @-8s @-25s @-25s\n", curPos, pCur->cgc_GetDate().cgc_GetString().cgc_c_str(),
                pCur->cgc_GetTime().cgc_GetString().cgc_c_str(), pCur->cgc_GetDiveSite().cgc_c_str(), pCur->cgc_GetLocation().cgc_c_str() );
        curPos++;
    }
}
