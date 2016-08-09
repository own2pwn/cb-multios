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
#include <libcgc.h>
#include "stdlib.h"
#include "stdio.h"
#include "stdint.h"
#include "malloc.h"
#include "string.h"
#include "device.h"
#include "user.h"
#include "io.h"
#include "alarm.h"

cgc_Device Devices[MAX_DEVICES];

extern cgc_User Users[MAX_USERS];
const cgc_uint32_t *MagicPage = (const cgc_uint32_t *)0x4347C000;
cgc_uint16_t MagicIndex = 0;

// Generate a new DeviceId
cgc_uint16_t cgc_NewDeviceId(void) {
	cgc_uint16_t DeviceId;

	// Generate a 2-byte DeviceId from a 4-byte magic page value
	DeviceId = MagicPage[MagicIndex] % 65534;
	MagicIndex = (MagicIndex+1)%1024;

	// make sure the new ID is not zero (reserved) and isn't already in use
	while (DeviceId == 0 || (cgc_FindDeviceId(DeviceId) != MAX_DEVICES)) {
		DeviceId = MagicPage[MagicIndex] % 65534;
		MagicIndex = (MagicIndex+1)%1024;
	}

	return(DeviceId);
}

// Find an available device index
cgc_uint8_t cgc_FindAvailableDevice(void) {
	cgc_uint8_t i;
	cgc_uint8_t NewDevice = MAX_DEVICES+1;

	for (i = 1; i < MAX_DEVICES; i++) {
		if (Devices[i].Type == DEVICE_UNUSED) {
			NewDevice = i;
			break;
		}
	}
			
	return(NewDevice);
}

// Find a device given its DeviceId value
cgc_uint8_t cgc_FindDeviceId(cgc_uint16_t TargetId) {
	cgc_uint8_t i;

	for (i = 0; i < MAX_DEVICES; i++) {
		if (Devices[i].Type != DEVICE_UNUSED && Devices[i].DeviceId == TargetId) {
			break;
		}
	}

	return(i);

}

// Add device
cgc_uint16_t cgc_NewGenericDevice(cgc_uint8_t DeviceType, cgc_uint8_t NewDevice) {
	cgc_uint16_t DeviceId = cgc_NewDeviceId();
	
	// No device-specific attributes for this type device, so just
	// Add the new device
	Devices[NewDevice].Type = DeviceType;
	Devices[NewDevice].DeviceId = DeviceId;
	Devices[NewDevice].Attributes = NULL;

	return(DeviceId);
}

cgc_uint16_t cgc_NewContact(cgc_uint8_t NewDevice) {
	cgc_uint16_t DeviceId = cgc_NewDeviceId();
	cgc_uint8_t Mode;
	cgc_uint8_t State;
	cgc_pContact cgc_NewContact;

	// Read in the device-specific attributes
	if (!cgc_ReadBytes((unsigned char *)&Mode, 1)) {
		return(0);
	}
	if (!cgc_ReadBytes((unsigned char *)&State, 1)) {
		return(0);
	}
	
	// Add the new device
	Devices[NewDevice].Type = DEVICE_CONTACT;
	Devices[NewDevice].DeviceId = DeviceId;
	if ((cgc_NewContact = (cgc_pContact)cgc_calloc(sizeof(cgc_Contact))) == NULL) {
		return(0);
	}
	cgc_NewContact->Mode = Mode;
	cgc_NewContact->State = State;
	Devices[NewDevice].Attributes = cgc_NewContact;

	return(DeviceId);

}

cgc_uint16_t cgc_NewMotion(cgc_uint8_t NewDevice) {
	cgc_uint16_t DeviceId = cgc_NewDeviceId();
	cgc_uint8_t State;
	cgc_pMotion cgc_NewMotion;

	// Read in the device-specific attributes
	if (!cgc_ReadBytes((unsigned char *)&State, 1)) {
		return(0);
	}
	
	// Add the new device
	Devices[NewDevice].Type = DEVICE_MOTION;
	Devices[NewDevice].DeviceId = DeviceId;
	if ((cgc_NewMotion = (cgc_pMotion)cgc_calloc(sizeof(cgc_Motion))) == NULL) {
		return(0);
	}
	cgc_NewMotion->State = State;
	Devices[NewDevice].Attributes = cgc_NewMotion;

	return(DeviceId);

}

