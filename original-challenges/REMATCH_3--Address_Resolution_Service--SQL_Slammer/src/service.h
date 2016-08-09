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
#ifndef SERVICE_H
#define SERVICE_H

#include <libcgc.h>
#include "linkedlist.h"
#include "stdint.h"

#define QUERY_ALL     0x01
#define QUERY_SERVER  0x02
#define QUERY_ONE     0x03
#define QUERY_ADMIN   0x04

#define RESPONSE      0x05

#define LIST_SERVER   0x01
#define LIST_INSTANCE 0x02

typedef struct query_s {
  cgc_uint8_t type;
  cgc_uint16_t length;
  cgc_uint8_t *data;
} cgc_query;

typedef struct response_s {
  cgc_uint16_t size;
  cgc_uint8_t *data;
} cgc_response;

typedef struct serverInfo_s {
  char *name;
  cgc_linkedList *instances;
} cgc_serverInfo;

typedef struct instanceInfo_s {
  char *name;
  cgc_serverInfo *server;
  int   port;
  int   adminPortOffset;
  int   isClustered;
  char  *versionString;
  char  *optionString;
} cgc_instanceInfo;

extern cgc_linkedList *serverList;

void cgc_InitializeSimulation();

cgc_query *cgc_DestroyQuery(cgc_query *pQuery);
cgc_query *cgc_ParseQuery(char *inputBuffer);

cgc_response *cgc_DestroyResponse(cgc_response *pResponse);
cgc_response *cgc_GenerateBlankResponse();
int cgc_AddToResponse(cgc_response *pResponse, char *pString);
int cgc_SendResponse(cgc_response *pResponse);
int cgc_DumpResponse(cgc_response *pResponse);

#endif