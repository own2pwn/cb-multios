/*

Author: Debbie Nuttall <debbie@cromulence.com>

Copyright (c) 2016 Cromulence LLC

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
#ifndef CGCMB_H
#define CGCMB_H

#include "fs.h"

#define CGCMB_MAGIC 0xC47C4D42

// Message Types
#define CGCMB_NEGOTIATE                   0x01
#define CGCMB_NEGOTIATE_RESPONSE          0xf1
#define CGCMB_SESSION_SETUP               0x02
#define CGCMB_SESSION_SETUP_RESPONSE      0xf2
#define CGCMB_TREE_CONNECT                0x03
#define CGCMB_TREE_CONNECT_RESPONSE       0xf3
#define CGCMB_TREE_DISCONNECT             0x04
#define CGCMB_TREE_DISCONNECT_RESPONSE    0xf4
#define CGCMB_FILE_CREATE                 0x05
#define CGCMB_FILE_CREATE_RESPONSE        0xf5
#define CGCMB_FILE_CLOSE                  0x06
#define CGCMB_FILE_CLOSE_RESPONSE         0xf6
#define CGCMB_FILE_READ                   0x07
#define CGCMB_FILE_READ_RESPONSE          0xf7
#define CGCMB_FILE_WRITE                  0x08
#define CGCMB_FILE_WRITE_RESPONSE         0xf8
#define CGCMB_FILE_CANCEL                 0x09
#define CGCMB_FILE_CANCEL_RESPONSE        0xf9
#define CGCMB_TRANSACTION                 0x0a
#define CGCMB_TRANSACTION_RESPONSE        0xfa
#define CGCMB_ECHO                        0x0b
#define CGCMB_ECHO_RESPONSE               0xfb
#define CGCMB_END_SESSION                 0x0c
#define CGCMB_END_SESSION_RESPONSE        0xfc
#define CGCMB_ERROR_RESPONSE              0xff

// Error codes
#define CGCMB_ERROR_FORMAT        0xe1
#define CGCMB_ERROR_LENGTH        0xe2
#define CGCMB_ERROR_STATE         0xe3
#define CGCMB_ERROR_ACCESS        0xe4 
#define CGCMB_ERROR_SESSION       0xe5
#define CGCMB_ERROR_MSG           0xe6
#define CGCMB_ERROR_USER          0xe7
#define CGCMB_ERROR_FULL          0xe8
#define CGCMB_ERROR_NOT_FOUND     0xe9
#define CGCMB_ERROR_FILE          0xea
#define CGCMB_ERROR_TRANSACTION   0xeb


// Supported Dialects
#define CGCMB_DIALECT_1   0x313e
#define CGCMB_DIALECT_2   0x4155
#define CGCMB_DIALECT_3   0x2671

typedef enum {INITIAL, NEGOTIATE, SESSION, CLOSED} cgc_CGCMBServerStates_t;
typedef enum {CONNECTION_OPEN, CONNECTION_CLOSED} cgc_CGCMBConnectionStates_t;

#define MAX_CONNECTIONS 32
#define MAX_OPEN_FILES 16

#define MAX_READ_SIZE   2048
#define MAX_WRITE_SIZE  2048

typedef struct CGCMB_Connection_s {
  cgc_uint32_t connectionID;
  cgc_CGCMBConnectionStates_t state;
  cgc_fs_tree *openTree;
  cgc_fs_file *openFiles[MAX_OPEN_FILES];
} cgc_CGCMB_Connection;

typedef struct CGCMB_ServerState_s {
  cgc_CGCMBServerStates_t state;
  cgc_uint16_t dialect;
  cgc_uint32_t sessionKey;
  cgc_uint16_t userID;
  cgc_CGCMB_Connection *connections[MAX_CONNECTIONS];
} cgc_CGCMB_ServerState;

extern cgc_CGCMB_ServerState *mbServerState;

typedef struct CGCMB_Header_s {
  cgc_uint32_t protocol;
  cgc_uint8_t command;
  cgc_uint8_t status;
  cgc_uint8_t flags;
  cgc_uint8_t security[8];
} cgc_CGCMB_Header;

typedef struct CGCMB_Data_s {
  cgc_uint16_t count;
  cgc_uint8_t *data;
  cgc_uint16_t currentOffset;
} cgc_CGCMB_Data;

typedef struct CGCMB_Message_s {
  cgc_CGCMB_Header *header;
  cgc_CGCMB_Data *parameters;
  cgc_CGCMB_Data *data;
  void *parsed;
} cgc_CGCMB_Message;

typedef struct CGCMB_NEGOTIATE_s {
  cgc_uint32_t numDialects;
  cgc_uint16_t  *dialects;
} cgc_CGCMB_NEGOTIATE_Message;

typedef struct CGCMB_SESSION_SETUP_s {
  cgc_uint32_t sessionKey;
  cgc_uint32_t reserved;
  cgc_uint8_t *password;
  cgc_uint8_t *accountName;
} cgc_CGCMB_SESSION_SETUP_Message;

typedef struct CGCMB_TREE_CONNECT_s {
  cgc_uint32_t sessionKey;
  cgc_uint16_t userID;
  cgc_uint8_t *password;
  cgc_uint8_t *path;
  cgc_uint8_t *service;
} cgc_CGCMB_TREE_CONNECT_Message;

typedef struct CGCMB_TREE_DISCONNECT_s {
  cgc_uint32_t sessionKey;
  cgc_uint16_t userID;
  cgc_uint32_t connectionID;
} cgc_CGCMB_TREE_DISCONNECT_Message;

typedef struct CGCMB_FILE_CREATE_s {
  cgc_uint32_t sessionKey;
  cgc_uint16_t userID;
  cgc_uint32_t connectionID;
  cgc_uint32_t mode;
  cgc_uint8_t *filename;
} cgc_CGCMB_FILE_CREATE_Message;

typedef struct CGCMB_FILE_CLOSE_s {
  cgc_uint32_t sessionKey;
  cgc_uint16_t userID;
  cgc_uint32_t connectionID;
  cgc_uint16_t fileID;
} cgc_CGCMB_FILE_CLOSE_Message;

typedef struct CGCMB_FILE_READ_s {
  cgc_uint32_t sessionKey;
  cgc_uint16_t userID;
  cgc_uint32_t connectionID;
  cgc_uint16_t fileID;
  cgc_uint16_t bytesToRead;
  cgc_uint32_t readOffset;
} cgc_CGCMB_FILE_READ_Message;

typedef struct CGCMB_FILE_WRITE_s {
  cgc_uint32_t sessionKey;
  cgc_uint16_t userID;
  cgc_uint32_t connectionID;
  cgc_uint16_t fileID;
  cgc_uint8_t  mode;
  cgc_uint16_t bytesToWrite;
  cgc_uint32_t writeOffset;
  cgc_uint8_t *bytes;
} cgc_CGCMB_FILE_WRITE_Message;

typedef struct CGCMB_TRANSACTION_s {
  cgc_uint32_t sessionKey;
  cgc_uint16_t userID;
  cgc_uint32_t connectionID;
  cgc_uint16_t fileID;
} cgc_CGCMB_TRANSACTION_Message;

int cgc_ReceiveCGCMBMessage(cgc_TransportMessage *tpMessage, cgc_CGCMB_Message **pmbMessage);
void cgc_DestroyCGCMBMessage(cgc_CGCMB_Message **pmbMessage);
int cgc_ParseCGCMBMessage(cgc_CGCMB_Message *mbMessage);
int cgc_HandleCGCMBMessage(cgc_CGCMB_Message *mbMessage);
void cgc_TransmitCGCMBMessage(cgc_CGCMB_Message *mbMessage);
cgc_CGCMB_Message *cgc_CreateBlankCGCMBMessage();
int cgc_VerifyCGCMBMessage(cgc_CGCMB_Message *mbMessage);
int cgc_ReadFromData(cgc_uint8_t *dest, cgc_CGCMB_Data *source, cgc_uint32_t length);
void cgc_SendErrorResponse(cgc_uint32_t errorCode);
#endif