cgc_uint16_t cgc_NewHeat(cgc_uint8_t NewDevice) {
	cgc_uint16_t DeviceId = cgc_NewDeviceId();
	cgc_uint8_t CurrentTemperature;
	cgc_uint8_t ThresholdTemperature;
	cgc_pHeat cgc_NewHeat;

	// Read in the device-specific attributes
	if (!cgc_ReadBytes((unsigned char *)&CurrentTemperature, 1)) {
		return(0);
	}
	
	if (!cgc_ReadBytes((unsigned char *)&ThresholdTemperature, 1)) {
		return(0);
	}
	
	// Add the new device
	Devices[NewDevice].Type = DEVICE_HEAT;
	Devices[NewDevice].DeviceId = DeviceId;
	if ((cgc_NewHeat = (cgc_pHeat)cgc_calloc(sizeof(cgc_Heat))) == NULL) {
		return(0);
	}
	cgc_NewHeat->CurrentTemperature = CurrentTemperature;
	cgc_NewHeat->ThresholdTemperature = ThresholdTemperature;
	Devices[NewDevice].Attributes = cgc_NewHeat;

	return(DeviceId);

}

cgc_uint16_t cgc_NewSmoke(cgc_uint8_t NewDevice) {
	cgc_uint16_t DeviceId = cgc_NewDeviceId();
	cgc_uint8_t State;
	cgc_pSmoke cgc_NewSmoke;

	// Read in the device-specific attributes
	if (!cgc_ReadBytes((unsigned char *)&State, 1)) {
		return(0);
	}
	// Add the new device
	Devices[NewDevice].Type = DEVICE_SMOKE;
	Devices[NewDevice].DeviceId = DeviceId;
	if ((cgc_NewSmoke = (cgc_pSmoke)cgc_calloc(sizeof(cgc_Smoke))) == NULL) {
		return(0);
	}
	cgc_NewSmoke->State = State;
	Devices[NewDevice].Attributes = cgc_NewSmoke;

	return(DeviceId);

}

// Delete cgc_Device 
cgc_uint8_t cgc_DeleteDevice(cgc_uint16_t DeviceId) { 
	cgc_uint8_t Target; 
	cgc_uint8_t i;
	cgc_uint8_t j;
	cgc_pAlarmAttributes AA;
	
	if ((Target = cgc_FindDeviceId(DeviceId)) == MAX_DEVICES) {
		return(0);
	}

	// remove it from any alarms that reference it
	for (i = 0; i < MAX_DEVICES; i++) {
		if (Devices[i].Type != DEVICE_ALARM) {
			continue;
		}
		if (!Devices[i].Attributes) {
			continue;
		}
		AA = (cgc_pAlarmAttributes)(Devices[i].Attributes);
		for (j = 0; j < MAX_DEVICES; j++) {
			if (AA->DeviceList[j] == Target) {
				AA->DeviceList[j] = DEVICE_UNUSED;
				break;
			}
		}
	}

	// Process any changes in alarm state that would result from
	// this device being removed
	cgc_ProcessAlarms();

	// remove the device
	Devices[Target].Type = DEVICE_UNUSED;
	Devices[Target].DeviceId = 0;
	if (Devices[Target].Attributes) {
		cgc_free(Devices[Target].Attributes);
		Devices[Target].Attributes = NULL;
	}

	return(1);
}


