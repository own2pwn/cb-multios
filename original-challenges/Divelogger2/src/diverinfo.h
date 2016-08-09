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
#ifndef __DIVER_INFO_H__
#define __DIVER_INFO_H__

#include <string.h>

class cgc_DiverInfo
{
public:
    cgc_DiverInfo() { };
    ~cgc_DiverInfo() { };

    void cgc_SetFirstName( const cgc_String& sName ) { m_firstName = sName; };
    void cgc_SetLastName( const cgc_String& sName ) { m_lastName = sName; };
    void cgc_SetStreet( const cgc_String& sStreet ) { m_street = sStreet; };
    void cgc_SetCity( const cgc_String& sCity ) { m_city = sCity; };
    void cgc_SetState( const cgc_String& sState ) { m_state = sState; };
    void cgc_SetZipCode( const cgc_String& sZipCode ) { m_zipcode = sZipCode; };
    void cgc_SetPhoneNumber( const cgc_String& sPhoneNumber ) { m_phoneNumber = sPhoneNumber; };
    void cgc_SetPadiNumber( const cgc_String& sPadiNumber ) { m_padiNumber = sPadiNumber; };
    void cgc_SetPadiDate( const cgc_String& sPadiDate ) { m_padiDate = sPadiDate; };

    const cgc_String& cgc_GetFirstName( void ) const { return m_firstName; };
    const cgc_String& cgc_GetLastName( void ) const { return m_lastName; };
    const cgc_String& cgc_GetStreet( void ) const { return m_street; };
    const cgc_String& cgc_GetCity( void ) const { return m_city; };
    const cgc_String& cgc_GetState( void ) const { return m_state; };
    const cgc_String& cgc_GetZipCode( void ) const { return m_zipcode; };
    const cgc_String& cgc_GetPhoneNumber( void ) const { return m_phoneNumber; };
    const cgc_String& cgc_GetPadiNumber( void ) const { return m_padiNumber; };
    const cgc_String& cgc_GetPadiDate( void ) const { return m_padiDate; };

private:
    cgc_String m_firstName;
    cgc_String m_lastName;
    cgc_String m_street;
    cgc_String m_city;
    cgc_String m_state;
    cgc_String m_zipcode;
    cgc_String m_phoneNumber;
    cgc_String m_padiNumber;
    cgc_String m_padiDate;
};

#endif // __DIVER_INFO_H___
