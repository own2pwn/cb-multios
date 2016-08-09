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

#include "mp.h"

cgc_uint32_t next_card_id = FIRST_CARD_ID;
cgc_uint32_t next_card_auth_code = FIRST_CARD_AUTH_CODE;
cgc_uint32_t next_transaction_id = FIRST_TRANSACTION_ID;

// need function for transaction node comparison in the list.
//  -- should return SUCCESS when node nd contains value, 1 when not.
int cgc_transaction_node_contains(cgc_node_t * nd, void * key_value) {
	cgc_transaction_t * tr = (cgc_transaction_t *)nd->data;
	cgc_uint32_t * transaction_id = (cgc_uint32_t *)key_value;
	if (tr->transaction_id == *transaction_id) {
		return SUCCESS;
	} else {
		return 1;
	}
}

// need function for card node comparison in the list.
//  -- should return SUCCESS when node nd contains value, 1 when not.
int cgc_card_node_contains(cgc_node_t * nd, void * key_value) {
	cgc_card_t * c = (cgc_card_t *)nd->data;
	cgc_uint32_t * card_id = (cgc_uint32_t *)key_value;
	if (c->card_id == *card_id) {
		return SUCCESS;
	} else {
		return 1;
	}
}

// create a new card using the inital amount.
// the card_id starts at FIRST_CARD_ID and increments by 1.
// card_count is the number of cards in the card list.
// success: SUCCESS
// failure: ERRNO_MP_ALLOC
int cgc_card_create(cgc_card_t ** card, cgc_uint32_t card_count, cgc_uint32_t initial_amount) {
	ALLOC(sizeof(cgc_card_t), 0, (void **)card);
	(*card)->balance = initial_amount;
	(*card)->card_id = next_card_id;
	(*card)->auth_code = next_card_auth_code;
	next_card_id++;
	next_card_auth_code++;
	return SUCCESS;
}

// add the card to the card_list
//  success: SUCCESS
//  failure: ERRNO_MP_LIST_PUSH
int cgc_card_add_to_list(cgc_card_t * card, cgc_list_t * card_list) {
	cgc_node_t * nd = cgc_node_create((void *)card);
	if(SUCCESS != cgc_list_push(card_list, nd, (void *)&(card->card_id))) {return ERRNO_MP_LIST_PUSH;}

	return SUCCESS;
}

// create a new card with the given inital_amount as the balance and add it to the card_list
// success: SUCCESS
// failure: ERRNO_MP_ALLOC, ERRNO_MP_LIST_PUSH
int cgc_card_create_and_add_to_list(cgc_card_t ** card, cgc_list_t * card_list, cgc_uint32_t initial_amount) {
	// create card
    RET_ON_FAIL(cgc_card_create(card, card_list->count, initial_amount));

    // add card to list
    RET_ON_FAIL(cgc_card_add_to_list(*card, card_list));

    return SUCCESS;
}

// find card in card_list having card_id and return ptr to it via card
// success: SUCCESS
// failure: ERRNO_MP_NOT_FOUND if that card_id is not found; in that case card is undefined.
int cgc_card_get(cgc_card_t ** card, cgc_uint32_t card_id, cgc_list_t * card_list) {
	cgc_node_t * c1 = NULL;
	if(NULL == (c1 = cgc_list_find(card_list, &card_id))) {return ERRNO_MP_NOT_FOUND;}
	*card = (cgc_card_t *)c1->data;

	return SUCCESS;
}

// determine if card is valid (exists and has matching auth_code)
// valid: SUCCESS
// not valid: ERRNO_MP_INVALID_CARD
int cgc_card_is_valid(cgc_uint32_t card_id, cgc_uint32_t auth_code, cgc_list_t * card_list) {
	cgc_card_t * card = NULL;
	if(SUCCESS != cgc_card_get(&card, card_id, card_list)) {return ERRNO_MP_INVALID_CARD;}
	if(card->auth_code != auth_code) {return ERRNO_MP_INVALID_CARD;}

	return SUCCESS;
}