// Grant user access to device
cgc_uint8_t cgc_GrantAccess(cgc_uint16_t DeviceId, cgc_uint8_t UserId) {
	cgc_pAuthorizedCodes codes;
	cgc_uint8_t i;
	cgc_uint8_t NewCode;
	cgc_uint8_t DeviceIndex;

	// Look up the device index
	if ((DeviceIndex = cgc_FindDeviceId(DeviceId)) == MAX_DEVICES) {
		return(0);
	}

	// make sure it's a keypad or swipe device
	if (Devices[DeviceIndex].Type != DEVICE_KEYPAD && Devices[DeviceIndex].Type != DEVICE_SWIPE) {
		return(0);
	}

	// validate the userid
	if (UserId > MAX_USERS) {
		return(0);
	}
	if (Users[UserId].Username[0] == '\0') {
		return(0);
	}

	// allocate new attributes buffer if it's not already in place
	if (!Devices[DeviceIndex].Attributes) {
		if ((codes = (cgc_pAuthorizedCodes)cgc_calloc(sizeof(cgc_AuthorizedCodes))) == NULL) {
			return(0);
		}
		Devices[DeviceIndex].Attributes = codes;
	} else {
		codes = (cgc_pAuthorizedCodes)(Devices[DeviceIndex].Attributes);
	}

	// Find an open authorized code slot
	NewCode = MAX_USERS;
	for (i = 0; i < MAX_USERS; i++) {
		if (codes->AccessCodes[i][0] == '\0' && NewCode == MAX_USERS) {
			// found a open slot
			NewCode = i;
		}
		if (!cgc_strcmp(codes->AccessCodes[i], Users[UserId].Pin) || !cgc_strcmp(codes->AccessCodes[i], Users[UserId].AccessCode)) {
			// code is already in the list
			return(1);
		}
	}
	if (NewCode == MAX_USERS) {
		// No available slots
		return(0);
	}

	// Add the pin to a Keypad device
	if (Devices[DeviceIndex].Type == DEVICE_KEYPAD) {
		cgc_strcpy(codes->AccessCodes[NewCode], Users[UserId].Pin);
		codes->AccessCodes[NewCode][PIN_LEN] = '\0';
	// Add the access code to a swipe device
	} else if (Devices[DeviceIndex].Type == DEVICE_SWIPE) {	
		cgc_strcpy(codes->AccessCodes[NewCode], Users[UserId].AccessCode);
		codes->AccessCodes[NewCode][ACCESS_CODE_LEN] = '\0';
	}

	return(1);
}

cgc_uint8_t cgc_RevokeAccess(cgc_uint8_t UserIndex) {
	cgc_uint8_t i;
	cgc_uint8_t j;
	cgc_pAuthorizedCodes codes;

	// run through the devices
	for (i = 0; i < MAX_DEVICES; i++) {
		// only need to remove users from keypads and swipes
		if (Devices[i].Type != DEVICE_KEYPAD && Devices[i].Type != DEVICE_SWIPE) {
			continue;	
		}

		// See if this device has any pins or access codes associated with it
		if (!Devices[i].Attributes) {
			continue;
		}
		codes = Devices[i].Attributes;
		
		// run through the codes on this device
		for (j = 0; j < MAX_USERS; j++) {
			if (!cgc_strcmp(codes->AccessCodes[j], Users[UserIndex].Pin) || !cgc_strcmp(codes->AccessCodes[j], Users[UserIndex].AccessCode)) {
				// found matching code, remove it
				codes->AccessCodes[j][0] = '\0';
			}
		}
	}

	return(1);
}

// Associate device with alarm
cgc_uint8_t cgc_AddDeviceToAlarm(cgc_uint16_t DeviceId, cgc_uint16_t AlarmId) {
	cgc_uint8_t DeviceIndex;
	cgc_uint8_t AlarmIndex;
	cgc_pAlarmAttributes AA;
	cgc_uint8_t NewDevice;
	cgc_uint8_t i;

	if ((DeviceIndex = cgc_FindDeviceId(DeviceId)) == MAX_DEVICES) {
		return(0);
	}

	if ((AlarmIndex = cgc_FindDeviceId(AlarmId)) == MAX_DEVICES) {
		return(0);
	}

	// allocate new attributes buffer if it's not already in place
	if (!Devices[AlarmIndex].Attributes) {
		if ((AA = (cgc_pAlarmAttributes)cgc_calloc(sizeof(cgc_AlarmAttributes))) == NULL) {
			return(0);
		}
		Devices[AlarmIndex].Attributes = AA;
	} else {
		AA = (cgc_pAlarmAttributes)(Devices[AlarmIndex].Attributes);
	}

	// find an available device slot in this alarm
	// and make sure the new device isn't already in the list
	NewDevice = MAX_DEVICES;
	for (i = 0; i < MAX_DEVICES; i++) {
		if (AA->DeviceList[i] == DEVICE_UNUSED && NewDevice == MAX_DEVICES) {
			// found an available slot
			NewDevice = i;
		}
		if (AA->DeviceList[i] == DeviceIndex) {
			// device is already in the list
			return(1);
		}
	}
	if (NewDevice == MAX_DEVICES) {
		// no space
		return(0);
	}

	AA->DeviceList[NewDevice] = DeviceIndex;

	// process any alarms that might now be triggered by this device
	cgc_ProcessAlarms();

	return(1);
}

