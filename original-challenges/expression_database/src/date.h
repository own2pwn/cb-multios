/*

Author: Jason Williams <jdw@cromulence.com>

Copyright (c) 2014 Cromulence LLC

Permission is hereby granted, free of charge, to any person obtaining a copy
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
#ifndef __DATE_H__
#define __DATE_H__

#include <stdint.h>


typedef struct _DATE_TIME_STRUCT
{
    cgc_uint8_t hour;
    cgc_uint8_t minute;
    cgc_uint8_t second;

    cgc_uint8_t month;
    cgc_uint8_t day;
    cgc_uint8_t year;
} cgc_tDateTime;

cgc_int32_t cgc_date_compare( cgc_tDateTime date1, cgc_tDateTime date2 );
cgc_int32_t cgc_parse_date( const char *str, cgc_tDateTime *pDate );

#endif // __DATE_H__
