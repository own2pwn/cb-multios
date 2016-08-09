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
extern "C"
{
#include <libcgc.h>
#include <stdlib.h>
#include <stdint.h>
}

#include "packagetracker.h"
#include "comms.h"

#ifdef NULL
#undef NULL
#define NULL (0)
#endif

cgc_CPackageTracker::cgc_CPackageTracker( void *pMagicPage )
	: m_pMagicPage( pMagicPage ), m_bDeviceSleep( false ), m_bGPSOn( false ), m_bInternalError( false ), m_internalErrorCode( 0 ), m_batteryLevel( 0 ), m_geofencePointCount( 0 )
{

}

cgc_CPackageTracker::~cgc_CPackageTracker( )
{

}

void cgc_CPackageTracker::cgc_SeedSimulation( cgc_CPRNG *pRNG )
{
	m_deviceID = pRNG->cgc_GetRandomU32();

	m_batteryLevel = pRNG->cgc_GetRandomRange( 10, 50 );

	cgc_CGPSCoordinates oCoords( pRNG->cgc_GetRandomRange( 30, 160 ), pRNG->cgc_GetRandomRange( 0, 59 ), pRNG->cgc_GetRandomRange( 0, 59 ), pRNG->cgc_GetRandomRange( 0, 359 ), pRNG->cgc_GetRandomRange( 0, 59 ), pRNG->cgc_GetRandomRange( 0, 59 ) );

	m_oGPS.cgc_SetCoordinates( oCoords );
}

void cgc_CPackageTracker::cgc_Run( void )
{
	typedef struct cgc_MESSAGE_HEADER_STRUCT
	{
		cgc_uint8_t cmdNum;
		cgc_uint16_t cmdLength;
	} __attribute__((packed)) cgc_tMessageHeader;

	typedef struct cgc_MESSAGE_FOOTER_STRUCT
	{
		cgc_uint8_t checksum;
	} __attribute__((packed)) cgc_tMessageFooter;

	bool bDone = false;
		
	cgc_uint8_t *pCommandData = new cgc_uint8_t[ 65536 ];

	while ( !bDone )
	{
		cgc_tMessageHeader oMessageHeader;
		cgc_tMessageFooter oMessageFooter;
	
		cgc_uint8_t checksumData = 0;

		// Read command header
		if ( cgc_CommReadBytes( (cgc_uint8_t*)&oMessageHeader, sizeof(oMessageHeader) ) != sizeof(oMessageHeader) )
			break;

		checksumData = cgc_UpdateChecksum( (cgc_uint8_t*)&oMessageHeader, sizeof(oMessageHeader), checksumData );

		// Read command data
		cgc_uint32_t cmdLen = oMessageHeader.cmdLength;

		if ( cgc_CommReadBytes( pCommandData, oMessageHeader.cmdLength ) != oMessageHeader.cmdLength )
			break;

		checksumData = cgc_UpdateChecksum( (cgc_uint8_t*)pCommandData, cmdLen, checksumData );

		// Read footer
		if ( cgc_CommReadBytes( (cgc_uint8_t*)&oMessageFooter, sizeof(oMessageFooter) ) != sizeof(oMessageFooter) )
			break;

		// Check checksum
		if ( checksumData != oMessageFooter.checksum )
		{
			cgc_SendResponse( oMessageHeader.cmdNum, COMMAND_RESPONSE_BAD_CHECKSUM, NULL, 0 );
			continue;
		}
		
		if ( m_bDeviceSleep && oMessageHeader.cmdNum != COMMAND_WAKEUP && oMessageHeader.cmdNum != COMMAND_SHUTDOWN )
		{
			cgc_SendResponse( oMessageHeader.cmdNum, COMMAND_RESPONSE_SLEEP, NULL, 0 );
			continue;		
		}

		switch ( oMessageHeader.cmdNum )
		{
		case COMMAND_INFO:
			cgc_DoInfo( pCommandData, cmdLen );
			break;

		case COMMAND_SHUTDOWN:
			cgc_DoShutdown( pCommandData, cmdLen );
			bDone = true;
			break;

		case COMMAND_DEBUG:
			cgc_DoDebug( pCommandData, cmdLen );
			break;

		case COMMAND_UPDATE_GEOFENCE:
			cgc_DoUpdateGeofence( pCommandData, cmdLen );
			break;

		case COMMAND_POSITION:
			cgc_DoPosition( pCommandData, cmdLen );
			break;

		case COMMAND_GPS_OFF:
			cgc_DoGPSOff( pCommandData, cmdLen );
			break;
	
		case COMMAND_GPS_ON:
			cgc_DoGPSOn( pCommandData, cmdLen );
			break;

		case COMMAND_SLEEP:
			cgc_DoSleep( pCommandData, cmdLen );
			break;

		case COMMAND_WAKEUP:
			cgc_DoWakeup( pCommandData, cmdLen );
			break;

		default:
			// Unknown command
			cgc_SendResponse( oMessageHeader.cmdNum, COMMAND_RESPONSE_INVALID, NULL, 0 );
			break;
		}

		// Check for unrecoverable error
		if ( cgc_HasError() )
			break;	
	}

	// Delete command buffer		
	delete pCommandData;
}

