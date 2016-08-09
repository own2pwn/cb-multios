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
#ifndef SLS_H 
#define SLS_H




typedef struct SLSMessage_s {
  cgc_uint16_t length;
  cgc_uint16_t version;
  cgc_uint8_t type;
  cgc_uint32_t checksum;
  cgc_uint32_t connection_id;
  cgc_uint16_t msg_length;
  cgc_uint8_t *message;
} cgc_SLS_MESSAGE;

#define SLS_HEADER_LENGTH (2+2+1+4+4+2)

typedef struct ClientContext_s {
  cgc_uint32_t connection_id;
  cgc_uint8_t in_handshake;
  cgc_uint32_t last_sequence_number;
  cgc_uint8_t is_connected;
  cgc_uint8_t client_done;
  cgc_uint8_t have_key;
  cgc_uint16_t cipher_suite;
  cgc_uint32_t *client_key;
  cgc_uint32_t *server_key;
  cgc_uint32_t *client_secret;
  cgc_uint32_t *server_secret;
  cgc_uint32_t *cookie;
  cgc_uint32_t *magic;
} cgc_CLIENT_CONTEXT;

#define MAX_CONNECTIONS 32
// Server State
typedef struct ServerState_s {
  cgc_uint8_t is_initialized;
  cgc_uint8_t should_exit;
  struct SLSFunctions_s *functions;
  cgc_CLIENT_CONTEXT *connections[MAX_CONNECTIONS];
  cgc_uint32_t *cookie_base;
} cgc_SERVER_STATE;



typedef struct SLSFunctions_s {
  cgc_SLS_MESSAGE *(*receive_msg)(cgc_SERVER_STATE *);
  int (*send_msg)(cgc_SLS_MESSAGE *);
  void (*cleanup)(cgc_SERVER_STATE *);
  int (*version_check)(cgc_uint16_t);
  void (*send_error_msg)(cgc_uint16_t, cgc_uint8_t);
  cgc_uint32_t (*generate_checksum)(cgc_SLS_MESSAGE *);
  void (*handle_heartbeat)(cgc_SLS_MESSAGE *);
  void (*destroy_message)(cgc_SLS_MESSAGE *);
  void (*handle_handshake)(cgc_SERVER_STATE *, cgc_SLS_MESSAGE *);
  cgc_CLIENT_CONTEXT *(*get_connection)(cgc_SERVER_STATE *,cgc_uint32_t);
  void (*set_cookie)(cgc_SERVER_STATE *);
  cgc_CLIENT_CONTEXT *(*lookup_context)(cgc_SERVER_STATE *, cgc_uint32_t);
  void (*handle_application)(cgc_SERVER_STATE *, cgc_CLIENT_CONTEXT *, cgc_SLS_MESSAGE *);
  void (*handle_changespec)(cgc_SERVER_STATE *, cgc_CLIENT_CONTEXT *, cgc_SLS_MESSAGE *);
  void (*handle_error)(cgc_SERVER_STATE *, cgc_SLS_MESSAGE *);

} cgc_SLS_FUNCTION_PTRS;

#pragma pack(push, 1)
typedef struct SLSError_s {
  cgc_uint16_t error_code;
  cgc_uint8_t  severity;
} cgc_SLS_ERROR_MESSAGE;

typedef struct SLSHandshake_s {
  cgc_uint16_t msg_type;
  cgc_uint32_t sequence_number;
  cgc_uint32_t client_id;
  cgc_uint16_t msg_length;
  cgc_uint8_t *message;
} cgc_SLS_HANDSHAKE_MESSAGE;
#define SLS_HANDSHAKE_HEADER_LEN (2+4+4+2)

typedef struct SLSHeartbeat_s {
  cgc_uint8_t type;
  cgc_uint16_t payload_length;
  cgc_uint8_t *payload;
} cgc_SLS_HEARTBEAT_MESSAGE;
#define SLS_HEARTBEAT_HEADER_LEN (1 + 2)

typedef struct SLSChangespec_s {
  cgc_uint16_t new_cipher_suite;
} cgc_SLS_CHANGESPEC_MESSAGE;

typedef struct SLSApplication_s {
  cgc_uint8_t type;
  cgc_uint16_t length;
  cgc_uint8_t *contents;
} cgc_SLS_APPLICATION_MESSAGE;
#define SLS_APPLICATION_HEADER_LEN (1 + 2)

#pragma pack(pop)

#define SLS_VERSION 0xff01

#define SLS_TYPE_CHANGESPEC 0x01
#define SLS_TYPE_ERROR      0x02
#define SLS_TYPE_HANDSHAKE  0x03
#define SLS_TYPE_APPLICATION 0x04
#define SLS_TYPE_HEARTBEAT  0x05
#define SLS_TYPE_SHUTDOWN   0x06

#endif