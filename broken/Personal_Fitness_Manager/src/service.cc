/*

Author: James Nuttall <james@cromulence.com>

Copyright (c) 2016 Cromulence LLC

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

#include "common.h"
#include "sensor_manager.h"
#include "service.h"

#define SYNC_BYTE_1 0xc3
#define SYNC_BYTE_2 0x3c



// Format:
// 2b: sync -- 0xc3c3 (not saved in packet -- just read and discarded)
// 2b: type -- PacketType enum
// 2b: len of data (max is MAX_DATA_LEN in service.h)
// *b: data

typedef struct 
{
	cgc_uint16_t type;
	cgc_uint16_t data_len;
	cgc_uint8_t *data;
} cgc_InPacket;


void cgc_SendResponse ( cgc_uint16_t category, cgc_uint8_t error )
{
	/*
		2b sync: 0xc3 0x3c
		2b type: response? 0x33 0x00
		2b len: len of DATA 0x02 0x00
		DATA
		 1b: type of message this is responding to
		 1b: error code
	*/
#define RESP_LEN 9
	cgc_uint8_t buff[ RESP_LEN ];
	
	// sync
	buff[0] = 0xc3;
	buff[1] = 0x3c;

	// response type
	buff[2] = 0x33;
	buff[3] = 0x00;

	// data len
	buff[4] = 0x03;
	buff[5] = 0x00;

	// data
	cgc_memcpy ( ( cgc_uint8_t* )&buff[6], &category, sizeof( cgc_uint16_t ) );

	buff[8] = error;


	cgc_write( buff, RESP_LEN );
}

cgc_InPacket* cgc_ReadInput()
{
	cgc_InPacket *pkt = new cgc_InPacket();
	pkt->data = NULL;

	cgc_uint8_t sync[2];

	cgc_uint16_t error = 0;

	// Read and verify SYNC
	cgc_receive_bytes( ( cgc_uint8_t* )&sync[ 0 ], 1 );

	if ( sync[ 0 ] != SYNC_BYTE_1 )
	{
		goto leave_fast;
	}
	cgc_receive_bytes( ( cgc_uint8_t* )&sync[1], 1 );
	if ( sync[ 1 ] != SYNC_BYTE_2 )
	{
		error = ERROR_BAD_VALUE;
		goto return_error;
	}

	// Read and verify type field
	cgc_receive_bytes( ( cgc_uint8_t* )&pkt->type, 2 );

	// Read and verify data length field
	cgc_receive_bytes( ( cgc_uint8_t* )&pkt->data_len, 2 );

	// verify len not too long
	if ( pkt->data_len > MAX_DATA_LEN )
	{
		error = ERROR_BAD_VALUE;
		goto return_error;
	}

	// Read data field
	pkt->data = new cgc_uint8_t[ pkt->data_len + 1 ];

	cgc_receive_bytes( pkt->data, pkt->data_len );

	pkt->data[ pkt->data_len ] = '\0';

	// Return type, len, data
	return pkt;

return_error:
	cgc_SendResponse( GENERAL, error );
	
leave_fast:
	if ( pkt )
	{
		if ( pkt->data )
			delete[] pkt->data;

		delete pkt;
	}
	return NULL;
}

//
// Used to verify python's magic numbers and our magic numbers match
// TEST ONLY
//
bool cgc_VerifyMagicMatch( cgc_uint8_t *remote, cgc_uint8_t *local )
{
	for ( int i = 0; i < 5; i++ )
	{
		for ( int j = 0; j < 5; j++ )
		{
			if ( remote[ i ] == local[ j ] )
				goto next_num;
		}
		goto fail;
next_num:
	int x = 3;
	}
	return true;

fail:
	return false;
}