void cgc_CPackageTracker::cgc_SendResponse( cgc_uint8_t cmdNum, cgc_uint8_t cmdResponse, cgc_uint8_t *pResponseData, cgc_uint16_t responseLength )
{
	typedef struct cgc_RESPONSE_MESSAGE_HEADER
	{
		cgc_uint8_t cmdNum;
		cgc_uint8_t cmdResponse;
		cgc_uint16_t responseLen;
	} __attribute__((packed)) cgc_tResponseHeader;

	typedef struct cgc_RESPONSE_MESSAGE_FOOTER
	{
		cgc_uint8_t checksum;
	} __attribute__((packed)) cgc_tResponseFooter;

	cgc_tResponseHeader oResponseHeader;
	cgc_tResponseFooter oResponseFooter;

	oResponseHeader.cmdNum = cmdNum;
	oResponseHeader.cmdResponse = cmdResponse;
	oResponseHeader.responseLen = responseLength;

	oResponseFooter.checksum = 0;

	if ( cgc_CommSendBytes( (cgc_uint8_t*)&oResponseHeader, sizeof(oResponseHeader) ) != sizeof(oResponseHeader) )
	{
		cgc_SetError( INTERNAL_ERROR_COMMS );
		return;
	}

	// Update checksum
	oResponseFooter.checksum = cgc_UpdateChecksum( (cgc_uint8_t*)&oResponseHeader, sizeof(oResponseHeader), oResponseFooter.checksum );

	// Check for response data and send it
	if ( oResponseHeader.responseLen > 0 )
	{
		if ( !pResponseData )
		{
			cgc_SetError( INTERNAL_ERROR_BAD_DATA );
			return;
		}

		if ( cgc_CommSendBytes( (cgc_uint8_t*)pResponseData, responseLength ) != responseLength )
		{
			cgc_SetError( INTERNAL_ERROR_COMMS );
			return;
		}
	
		oResponseFooter.checksum = cgc_UpdateChecksum( (cgc_uint8_t*)pResponseData, responseLength, oResponseFooter.checksum );
	}

	// Send footer
	if ( cgc_CommSendBytes( (cgc_uint8_t*)&oResponseFooter, sizeof(oResponseFooter) ) != sizeof(oResponseFooter) )
	{
		cgc_SetError( INTERNAL_ERROR_COMMS );
		return;
	}
}

cgc_uint8_t cgc_CPackageTracker::cgc_UpdateChecksum( cgc_uint8_t *pData, cgc_uint32_t dataLen, cgc_uint8_t inputChecksum )
{
	for ( cgc_uint32_t i = 0; i < dataLen; i++ )
	{
		inputChecksum += pData[i];
	}	

	return (inputChecksum);
}

void cgc_CPackageTracker::cgc_DoInfo( cgc_uint8_t *pData, cgc_uint32_t dataLen )
{
	typedef struct cgc_INFO_RESPONSE_STRUCT
	{
		cgc_uint8_t deviceState;
		cgc_uint8_t batteryLevel;
		cgc_uint32_t deviceID;
	} __attribute__((packed)) cgc_tInfoResponse;

	cgc_tInfoResponse oResponse;

	oResponse.batteryLevel = cgc_GetBatteryLevel();
	oResponse.deviceID = cgc_GetDeviceID();

	// Set device state
	if ( m_bGPSOn )
	{
		if ( m_oGPS.cgc_IsLocked() )
			oResponse.deviceState = GPS_STATE_ON;
		else
			oResponse.deviceState = GPS_STATE_NOT_LOCKED;
	}
	else
		oResponse.deviceState = GPS_STATE_OFF;

	// Send INFO response	
	cgc_SendResponse( COMMAND_INFO, COMMAND_RESPONSE_OK, (cgc_uint8_t*)&oResponse, sizeof(oResponse) ); 	
}

