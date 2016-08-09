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
#ifndef __PACKAGETRACKER_H__
#define __PACKAGETRACKER_H__

#include "workerprng.h"
#include "geofence.h"

#define MAX_ANALOG_CHANNELS	(4)
#define MAX_DIGITAL_OUTPUTS	(4)

#define MAX_GEOFENCE_POINTS	(20)

class cgc_CPackageTracker
{
public:
	cgc_CPackageTracker( void *pMagicData );
	~cgc_CPackageTracker( );

	void cgc_SetDeviceID( cgc_uint32_t deviceID ) { m_deviceID = deviceID; };
	cgc_uint32_t cgc_GetDeviceID( void ) const { return m_deviceID; };

	void cgc_SeedSimulation( cgc_CPRNG *pRNG );

	void cgc_Run( void );

public:
	static const cgc_uint8_t INTERNAL_ERROR_COMMS = 1;
	static const cgc_uint8_t INTERNAL_ERROR_BAD_DATA = 2;

	// Available commands
	static const cgc_uint8_t COMMAND_INFO = 0;
	static const cgc_uint8_t COMMAND_SHUTDOWN = 1;
	static const cgc_uint8_t COMMAND_DEBUG = 2;
	static const cgc_uint8_t COMMAND_UPDATE_GEOFENCE = 3;
	static const cgc_uint8_t COMMAND_POSITION = 4;
	static const cgc_uint8_t COMMAND_GPS_OFF = 5;
	static const cgc_uint8_t COMMAND_GPS_ON = 6;
	static const cgc_uint8_t COMMAND_SLEEP = 7;
	static const cgc_uint8_t COMMAND_WAKEUP = 8;

	// Available command responses
	static const cgc_uint8_t COMMAND_RESPONSE_OK = 0;
	static const cgc_uint8_t COMMAND_RESPONSE_INVALID = 1;
	static const cgc_uint8_t COMMAND_RESPONSE_BAD_CHECKSUM = 2;
	static const cgc_uint8_t COMMAND_RESPONSE_MAX_POINTS = 3;
	static const cgc_uint8_t COMMAND_RESPONSE_SLEEP = 4;
	static const cgc_uint8_t COMMAND_RESPONSE_GPS_OFF = 5;

	// GPS states
	static const cgc_uint8_t GPS_STATE_ON = 0;
	static const cgc_uint8_t GPS_STATE_OFF = 1;
	static const cgc_uint8_t GPS_STATE_NOT_LOCKED = 2;

	// GEO FENCE Update responses
	static const cgc_uint8_t GEOFENCE_UPDATE_NEW = 0;
	static const cgc_uint8_t GEOFENCE_UPDATE_ADD = 1;
	static const cgc_uint8_t GEOFENCE_UPDATE_CLEAR = 2;

	static const cgc_uint8_t GEOFENCE_RESPONSE_IN_BOUNDS = 0;
	static const cgc_uint8_t GEOFENCE_RESPONSE_OUT_OF_BOUNDS = 1;

	// Debug commands
	static const cgc_uint8_t DEBUG_SET_BATTERY_LEVEL = 0;
	static const cgc_uint8_t DEBUG_SET_COORDINATES = 1;
	static const cgc_uint8_t DEBUG_SET_DEVICE_ID = 2;

private:
	bool cgc_HasError( void ) { return m_bInternalError; };
	void cgc_SetError( cgc_uint8_t errorCode )
	{
		m_bInternalError = true;
		m_internalErrorCode = errorCode;
	}

	void cgc_SendResponse( cgc_uint8_t cmdNum, cgc_uint8_t cmdResponse, cgc_uint8_t *pResponseData, cgc_uint16_t responseLength );

	cgc_uint8_t cgc_UpdateChecksum( cgc_uint8_t *pData, cgc_uint32_t dataLen, cgc_uint8_t inputChecksum );

	void cgc_DoInfo( cgc_uint8_t *pData, cgc_uint32_t dataLen );
	void cgc_DoShutdown( cgc_uint8_t *pData, cgc_uint32_t dataLen );
	void cgc_DoUpdateGeofence( cgc_uint8_t *pData, cgc_uint32_t dataLen );
	void cgc_DoDebug( cgc_uint8_t *pData, cgc_uint32_t dataLen );
	void cgc_DoPosition( cgc_uint8_t *pData, cgc_uint32_t dataLen );
	void cgc_DoGPSOff( cgc_uint8_t *pData, cgc_uint32_t dataLen );
	void cgc_DoGPSOn( cgc_uint8_t *pData, cgc_uint32_t dataLen );	
	void cgc_DoSleep( cgc_uint8_t *pData, cgc_uint32_t dataLen );	
	void cgc_DoWakeup( cgc_uint8_t *pData, cgc_uint32_t dataLen );

	cgc_uint8_t cgc_GetBatteryLevel( void ) const { return m_batteryLevel; };

private:
	cgc_uint8_t m_geofencePointCount;
	cgc_CGeofencePoint m_geofencePoints[MAX_GEOFENCE_POINTS];
	
	bool m_bDeviceSleep;
	bool m_bGPSOn;
	
	bool m_bInternalError;
	cgc_uint8_t m_internalErrorCode;

	cgc_CGPS m_oGPS;

	cgc_uint8_t m_batteryLevel;	

	cgc_uint32_t m_deviceID;

	void *m_pMagicPage;
};

#endif // __PACKAGETRACKER_H__
