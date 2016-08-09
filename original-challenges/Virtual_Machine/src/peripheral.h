/*

Author: Jason Williams <jdw@cromulence.com>

Copyright (c) 2015 Cromulence LLC

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
#ifndef __PERIPHERAL_H__
#define __PERIPHERAL_H__

#include <cutil_list.h>

class cgc_CPeripheral
{
public:
	cgc_CPeripheral( cgc_uint8_t peripheralID );
	virtual ~cgc_CPeripheral( );

	virtual bool cgc_Init( void ) = 0;
	virtual bool cgc_Read( cgc_uint8_t *pDest, cgc_uint16_t length ) = 0;
	virtual bool cgc_Write( cgc_uint8_t *pDest, cgc_uint16_t length ) = 0;

	cgc_uint8_t cgc_GetPeripheralID( void ) const { return m_peripheralID; };

private:
	cgc_uint8_t m_peripheralID;

public:
	CUtil::DLL_LINK( cgc_CPeripheral ) m_peripheralListLink;
};

class cgc_CSendDevice : public cgc_CPeripheral
{
public:
	cgc_CSendDevice( cgc_uint8_t peripheralID );
	~cgc_CSendDevice( );

	bool cgc_Init( void );
	bool cgc_Read( cgc_uint8_t *pDest, cgc_uint16_t length );
	bool cgc_Write( cgc_uint8_t *pDest, cgc_uint16_t length );

private:
};

class cgc_CReadDevice : public cgc_CPeripheral
{
public:
	cgc_CReadDevice( cgc_uint8_t peripheralID );
	~cgc_CReadDevice( );

	bool cgc_Init( void );
	bool cgc_Read( cgc_uint8_t *pDest, cgc_uint16_t length );
	bool cgc_Write( cgc_uint8_t *pDest, cgc_uint16_t length );

private:
};

#endif // __PERIPHERAL_H__