void cgc_CPackageTracker::cgc_DoShutdown( cgc_uint8_t *pData, cgc_uint32_t dataLen )
{
	// Just shutdown
	cgc_SendResponse( COMMAND_SHUTDOWN, COMMAND_RESPONSE_OK, NULL, 0 );
}

void cgc_CPackageTracker::cgc_DoUpdateGeofence( cgc_uint8_t *pData, cgc_uint32_t dataLen )
{
	typedef struct cgc_UPDATE_GEOFENCE_STRUCT
	{
		cgc_uint8_t updateAction;
		cgc_uint8_t pointCount;
	} __attribute__((packed)) cgc_tUpdateGeofence;

	typedef struct cgc_GEOFENCE_POINT_DATA
	{
		cgc_uint32_t distance;
		cgc_uint8_t coordData[6];
	} __attribute__((packed)) cgc_tGeofencePointData;
		
	typedef struct cgc_GEOFENCE_RESPONSE_DATA
	{
		cgc_uint8_t responseStatus;
		cgc_uint8_t coordData[6];
		cgc_uint32_t distanceMeters;
	} __attribute__((packed)) cgc_tGeofenceResponseData;

	cgc_tGeofenceResponseData oResponse;

	cgc_tUpdateGeofence oUpdateData;

	if ( dataLen < sizeof(oUpdateData) )
	{
		cgc_SetError( INTERNAL_ERROR_COMMS );
		return;
	}

	cgc_uint32_t commandPos = 0;
	cgc_memcpy( &oUpdateData, pData+commandPos, sizeof(oUpdateData) );
	commandPos += sizeof(oUpdateData);

	switch ( oUpdateData.updateAction )
	{
	case GEOFENCE_UPDATE_NEW:
		// Add points
		if ( oUpdateData.pointCount > MAX_GEOFENCE_POINTS )
		{
			cgc_SendResponse( COMMAND_UPDATE_GEOFENCE, COMMAND_RESPONSE_MAX_POINTS, NULL, 0 );
			return;
		}

		// Reset points
		for ( cgc_uint8_t i = 0; i < oUpdateData.pointCount; i++ )
		{
			cgc_tGeofencePointData oPointData;

			if ( (dataLen - commandPos) < sizeof(oPointData) )
			{
				cgc_SetError( INTERNAL_ERROR_COMMS );	
				return;
			}

			cgc_memcpy( &oPointData, pData+commandPos, sizeof(oPointData) );
			commandPos += sizeof(oPointData);

			m_geofencePoints[i].cgc_SetDistance( oPointData.distance );
			m_geofencePoints[i].cgc_GetCoords().cgc_Deserialize( oPointData.coordData, 6 );
		}

		// Set point count
		m_geofencePointCount = oUpdateData.pointCount;

		break;

	case GEOFENCE_UPDATE_ADD:
		{
			// Add to existing points
			if ( oUpdateData.pointCount > MAX_GEOFENCE_POINTS )
			{
				cgc_SendResponse( COMMAND_UPDATE_GEOFENCE, COMMAND_RESPONSE_MAX_POINTS, NULL, 0 );
				return;
			}

#ifdef PATCHED_1
			// Catch vulnerability where we failed to check the addition of the new points to the GeofencePoint array
			if ( m_geofencePointCount + oUpdateData.pointCount > MAX_GEOFENCE_POINTS )
			{
				cgc_SendResponse( COMMAND_UPDATE_GEOFENCE, COMMAND_RESPONSE_MAX_POINTS, NULL, 0 );
				return;
			}
#endif

			// Update points
			cgc_uint32_t maxPointCounts = m_geofencePointCount + oUpdateData.pointCount;
			for ( cgc_uint32_t i = m_geofencePointCount; i < maxPointCounts; i++ )
			{
				cgc_tGeofencePointData oPointData;

				if ( (dataLen - commandPos) < sizeof(oPointData) )
				{
					cgc_SetError( INTERNAL_ERROR_COMMS );
					return;
				}

				cgc_memcpy( &oPointData, pData+commandPos, sizeof(oPointData) );
				commandPos += sizeof(oPointData);

				m_geofencePoints[i].cgc_SetDistance( oPointData.distance );
				m_geofencePoints[i].cgc_GetCoords().cgc_Deserialize( oPointData.coordData, 6 );
			}

			// Set point count
			m_geofencePointCount = maxPointCounts;
		}
		break;

	case GEOFENCE_UPDATE_CLEAR:
		m_geofencePointCount = 0;
		cgc_SendResponse( COMMAND_UPDATE_GEOFENCE, COMMAND_RESPONSE_OK, NULL, 0 );
		return;
		break;

	default:
		cgc_SendResponse( COMMAND_UPDATE_GEOFENCE, COMMAND_RESPONSE_INVALID, NULL, 0 );
		return;
		break;
	}	

	// Check Geo Fencing -- IF GPS is ACTIVE
	if ( m_bGPSOn )
	{
		cgc_CGPSCoordinates oMyCoords;

		m_oGPS.cgc_GetLastCoordinates( oMyCoords );


		for ( cgc_uint8_t i = 0; i < m_geofencePointCount; i++ )
		{
			// CHECK if they leave any of the geofenced points
			cgc_uint32_t distanceFromPoint = cgc_CGPS::cgc_GetDistanceMeters( oMyCoords, m_geofencePoints[i].cgc_GetCoords() );

			if ( distanceFromPoint > m_geofencePoints[i].cgc_GetDistance() )
			{
				// Error outside of geofence
				oResponse.responseStatus = GEOFENCE_RESPONSE_OUT_OF_BOUNDS;
				oResponse.distanceMeters = distanceFromPoint;
				
				m_geofencePoints[i].cgc_GetCoords().cgc_Serialize( oResponse.coordData, 6 );

				cgc_SendResponse( COMMAND_UPDATE_GEOFENCE, COMMAND_RESPONSE_OK, (cgc_uint8_t*)&oResponse, sizeof(oResponse) );
				return;	
			}			
		}

		// Not outside of bounds
		oResponse.distanceMeters = 0;
		oResponse.responseStatus = GEOFENCE_RESPONSE_IN_BOUNDS;

#ifdef PATCHED_2
		// Prevent an uninitialized variable from leaking data back
		cgc_memset( oResponse.coordData, 0, 6 );
#endif
			
		// Send response data	
		cgc_SendResponse( COMMAND_UPDATE_GEOFENCE, COMMAND_RESPONSE_OK, (cgc_uint8_t*)&oResponse, sizeof(oResponse) );
		return;	
	}
	else
	{
		cgc_SendResponse( COMMAND_UPDATE_GEOFENCE, COMMAND_RESPONSE_GPS_OFF, NULL, 0 );
	}
}