// subtract amount from the balance of card with id card_id in card_list
// success: SUCCESS
// failure: ERRNO_MP_NOT_FOUND, ERRNO_MP_PURCHASE_ISF
int cgc_card_purchase(cgc_uint32_t card_id, cgc_list_t * card_list, cgc_uint32_t amount) {

	int ret = 0;
	// find card
	cgc_card_t * card = NULL;
	RET_ON_FAIL(cgc_card_get(&card, card_id, card_list));

	// update card balance if sufficient funds
	if(card->balance >= amount) {
		card->balance -= amount;
	} else {
		return ERRNO_MP_PURCHASE_ISF;
	}

	return SUCCESS;
}

// add amount to the balance of card with id card_id in card_list
// success: SUCCESS
// failure: ERRNO_MP_NOT_FOUND, ERRNO_MP_RECHARGE_FULL
int cgc_card_recharge(cgc_uint32_t card_id, cgc_list_t * card_list, cgc_uint32_t amount) {

	// find card having card_id
	cgc_card_t * card = NULL;
	RET_ON_FAIL(cgc_card_get(&card, card_id, card_list));

	// if balance will wrap around, return error.
	if(card->balance + amount >= amount) {
		// update card balance
		card->balance += amount;
	} else {
		return ERRNO_MP_RECHARGE_FULL;
	}

	return SUCCESS;
}

// look up the balance of the card with card_id and write it into balance.
// success: SUCCESS
// failure: ERRNO_MP_NOT_FOUND
int cgc_card_get_balance(cgc_uint32_t card_id, cgc_list_t * card_list, cgc_uint32_t * balance) {

	// find card having card_id
	cgc_card_t * card = NULL;
	RET_ON_FAIL(cgc_card_get(&card, card_id, card_list));

	// return card balance
	*balance = card->balance;

	return SUCCESS;
}

// refund amount from purchase transaction to card in card_list
// success: SUCCESS
// failure: ERRNO_MP_NOT_FOUND, ERRNO_MP_REFUND_FULL
int cgc_card_refund(cgc_uint32_t card_id, cgc_list_t * card_list, cgc_transaction_t * tr) {

	// verify card_id matches that used in the transaction
	if(card_id != tr->card_id) {
		RET_ON_FAIL(ERRNO_MP_NOT_FOUND);
	}

	// find card having card_id
	cgc_card_t * card = NULL;
	RET_ON_FAIL(cgc_card_get(&card, card_id, card_list));

	// get purchase out of transaction
	cgc_packet_data_purchase_t * p = (cgc_packet_data_purchase_t *)tr->details;

	// if balance will wrap around, return error.
	if(card->balance + p->cost >= p->cost) {
		// add purchase amount to card balance
		card->balance += p->cost;
	} else {
		return ERRNO_MP_REFUND_FULL;
	}	

	return SUCCESS;
}

// create a new transaction using the values from the packet header.
// the transaction_id starts at FIRST_TRANSACTION_ID and increments by 1.
// tr_count is the number of transactions in the transaction list.
// success: SUCCESS, 
// failure: ERRNO_MP_ALLOC
int cgc_transaction_create(cgc_transaction_t ** tr, cgc_uint32_t tr_count, cgc_packet_head_t * pkt_hdr_ptr) {
	ALLOC(sizeof(cgc_transaction_t), 0, (void **)tr);
	(*tr)->op_code = pkt_hdr_ptr->op_code;
	(*tr)->state = pkt_hdr_ptr->pkt_type;
	(*tr)->card_id = pkt_hdr_ptr->card_id;
	(*tr)->transaction_id = next_transaction_id;
	(*tr)->details = NULL;
	next_transaction_id++;
	return SUCCESS;
}

