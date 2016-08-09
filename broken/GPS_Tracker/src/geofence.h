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
#ifndef __GEOFENCE_H__
#define __GEOFENCE_H__

#include "gps.h"

class cgc_CGeofencePoint
{
public:
	cgc_CGeofencePoint( );
	cgc_CGeofencePoint( cgc_CGPSCoordinates &coords, cgc_uint32_t distance );
	~cgc_CGeofencePoint( );

	void cgc_SetDistance( cgc_uint32_t distance ) { m_distance = distance; };
	cgc_uint32_t cgc_GetDistance( void ) const { return m_distance; };
	cgc_CGPSCoordinates &cgc_GetCoords( void ) { return m_coords; };
	
private:
	cgc_CGPSCoordinates m_coords;
	cgc_uint32_t m_distance;
};

#endif // __GEOFENCE_H__
