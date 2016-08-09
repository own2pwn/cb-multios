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
#ifndef CGCRPC_H
#define CGCRPC_H


#define CGCRPC_SUCCESS          0
#define CGCRPC_FAILURE          -1

#define CGCRPC_ERROR_FORMAT     2
#define CGCRPC_ERROR_COMMAND    4
#define CGCRPC_ERROR_NOT_FOUND  8
#define CGCRPC_ERROR_VERSION    10

#define MAX_CGCRPC_CONNECTIONS  10
#define MAX_CGCRPC_ENDPOINTS    10

#define MAX_CGCRPC_RESPONSE_SIZE 2048

// CGCRPC Commands

/*  Generic Command Format
  cgc_uint16_t command
  ...command specific options

  Generic Response Format
  cgc_uint16_t command
  ...command specific options
*/
#define CGCRPC_BIND             0xa0
/*
  cgc_uint16_t nameLength
  cgc_uint8_t name[nameLength]
  cgc_uint16_t version
*/
#define CGCRPC_BIND_RESPONSE    0x0a
/*
  cgc_uint8_t responseCode
  cgc_uint32_t  bindID
*/
#define CGCRPC_WRITE_TO_PIPE    0xa1
/*
  cgc_uint32_t bindID 
  cgc_uint16_t opCode
  cgc_uint16_t requestLength
  cgc_uint8_t *requestData
*/
#define CGCRPC_WRITE_TO_PIPE_RESPONSE 0x1a
/*
  cgc_uint32_t bindID
  cgc_uint16_t responseLength
  cgc_uint8_t *responseBytes
*/

typedef struct CGCRPC_Endpoint_s {
  char      name[64];
  cgc_uint16_t  version;
  void      (*handler)(cgc_uint16_t, cgc_uint8_t *, cgc_uint32_t, cgc_uint8_t **, cgc_uint16_t *);
} cgc_CGCRPC_Endpoint;

typedef struct CGCRPC_Connection_s {
  cgc_uint32_t bindID;
  cgc_CGCRPC_Endpoint *endpoint;
} cgc_CGCRPC_Connection;

extern cgc_CGCRPC_Endpoint endpoints[MAX_CGCRPC_ENDPOINTS];
extern cgc_CGCRPC_Connection connections[MAX_CGCRPC_CONNECTIONS];

cgc_uint32_t cgc_HandleCGCRPCMessage(void *data, cgc_uint32_t dataLength, void **output, cgc_uint32_t *outputLength);
void cgc_InitializeCGCRPC();

#endif