// add new transaction to the transaction list
// success: SUCCESS, 
// failure: ERRNO_MP_LIST_PUSH
int cgc_transaction_add_to_list(cgc_transaction_t * tr, cgc_list_t * tr_list) {
	cgc_node_t * nd = cgc_node_create((void *)tr);
	if(SUCCESS != cgc_list_push(tr_list, nd, (void *)&(tr->transaction_id))) {return ERRNO_MP_LIST_PUSH;}
	return SUCCESS;
}

// create a new transaction and add it to the transaction_list
// success: SUCCESS, 
// failure: ERRNO_MP_ALLOC, ERRNO_MP_LIST_PUSH
int cgc_transaction_create_and_add_to_list(cgc_transaction_t ** tr, 
										cgc_list_t * tr_list, 
										cgc_packet_head_t * pkt_hdr_ptr, 
										cgc_uint32_t card_id) {
	RET_ON_FAIL(cgc_transaction_create(tr, tr_list->count, pkt_hdr_ptr));

    // update transaction with new card id
    (*tr)->card_id = card_id;

    RET_ON_FAIL(cgc_transaction_add_to_list(*tr, tr_list));

    return SUCCESS;
}

// find transaction in tr_list having transaction_id and op_code, and return ptr to it via tr
// success: SUCCESS
// failure: ERRNO_MP_NOT_FOUND if that transaction_id is not found; in that case tr is undefined.
int cgc_transaction_get(cgc_transaction_t ** tr, cgc_uint32_t transaction_id, cgc_OP_CODE op_code, cgc_list_t * tr_list) {
	cgc_node_t * tr1 = NULL;
	if(NULL == (tr1 = cgc_list_find(tr_list, (void *)&transaction_id))) {return ERRNO_MP_NOT_FOUND;}
	*tr = (cgc_transaction_t *)tr1->data;
	if ((*tr)->op_code != op_code) {return ERRNO_MP_NOT_FOUND;}

	return SUCCESS;
}

// set the transaction state to a new value
void cgc_transaction_update_state(cgc_transaction_t ** tr, cgc_PKT_TYPE state) {
	(*tr)->state = state;
}

// assign the details content to the transaction
void cgc_transaction_set_details(cgc_transaction_t ** tr, void * pd) {
    (*tr)->details = pd;
}

// find the transaction in tr_list that was a purchase with purchase_id and return it in ref_tr
// success: SUCCESS
// failure: ERRNO_MP_NOT_FOUND
int cgc_transaction_get_and_verify_purchase_id(cgc_list_t * tr_list, cgc_packet_data_refund_t * pdr, cgc_transaction_t ** ref_tr) {
	RET_ON_FAIL(cgc_transaction_get(ref_tr, pdr->transaction_id, PURCHASE, tr_list));

	// verify purchase_id
	cgc_packet_data_purchase_t * p = (cgc_packet_data_purchase_t *)(*ref_tr)->details;
	if(p->purchase_id != pdr->purchase_id) {return ERRNO_MP_NOT_FOUND;}

	return SUCCESS;
}

// delete the purchase details from the transaction
// success: SUCCESS
// failure: ERRNO_MP_ALLOC
int cgc_transaction_rm_purchase_details(cgc_transaction_t ** tr) {

	cgc_packet_data_purchase_t * pdp = NULL;
    pdp = (cgc_packet_data_purchase_t *)(*tr)->details;

    // get purchase vendor location info to use in dealloc
    cgc_uint8_t vendor_loc_sz = pdp->v.vendor_location_sz;
    char * vendor_loc = pdp->v.vendor_location;

    // dealloc purchase_location
    if(SUCCESS != deallocate((void *)vendor_loc, vendor_loc_sz * sizeof(char))) {return ERRNO_MP_ALLOC;}

    // dealloc purchase and vendor details
    if(SUCCESS != deallocate((void *)pdp, sizeof(cgc_packet_data_purchase_t))) {return ERRNO_MP_ALLOC;}

    (*tr)->details = NULL;

    return SUCCESS;
}