void cgc_CPackageTracker::cgc_DoDebug( cgc_uint8_t *pData, cgc_uint32_t dataLen )
{
	typedef struct cgc_DEBUG_COMMAND_STRUCT
	{
		cgc_uint64_t debugAuthToken;
		cgc_uint8_t debugCommand;
		cgc_uint8_t debugCommandLen;
	} __attribute__((packed)) cgc_tDebugCommand;


	cgc_tDebugCommand oDebugCommand;

	if ( dataLen < sizeof(oDebugCommand) )
	{
		cgc_SetError( INTERNAL_ERROR_COMMS );
		return;
	}

	cgc_uint64_t expectedAuthToken = *((cgc_uint64_t*)m_pMagicPage) ^ 0xDEB4370331337A7F;
	
	cgc_uint32_t commandPos = 0;	
	cgc_memcpy( &oDebugCommand, pData+commandPos, sizeof(oDebugCommand) );
	commandPos += sizeof(oDebugCommand);

	
	// Check inner length
	if ( oDebugCommand.debugCommandLen+sizeof(oDebugCommand) != dataLen )
	{
		cgc_SendResponse( COMMAND_DEBUG, COMMAND_RESPONSE_INVALID, NULL, 0 );
		return;
	}

	if ( oDebugCommand.debugAuthToken != expectedAuthToken )
	{
		cgc_SendResponse( COMMAND_DEBUG, COMMAND_RESPONSE_INVALID, NULL, 0 );
		return;
	}

	// Enter debug mode
	// Accept a few debug commands
	
	switch ( oDebugCommand.debugCommand )
	{
	case DEBUG_SET_BATTERY_LEVEL:
		if ( oDebugCommand.debugCommandLen != 1 )
		{
			cgc_SendResponse( COMMAND_DEBUG, COMMAND_RESPONSE_INVALID, NULL, 0 );
			return;
		}

		m_batteryLevel = *((cgc_uint8_t*)(pData+commandPos));
		break;

	case DEBUG_SET_COORDINATES:
		if ( oDebugCommand.debugCommandLen != 6 )
		{
			cgc_SendResponse( COMMAND_DEBUG, COMMAND_RESPONSE_INVALID, NULL, 0 );
			return;
		}

		{
			cgc_CGPSCoordinates oCoords;

			oCoords.cgc_Deserialize( (pData+commandPos), 6 );

			m_oGPS.cgc_SetCoordinates( oCoords );
		}
		break;

	case DEBUG_SET_DEVICE_ID:
		if ( oDebugCommand.debugCommandLen != 4 )
		{
			cgc_SendResponse( COMMAND_DEBUG, COMMAND_RESPONSE_INVALID, NULL, 0 );
			return;
		}

		m_deviceID = *((cgc_uint32_t*)(pData+commandPos));
		break;
	
	default:		
		cgc_SendResponse( COMMAND_DEBUG, COMMAND_RESPONSE_INVALID, NULL, 0 );
		return;
	}

	cgc_SendResponse( COMMAND_DEBUG, COMMAND_RESPONSE_OK, NULL, 0 );
}