//
//	read input, parse command, respond to command
//
int main( void )
{
	cgc_uint8_t *magic_page = ( cgc_uint8_t* )MAGIC_PAGE;
	
	// INIT VALUES HERE
	cgc_InPacket *pkt;
	cgc_SensorManager sensorManager;
	cgc_FitnessSensor::cgc_GenerateTypeValues();
	for ( int i = 0; i < MAX_SENSOR_VALUE; i++ )
	{
		if (cgc_FitnessSensor::m_sensorArray[ i ] == 0)
			cgc_printf( "fnd: $x ", cgc_FitnessSensor::m_sensorArray[ i ] );
	}

	sensorManager.cgc_SetCurrentUser( INVALID_USER );

	while ( 1 ) 
	{
		pkt = cgc_ReadInput();
		if ( pkt == NULL )
			continue;
		
		//
		// PARSE PACKET
		//
		switch ( pkt->type )
		{/*
			case TEST_VERIFY:
			{
				bool match = false;

				match = cgc_VerifyMagicMatch( pkt->data, cgc_FitnessSensor::m_sensorArray );
				#ifdef HUMAN_PRINT
				cgc_printf("py magic: ");
					for ( int i = 0; i < pkt->data_len; i++ )
					{
						cgc_printf( "$02x.", pkt->data[ i ] );
					}
					cgc_printf( "\n" );
					
					
					cgc_printf("my magic: ");
					for ( int i = 0; i < 5; i++ )
					{
						cgc_printf( "$02x.", cgc_FitnessSensor::m_sensorArray[ i ] );
					}
					cgc_printf( "\n" );
				#endif
				if ( match == false )
					cgc_SendResponse( TEST_VERIFY, ERROR_TEST_MAGIC_MISMATCH );
				else
					cgc_SendResponse( TEST_VERIFY, ERROR_TEST_MAGIC_MATCH );
				break;
			}*/
			case REGISTER_SENSOR:
			{
				// Register this fitness sensor for the current user
				
				// current user and list owned by sensor_manager
				if ( sensorManager.cgc_GetCurrentUser() == INVALID_USER )
				{
					cgc_SendResponse( REGISTER_SENSOR, ERROR_NO_USER );
					continue;
				}

				// Data: 
				// 2b: sensor type
				// 4b: sensor MAC
				const int MIN_SENSOR_LEN = sizeof( cgc_uint16_t ) + sizeof( cgc_uint32_t ); // 2b for ID, 4b for MAC
				if ( pkt->data_len < MIN_SENSOR_LEN )
				{
					// data field does not have enough data for this type of packet
					// discard and print error
					//cgc_printf( "sensor data too short\n" );
					cgc_SendResponse( REGISTER_SENSOR, ERROR_BAD_VALUE );
					continue;
				}

				cgc_uint16_t id = *( cgc_uint16_t* )&pkt->data[ 0 ];
				cgc_uint32_t mac = *( cgc_uint32_t* )&pkt->data[ sizeof( cgc_uint16_t ) ];

				
				// matching 'delete' is in cgc_RemoveSensor in sensor_manager.cc
				// MAC is verified in the sensor's constructor
				cgc_FitnessSensor *sensor = new cgc_FitnessSensor( id, mac, &pkt->data[6], pkt->data_len - 6 );
				sensor->cgc_SetUser( sensorManager.cgc_GetCurrentUser() );

				// read sensor information
				#ifndef TEST_VER
					if ( sensor->cgc_IsInvalid() == true )
					{
						// bad sensor MAC, discard, return error
						cgc_SendResponse( REGISTER_SENSOR, ERROR_BAD_SENSOR_MAC );
						delete sensor;
						break;
					}
				#endif
				int resp = sensorManager.cgc_AddSensor( sensor );

				// Add this sensor to sensor list
				if ( resp == 1 )
				{
					cgc_SendResponse( REGISTER_SENSOR, ERROR_DUPLICATE_SENSOR );
					delete sensor;
					break;
				}
				else if ( resp == 2 )
				{
					cgc_SendResponse( REGISTER_SENSOR, ERROR_FULL_SENSORS );
					delete sensor;
					break;
				}

				cgc_SendResponse( REGISTER_SENSOR, NO_ERROR );
				break;
			}
			case REGISTER_USER:
			{
				cgc_uint8_t field_count = pkt->data[ 0 ];

				cgc_uint16_t expected_size = ( field_count * 3 ) + 3;

				if ( pkt->data_len != expected_size )
				{
					cgc_SendResponse( REGISTER_USER, ERROR_BAD_VALUE );
					continue;
				}

				cgc_uint16_t new_id = *( cgc_uint16_t* )&pkt->data[ 1 ];

				cgc_User *user = new cgc_User( new_id );

				int d_offset = 3;

				// read all field values
				for ( int i = 0; i < field_count-1; i++, d_offset += 3 )
				{
					switch ( pkt->data[ d_offset ] )
					{
						case WEIGHT:
						{
							user->cgc_SetWeight( *( cgc_uint16_t* )&pkt->data[ d_offset + 1 ] );
							break;
						}
						case HEIGHT:
						{
							user->cgc_SetHeight( *( cgc_uint16_t* )&pkt->data[ d_offset + 1 ] );
							break;
						}

						default:
						{
							cgc_SendResponse( REGISTER_USER, ERROR_BAD_VALUE );
							break;
						}
					}
				}

				// Add user
				int resp = sensorManager.cgc_AddUser( user );

				if ( resp == RET_USER_FULL )
					cgc_SendResponse( REGISTER_USER, ERROR_USER_FULL );
				else if ( resp == RET_USER_REDUNDANT )
					cgc_SendResponse( REGISTER_USER, ERROR_DUPLICATE_USER );
				else
					cgc_SendResponse( REGISTER_USER, NO_ERROR );

				break;
			}
			case CHANGE_CURRENT_USER:
			{
				// given a user ID,
				// look for the id and change to it

				if ( pkt->data_len != sizeof( cgc_uint16_t ) )
				{
					cgc_SendResponse( CHANGE_CURRENT_USER, ERROR_BAD_VALUE );
					break;
				}

				cgc_uint16_t new_id = *( cgc_uint16_t* )&pkt->data[ 0 ];

				// This will add this user if needed
				cgc_uint8_t ret = sensorManager.cgc_SetCurrentUser( new_id );

				if ( ret != 0 )
				{
					cgc_SendResponse( CHANGE_CURRENT_USER, ERROR_REG_THIS_USER );
					break;
				}

				cgc_SendResponse( CHANGE_CURRENT_USER, NO_ERROR );
				break;
			}
			case INPUT_SENSOR_DATA:
			{	
				cgc_uint16_t sensor_id = *( cgc_uint16_t* )&pkt->data[ 0 ];

				if ( sensorManager.cgc_VerifySensor( sensor_id ) == false )
				{
					// this sensor isn't registered for the current user
					cgc_SendResponse( INPUT_SENSOR_DATA, ERROR_SENSOR_NOT_REGISTERED );
					break;
				}
				// we have the sensor_id. we need the type of sensor this is
				cgc_FitnessSensor* sensor = sensorManager.cgc_GetSensor( sensor_id );

				if ( sensor == NULL )
				{
					cgc_SendResponse( INPUT_SENSOR_DATA, ERROR_SENSOR_NOT_REGISTERED );
					break;
				}

				int sensor_type = sensor->cgc_GetType();

				if ( sensor_type == BIKE )
				{
					// size of the sensor id + data
					if ( pkt->data_len != ( sizeof( cgc_uint16_t ) + sizeof( cgc_uint16_t ) ) )
					{
						// sensor ID missing
						cgc_SendResponse( INPUT_SENSOR_DATA, ERROR_BAD_VALUE );
						break;
					}

					cgc_uint16_t sensor_id = *( cgc_uint16_t* )&pkt->data[ 0 ];
					cgc_uint16_t bike_dist = *( cgc_uint16_t* )&pkt->data[ sizeof( cgc_uint16_t ) ];

					// use sensor_id to check if this ID is registred with this user
					

					cgc_uint16_t new_dist = cgc_FitnessSensor::cgc_HandleBikeSensor( bike_dist );

					// add this distance to this user's total distance traveled
					if ( !sensorManager.cgc_AddDistance( new_dist, sensorManager.cgc_GetCurrentUser() ) )
						cgc_SendResponse( INPUT_SENSOR_DATA, ERROR_BAD_VALUE );
					else						
						cgc_SendResponse( INPUT_SENSOR_DATA, NO_ERROR );
					break;
				}
				else if ( sensor_type == HEART )
				{
					if ( pkt->data_len != ( sizeof( cgc_uint16_t ) + sizeof( cgc_uint16_t ) ) )
					{
						// sensor ID missing
						cgc_SendResponse( INPUT_SENSOR_DATA, ERROR_BAD_VALUE );
						break;
					}

					cgc_uint16_t sensor_id = *( cgc_uint16_t* )&pkt->data[ 0 ];
					cgc_uint16_t beats = *( cgc_uint16_t* )&pkt->data[ sizeof( cgc_uint16_t ) ];

					// use sensor_id to check if this ID is registred with this user
					if ( sensorManager.cgc_VerifySensor( sensor_id ) == false )
					{
						// this sensor isn't registered for the current user
						cgc_SendResponse( INPUT_SENSOR_DATA, ERROR_SENSOR_NOT_REGISTERED );
						break;
					}

					cgc_uint16_t new_value = cgc_FitnessSensor::cgc_HandleHeartSensor( beats );

					// add this distance to this user's total distance traveled
					if ( !sensorManager.cgc_AddDistance( new_value, sensorManager.cgc_GetCurrentUser() ) )
						cgc_SendResponse( INPUT_SENSOR_DATA, ERROR_BAD_VALUE );
					else						
						cgc_SendResponse( INPUT_SENSOR_DATA, NO_ERROR );
					break;
				}
				else if ( sensor_type == SCALE )
				{
					if ( pkt->data_len != ( sizeof( cgc_uint16_t ) + sizeof( cgc_uint16_t ) ) )
					{
						// sensor ID missing
						cgc_SendResponse( INPUT_SENSOR_DATA, ERROR_BAD_VALUE );
						break;
					}

					cgc_uint16_t sensor_id = *( cgc_uint16_t* )&pkt->data[ 0 ];
					cgc_uint16_t weight = *( cgc_uint16_t* )&pkt->data[ sizeof( cgc_uint16_t ) ];

					// use sensor_id to check if this ID is registred with this user
					if ( sensorManager.cgc_VerifySensor( sensor_id ) == false )
					{
						// this sensor isn't registered for the current user
						cgc_SendResponse( INPUT_SENSOR_DATA, ERROR_SENSOR_NOT_REGISTERED );
						break;
					}

					cgc_uint16_t new_weight = cgc_FitnessSensor::cgc_HandleScaleSensor( weight );

					// add this distance to this user's total distance traveled
					if ( !sensorManager.cgc_AddDistance( new_weight, sensorManager.cgc_GetCurrentUser() ) )
						cgc_SendResponse( INPUT_SENSOR_DATA, ERROR_BAD_VALUE );
					else						
						cgc_SendResponse( INPUT_SENSOR_DATA, NO_ERROR );
					break;
				}
				else if ( sensor_type == STEP )
				{
					if ( pkt->data_len != ( sizeof( cgc_uint16_t ) + sizeof( cgc_uint16_t ) ) )
					{
						// sensor ID missing
						cgc_SendResponse( INPUT_SENSOR_DATA, ERROR_BAD_VALUE );
						break;
					}

					cgc_uint16_t sensor_id = *( cgc_uint16_t* )&pkt->data[ 0 ];
					cgc_uint16_t steps = *( cgc_uint16_t* )&pkt->data[ sizeof( cgc_uint16_t ) ];

					// use sensor_id to check if this ID is registred with this user
					if ( sensorManager.cgc_VerifySensor( sensor_id ) == false )
					{
						// this sensor isn't registered for the current user
						cgc_SendResponse( INPUT_SENSOR_DATA, ERROR_SENSOR_NOT_REGISTERED );
						break;
					}

					cgc_uint16_t new_dist = cgc_FitnessSensor::cgc_HandleStepSensor( steps );

					// add this distance to this user's total distance traveled
					if ( !sensorManager.cgc_AddDistance( new_dist, sensorManager.cgc_GetCurrentUser() ) )
					{
						cgc_SendResponse( INPUT_SENSOR_DATA, ERROR_BAD_VALUE );
					}
					else						
						cgc_SendResponse( INPUT_SENSOR_DATA, NO_ERROR );
					break;
				}
				else if ( sensor_type == RUN )
				{
					if ( pkt->data_len != ( sizeof( cgc_uint16_t ) + sizeof( cgc_uint16_t ) ) )
					{
						// sensor ID missing
						
						cgc_SendResponse( INPUT_SENSOR_DATA, ERROR_BAD_VALUE );
						break;
					}

					cgc_uint16_t sensor_id = *( cgc_uint16_t* )&pkt->data[ 0 ];
					cgc_uint16_t dist = *( cgc_uint16_t* )&pkt->data[ sizeof( cgc_uint16_t ) ];

					// use sensor_id to check if this ID is registred with this user
					if ( sensorManager.cgc_VerifySensor( sensor_id ) == false )
					{
						// this sensor isn't registered for the current user
						cgc_SendResponse( INPUT_SENSOR_DATA, ERROR_SENSOR_NOT_REGISTERED );
						break;
					}

					cgc_FitnessSensor::cgc_HandleRunSensor( dist );
					cgc_SendResponse( INPUT_SENSOR_DATA, NO_ERROR );
				}
				else
				{
					cgc_SendResponse( INPUT_SENSOR_DATA, ERROR_BAD_VALUE );
				}
				break;
			}
			case REMOVE_SENSOR:
			{
				if ( pkt->data_len != sizeof( cgc_uint16_t ) )
				{
					// sensor ID missing
					cgc_SendResponse( REMOVE_SENSOR, ERROR_BAD_VALUE );
					break;
				}

				cgc_uint16_t sensor_id = *( cgc_uint16_t* )&pkt->data[ 0 ];

				// Verify this sensor is in this user's list
				if ( sensorManager.cgc_VerifySensor( sensor_id ) == false )
				{
					// this sensor isn't registered for the current user

					cgc_SendResponse( REMOVE_SENSOR, ERROR_SENSOR_NOT_REGISTERED );
					break;
				}

				bool ret = sensorManager.cgc_RemoveSensor( sensor_id, sensorManager.cgc_GetCurrentUser() );

				if ( ret == false )
				{
					cgc_SendResponse( REMOVE_SENSOR, ERROR_BAD_VALUE );
					break;
				}

				cgc_SendResponse( REMOVE_SENSOR, NO_ERROR );
				break;
			}
			case REQUEST_SENSORS:
			{
				// respond with the ID and MAC of all sensors (not just for this user)
				cgc_uint16_t data_len = 0;

				cgc_uint8_t *data;

				data = sensorManager.cgc_ListSensors( data_len );

				cgc_uint8_t *buff = new cgc_uint8_t[ data_len + 6 ];
	
				// sync
				buff[0] = 0xc3;
				buff[1] = 0x3c;

				// response type
				buff[2] = 0x33;
				buff[3] = 0x00;

				// data len
				cgc_memcpy( &buff[4], &data_len, sizeof( cgc_uint16_t ) );

				// data
				cgc_memcpy ( ( cgc_uint8_t* )&buff[6], data, data_len );

				delete[] data;

				cgc_write( buff, data_len + 6 );

				delete[] buff;

				break;
			}
			case REQUEST_HW_IDS:
			{
				// return buffer of hardware ids
				// len, 5 IDs
				cgc_uint16_t data_len = 0;

				// each sensor is 2b ID, 4b mac: 6B
				cgc_uint8_t data[ 7 ];
				cgc_bzero( data, 7 );

				sensorManager.cgc_ListHwIds( data_len, data );

				cgc_uint8_t *buff = new cgc_uint8_t[ data_len + 6 ];
	
				// sync
				buff[0] = 0xc3;
				buff[1] = 0x3c;

				// response type
				buff[2] = 0x44;
				buff[3] = 0x77;

				// data len
				cgc_memcpy( &buff[4], &data_len, sizeof( cgc_uint16_t ) );

				// data
				cgc_memcpy ( ( cgc_uint8_t* )&buff[6], data, data_len );

				cgc_write( buff, data_len + 6 );

				break;
			}
			case END:
			{
				return 0;
			}
			default:
			{
				// error: bad command
				cgc_SendResponse( GENERAL, ERROR_BAD_VALUE );
			}
			
		}
	}

	return 0;
}