// remove transaction node from the transaction list and return ptr to it in tr
// success: SUCCESS, 
// failure: ERRNO_MP_LIST_RM
int cgc_transaction_rm_from_list(cgc_node_t ** tr, cgc_list_t * tr_list, cgc_uint32_t transaction_id) {
	if(NULL == (*tr = cgc_list_remove(tr_list, (void *)&transaction_id))) {return ERRNO_MP_LIST_RM;}
	return SUCCESS;
}

// remove transaction node from transaction list, and dealloc
//  the data pointer to by transaction->details should be reset and deallocated before calling this function
// success: SUCCESS
// failure: ERRNO_MP_ALLOC, ERRNO_MP_LIST_RM
int cgc_transaction_destroy(cgc_list_t * tr_list, cgc_uint32_t transaction_id) {
    //rm transaction node from list and get ptr to transaction node
    cgc_node_t * tr_node = NULL;
    RET_ON_FAIL(cgc_transaction_rm_from_list(&tr_node, tr_list, transaction_id));
    cgc_transaction_t * tr = (cgc_transaction_t *)tr_node->data;

    // deallocate the memory used by the transaction
    if(SUCCESS != deallocate((void *)tr, sizeof(cgc_transaction_t))) {return ERRNO_MP_ALLOC;}
   
    //destroy node
    RET_ON_FAIL(cgc_node_destroy(tr_node));

    return SUCCESS;
}

// for the HISTORY op_code, determine how many transactions will be sent by cgc_transaction_list_send_last_n()
//  create a packet_data_history struct and cgc_send it with the count value equal to the number of
//  transactions that will be sent.
// success: SUCCESS
// failure: ERRNO_MP_NO_HISTORY
int cgc_transaction_get_history_count_and_send(cgc_uint32_t card_id, cgc_list_t * tr_list, cgc_uint32_t max_tr_count) {

	if(0 == tr_list->count) {return ERRNO_MP_NO_HISTORY;}

	int tr_send_count = 0;
	cgc_node_t * curr = tr_list->tail->prev;

	while(curr != tr_list->head && (max_tr_count > tr_send_count)) {

		// curr->data is the pointer to the transaction entry;
		// get transaction
		cgc_transaction_t * tr = (cgc_transaction_t *)curr->data;

		// if this transaction matches card_id, count it
		if(card_id == tr->card_id) {

			// increment tr_send_count
			tr_send_count++;
		}

		// get next node
		curr = curr->prev;
	}

	// create and cgc_send the history details
	cgc_packet_data_history_t pdh;
	pdh.count = tr_send_count;

	// cgc_send history details
	cgc_transaction_send_history(&pdh);

	return SUCCESS;
}

// for the HISTORY op_code, start at the end of the transaction_list and cgc_send up to tr_count
// transactions that match card_id.
// success: SUCCESS
// failure: ERRNO_MP_NO_HISTORY
int cgc_transaction_list_send_last_n(cgc_uint32_t card_id, cgc_list_t * tr_list, cgc_uint32_t tr_count) {

	if(0 == tr_list->count) {return ERRNO_MP_NO_HISTORY;}

	int tr_sent_count = 0;
	cgc_node_t * curr = tr_list->tail->prev;

	while(curr != tr_list->head && (tr_count > tr_sent_count)) {

		// curr->data is the pointer to the transaction entry;
		// get transaction
		cgc_transaction_t * tr = (cgc_transaction_t *)curr->data;

		// if this transaction matches card_id, cgc_send transaction
		if(card_id == tr->card_id) {

			// cgc_send transaction header except the details pointer.
		    cgc_send((char *)tr, sizeof(cgc_transaction_t) - sizeof(void *));

			// cgc_send transaction details
			cgc_transaction_send_details(tr);

			// increment tr_sent_count
			tr_sent_count++;
		}

		// get next node
		curr = curr->prev;
	}

	return SUCCESS;
}

