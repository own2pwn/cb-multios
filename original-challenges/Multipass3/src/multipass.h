/*
 * Copyright (C) Narf Industries <info@narfindustries.com>
 *
 * Permission is hereby granted, cgc_free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef MULTIPASS_H
#define MULTIPASS_H
#define FIRST_CARD_ID 			223344
#define FIRST_CARD_AUTH_CODE	114466
#define FIRST_TRANSACTION_ID	191919
#include "libc.h"

/* Types of Vendors */
typedef enum {
	TRANSIT = 0,	// 0
	VENDING,		// 1
	CONVENIENCE,	// 2
	RESTAURANT,		// 3
	GAS,			// 4
	MOVIE,			// 5
	CAR 			// 6

} cgc_VENDOR_TYPE;

/* Types of Operations */
typedef enum {
	PURCHASE = 0,	// 0
	RECHARGE,		// 1
	BALANCE,		// 2
	HISTORY,		// 3
	ISSUE,			// 4
	REFUND			// 5
} cgc_OP_CODE;

/* Types of Packets */
typedef enum {
	INIT = 0,	// 0
	AUTH,		// 1
	OPS,		// 2
	FIN 		// 3
} cgc_PKT_TYPE;

/* Types of Errors */
#define LIST_PUSH_MSG "Failed to extend list."
#define LIST_RM_MSG "Failed to remove node from list."
#define ALLOC_MSG "Failed to allocate memory."
#define DEALLOC_MSG "Failed to deallocate memory."
#define PURCHASE_ISF_MSG "Insuffient funds for purchase."
#define RECHARGE_FULL_MSG "Unable to recharge card. Balance near max."
#define REFUND_FULL_MSG "Unable to refund card. Balance near max."
#define NO_HISTORY_MSG "No matching transaction history."
#define INVALID_OPCODE_MSG "Invalid cgc_OP_CODE."
#define INVALID_PKT_TYPE_MSG "Invalid cgc_PKT_TYPE."
#define INVALID_V_TYPE_MSG "Invalid cgc_VENDOR_TYPE."
#define NO_AUTH_MSG "Transaction has not completed AUTH."
#define NO_INIT_MSG "Transaction has not completed INIT."
#define NO_OPS_MSG "Transaction has not completed OPS."
#define NOT_FOUND_MSG "Card or transaction not found."
#define INVALID_CARD_MSG "Invalid card or auth code."
#define RECV_FAIL_MSG "Receive failed."
#define UNK_ERROR_MSG "Unknown Error."


// this enum is used as 1 byte (0-255)
typedef enum {
	OK = 0,
	ERRNO_MP_LIST_PUSH,		// Failed to add node to list
	ERRNO_MP_LIST_RM,		// Failed to rm node from list
	ERRNO_MP_ALLOC,			// Failed to allocate memory
	ERRNO_MP_DEALLOC,		// Failed to deallocate memory
	ERRNO_MP_PURCHASE_ISF,		// Insufficient Funds
	ERRNO_MP_RECHARGE_FULL,	// Recharge would case card balance to wrap
	ERRNO_MP_REFUND_FULL,	// Refund would cause card balance to wrap
	ERRNO_MP_NO_HISTORY,	// No matching transaction history in transaction list.
	ERRNO_MP_INVALID_OPCODE,	// Invalid Op_code
	ERRNO_MP_INVALID_P_TYPE,	// Invalid Packet Type
	ERRNO_MP_INVALID_V_TYPE,	// Invalid Vendor Type
	ERRNO_MP_NO_AUTH,		// Transaction did not complete AUTH step
	ERRNO_MP_NO_INIT,		// Transaction did not complete INIT step
	ERRNO_MP_NO_OPS,		// Transaction did not complete OPS step
	ERRNO_MP_NOT_FOUND,		// When the card or transaction is not in the list
	ERRNO_MP_INVALID_CARD,	// Card does not exist or is invalid
	ERRNO_MP_RECV,			// Failed to receive expected number of bytes
	ERRNO_MP_UNK			// Unknown error
} cgc_STATUS;

static char * ERROR_MSGS[] = {
	"NOERROR",
	LIST_PUSH_MSG,
	LIST_RM_MSG,
	ALLOC_MSG,
	DEALLOC_MSG,
	PURCHASE_ISF_MSG,
	RECHARGE_FULL_MSG,
	REFUND_FULL_MSG,
	NO_HISTORY_MSG,
	INVALID_OPCODE_MSG,
	INVALID_PKT_TYPE_MSG,
	INVALID_V_TYPE_MSG,
	NO_AUTH_MSG,
	NO_INIT_MSG,
	NO_OPS_MSG,
	NOT_FOUND_MSG,
	INVALID_CARD_MSG,
	RECV_FAIL_MSG,
	UNK_ERROR_MSG,
};



