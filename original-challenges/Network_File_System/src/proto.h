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
#ifndef SERVICE_H
#define SERVICE_H
#include <libcgc.h>

// cgc_Request
typedef struct _request {
	cgc_uint8_t Type;
	char *Username;
	char *Password;
	char *Filename;
	char *Filename2;
	cgc_uint8_t DataLen;
	char *Data;
	cgc_uint32_t Offset;
	cgc_uint8_t ReadWriteLength;
} cgc_Request, *cgc_pRequest;

// Responses
#define RESP_SUCCESS (0)
#define RESP_LOGIN_FAILED (1)
#define RESP_SYSTEM_FAILURE (2)
#define RESP_INVALID_FILE (3)
#define RESP_DELETE_FAILED (4)
#define RESP_RENAME_FAILED (5)

// packet formats
#define RESPONSE_HEADER_LEN (4)
typedef struct __attribute__((__packed__)) _response {
	cgc_uint8_t Type;
	cgc_uint8_t Code;
	cgc_uint16_t DataLen;
	char *Data;
} cgc_Response, *cgc_pResponse;

#define REQUEST_HEADER_LEN (1)
typedef struct _request_header {
	cgc_uint8_t Type;
} cgc_RequestHeader, *cgc_pRequestHeader;

// login type
#define CFS_LOGIN (0)
typedef struct _login_header {
	cgc_uint8_t UsernameLen;
	cgc_uint8_t PasswordLen;
	// char Username[UsernameLen];
	// char Password[UsernameLen];
} cgc_LoginHeader, *cgc_pLoginHeader;

// cgc_Request Directory Listing
#define CFS_DIR (1)

// Read from file
#define CFS_READ (2)
typedef struct __attribute__((__packed__)) _read_header {
	cgc_uint32_t Offset;	
	cgc_uint8_t Length;
	cgc_uint8_t FilenameLen;
	// char Filename[FilenameLen]
} cgc_ReadHeader, *cgc_pReadHeader;

// Write to file
#define CFS_WRITE (3)
#define CFS_WRITE_APPEND (4)
typedef struct __attribute__((__packed__)) _write_header {
	cgc_uint8_t Length;
	cgc_uint8_t FilenameLen;
	// char Filename[FilenameLen]
	// char Data[Length]
} cgc_WriteHeader, *cgc_pWriteHeader;

// Remove file
#define CFS_DEL (5)
typedef struct __attribute__((__packed__)) _del_header {
	cgc_uint8_t FilenameLen;
	// char Filename[FilenameLen]
} cgc_DelHeader, *cgc_pDelHeader;

// Rename file
#define CFS_RENAME (6)
typedef struct __attribute__((__packed__)) _rename_header {
	cgc_uint8_t OldFilenameLen;
	cgc_uint8_t NewFilenameLen;
	// char OldFilename[FilenameLen]
	// char newFilename[FilenameLen]
} cgc_RenameHeader, *cgc_pRenameHeader;

cgc_pRequest cgc_ReceiveRequest(void);
cgc_pResponse cgc_HandleRequest(cgc_pRequest pReq);
cgc_uint8_t cgc_SendResponse(cgc_pResponse pResp);
cgc_uint8_t cgc_FreeRequest(cgc_pRequest pReq);
cgc_uint8_t cgc_FreeResponse(cgc_pResponse pResp);

#endif