// cgc_send transaction details (call func based on op_code).
// success: SUCCESS
// failure: ERRNO_MP_INVALID_OPCODE
int cgc_transaction_send_details(cgc_transaction_t * tr) {

#ifdef PATCHED
	// VULNERABLE CODE: the transaction does not have details in the AUTH phase
	if (tr->state == AUTH) {
		return SUCCESS;
	}
#endif
	// get op_code to determine type of transaction
	switch(tr->op_code) {
		case PURCHASE :
			cgc_transaction_send_purchase(tr->details);
			break;
		case RECHARGE :
			cgc_transaction_send_recharge(tr->details);
			break;
		case BALANCE :
			cgc_transaction_send_balance(tr->details);
			break;
		case HISTORY :
			cgc_transaction_send_history(tr->details);
			break;
		case ISSUE :
			cgc_transaction_send_issue(tr->details);
			break;
		case REFUND :
			cgc_transaction_send_refund(tr->details);
			break;
		default :
			return ERRNO_MP_INVALID_OPCODE;
	}
	return SUCCESS;
}

// cgc_send purchase details
void cgc_transaction_send_purchase(void * details) {
	cgc_packet_data_purchase_t * pdp = (cgc_packet_data_purchase_t *)details;
    cgc_vendor_t * v = &(pdp->v);

    // sanity check to ensure cost wasn't zero
    if(pdp->cost > 0) { 
		// cgc_send first 2 fields in purchase + fixed vendor fields (not location)
	    cgc_send((char *)pdp, 14);
	    // cgc_send vendor info
	    cgc_transaction_send_vendor_loc(v);
    }
}

// cgc_send recharge details
void cgc_transaction_send_recharge(void * details) {
	cgc_packet_data_recharge_t * pdr = (cgc_packet_data_recharge_t *)details;
    cgc_vendor_t * v = &(pdr->v);

    // sanity check to ensure amount wasn't zero
    if(pdr->amount > 0) {
		// cgc_send first field in recharge + fixed vendor fields (not location)
	    cgc_send((char *)pdr, 10);
	    // cgc_send vendor info
	    cgc_transaction_send_vendor_loc(v);
	}
}

// cgc_send balance details
void cgc_transaction_send_balance(void * details) {
	cgc_packet_data_balance_t * pdb = (cgc_packet_data_balance_t *)details;
    cgc_send((char *)pdb, sizeof(cgc_packet_data_balance_t));
}

// cgc_send history details
void cgc_transaction_send_history(void * details) {
	cgc_packet_data_history_t * pdh = (cgc_packet_data_history_t *)details;
    cgc_send((char *)pdh, sizeof(cgc_packet_data_history_t));
}

// cgc_send issue details
void cgc_transaction_send_issue(void * details) {
	cgc_packet_data_issue_t * pdi = (cgc_packet_data_issue_t *)details;
    cgc_send((char *)pdi, sizeof(cgc_packet_data_issue_t));
}

// cgc_send refund details
void cgc_transaction_send_refund(void * details) {
	cgc_packet_data_refund_t * pdr = (cgc_packet_data_refund_t *)details;
    cgc_send((char *)pdr, sizeof(cgc_packet_data_refund_t));
}

// cgc_send vendor location details
void cgc_transaction_send_vendor_loc(cgc_vendor_t * v) {
    cgc_send((char *)v->vendor_location, v->vendor_location_sz);
}

// recv issue details
// success: SUCCESS
// failure: ERRNO_MP_ALLOC, ERRNO_MP_RECV
int cgc_transaction_recv_issue(cgc_packet_data_issue_t ** pdi) {
    // create memory for issue data
    ALLOC(sizeof(cgc_packet_data_issue_t), 0, (void **)pdi);
    if(sizeof(cgc_packet_data_issue_t) != cgc_recv_all((char *)*pdi, sizeof(cgc_packet_data_issue_t))) {return ERRNO_MP_RECV;}

    return SUCCESS;
}