/* 
  Headers and Data

  The Common Packet Header is sent between client and server
  for all communication.

  The Data values are used in client-server communications
  as well as in the transaction log.
*/

/* Common Packet Header */
typedef struct packet_head {
	cgc_uint32_t card_id;			// 4 bytes
	cgc_uint32_t auth_code;			// 4 bytes
	cgc_PKT_TYPE pkt_type : 8;		// 1 byte
	cgc_OP_CODE op_code : 8;		// 1 byte
	cgc_STATUS status : 8;			// 1 byte
	cgc_uint32_t transaction_id;	// 4 bytes
} cgc_packet_head_t;

/* Error Data */
typedef struct packet_data_error {
	cgc_uint32_t length;
	char * msg;
} cgc_packet_data_error_t;

/* Vendor Data */
typedef struct vendor {
	cgc_uint32_t vendor_id;			
	cgc_VENDOR_TYPE vendor_type : 8;
	cgc_uint8_t vendor_location_sz;
	char * vendor_location;
} cgc_vendor_t;

/* Issue Data */
typedef struct packet_data_issue {
	cgc_uint32_t amount;
} cgc_packet_data_issue_t;

/* Balance Data */
typedef struct packet_data_balance {
	cgc_uint32_t balance;
} cgc_packet_data_balance_t;

/* History Data */
typedef struct packet_data_history {
	cgc_uint32_t count;
} cgc_packet_data_history_t;

/* Purchase Data */
typedef struct packet_data_purchase {
	cgc_uint32_t purchase_id;
	cgc_uint32_t cost;
	cgc_vendor_t v;
} cgc_packet_data_purchase_t;

/* Recharge Data */
typedef struct packet_data_recharge {
	cgc_uint32_t amount;
	cgc_vendor_t v;
} cgc_packet_data_recharge_t;

/* Refund Data */
typedef struct packet_data_refund {
	cgc_uint32_t purchase_id;
	cgc_uint32_t transaction_id;
} cgc_packet_data_refund_t;




typedef struct transaction {
        cgc_OP_CODE op_code : 8;
        cgc_PKT_TYPE state : 8;
        cgc_STATUS status : 8;
        cgc_uint32_t card_id;
        cgc_uint32_t transaction_id;
        void * details;
} cgc_transaction_t;


extern cgc_uint32_t NEXT_CARD_ID;
extern cgc_uint32_t NEXT_TXN_ID;
extern cgc_uint32_t NEXT_AUTH_CODE;
// used when we return null for some packet ...
extern cgc_uint32_t ERRNO;
extern size_t HIST_SZ;

typedef struct pack_and_data {
    cgc_packet_head_t *ph;
    size_t pay_data_l;

    void * data;
} cgc_pack_and_data_t;





int cgc_recv_header(cgc_packet_head_t *p);
void cgc_init_resp_structs();
cgc_pack_and_data_t * cgc_generate_new_init_and_init_resp(cgc_pack_and_data_t *pin);
cgc_packet_head_t * cgc_alloc_new_pkt();
cgc_pack_and_data_t * cgc_process_client_fin(cgc_pack_and_data_t *fin_pkt);
cgc_pack_and_data_t * cgc_create_auth_resp(cgc_uint32_t cid, cgc_uint32_t acd, cgc_uint32_t txn_id);
cgc_packet_head_t * cgc_create_basic_packet_head(cgc_uint32_t card_id, cgc_uint32_t auth_id, cgc_uint32_t txn_id, size_t payload_sz);
cgc_uint32_t cgc_register_card_id();
cgc_uint32_t cgc_register_transaction();
cgc_uint32_t cgc_register_auth_code();
cgc_pack_and_data_t * cgc_generate_error(cgc_packet_head_t *ph);
cgc_pack_and_data_t * cgc_process_client_purchase_op(cgc_pack_and_data_t * pin);
cgc_pack_and_data_t * cgc_process_client_recharge_op(cgc_pack_and_data_t *pad);
cgc_pack_and_data_t * cgc_process_client_balance(cgc_pack_and_data_t *pin);
cgc_pack_and_data_t * cgc_process_client_history(cgc_pack_and_data_t *padi, size_t *ds);
cgc_pack_and_data_t * cgc_process_client_refund_op(cgc_pack_and_data_t * pin);
cgc_pack_and_data_t * cgc_process_client_auth(cgc_pack_and_data_t * pin);
#endif