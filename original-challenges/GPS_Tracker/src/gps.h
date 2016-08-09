/*

Author: Jason Williams

Copyright (c) 2015 Cromulence LLC

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
#ifndef __GPS_H__
#define __GPS_H__

#include "workerprng.h"

// Longitude 0-360, latitude 0-180
class cgc_CGPSCoordinates
{
public:
	cgc_CGPSCoordinates( );
	cgc_CGPSCoordinates( cgc_uint16_t latDegrees, cgc_uint8_t latMinutes, cgc_uint8_t latSeconds, cgc_uint16_t longDegrees, cgc_uint8_t longMinutes, cgc_uint8_t longSeconds );
	~cgc_CGPSCoordinates( );

	cgc_uint16_t cgc_GetLatitudeDegrees( void ) const { return m_latDegrees; };
	cgc_uint8_t cgc_GetLatitudeMinutes( void ) const { return m_latMinutes; };
	cgc_uint8_t cgc_GetLatitudeSeconds( void ) const { return m_latSeconds; };

	cgc_uint16_t cgc_GetLongitudeDegrees( void ) const { return m_longDegrees; };
	cgc_uint8_t cgc_GetLongitudeMinutes( void ) const { return m_longMinutes; };
	cgc_uint8_t cgc_GetLongitudeSeconds( void ) const { return m_longSeconds; };

	cgc_uint32_t cgc_GetSerializedSize( void );

	cgc_uint32_t cgc_Serialize( cgc_uint8_t *pDest, cgc_uint32_t destLen );
	cgc_uint32_t cgc_Deserialize( cgc_uint8_t *pSource, cgc_uint32_t sourceLen );

	double cgc_GetLatRadians( void );
	double cgc_GetLongRadians( void );

private:
	cgc_uint16_t m_latDegrees;
	cgc_uint8_t m_latMinutes;
	cgc_uint8_t m_latSeconds;

	cgc_uint16_t m_longDegrees;
	cgc_uint8_t m_longMinutes;
	cgc_uint8_t m_longSeconds;
};

class cgc_CGPS
{
public:
	cgc_CGPS( );
	~cgc_CGPS( );

	bool cgc_SetCoordinates( cgc_CGPSCoordinates &newCoords );

	bool cgc_GetLastCoordinates( cgc_CGPSCoordinates &coords )
	{
		if ( !m_bLocked )
			return (false);

		coords = m_lastCoordinates;
		
		return (true);	
	}

	bool cgc_IsLocked( void ) const { return m_bLocked; };

	static cgc_uint32_t cgc_GetDistanceMeters( cgc_CGPSCoordinates &coord1, cgc_CGPSCoordinates &coord2 );

private:
	bool m_bLocked;
	cgc_CGPSCoordinates m_lastCoordinates;
};

#endif // __GPS_H__
