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
#ifndef __DIVE_H__
#define __DIVE_H__

#include <string.h>

#define DIVE_BIN_COUNT      (128)

class cgc_DiveBin
{
public:
    cgc_DiveBin() { };
    ~cgc_DiveBin() { };

    void cgc_SetTimeVal( cgc_uint32_t cgc_timeval ) { m_cgc_timeval = cgc_timeval; };
    void cgc_SetDepth( cgc_uint32_t depth ) { m_depth = depth; };

    cgc_uint32_t cgc_GetTimeVal( void ) const { return m_cgc_timeval; };
    cgc_uint32_t cgc_GetDepth( void ) const { return m_depth; };

private:
    cgc_uint32_t m_cgc_timeval;
    cgc_uint32_t m_depth;
};

class cgc_Dive : public cgc_DLItem
{
public:
    cgc_Dive() : cgc_DLItem(), m_diveBinCount( 0 ) { };
    ~cgc_Dive() { };

    void cgc_SetDiveSite( const cgc_String &sDiveSite ) { m_diveSite = sDiveSite; };
    void cgc_SetDate( const cgc_Date &oDate ) { m_date = oDate; };
    void cgc_SetTime( const cgc_Time &oTime  ) { m_time = oTime; };
    void cgc_SetLocation( const cgc_String &sLocation ) { m_location = sLocation; };
    void cgc_SetMaxDepth( cgc_uint32_t maxDepth ) { m_maxDepth = maxDepth; };
    void cgc_SetAvgDepth( cgc_uint32_t avgDepth ) { m_avgDepth = avgDepth; };
    void cgc_SetDuration( cgc_uint32_t diveDuration ) { m_diveDuration = diveDuration; };
    void cgc_SetO2Percentage( cgc_uint32_t o2Percentage ) { m_o2Percentage = o2Percentage; };
    void cgc_SetStartPressure( cgc_uint32_t startPressure ) { m_startPressure = startPressure; };
    void cgc_SetEndPressure( cgc_uint32_t endPressure ) { m_endPressure = endPressure; };

    const cgc_String& cgc_GetDiveSite( void ) const { return m_diveSite; };
    const cgc_Date& cgc_GetDate( void ) const { return m_date; };
    const cgc_Time& cgc_GetTime( void ) const { return m_time; };
    const cgc_String& cgc_GetLocation( void ) const { return m_location; };
    cgc_uint32_t cgc_GetMaxDepth( void ) const { return m_maxDepth; };
    cgc_uint32_t cgc_GetAvgDepth( void ) const { return m_avgDepth; };
    cgc_uint32_t cgc_GetDuration( void ) const { return m_diveDuration; };
    cgc_uint32_t cgc_GetO2Percentage( void ) const { return m_o2Percentage; };
    cgc_uint32_t cgc_GetStartPressure( void ) const { return m_startPressure; };
    cgc_uint32_t cgc_GetEndPressure( void ) const { return m_endPressure; };

    cgc_uint32_t cgc_GetBinCount( void ) const { return m_diveBinCount; };
    cgc_uint32_t cgc_AddDiveBin( cgc_uint32_t cgc_timeval, cgc_uint32_t depth );
    const cgc_DiveBin& cgc_GetDiveBin( cgc_uint32_t index ) const { return m_diveBins[index]; };

private:
    cgc_DiveBin m_diveBins[DIVE_BIN_COUNT];
    cgc_uint32_t m_diveBinCount;

    cgc_String m_diveSite;
    cgc_Date m_date;
    cgc_Time m_time;
    cgc_String m_location;
    cgc_uint32_t m_maxDepth;
    cgc_uint32_t m_avgDepth;
    cgc_uint32_t m_diveDuration;
    cgc_uint32_t m_o2Percentage;
    cgc_uint32_t m_startPressure;
    cgc_uint32_t m_endPressure;
};

#endif // __DIVE_H___