// recv purchase details
// success: SUCCESS
// failure: ERRNO_MP_ALLOC, ERRNO_MP_RECV
int cgc_transaction_recv_purchase(cgc_packet_data_purchase_t ** pdp) {
    // create memory for purchase data + vendor
    ALLOC(sizeof(cgc_packet_data_purchase_t), 0, (void **)pdp);
    // read purchase data and vendor data (except vendor_location)
    if(14 != cgc_recv_all((char *)*pdp, 14)) {return ERRNO_MP_RECV;}

    cgc_uint32_t vls = (*pdp)->v.vendor_location_sz;
    if(vls == 0) {return ERRNO_MP_RECV;}

    // create memory for vendor_location char array
    ALLOC(vls, 0, (void **)&((*pdp)->v.vendor_location));
    // read vendor_location (size is in previous read)
    if(vls != cgc_recv_all((char *)(*pdp)->v.vendor_location, vls)) {return ERRNO_MP_RECV;}

    return SUCCESS;
}

// recv recharge details
// success: SUCCESS
// failure: ERRNO_MP_ALLOC, ERRNO_MP_RECV
int cgc_transaction_recv_recharge(cgc_packet_data_recharge_t ** pdr) {
    // create memory for recharge data + vendor
    ALLOC(sizeof(cgc_packet_data_recharge_t), 0, (void **)pdr);
    // read recharge data and vendor data (except vendor_location)
    if(10 != cgc_recv_all((char *)*pdr, 10)) {return ERRNO_MP_RECV;}

    cgc_uint32_t vls = (*pdr)->v.vendor_location_sz;
    if(vls == 0) {return ERRNO_MP_RECV;}

    // create memory for vendor_location char array
    ALLOC(vls, 0, (void **)&((*pdr)->v.vendor_location));
    // read vendor_location (size is in previous read)
    if(vls != cgc_recv_all((char *)(*pdr)->v.vendor_location, vls)) {return ERRNO_MP_RECV;}

    return SUCCESS;
}

// Allocate memory to store balance details
// success: SUCCESS
// failure: ERRNO_MP_ALLOC, ERRNO_MP_RECV
int cgc_transaction_recv_balance(cgc_packet_data_balance_t ** pdb) {
    // create memory for balance data
    ALLOC(sizeof(cgc_packet_data_balance_t), 0, (void **)pdb);

    return SUCCESS;
}

// recv history details
// success: SUCCESS
// failure: ERRNO_MP_ALLOC, ERRNO_MP_RECV
int cgc_transaction_recv_history(cgc_packet_data_history_t ** pdh) {
    // create memory for history data
    ALLOC(sizeof(cgc_packet_data_history_t), 0, (void **)pdh);
    if(sizeof(cgc_packet_data_history_t) != cgc_recv_all((char *)*pdh, sizeof(cgc_packet_data_history_t))) {return ERRNO_MP_RECV;}

    return SUCCESS;
}

// recv refund details
// success: SUCCESS
// failure: ERRNO_MP_ALLOC, ERRNO_MP_RECV
int cgc_transaction_recv_refund(cgc_packet_data_refund_t ** pdr) {
    // create memory for refund data
    ALLOC(sizeof(cgc_packet_data_refund_t), 0, (void **)pdr);
    if(sizeof(cgc_packet_data_refund_t) != cgc_recv_all((char *)*pdr, sizeof(cgc_packet_data_refund_t))) {return ERRNO_MP_RECV;}

    return SUCCESS;
}


