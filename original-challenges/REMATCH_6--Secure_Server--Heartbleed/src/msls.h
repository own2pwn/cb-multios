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
#ifndef MSLS_H
#define MSLS_H

#include "sls.h"

#define MSLS_VERSION 16

#define MAX_CIPHER_SUITES 12
#define MSLS_COOKIE_SIZE 128



#define MSLS_MAX_MSG_LENGTH 2048

#define MSLS_ERROR_LENGTH         0xe0
#define MSLS_ERROR_VERSION        0xe1
#define MSLS_ERROR_CHECKSUM       0xe2
#define MSLS_ERROR_CONNECTION     0xe3
#define MSLS_ERROR_HANDSHAKE_TYPE 0xe4
#define MSLS_ERROR_OUTOFORDER     0xe5
#define MSLS_ERROR_SEQUENCE       0xe6
#define MSLS_ERROR_HASH           0xe7

#define MSLS_SEVERITY_WARNING  0xa0
#define MSLS_SEVERITY_CRITICAL 0xa1
#define MSLS_SEVERITY_FATAL    0xa2

#define MSLS_HEARTBEAT_REQUEST  0x41
#define MSLS_HEARTBEAT_RESPONSE 0x42



cgc_SLS_FUNCTION_PTRS *cgc_msls_get_sls_functions();
cgc_SLS_MESSAGE *cgc_msls_receive_msg(cgc_SERVER_STATE *state);
int cgc_msls_send_msg(cgc_SLS_MESSAGE *msg);
void cgc_msls_cleanup(cgc_SERVER_STATE *state);
int cgc_msls_version_check(cgc_uint16_t version);
void cgc_msls_destroy_msg(cgc_SLS_MESSAGE *msg);
void cgc_msls_send_error_msg(cgc_uint16_t error_code, cgc_uint8_t severity);
cgc_uint32_t cgc_msls_generate_checksum(cgc_SLS_MESSAGE *msg);
cgc_uint32_t cgc_msls_generate_checksum_outgoing(cgc_SLS_MESSAGE *msg);
void cgc_msls_handle_heartbeat(cgc_SLS_MESSAGE *msg);
void cgc_msls_handle_handshake(cgc_SERVER_STATE *state, cgc_SLS_MESSAGE *msg);
void cgc_msls_handle_error(cgc_SERVER_STATE *state, cgc_SLS_MESSAGE *msg);
void cgc_msls_handle_changespec(cgc_SERVER_STATE *state,cgc_CLIENT_CONTEXT *connection, cgc_SLS_MESSAGE *msg);
void cgc_msls_handle_application(cgc_SERVER_STATE *state, cgc_CLIENT_CONTEXT *connection, cgc_SLS_MESSAGE *msg);



cgc_uint32_t cgc_get_cgc_random();
#endif