cgc_uint8_t cgc_IsValidCredential(cgc_uint8_t DeviceIndex, char *TargetCode) {
	cgc_uint8_t i;
	cgc_pAuthorizedCodes Codes;

	// valid index?
	if (DeviceIndex == 0 || DeviceIndex >= MAX_DEVICES) {
		return(0);
	}

	if (!TargetCode) {
		return(0);
	}

	// Is it a keypad or swipe device (the only ones with credentials)
	if (Devices[DeviceIndex].Type != DEVICE_KEYPAD && Devices[DeviceIndex].Type != DEVICE_SWIPE) {
		return(0);
	}

	// Does this device have attributes defined yet?
	if (!Devices[DeviceIndex].Attributes) {
		return(0);
	}

	Codes = (cgc_pAuthorizedCodes)(Devices[DeviceIndex].Attributes);
	for (i = 0; i < MAX_USERS; i++) {
		if (!cgc_strcmp(Codes->AccessCodes[i], TargetCode)) {
			return(1);
		}
	}

	return(0);
}

cgc_uint8_t cgc_UpdateDevice(cgc_uint16_t DeviceId) {
	cgc_uint8_t DeviceIndex;
	char Pin[6];
	char Code[32];
	cgc_uint8_t NewState;
	cgc_uint8_t NewTemperature;
	cgc_pContact ContactAttributes;
	cgc_pMotion MotionAttributes;
	cgc_pSmoke SmokeAttributes;
	cgc_pHeat HeatAttributes;

	if ((DeviceIndex = cgc_FindDeviceId(DeviceId)) == MAX_DEVICES) {
		return(0);
	}

	switch (Devices[DeviceIndex].Type) {
		case DEVICE_KEYPAD:
			// read in the PIN
			if (!cgc_ReadBytes((unsigned char *)&Pin, PIN_LEN)) {
				return(0);
			}
			Pin[PIN_LEN] = '\0';
	
			// see if it's valid for the specified Keypad device
			if (cgc_IsValidCredential(DeviceIndex, Pin)) {
				// yes, change the alarm state
				cgc_InvertAlarmState(DeviceIndex);
				// Process any alarm changes
				cgc_ProcessAlarms();
			} 

			break;

		case DEVICE_SWIPE:
			// read in the Code
			if (!cgc_ReadBytes((unsigned char *)&Code, ACCESS_CODE_LEN)) {
				return(0);
			}
			Code[ACCESS_CODE_LEN] = '\0';
	
			// see if it's valid for the specified Keypad device
			if (cgc_IsValidCredential(DeviceIndex, Code)) {
				// yes, change the alarm state
				cgc_InvertAlarmState(DeviceIndex);
				// Process any alarm changes
				cgc_ProcessAlarms();
			} 

			break;

		case DEVICE_CONTACT:
			// Read in the new state
			if (!cgc_ReadBytes((unsigned char *)&NewState, 1)) {
				return(0);
			}

			// Update the device state
			if (!Devices[DeviceIndex].Attributes) {
				return(0);
			}
			ContactAttributes = (cgc_pContact)(Devices[DeviceIndex].Attributes);
			ContactAttributes->State = NewState;

			// Process any alarm changes
			cgc_ProcessAlarms();
			
			break;

		case DEVICE_MOTION:
			// Read in the new state
			if (!cgc_ReadBytes((unsigned char *)&NewState, 1)) {
				return(0);
			}

			// Update the device state
			if (!Devices[DeviceIndex].Attributes) {
				return(0);
			}
			MotionAttributes = (cgc_pMotion)(Devices[DeviceIndex].Attributes);
			MotionAttributes->State = NewState;

			// Process any alarm changes
			cgc_ProcessAlarms();
			
			break;

		case DEVICE_SMOKE:
			// Read in the new state
			if (!cgc_ReadBytes((unsigned char *)&NewState, 1)) {
				return(0);
			}

			// Update the device state
			if (!Devices[DeviceIndex].Attributes) {
				return(0);
			}
			SmokeAttributes = (cgc_pSmoke)(Devices[DeviceIndex].Attributes);
			SmokeAttributes->State = NewState;

			// Process any alarm changes
			cgc_ProcessAlarms();
			
			break;

		case DEVICE_HEAT:
			// Read in the new state
			if (!cgc_ReadBytes((unsigned char *)&NewTemperature, 1)) {
				return(0);
			}

			// Update the device state
			if (!Devices[DeviceIndex].Attributes) {
				return(0);
			}
			HeatAttributes = (cgc_pHeat)(Devices[DeviceIndex].Attributes);
			HeatAttributes->CurrentTemperature = NewTemperature;

			// Process any alarm changes
			cgc_ProcessAlarms();
			
			break;


		default:
			return(0);
			break;
	}

	return(1);
}

