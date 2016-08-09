/*

Author: Joe Rogers <joe@cromulence.com>

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
#ifndef DEVICE_H
#define DEVICE_H

#include <libcgc.h>
#include "stdint.h"
#include "user.h"

#define MAX_DEVICES (128)

#define DEVICE_UNUSED  (0)
#define DEVICE_KEYPAD  (1)
#define DEVICE_SWIPE   (2)
#define DEVICE_CONTACT (3)
#define DEVICE_MOTION  (4)
#define DEVICE_HEAT    (5)
#define DEVICE_SMOKE   (6)
#define DEVICE_ALARM   (7)

// Generic cgc_Device handle
typedef struct _device {
	cgc_uint8_t Type;
	cgc_uint16_t DeviceId;
	void *Attributes;
} cgc_Device, *cgc_pDevice;

// List of authorized codes for a given keypad or swipe
typedef struct _authorized_codes {
	char AccessCodes[MAX_USERS][ACCESS_CODE_LEN+1];
} cgc_AuthorizedCodes, *cgc_pAuthorizedCodes;

// Alarm control Devices
#define ALARM_DISARMED (0)
#define ALARM_ARMED (1)
#define ALARM_INACTIVE (0)
#define ALARM_ACTIVE (1)
typedef struct _alarm_attributes {
	cgc_uint8_t Armed;
	cgc_uint8_t Active;
	cgc_uint8_t DeviceList[MAX_DEVICES];
} cgc_AlarmAttributes, *cgc_pAlarmAttributes;

#define CONTACT_NO     (0)
#define CONTACT_NC     (1)
#define CONTACT_OPEN   (0)
#define CONTACT_CLOSED (1)
typedef struct _contact {
	cgc_uint8_t Mode;
	cgc_uint8_t State;
} cgc_Contact, *cgc_pContact;

#define MOTION_INACTIVE (0)
#define MOTION_ACTIVE   (1)
typedef struct _motion {
	cgc_uint8_t State;
} cgc_Motion, *cgc_pMotion;

typedef struct _heat {
	cgc_uint8_t CurrentTemperature;
	cgc_uint8_t ThresholdTemperature;
} cgc_Heat, *cgc_pHeat;

#define SMOKE_INACTIVE (0)
#define SMOKE_ACTIVE   (1)
typedef struct _smoke {
	cgc_uint8_t State;
} cgc_Smoke, *cgc_pSmoke;

cgc_uint8_t cgc_FindAvailableDevice(void);
cgc_uint8_t cgc_FindDeviceId(cgc_uint16_t TargetId);
cgc_uint16_t cgc_NewGenericDevice(cgc_uint8_t DeviceType, cgc_uint8_t NewDevice);
cgc_uint16_t cgc_NewContact(cgc_uint8_t NewDevice);
cgc_uint16_t cgc_NewMotion(cgc_uint8_t NewDevice);
cgc_uint16_t cgc_NewHeat(cgc_uint8_t NewDevice);
cgc_uint16_t cgc_NewSmoke(cgc_uint8_t NewDevice);
cgc_uint16_t cgc_NewStrike(cgc_uint8_t NewDevice);
cgc_uint8_t cgc_DeleteDevice(cgc_uint16_t DeviceId);
cgc_uint8_t cgc_GrantAccess(cgc_uint16_t DeviceId, cgc_uint8_t UserId);
cgc_uint8_t cgc_RevokeAccess(cgc_uint8_t UserId);
cgc_uint8_t cgc_AddDeviceToAlarm(cgc_uint16_t DeviceId, cgc_uint16_t AlarmId);
cgc_uint8_t cgc_RevokeAccess(cgc_uint8_t UserId);
cgc_uint8_t cgc_UpdateDevice(cgc_uint16_t DeviceId);
cgc_uint8_t cgc_ListValidAlarmCodes(cgc_uint16_t DeviceId);

#endif

