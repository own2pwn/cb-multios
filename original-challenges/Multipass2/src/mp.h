/*
 * Copyright (C) Narf Industries <info@narfindustries.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
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
#include "service.h"

#define FIRST_CARD_ID 			223344
#define FIRST_CARD_AUTH_CODE	114466
#define FIRST_TRANSACTION_ID	191919

typedef enum {
	TRANSIT = 0,	// 0
	VENDING,		// 1
	CONVENIENCE,	// 2
	RESTAURANT,		// 3
	GAS,			// 4
	MOVIE,			// 5
	CAR 			// 6
} cgc_VENDOR_TYPE;

typedef enum {
	PURCHASE = 0,	// 0
	RECHARGE,		// 1
	BALANCE,		// 2
	HISTORY,		// 3
	ISSUE,			// 4
	REFUND			// 5
} cgc_OP_CODE;

typedef enum {
	INIT = 0,	// 0
	AUTH,		// 1
	OPS,		// 2
	FIN 		// 3
} cgc_PKT_TYPE;


typedef struct vendor {
	cgc_uint32_t vendor_id;			
	cgc_VENDOR_TYPE vendor_type : 8;
	cgc_uint8_t vendor_location_sz;
	char * vendor_location;
} cgc_vendor_t;

typedef struct packet_head {
	cgc_uint32_t card_id;			// 4 bytes
	cgc_uint32_t auth_code;			// 4 bytes
	cgc_PKT_TYPE pkt_type : 8;		// 1 byte
	cgc_OP_CODE op_code : 8;		// 1 byte
	cgc_STATUS status : 8;			// 1 byte
	cgc_uint32_t transaction_id;	// 4 bytes
} cgc_packet_head_t;

typedef struct packet_data_error {
	cgc_uint32_t length;
	char * msg;
} cgc_packet_data_error_t;

typedef struct packet_data_issue {
	cgc_uint32_t amount;
} cgc_packet_data_issue_t;

typedef struct packet_data_balance {
	cgc_uint32_t balance;
} cgc_packet_data_balance_t;

typedef struct packet_data_history {
	cgc_uint32_t count;
} cgc_packet_data_history_t;

typedef struct packet_data_purchase {
	cgc_uint32_t purchase_id;
	cgc_uint32_t cost;
	cgc_vendor_t v;
} cgc_packet_data_purchase_t;

typedef struct packet_data_recharge {
	cgc_uint32_t amount;
	cgc_vendor_t v;
} cgc_packet_data_recharge_t;

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

typedef struct card {
	cgc_uint32_t card_id;
	cgc_uint32_t auth_code;
	cgc_uint32_t balance;
} cgc_card_t;


int cgc_transaction_node_contains(struct node * nd, void * key_value);
int cgc_card_node_contains(struct node * nd, void * key_value);

int cgc_card_create(cgc_card_t ** card, cgc_uint32_t card_count, cgc_uint32_t initial_amount);
int cgc_card_add_to_list(cgc_card_t * card, cgc_list_t * card_list);
int cgc_card_create_and_add_to_list(cgc_card_t ** card, cgc_list_t * card_list, cgc_uint32_t initial_amount);
int cgc_card_get(cgc_card_t ** card, cgc_uint32_t card_id, cgc_list_t * card_list);
int cgc_card_is_valid(cgc_uint32_t card_id, cgc_uint32_t auth_code, cgc_list_t * card_list);

int cgc_card_purchase(cgc_uint32_t card_id, cgc_list_t * card_list, cgc_uint32_t amount);
int cgc_card_recharge(cgc_uint32_t card_id, cgc_list_t * card_list, cgc_uint32_t amount);
int cgc_card_get_balance(cgc_uint32_t card_id, cgc_list_t * card_list, cgc_uint32_t * balance);
int cgc_card_refund(cgc_uint32_t card_id, cgc_list_t * card_list, cgc_transaction_t * tr);

int cgc_transaction_create(cgc_transaction_t ** tr, cgc_uint32_t tr_count, cgc_packet_head_t * pkt_hdr_ptr);
int cgc_transaction_add_to_list(cgc_transaction_t * tr, cgc_list_t * tr_list);
int cgc_transaction_create_and_add_to_list(cgc_transaction_t ** tr, cgc_list_t * tr_list, cgc_packet_head_t * pkt_hdr_ptr, cgc_uint32_t card_id);
int cgc_transaction_get(cgc_transaction_t ** tr, cgc_uint32_t transaction_id, cgc_OP_CODE op_code, cgc_list_t * tr_list);
void cgc_transaction_update_state(cgc_transaction_t ** tr, cgc_PKT_TYPE state);
void cgc_transaction_set_details(cgc_transaction_t ** tr, void * pd);

int cgc_transaction_get_and_verify_purchase_id(cgc_list_t * tr_list, cgc_packet_data_refund_t * pdr, cgc_transaction_t ** ref_tr);
int cgc_transaction_rm_purchase_details(cgc_transaction_t ** tr);
int cgc_transaction_rm_from_list(cgc_node_t ** tr, cgc_list_t * tr_list, cgc_uint32_t transaction_id);
int cgc_transaction_destroy(cgc_list_t * tr_list, cgc_uint32_t transaction_id);

int cgc_transaction_get_history_count_and_send(cgc_uint32_t card_id, cgc_list_t * tr_list, cgc_uint32_t max_tr_count);
int cgc_transaction_list_send_last_n(cgc_uint32_t card_id, cgc_list_t * tr_list, cgc_uint32_t tr_count);

int cgc_transaction_send_details(cgc_transaction_t * tr);
void cgc_transaction_send_purchase(void * details);
void cgc_transaction_send_recharge(void * details);
void cgc_transaction_send_balance(void * details);
void cgc_transaction_send_history(void * details);
void cgc_transaction_send_issue(void * details);
void cgc_transaction_send_refund(void * details);
void cgc_transaction_send_vendor_loc(cgc_vendor_t * v);

int cgc_transaction_recv_issue(cgc_packet_data_issue_t ** pdi);
int cgc_transaction_recv_purchase(cgc_packet_data_purchase_t ** pdp);
int cgc_transaction_recv_recharge(cgc_packet_data_recharge_t ** pdr);
int cgc_transaction_recv_balance(cgc_packet_data_balance_t ** pdb);
int cgc_transaction_recv_history(cgc_packet_data_history_t ** pdh);
int cgc_transaction_recv_refund(cgc_packet_data_refund_t ** pdr);


int cgc_transaction_init_done(cgc_transaction_t ** tr, cgc_packet_head_t * pkt_hdr_ptr, cgc_list_t * tr_list);
int cgc_transaction_authd(cgc_transaction_t ** tr, cgc_packet_head_t * pkt_hdr_ptr, cgc_list_t * tr_list);
int cgc_transaction_ops_done(cgc_transaction_t ** tr, cgc_packet_head_t * pkt_hdr_ptr, cgc_list_t * tr_list);

int cgc_packet_from_transaction(cgc_transaction_t * tr, cgc_packet_head_t ** pkt_hdr, cgc_uint32_t auth_code);
int cgc_packet_from_transaction_and_send(cgc_transaction_t * tr, cgc_uint32_t auth_code);
 #endif