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

extern "C"
{
	#include <libcgc.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <prng.h>
}

#include "cutil_string.h"
#include "cutil_list.h"
#include "common.h"
#include "controller.h"
#include "sensor.h"
#include "service.h"
#include "comms.h"

//#define NO_ERROR 0

//#define CHECKSUM_NUM_BYTES 4
#define HEADER_NUM_BYTES 4


//
//	read input, parse command, respond to command
//
int main( void )
{
	cgc_uint8_t *magic_page = ( cgc_uint8_t* )MAGIC_PAGE;

	cgc_Controller ctlr;

	ctlr.cgc_GetMsgOut()->cgc_SetVersion(ctlr.cgc_GetVersion());

	while ( 1 ) 
	{
		ctlr.cgc_GetMsgIn()->cgc_ReadInput();

		// if given length is longer than the string, NO
		if ( ctlr.cgc_GetMsgIn()->cgc_GetLenValue() != ctlr.cgc_GetMsgIn()->cgc_GetMessage().cgc_GetLength()/2 - HEADER_NUM_BYTES)
		{
			cgc_printf("Error: length is too long\n");
			continue;
		}

		//
		// PARSE BODY
		//
		switch ( ctlr.cgc_GetMsgIn()->cgc_GetType() )
		{
			case POWER_ON_OFF:
			{
				int val = ctlr.cgc_GetMsgIn()->cgc_GetValue();
				
				ctlr.cgc_setPowerOn( ( val == 1 ) ? true : false );

				ctlr.cgc_GetMsgOut()->cgc_SetResponse( NO_ERROR );
				ctlr.cgc_GetMsgOut()->cgc_SendAsBasic();

				break;
			}
			case TEMP_SET:
			{
				int val = ctlr.cgc_GetMsgIn()->cgc_GetValue();
				
				if ( ctlr.cgc_setSetTemp( val ) == false )
				{
					// failed to set temp
					ctlr.cgc_GetMsgOut()->cgc_SetResponse( ERROR_TEMP_RANGE );
					ctlr.cgc_GetMsgOut()->cgc_SendAsBasic();
				}
				else
				{
					ctlr.cgc_GetMsgOut()->cgc_SetResponse( NO_ERROR );
					ctlr.cgc_GetMsgOut()->cgc_SendAsBasic();
				}

				break;
			}
			case SENSOR_ADD:
			{
				if ( ctlr.cgc_IsSensorInList( ctlr.cgc_GetMsgIn()->cgc_GetSensorId() ) )
				{
					ctlr.cgc_GetMsgOut()->cgc_SetResponse( ERROR_ID_INUSE );
					ctlr.cgc_GetMsgOut()->cgc_SendAsBasic();
					break;
				}

				if ( ctlr.cgc_GetSensorCount() >= MAX_SENSORS )
				{
					ctlr.cgc_GetMsgOut()->cgc_SetResponse( ERROR_SENSORS_FULL );
					ctlr.cgc_GetMsgOut()->cgc_SendAsBasic();
					break;
				}
  
  				cgc_uint16_t a = ctlr.cgc_GetMsgIn()->cgc_GetSensorId();
  				cgc_uint32_t b = ctlr.cgc_GetMsgIn()->cgc_GetSensorAddress();
  				cgc_uint32_t c = ctlr.cgc_GetMsgIn()->cgc_GetSensorCoefficient();

				ctlr.cgc_AddSensor( new cgc_Sensor( a, b, c, true ) );
				
				ctlr.cgc_GetMsgOut()->cgc_SetResponse( NO_ERROR );
				ctlr.cgc_GetMsgOut()->cgc_SendAsBasic();

				break;
			}
			case SENSOR_REMOVE:
			{
				int val = ctlr.cgc_GetMsgIn()->cgc_GetValue();

				if ( ctlr.cgc_IsSensorInList( val ) == false)
				{
					ctlr.cgc_GetMsgOut()->cgc_SetResponse( ERROR_BAD_ID );
					ctlr.cgc_GetMsgOut()->cgc_SendAsBasic();
					break;
				}
				ctlr.cgc_RemoveSensor( val );
				ctlr.cgc_GetMsgOut()->cgc_SetResponse( NO_ERROR );
				ctlr.cgc_GetMsgOut()->cgc_SendAsBasic();
				break;
			}
			case SENSOR_SMOKE:
			{
				if ( ctlr.cgc_GetMsgIn()->cgc_GetValue() )
					ctlr.cgc_EnableSmokeSensor();
				else
					ctlr.cgc_DisableSmokeSensor();

				ctlr.cgc_GetMsgOut()->cgc_SetResponse( NO_ERROR );
				ctlr.cgc_GetMsgOut()->cgc_SendAsBasic();
				break;
			}
			case PROGRAM_UPDATE:
			{
				// update our program with one sent by the CRS

				// use ctlr.m_controllerProgram from incoming message
				int num_program_steps = ctlr.cgc_GetMsgIn()->cgc_GetValue();

				if ( num_program_steps > MAX_PROGRAM_STEPS )
				{
					// Too many steps. Return error
					ctlr.cgc_GetMsgOut()->cgc_SetResponse( ERROR_BAD_PROGRAM );
					ctlr.cgc_GetMsgOut()->cgc_SendAsBasic();
					break;
				}

				// in a loop, read each program variables into ctlr.m_controllerProgram[i]

				int num_nibbles_to_start = 16; // 8 bytes
				int num_nibbles_for_each_program = 24; // 12 bytes


				// keep original copy in the backup
				// cgc_write all new values to the main program
				// clear backup when done

				ctlr.cgc_BackupProgram();
				ctlr.cgc_ClearProgram();

				int j = 0;
				for ( int i = 0; i < num_program_steps; i++ )
				{
					cgc_uint32_t a, b, c;
					int offset = num_nibbles_to_start + ( i * num_nibbles_for_each_program );

					ctlr.cgc_GetMsgIn()->cgc_GetProgramValues( offset, a, b, c );

					// verify values here
					if ( a == 0 ) 
					{
						if ( ctlr.cgc_IsSensorInList( b ) == false && b != 0 )
						{
							ctlr.cgc_GetMsgOut()->cgc_SetResponse( ERROR_NO_SENSOR );
							ctlr.cgc_GetMsgOut()->cgc_SendAsBasic();
							ctlr.cgc_RevertProgram();
							goto leave_case;
						}
					}
					else if ( a == 1 )
					{
						if ( b < ( 30 * 60 ) )
						{
							ctlr.cgc_GetMsgOut()->cgc_SetResponse( ERROR_TIME_VALUE );
							ctlr.cgc_GetMsgOut()->cgc_SendAsBasic();
							ctlr.cgc_RevertProgram();
							goto leave_case;
						}
					}
					else
					{
						ctlr.cgc_GetMsgOut()->cgc_SetResponse( ERROR_BAD_PROGRAM );
						ctlr.cgc_GetMsgOut()->cgc_SendAsBasic();
						ctlr.cgc_RevertProgram();
						goto leave_case;
					}

					// save program here
					ctlr.m_controllerProgram[ j ].cgc_setStepType( a );

					if ( a == 0 )
						ctlr.m_controllerProgram[ j ].cgc_setSensorId( b );
					else
						ctlr.m_controllerProgram[ j ].cgc_setDuration( b );

					if ( c < 175 || c > 350 )
					{
						ctlr.cgc_GetMsgOut()->cgc_SetResponse( ERROR_TEMP_RANGE );
						ctlr.cgc_GetMsgOut()->cgc_SendAsBasic();
						ctlr.cgc_RevertProgram();
						goto leave_case;
					}

					ctlr.m_controllerProgram[ j ].cgc_setSetTemp( c );

					j++;
				}
				
				ctlr.cgc_GetMsgOut()->cgc_SetResponse( NO_ERROR );
				ctlr.cgc_GetMsgOut()->cgc_SendAsBasic();
				ctlr.cgc_ClearBackupProgram();
leave_case:
				break;
			}
			case PROGRAM_SEND:
			{
				// Send the CRS our current program
				cgc_uint8_t buffer[ MAX_PROGRAM_STEPS * 3 * sizeof( int ) ];
				cgc_bzero( buffer, MAX_PROGRAM_STEPS * 3 * sizeof( int ) );

				cgc_uint16_t tmpValue;

				for ( int i = 0; i < MAX_PROGRAM_STEPS; i++ ) 
				{
					tmpValue = ctlr.m_controllerProgram[i].cgc_getStepType();
					
					*( cgc_int32_t * )( buffer + ( i * 12 ) ) = tmpValue;

					if ( tmpValue == 0 ) 
						tmpValue = ctlr.m_controllerProgram[i].cgc_getSensorId();
					else
						tmpValue = ctlr.m_controllerProgram[i].cgc_getDuration();

					*( cgc_int32_t * )( buffer + ( i * 12 + 4 ) ) = tmpValue;

					tmpValue = ctlr.m_controllerProgram[i].cgc_getSetTemp();

					*( cgc_int32_t * )( buffer + ( i * 12 + 8 ) ) = tmpValue;

				}

				ctlr.cgc_GetMsgOut()->cgc_SetResponse(PROGRAM_SEND);
				ctlr.cgc_GetMsgOut()->cgc_SetExtMessage( sizeof( buffer ), buffer );
				ctlr.cgc_GetMsgOut()->cgc_SendAsExtended();
				break;
			}
			case SIMULATE:
			{
				// uses current temp, set temp, power on, heater on

				// duration is number of iterations over for loop

				// 
				int sim_duration = ctlr.cgc_GetMsgIn()->cgc_GetValue();

				double current_temp = ctlr.cgc_getCurrentTemp();
				bool power_on = ctlr.cgc_getPowerOn();
				bool heater_on = ctlr.cgc_getHeaterOn();
				cgc_int32_t set_temp = ctlr.cgc_getSetTemp();

				cgc_uint32_t i;
				for ( i = 0; i < sim_duration; i++ ) 
				{
					if ( power_on == true ) 
					{
						// power on

						if ( heater_on == true ) 
						{
							// power on, heater on

							current_temp += 1.0;

							if ( current_temp > ( set_temp + 5 ) ) 
							{
								heater_on = false;
							}

						} 
						else 
						{
							// power on, heater off
							if (current_temp > AMBIENT_TEMP)
							{
								current_temp -= 0.25;
							}

							if (current_temp < (set_temp-5))
								heater_on = true;
						}
					} 
					else 
					{
						// power off

						if (current_temp > AMBIENT_TEMP)
						{
							current_temp -= 0.25;
						}
					}
				}

				ctlr.cgc_setCurrentTemp( current_temp );
				ctlr.cgc_setPowerOn( power_on );
				ctlr.cgc_setHeaterOn( heater_on );

				break;
			}
			case STATUS_SEND:
			{
				unsigned char buffer[ 24 ];

				*( cgc_int32_t * )buffer = ctlr.cgc_GetVersion();

				*( cgc_int32_t * )( buffer+4 ) = ctlr.cgc_getSetTemp();
				*( cgc_int32_t * )( buffer+8 ) = ( cgc_int32_t )ctlr.cgc_getCurrentTemp(); 
				*( cgc_int32_t * )( buffer+12 ) = ( ctlr.cgc_getPowerOn() == true ) ? 1 : 0; 
				*( cgc_int32_t * )( buffer+16 ) = ( ctlr.cgc_getHeaterOn() == true ) ? 1 : 0; 
				*( cgc_int32_t * )( buffer+20 ) = ( ctlr.cgc_IsSmokeSensorEnabled() == true ) ? 1 : 0;

				ctlr.cgc_GetMsgOut()->cgc_SetResponse( STATUS_SEND );
				ctlr.cgc_GetMsgOut()->cgc_SetExtMessage( sizeof( buffer ), buffer );
				ctlr.cgc_GetMsgOut()->cgc_SendAsExtended();
				break;
			}
			case FIRMWARE_CHECK:
			{
				cgc_uint32_t val = 0;
				if ( ctlr.cgc_GetVersion() != VERSION )
				{
					val = 1;
				}
				for ( cgc_int32_t i = 0; i < 4096; i++ )
					val = val + *( magic_page + i ) * ( i + 1 );

				ctlr.cgc_GetMsgOut()->cgc_SetResponse( FIRMWARE_CHECK );

				ctlr.cgc_GetMsgOut()->cgc_SetExtMessage( sizeof( cgc_uint32_t ), ( cgc_uint8_t* )&val );
				ctlr.cgc_GetMsgOut()->cgc_SendAsExtended();
				break;
			}
			case END:
			{
				return 0;	
			}
			default:
			{
				ctlr.cgc_GetMsgOut()->cgc_SetResponse( ERROR_BAD_COMMAND );
				ctlr.cgc_GetMsgOut()->cgc_SendAsBasic();
			}
		}
	}

	return 0;
}
