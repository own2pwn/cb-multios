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
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "fs.h"
#include "stdio.h"
#include "malloc.h"
#include "netstuff.h"
#include "cgcrpc.h"

cgc_CGCRPC_Connection connections[MAX_CGCRPC_CONNECTIONS];
cgc_CGCRPC_Endpoint endpoints[MAX_CGCRPC_ENDPOINTS];


void cgc_InitializeCGCRPC()
{
  cgc_strcpy(endpoints[0].name, "NETSTUFF");
  endpoints[0].version = 0xDC;
  endpoints[0].handler = &cgc_NETSTUFF_Handler;

}

cgc_uint32_t cgc_AddConnection(cgc_CGCRPC_Endpoint *endpoint)
{
  for (int i=0; i < MAX_CGCRPC_CONNECTIONS; i++)
  {
    if (connections[i].bindID == 0)
    {
      connections[i].bindID = cgc_random_in_range(1, 0xffffffff);
      connections[i].endpoint = endpoint;
      return connections[i].bindID;
    }
  }
  return 0;
}

cgc_uint32_t cgc_BindEndpoint(char *name, cgc_uint16_t version)
{
  for (int i=0; i< MAX_CGCRPC_ENDPOINTS; i++)
  {
    if (endpoints[i].name[0] != '\0')
    {
      if ((cgc_strcmp(endpoints[i].name, name) == 0) && (endpoints[i].version == version))
      {
        return cgc_AddConnection(&endpoints[i]);
      }
    }
  }
  return 0;
}

cgc_CGCRPC_Endpoint *cgc_LookupBindID(cgc_uint32_t bindID)
{
  if (bindID == 0)
  {
    return NULL;
  }
  for (int i = 0; i< MAX_CGCRPC_CONNECTIONS; i++)
  {
    if (connections[i].bindID == bindID)
    {
      return connections[i].endpoint;
    }
  }
  return NULL;
}


cgc_uint32_t cgc_HandleCGCRPCMessage(void *data, cgc_uint32_t dataLength, void **output, cgc_uint32_t *outputLength)
{
  if (data == NULL)
  {
    return CGCRPC_FAILURE;
  }
  if (dataLength < 2)
  {
    return CGCRPC_ERROR_FORMAT;
  }
  cgc_uint16_t command = *(cgc_uint16_t *)data;
  dataLength -= 2;
  data += 2;

  switch(command)
  {
    case CGCRPC_BIND:
    {
      
      if (dataLength < 2)
      {
        return CGCRPC_ERROR_FORMAT;
      }
      cgc_uint16_t nameLength = *(cgc_uint16_t *)data;
      data += 2;
      dataLength -= 2;
      if ((nameLength != dataLength - 2) || (nameLength == 0))
      {
        return CGCRPC_ERROR_FORMAT;
      }
      char *name = cgc_calloc(nameLength + 1);
      cgc_memcpy(name, data, nameLength);
      data += nameLength;
      cgc_uint16_t version = *(cgc_uint16_t *)data;
      cgc_uint32_t bindID = cgc_BindEndpoint(name, version);
      cgc_free(name);
      if (bindID == 0)
      {
        return CGCRPC_ERROR_NOT_FOUND;
      }
      cgc_uint8_t *response = cgc_calloc(2 + 1 + 4);
      *output = response;
      *(cgc_uint16_t *)response = CGCRPC_BIND_RESPONSE;
      response += 2;
      *(cgc_uint8_t *)response = CGCRPC_SUCCESS;
      response += 1;
      *(cgc_uint32_t *)response = bindID;
      *outputLength = 2 + 1 + 4;
      
      break;
    }
    case CGCRPC_WRITE_TO_PIPE:
    {
      if (dataLength < 8)
      {
        return CGCRPC_ERROR_FORMAT;
      }
      
      cgc_uint32_t bindID = *(cgc_uint32_t *)data;
      data += 4;
      cgc_uint16_t opCode = *(cgc_uint16_t *)data;
      data += 2;
      cgc_uint16_t requestLength = *(cgc_uint16_t *)data;
      data += 2;
      if (requestLength != dataLength - 8)
      {
        return CGCRPC_ERROR_FORMAT;
      }
      cgc_CGCRPC_Endpoint *endpoint = cgc_LookupBindID(bindID);
      if (endpoint == NULL)
      {
        return CGCRPC_ERROR_NOT_FOUND;
      }
      cgc_uint8_t *endpointData = NULL;
      cgc_uint16_t endpointDataLength;
      if (requestLength == 0)
      {
        data = NULL;
      }
      endpoint->handler(opCode, data, requestLength, &endpointData, &endpointDataLength);

      if (endpointDataLength > MAX_CGCRPC_RESPONSE_SIZE)
      {
        endpointDataLength = MAX_CGCRPC_RESPONSE_SIZE;
      }
      cgc_uint8_t *response = cgc_calloc(2 + 4 + 2 + endpointDataLength);
      *output = response;
      
      *(cgc_uint16_t *)response = CGCRPC_WRITE_TO_PIPE_RESPONSE;
      response += 2;
      *(cgc_uint32_t *)response = bindID;
      response += 4;
      *(cgc_uint16_t *)response = endpointDataLength;
      response += 2;
      cgc_memcpy(response, endpointData, endpointDataLength);
      
      cgc_free(endpointData);
      *outputLength = 2 + 4 + 2 + endpointDataLength;
      break;
    }
    default:
    {
      return CGCRPC_ERROR_COMMAND;
      break;
    }
  }
  return CGCRPC_SUCCESS;
}