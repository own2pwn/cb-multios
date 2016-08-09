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
 
#ifndef ACCOUNT_H
#define ACCOUNT_H
#include "ll.h"
#include "multipass.h"

#define TXN_OPEN 1
#define TXN_CLOSED 0

typedef struct account_entry {
	cgc_uint32_t card_id;
	cgc_uint32_t auth_code;
	cgc_uint32_t initial_balance;
	cgc_uint32_t balance;
	cgc_alloc_head_t *txn_log;

} cgc_account_entry_t;

typedef struct accounts_listing {
	cgc_alloc_head_t *accounts;
	cgc_uint32_t n_entries;

} cgc_accounts_listing_t;

typedef struct txn_entry {
	char is_open;
	// duplicate, but easier to reference
	cgc_uint32_t transaction_id;
	size_t data_sz;
	cgc_packet_head_t *p;
	cgc_PKT_TYPE state;
	void * data;

} cgc_txn_entry_t;

typedef struct acct_txn_entry{
	cgc_account_entry_t *ae;
	cgc_txn_entry_t *txn;
}cgc_acct_txn_entry_t;

cgc_txn_entry_t * cgc_get_transaction(cgc_account_entry_t *ae, cgc_uint32_t txn_id);
cgc_account_entry_t * cgc_get_account(cgc_uint32_t card_id, cgc_uint32_t auth_code);
int cgc_create_account(cgc_uint32_t card_id,	cgc_uint32_t auth_code, cgc_packet_data_balance_t *b);
void cgc_init_account_register();
cgc_txn_entry_t * cgc_open_txn(cgc_account_entry_t *ae, cgc_packet_head_t * ph);
size_t cgc_get_size_of_data_for_pkt_head_only(cgc_packet_head_t *ph, cgc_uint32_t is_client);
cgc_txn_entry_t * cgc_add_transaction_log_entry(cgc_account_entry_t * ae,  cgc_pack_and_data_t *pad);
cgc_txn_entry_t * cgc_transaction_iterator(cgc_account_entry_t *ae);
int cgc_finalize_transaction(cgc_txn_entry_t * te);
cgc_txn_entry_t * cgc_remove_transaction_log_entry(cgc_account_entry_t * ae, cgc_txn_entry_t * ti);

#endif