// check the given transaction_id to verify there is an assocated transaction in the INIT state
//  having the same op_code
// return associated transaction via tr
// success: SUCCESS, 
// failure: ERRNO_MP_NOT_FOUND, ERRNO_MP_INVALID_CARD, ERRNO_MP_NO_INIT
int cgc_transaction_init_done(cgc_transaction_t ** tr, cgc_packet_head_t * pkt_hdr_ptr, cgc_list_t * tr_list) {
	RET_ON_FAIL(cgc_transaction_get(tr, pkt_hdr_ptr->transaction_id, pkt_hdr_ptr->op_code, tr_list));
	if(pkt_hdr_ptr->card_id != (*tr)->card_id) {return ERRNO_MP_INVALID_CARD;}
	if((*tr)->state != INIT) {return ERRNO_MP_NO_INIT;}

	return SUCCESS;
}

// check the given transaction_id to verify there is an assocated transaction in the AUTH state
//  having the same op_code
// return associated transaction via tr
// success: SUCCESS
// failure: ERRNO_MP_NOT_FOUND, ERRNO_MP_INVALID_CARD, ERRNO_MP_NO_AUTH
int cgc_transaction_authd(cgc_transaction_t ** tr, cgc_packet_head_t * pkt_hdr_ptr, cgc_list_t * tr_list) {
	RET_ON_FAIL(cgc_transaction_get(tr, pkt_hdr_ptr->transaction_id, pkt_hdr_ptr->op_code, tr_list));
	if(pkt_hdr_ptr->card_id != (*tr)->card_id) {return ERRNO_MP_INVALID_CARD;}
	if((*tr)->state != AUTH) {return ERRNO_MP_NO_AUTH;}

	return SUCCESS;
}

// check the given transaction id to verify there is an associated transaction in the OPS state
//  having the same op_code
// return associated transaction via tr
// success: SUCCESS
// failure: ERRNO_MP_NOT_FOUND, ERRNO_MP_INVALID_CARD, ERRNO_MP_NO_OPS
int cgc_transaction_ops_done(cgc_transaction_t ** tr, cgc_packet_head_t * pkt_hdr_ptr, cgc_list_t * tr_list) {
	RET_ON_FAIL(cgc_transaction_get(tr, pkt_hdr_ptr->transaction_id, pkt_hdr_ptr->op_code, tr_list));
	if(pkt_hdr_ptr->card_id != (*tr)->card_id) {return ERRNO_MP_INVALID_CARD;}
	if((*tr)->state != OPS) {return ERRNO_MP_NO_OPS;}

	return SUCCESS;
}

// create new packet header with values from transaction.
// success: SUCCESS
// failure: ERRNO_MP_ALLOC
int cgc_packet_from_transaction(cgc_transaction_t * tr, cgc_packet_head_t ** pkt_hdr, cgc_uint32_t auth_code) {
	ALLOC(sizeof(cgc_packet_head_t), 0, (void **)pkt_hdr);
	(*pkt_hdr)->card_id = tr->card_id;
	(*pkt_hdr)->auth_code = auth_code;
	(*pkt_hdr)->pkt_type = tr->state;
	(*pkt_hdr)->op_code = tr->op_code;
	(*pkt_hdr)->status = tr->status;
	(*pkt_hdr)->transaction_id = tr->transaction_id;
	return SUCCESS;
}

// create new packet header with values from transaction.
// and cgc_send packet header
// success: SUCCESS
// failure: ERRNO_MP_ALLOC
int cgc_packet_from_transaction_and_send(cgc_transaction_t * tr, cgc_uint32_t auth_code) {
    cgc_packet_head_t * new_pkt_header = NULL;
    RET_ON_FAIL(cgc_packet_from_transaction(tr, &new_pkt_header, auth_code));

    // cgc_send packet_header to client
    cgc_send((char *)new_pkt_header, sizeof(cgc_packet_head_t));

    // free memory for new_pkt_header.
    if(SUCCESS != deallocate((void *)new_pkt_header, sizeof(cgc_packet_head_t))) {return ERRNO_MP_ALLOC;}

    return SUCCESS;
}