void cgc_SortCodes(cgc_pAuthorizedCodes Codes) {
	cgc_uint8_t Sorted = 0;
	char Temp[ACCESS_CODE_LEN+1];
	int res;
	cgc_uint8_t i;

	while (!Sorted) {
		Sorted = 1;
		for (i = 0; i < MAX_DEVICES-1; i++) {
			res = cgc_strcmp(Codes->AccessCodes[i], Codes->AccessCodes[i+1]);
			if (res > 0) {
				cgc_memcpy(Temp, Codes->AccessCodes[i+1], 32);
				cgc_memcpy(Codes->AccessCodes[i+1], Codes->AccessCodes[i], 32);
				cgc_memcpy(Codes->AccessCodes[i], Temp, 32);
				Sorted = 0;
			}
		}
	}
}

cgc_uint8_t cgc_ListValidAlarmCodes(cgc_uint16_t DeviceId) {
	cgc_uint8_t DeviceIndex;
	cgc_pAuthorizedCodes codes;
	cgc_uint8_t i;
	cgc_uint8_t AlarmCodeCount = 0;

	if ((DeviceIndex = cgc_FindDeviceId(DeviceId)) == MAX_DEVICES) {
		return(0);
	}

	// make sure it's a keypad or swipe device
	if (Devices[DeviceIndex].Type != DEVICE_KEYPAD && Devices[DeviceIndex].Type != DEVICE_SWIPE) {
		return(0);
	}

	// make a copy of the device's valid codes
	if (!Devices[DeviceIndex].Attributes) {
		cgc_SendBytes((unsigned char *)&AlarmCodeCount, 1);
		cgc_SendResp(RESP_SUCCESS, NULL);
		return(1);
	}
	if ((codes = (cgc_pAuthorizedCodes)cgc_calloc(sizeof(cgc_AuthorizedCodes))) == NULL) {
		return(0);
	}
	cgc_memcpy(codes, Devices[DeviceIndex].Attributes, sizeof(cgc_AuthorizedCodes));
	cgc_SortCodes(codes);

	// once through to get a count of codes we'll be sending
	for (i = 0; i < MAX_USERS; i++) {
		if (codes->AccessCodes[i][0] == '\0') {
			continue;
		}
		AlarmCodeCount++;
	}

	// Send the count
	cgc_SendBytes((unsigned char *)&AlarmCodeCount, 1);

	// Send the codes
	for (i = 0; i < MAX_USERS; i++) {
		if (codes->AccessCodes[i][0] == '\0') {
			continue;
		}
		cgc_SendBytes((unsigned char *)&codes->AccessCodes[i], 32);
	}

	cgc_SendResp(RESP_SUCCESS, NULL);

	cgc_free(codes);

	return(1);
}
