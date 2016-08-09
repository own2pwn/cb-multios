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
#ifndef __DATETIME_H__
#define __DATETIME_H__

#include <stdint.h>
#include <string.h>

// Forward declarations
class cgc_Date;
class cgc_Time;

class cgc_Date
{
public:
    cgc_Date() : m_day( 1 ), m_month( 1 ), m_year( 1900 ) { };
    ~cgc_Date() { };

    void cgc_SetDate( cgc_uint8_t day, cgc_uint8_t month, cgc_uint16_t year )
    {
        m_day = day;
        m_month = month;
        m_year = year;
    }

    bool cgc_SetDate( const char* pszInStr );
    bool cgc_SetDate( const cgc_String& instr );
    void cgc_FromUnixTimeval( cgc_uint32_t cgc_timeval );

    cgc_String cgc_GetString( void ) const;

private:
    cgc_uint8_t     m_day;
    cgc_uint8_t     m_month;
    cgc_uint16_t    m_year;
};

class cgc_Time
{
public:
    cgc_Time() : m_hour( 0 ), m_minute( 0 ), m_second( 0 ) { };
    cgc_Time( cgc_uint8_t hour, cgc_uint8_t minute, cgc_uint8_t second ) : m_hour( hour ), m_minute( minute ), m_second( second ) { };
    ~cgc_Time() { };

    void cgc_SetTime( cgc_uint8_t hour, cgc_uint8_t minute, cgc_uint8_t second )
    {
        m_hour = hour;
        m_minute = minute;
        m_second = second;
    }

    bool cgc_SetTime( const char *pszInStr );
    bool cgc_SetTime( const cgc_String& inStr );

    void cgc_FromUnixTimeval( cgc_uint32_t cgc_timeval );

    cgc_String cgc_GetString( void ) const;

private:
    cgc_uint8_t     m_hour;
    cgc_uint8_t     m_minute;
    cgc_uint8_t     m_second;
};

#endif // __DATETIME_H__
