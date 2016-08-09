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

#include "sensor_manager.h"


cgc_SensorManager::~cgc_SensorManager() 
{
	m_userList.cgc_DeleteAll();
	m_userList.cgc_DeleteAll();
}

void cgc_SensorManager::cgc_PrintSensors()
{
	cgc_printf( "  print sensors for user: $x\n", m_currentUser );
	for ( cgc_FitnessSensor *pCur = m_sensorList.cgc_GetFirst(); pCur; pCur = m_sensorList.cgc_GetNext( pCur ) )
	{
		pCur->cgc_Print();
	}
	cgc_printf("\n");
}

cgc_uint8_t *cgc_SensorManager::cgc_ListSensors( cgc_uint16_t &buffer_len)
{
	buffer_len = 0;
				
// each sensor is 2b ID, 4b mac: 6B
	cgc_uint8_t data[ 6 * MAX_SENSORS_PER_USER * MAX_USERS ];
	cgc_bzero( data, 6 * MAX_SENSORS_PER_USER * MAX_USERS );

	cgc_uint8_t *buff = (cgc_uint8_t *)&data;

	int count_users = 0;
	
	// for each user, list the first MAX_SENSORS_PER_USER
	for ( cgc_User *pUser = m_userList.cgc_GetFirst(); pUser; pUser = m_userList.cgc_GetNext( pUser ) )
	{
#ifdef PATCHED_1
		if ( count_users >= MAX_USERS )
			break;
#endif
		count_users++;

		int count_per_user = 0;
		for ( cgc_FitnessSensor *pSensor = m_sensorList.cgc_GetFirst(); pSensor; pSensor = m_sensorList.cgc_GetNext( pSensor ) )
		{

			if ( pSensor->cgc_GetUser() == pUser->cgc_GetId() )
			{

				if ( count_per_user >= MAX_SENSORS_PER_USER )
					break;

				count_per_user++;
				cgc_uint16_t id = pSensor->cgc_GetID();
				cgc_uint32_t mac = pSensor->cgc_GetMacAsInt();
				cgc_memcpy( &buff[buffer_len], &id, sizeof( cgc_uint16_t ) );

				buffer_len += sizeof( cgc_uint16_t );
				cgc_memcpy( &buff[buffer_len], &mac, sizeof( cgc_uint32_t ) );
				buffer_len += sizeof( cgc_uint32_t );

				if (pSensor->cgc_GetType() == BIKE)
				{
					cgc_uint8_t *data = pSensor->cgc_GetData();

					if (data)
					{
						cgc_uint32_t len = cgc_strlen((char *)data);
#ifdef PATCHED_1		
						if ( buffer_len < ( 6 * MAX_SENSORS_PER_USER * MAX_USERS - len + 1) )
						{
							cgc_memcpy(&buff[buffer_len], data, len);
							buffer_len += len;
						}
#else
						cgc_memcpy(&buff[buffer_len], data, len);

						return NULL;
#endif
					}
				}
			}
		}
	}
	cgc_uint8_t *rbuf = new cgc_uint8_t[buffer_len];
	cgc_memcpy(rbuf, data, buffer_len);
	return rbuf;
}

bool cgc_SensorManager::cgc_ListHwIds( cgc_uint16_t &buffer_len, cgc_uint8_t* buff )
{
	buffer_len = 0;
	for ( int i = 0; i < MAX_SENSOR_VALUE; i ++ )
	{
		buff[i] = cgc_FitnessSensor::m_sensorArray[i];
		buffer_len++;
	}
	return true;
}

bool cgc_MacsAreSame( cgc_SensorMacFormat a, cgc_SensorMacFormat b )
{
	if ( a.Type == b.Type &&
		 a.Val_1 == b.Val_1 && 
		 a.Val_2 == b.Val_2 && 
		 a.Val_3 == b.Val_3)
		return true;
	return false;
}

//
// Returns pointer to sensor if ID matches AND this is a sensor for the current user
//
cgc_FitnessSensor* cgc_SensorManager::cgc_GetSensor( cgc_uint16_t id )
{
	// given the sensor id, return a pointer to the sensor
	for ( cgc_FitnessSensor *pCur = m_sensorList.cgc_GetFirst(); pCur; pCur = m_sensorList.cgc_GetNext( pCur ) )
	{
		if ( pCur->cgc_GetID() == id && pCur->cgc_GetUser() == m_currentUser )
		{
			return pCur;
		}
	}
	return NULL;
}



