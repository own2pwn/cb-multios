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
#include <libcgc.h>
#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "malloc.h"
#include "sls.h"
#include "msls.h"
#include "msls_handshake.h"
#include "application.h"

cgc_uint8_t APPLICATION_MSG_BOARD[APPLICATION_MAX_MESSAGES][APPLICATION_MSG_LENGTH];
int numMessages = 0;

void cgc_msls_handle_application(cgc_SERVER_STATE *state, cgc_CLIENT_CONTEXT *connection,  cgc_SLS_MESSAGE *msg)
{
  cgc_debug_print("Handling Application Message ($d)\n", msg->msg_length);
  if (msg->msg_length < SLS_APPLICATION_HEADER_LEN)
  {
    return;
  }
  cgc_SLS_APPLICATION_MESSAGE *ap_msg = (cgc_SLS_APPLICATION_MESSAGE *)msg->message;
  
  cgc_debug_print("Type: $d Len: $d\n", ap_msg->type, ap_msg->length);
  switch(ap_msg->type)
  {
    case APPLICATION_TYPE_LIST_BOARD:
    {
      cgc_debug_print("APP: List Board\n");
      cgc_SLS_MESSAGE *response = cgc_calloc(sizeof(cgc_SLS_MESSAGE));
      cgc_SLS_APPLICATION_MESSAGE *ap_response = cgc_calloc(sizeof(cgc_SLS_APPLICATION_MESSAGE));
      cgc_APPLICATION_RESPONSE_MSG *ar_msg = cgc_calloc(sizeof(cgc_APPLICATION_RESPONSE_MSG));
      response->type = SLS_TYPE_APPLICATION;
      response->version = SLS_VERSION;
      response->connection_id = msg->connection_id;
      response->message = (cgc_uint8_t *)ap_response;
      response->length = SLS_HEADER_LENGTH + SLS_APPLICATION_HEADER_LEN + sizeof(cgc_APPLICATION_RESPONSE_MSG);
      response->msg_length = SLS_APPLICATION_HEADER_LEN + sizeof(cgc_APPLICATION_RESPONSE_MSG);
      ap_response->type = APPLICATION_TYPE_RESPONSE;
      ap_response->length = sizeof(cgc_APPLICATION_RESPONSE_MSG);
      ap_response->contents = (cgc_uint8_t *)ar_msg;
      cgc_sprintf((char *)ar_msg->message, "$d of $d slots filled\n", numMessages, APPLICATION_MAX_MESSAGES);
      cgc_msls_encrypt(ap_response->contents, ap_response->length, connection);
      cgc_msls_send_msg(response);
      cgc_msls_destroy_msg(response);
      break;
    }
    case APPLICATION_TYPE_POST_MESSAGE:
    {
      if (ap_msg->length < sizeof(cgc_APPLICATION_POST_MSG))
      {
        return;
      }
      cgc_msls_decrypt(((cgc_uint8_t *)ap_msg + SLS_APPLICATION_HEADER_LEN), ap_msg->length, connection);
      cgc_APPLICATION_POST_MSG *post_msg = (cgc_APPLICATION_POST_MSG *)((cgc_uint8_t *)ap_msg + SLS_APPLICATION_HEADER_LEN);
      int ret_code = cgc_post_new_message(post_msg->message);
      cgc_debug_print("Posting to slot $d\n", numMessages-1);
      cgc_SLS_MESSAGE *response = cgc_calloc(sizeof(cgc_SLS_MESSAGE));
      cgc_SLS_APPLICATION_MESSAGE *ap_response = cgc_calloc(sizeof(cgc_SLS_APPLICATION_MESSAGE));
      cgc_APPLICATION_RESPONSE_MSG *ar_msg = cgc_calloc(sizeof(cgc_APPLICATION_RESPONSE_MSG));
      response->type = SLS_TYPE_APPLICATION;
      response->version = SLS_VERSION;
      response->connection_id = msg->connection_id;
      response->message = (cgc_uint8_t *)ap_response;
      response->length = SLS_HEADER_LENGTH + SLS_APPLICATION_HEADER_LEN + sizeof(cgc_APPLICATION_RESPONSE_MSG);
      response->msg_length = SLS_APPLICATION_HEADER_LEN + sizeof(cgc_APPLICATION_RESPONSE_MSG);
      ap_response->type = APPLICATION_TYPE_RESPONSE;
      ap_response->length = sizeof(cgc_APPLICATION_RESPONSE_MSG);
      ap_response->contents = (cgc_uint8_t *)ar_msg;
      cgc_sprintf((char *)ar_msg->message, "POST returned: $d\n", ret_code);
      cgc_msls_encrypt(ap_response->contents, ap_response->length, connection);
      cgc_msls_send_msg(response);
      cgc_msls_destroy_msg(response);
      break;
    }
    case APPLICATION_TYPE_DELETE_MESSAGE:
    {
      if (ap_msg->length < sizeof(cgc_APPLICATION_DELETE_MSG))
      {
        return;
      }
      cgc_msls_decrypt(((cgc_uint8_t *)ap_msg + SLS_APPLICATION_HEADER_LEN), ap_msg->length, connection);
      cgc_APPLICATION_DELETE_MSG *del_msg = (cgc_APPLICATION_DELETE_MSG *)((cgc_uint8_t *)ap_msg + SLS_APPLICATION_HEADER_LEN);
      int ret_code = cgc_delete_message(del_msg->msg_num);
      cgc_debug_print("Deleting message $d\n", del_msg->msg_num);
      cgc_SLS_MESSAGE *response = cgc_calloc(sizeof(cgc_SLS_MESSAGE));
      cgc_SLS_APPLICATION_MESSAGE *ap_response = cgc_calloc(sizeof(cgc_SLS_APPLICATION_MESSAGE));
      cgc_APPLICATION_RESPONSE_MSG *ar_msg = cgc_calloc(sizeof(cgc_APPLICATION_RESPONSE_MSG));
      response->type = SLS_TYPE_APPLICATION;
      response->version = SLS_VERSION;
      response->connection_id = msg->connection_id;
      response->message = (cgc_uint8_t *)ap_response;
      response->length = SLS_HEADER_LENGTH + SLS_APPLICATION_HEADER_LEN + sizeof(cgc_APPLICATION_RESPONSE_MSG);
      response->msg_length = SLS_APPLICATION_HEADER_LEN + sizeof(cgc_APPLICATION_RESPONSE_MSG);
      ap_response->type = APPLICATION_TYPE_RESPONSE;
      ap_response->length = sizeof(cgc_APPLICATION_RESPONSE_MSG);
      ap_response->contents = (cgc_uint8_t *)ar_msg;
      cgc_sprintf((char *)ar_msg->message, "DELETE returned: $d\n", ret_code);
      cgc_msls_encrypt(ap_response->contents, ap_response->length, connection);
      cgc_msls_send_msg(response);
      cgc_msls_destroy_msg(response);
      break;
    }
    case APPLICATION_TYPE_CLEAR_BOARD:
    {
      cgc_clear_message_board();
      cgc_SLS_MESSAGE *response = cgc_calloc(sizeof(cgc_SLS_MESSAGE));
      cgc_SLS_APPLICATION_MESSAGE *ap_response = cgc_calloc(sizeof(cgc_SLS_APPLICATION_MESSAGE));
      cgc_APPLICATION_RESPONSE_MSG *ar_msg = cgc_calloc(sizeof(cgc_APPLICATION_RESPONSE_MSG));
      response->type = SLS_TYPE_APPLICATION;
      response->version = SLS_VERSION;
      response->connection_id = msg->connection_id;
      response->message = (cgc_uint8_t *)ap_response;
      response->length = SLS_HEADER_LENGTH + SLS_APPLICATION_HEADER_LEN + sizeof(cgc_APPLICATION_RESPONSE_MSG);
      response->msg_length = SLS_APPLICATION_HEADER_LEN + sizeof(cgc_APPLICATION_RESPONSE_MSG);
      ap_response->type = APPLICATION_TYPE_RESPONSE;
      ap_response->length = sizeof(cgc_APPLICATION_RESPONSE_MSG);
      ap_response->contents = (cgc_uint8_t *)ar_msg;
      cgc_sprintf((char *)ar_msg->message, "Cleared Board\n", numMessages, APPLICATION_MAX_MESSAGES);
      cgc_msls_encrypt(ap_response->contents, ap_response->length, connection);
      cgc_msls_send_msg(response);
      cgc_msls_destroy_msg(response);
      break;
    }
    case APPLICATION_TYPE_READ_MESSAGE:
     {
      if (ap_msg->length < sizeof(cgc_APPLICATION_READ_MSG))
      {
        return;
      }
      cgc_msls_decrypt(((cgc_uint8_t *)ap_msg + SLS_APPLICATION_HEADER_LEN), ap_msg->length, connection);
      cgc_APPLICATION_READ_MSG *read_msg = (cgc_APPLICATION_READ_MSG *)((cgc_uint8_t *)ap_msg + SLS_APPLICATION_HEADER_LEN);

      cgc_debug_print("Reading slot $d\n", read_msg->msg_num);
      cgc_SLS_MESSAGE *response = cgc_calloc(sizeof(cgc_SLS_MESSAGE));
      cgc_SLS_APPLICATION_MESSAGE *ap_response = cgc_calloc(sizeof(cgc_SLS_APPLICATION_MESSAGE));
      cgc_APPLICATION_RESPONSE_MSG *ar_msg = cgc_calloc(sizeof(cgc_APPLICATION_RESPONSE_MSG));
      response->type = SLS_TYPE_APPLICATION;
      response->version = SLS_VERSION;
      response->connection_id = msg->connection_id;
      response->message = (cgc_uint8_t *)ap_response;
      response->length = SLS_HEADER_LENGTH + SLS_APPLICATION_HEADER_LEN + sizeof(cgc_APPLICATION_RESPONSE_MSG);
      response->msg_length = SLS_APPLICATION_HEADER_LEN + sizeof(cgc_APPLICATION_RESPONSE_MSG);
      ap_response->type = APPLICATION_TYPE_RESPONSE;
      ap_response->length = sizeof(cgc_APPLICATION_RESPONSE_MSG);
      ap_response->contents = (cgc_uint8_t *)ar_msg;
      if (read_msg->msg_num >= APPLICATION_MAX_MESSAGES)
      {
        cgc_sprintf((char *)ar_msg->message, "INVALID MESSAGE");
      } else {
        cgc_memcpy((char *)ar_msg->message, APPLICATION_MSG_BOARD[read_msg->msg_num], APPLICATION_MSG_LENGTH);
      }
      cgc_msls_encrypt(ap_response->contents, ap_response->length, connection);
      cgc_msls_send_msg(response);
      cgc_msls_destroy_msg(response);
      break;
    }
    default:
      return;
  }

}

void cgc_clear_message_board()
{
  cgc_memset(APPLICATION_MSG_BOARD, 0, sizeof(APPLICATION_MSG_BOARD));
  numMessages = 0;
}


int cgc_delete_message(cgc_uint8_t msg_num)
{
  if (msg_num >= APPLICATION_MAX_MESSAGES)
  {
    return -1;
  }
  cgc_memset(APPLICATION_MSG_BOARD[msg_num], 0, APPLICATION_MSG_LENGTH);
  for (int i = msg_num; i < APPLICATION_MAX_MESSAGES - 1; i++)
  {
    cgc_memcpy(APPLICATION_MSG_BOARD[i], APPLICATION_MSG_BOARD[i+1], APPLICATION_MSG_LENGTH);
  }
  numMessages--;
  return 1;
}

int cgc_post_new_message(cgc_uint8_t *message)
{
  if (numMessages >= APPLICATION_MAX_MESSAGES)
  {
    return -1;
  }
  cgc_memcpy(APPLICATION_MSG_BOARD[numMessages], message, APPLICATION_MSG_LENGTH);
  numMessages++;
  return 1;
}