void cgc_CPackageTracker::cgc_DoPosition( cgc_uint8_t *pData, cgc_uint32_t dataLen )
{
	typedef struct cgc_POSITION_RESPONSE_STRUCT
	{
		cgc_uint8_t gpsState;
		cgc_uint8_t positionData[6];
	} __attribute__((packed)) cgc_tPositionResponse;
	
	cgc_tPositionResponse oResponse;

	cgc_CGPSCoordinates oCoords;

	if ( !m_bGPSOn )
		oResponse.gpsState = GPS_STATE_OFF;
	else
	{
		if ( !m_oGPS.cgc_GetLastCoordinates( oCoords ) )
			oResponse.gpsState = GPS_STATE_NOT_LOCKED;
		else
		{
			oResponse.gpsState = GPS_STATE_ON;

			// cgc_Serialize coordinates
			oCoords.cgc_Serialize( oResponse.positionData, 6 );
		}	
	}

	cgc_SendResponse( COMMAND_POSITION, COMMAND_RESPONSE_OK, (cgc_uint8_t*)&oResponse, sizeof(oResponse) );
}

void cgc_CPackageTracker::cgc_DoGPSOff( cgc_uint8_t *pData, cgc_uint32_t dataLen )
{
	// Turn off GPS
	m_bGPSOn = false;
	
	cgc_SendResponse( COMMAND_GPS_OFF, COMMAND_RESPONSE_OK, NULL, 0 );	
}

void cgc_CPackageTracker::cgc_DoGPSOn( cgc_uint8_t *pData, cgc_uint32_t dataLen )
{
	// Turn on GPS
	m_bGPSOn = true;

	cgc_SendResponse( COMMAND_GPS_ON, COMMAND_RESPONSE_OK, NULL, 0 );
}

void cgc_CPackageTracker::cgc_DoSleep( cgc_uint8_t *pData, cgc_uint32_t dataLen )
{
	// Sleep
	m_bDeviceSleep = true;
	
	cgc_SendResponse( COMMAND_SLEEP, COMMAND_RESPONSE_OK, NULL, 0 );
}

void cgc_CPackageTracker::cgc_DoWakeup( cgc_uint8_t *pData, cgc_uint32_t dataLen )
{
	// Wake
	m_bDeviceSleep = false;

	cgc_SendResponse( COMMAND_WAKEUP, COMMAND_RESPONSE_OK, NULL, 0 );
}