//
// Add this sensor to the list under the current user's name
//
int cgc_SensorManager::cgc_AddSensor( cgc_FitnessSensor *sensor )
{
	// Check if this sensor's ID is already in the list for this user
	for ( cgc_FitnessSensor *pCur = m_sensorList.cgc_GetFirst(); pCur; pCur = m_sensorList.cgc_GetNext( pCur ) )
	{
		if ( pCur->cgc_GetUser() == sensor->cgc_GetUser() && cgc_MacsAreSame( sensor->cgc_GetMAC(), pCur->cgc_GetMAC() ) )
		{
			// this sensor ID is already in the sensor list, don't add it again
			return 1;
		}
		if ( pCur->cgc_GetUser() == sensor->cgc_GetUser() && sensor->cgc_GetID() == pCur->cgc_GetID() )
		{
			// this sensor ID is already in the sensor list, don't add it again
			return 1;
		}
	}

	int count = 0;
	for ( cgc_FitnessSensor *pCur = m_sensorList.cgc_GetFirst(); pCur; pCur = m_sensorList.cgc_GetNext( pCur ) )
	{
		if ( pCur->cgc_GetUser() == sensor->cgc_GetUser() )
		{
			count += 1;
		}
	}

	// check if we're already maxed out on sensors for this user
	if ( count >= MAX_SENSORS_PER_USER )
	{
		return 2;
	}

	m_sensorList.cgc_AddLast( sensor );
	return 0;
}

//
// Remove a sensor from the sensor list
//
bool cgc_SensorManager::cgc_RemoveSensor( cgc_uint16_t id, cgc_uint16_t user )
{
	for ( cgc_FitnessSensor *pCur = m_sensorList.cgc_GetFirst(); pCur; pCur = m_sensorList.cgc_GetNext( pCur ) )
	{
		if ( pCur->cgc_GetID() == id && pCur->cgc_GetUser() == user )
		{
			delete pCur;
			return true;
		}
	}
	return false;
}

//
// Set the current user to the given user_id
// If user_id isn't in the list, add them
// Return 0 is success (current user is set)
// Return RET_USER_FULL if this new user couldn't be added
//
cgc_uint8_t cgc_SensorManager::cgc_SetCurrentUser( cgc_uint16_t user_id )
{
	if ( user_id == INVALID_USER )
	{
		m_currentUser = INVALID_USER;
		return 0;
	}

	for ( cgc_User *pCur = m_userList.cgc_GetFirst(); pCur; pCur = m_userList.cgc_GetNext( pCur ) )
	{
		if ( pCur->cgc_GetId() == user_id )
		{
			m_currentUser = user_id;
			return 0;
		}
	}

	return -1;
}

//
// Return 0 if this user_id is added
// Return 1 if this user_id is already in the list
// Return 2 if add failed (too many users)
//
cgc_uint8_t cgc_SensorManager::cgc_AddUser( cgc_User* new_user )
{
	int count = 0;
	// Is this user already in the list?
	for ( cgc_User *pCur = m_userList.cgc_GetFirst(); pCur; pCur = m_userList.cgc_GetNext( pCur ) )
	{
		count++;
		if ( pCur->cgc_GetId() == new_user->cgc_GetId() )
		{
			return RET_USER_REDUNDANT;
		}
	}
	// If user list is full, return error
	if ( count >= MAX_USERS )
		return RET_USER_FULL;

	// Add user to list
	m_userList.cgc_AddLast( new_user );

	return RET_USER_ADDED;
}

//
// Return true if this id is registered with the current user
//
bool cgc_SensorManager::cgc_VerifySensor( cgc_uint16_t id )
{
	for ( cgc_FitnessSensor *pCur = m_sensorList.cgc_GetFirst(); pCur; pCur = m_sensorList.cgc_GetNext( pCur ) )
	{
		if ( pCur->cgc_GetID() == id && pCur->cgc_GetUser() == this->m_currentUser )
		{
			return true;
		}
	}
	return false;
}

//
// Add new distance to a user's total distance
//
bool cgc_SensorManager::cgc_AddDistance( cgc_uint16_t new_dist, cgc_uint16_t user )
{
	cgc_User *thisUser;
	for ( thisUser = m_userList.cgc_GetFirst(); thisUser; thisUser = m_userList.cgc_GetNext( thisUser ) )
	{
		if ( thisUser->cgc_GetId() == user )
		{
			goto found;
		}
	}
	return false;

found:
	thisUser->cgc_AddTotalDistance( new_dist );

	return true;
} 



