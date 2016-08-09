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

#include "multipass.h"
#include "account.h"
#include "sheap.h"

cgc_uint32_t NEXT_CARD_ID = FIRST_CARD_ID;
cgc_uint32_t NEXT_TXN_ID = FIRST_TRANSACTION_ID;
cgc_uint32_t NEXT_AUTH_CODE = FIRST_CARD_AUTH_CODE;
cgc_uint32_t ERRNO;
size_t HIST_SZ;

cgc_packet_head_t * RESP_PACKET;
cgc_pack_and_data_t * RESP_PAD;

// check *any* packet for account auth information, then 
cgc_account_entry_t * cgc_check_auth_packet(cgc_packet_head_t *p){
	assert(p->pkt_type == AUTH);

	return cgc_get_account(p->card_id, p->auth_code);


}


// verify init packet just for issue
int cgc_verify_init(cgc_packet_head_t *p){
	if(p == NULL)
		return ERRNO_MP_INVALID_P_TYPE;

	if(p->pkt_type != INIT)
		return ERRNO_MP_INVALID_P_TYPE;

	if(p->op_code != INIT)
		return ERRNO_MP_INVALID_OPCODE;

	return OK;
}

cgc_packet_head_t * cgc_alloc_new_pkt(size_t payload_sz){
	cgc_packet_head_t *p =  cgc_calloc(1, sizeof(cgc_packet_head_t)+ payload_sz);
	return p;
}

cgc_uint32_t cgc_register_card_id(){
	cgc_uint32_t t = NEXT_CARD_ID;
	NEXT_CARD_ID++;
	return t;
}

cgc_uint32_t cgc_register_transaction(){
	cgc_uint32_t t = NEXT_TXN_ID;
	NEXT_TXN_ID++;
	return t;
}

cgc_uint32_t cgc_register_auth_code(){
	cgc_uint32_t t = NEXT_AUTH_CODE;
	NEXT_AUTH_CODE++;
	return t;
}

void cgc_init_resp_structs(){
	RESP_PAD = cgc_malloc(sizeof(cgc_pack_and_data_t));
	RESP_PACKET = cgc_alloc_new_pkt(2048);
	assert(RESP_PACKET != NULL);
	assert(RESP_PAD != NULL);
}

cgc_packet_head_t * cgc_clear_gen_pkt(size_t payload_len){
	char * pdata = (char *) RESP_PACKET;
	for(int i =0; i < sizeof(cgc_packet_head_t) + payload_len; ++i){
		pdata[i] = 0;
	}
	return RESP_PACKET;

}

cgc_pack_and_data_t * cgc_clear_pad(){
	RESP_PAD->ph = NULL;
	RESP_PAD->pay_data_l = 0;
	RESP_PAD->data = NULL;
	return RESP_PAD;
}

cgc_packet_head_t * cgc_create_basic_packet_head(cgc_uint32_t card_id, cgc_uint32_t auth_code, cgc_uint32_t txn_id, size_t payload_len){
	cgc_packet_head_t *h = cgc_clear_gen_pkt(payload_len);
	if(h == NULL){
		ERRNO = ERRNO_MP_ALLOC;
		return NULL;
	}
	h->card_id = card_id;
	h->auth_code= auth_code;
	h->transaction_id = txn_id;

	return h;

}

cgc_pack_and_data_t * cgc_create_basic_response(cgc_uint32_t card_id, cgc_uint32_t auth_code, cgc_uint32_t txn_id, cgc_PKT_TYPE t, cgc_OP_CODE o, cgc_STATUS s, size_t payload_len){
	cgc_packet_head_t * resp = cgc_create_basic_packet_head(card_id, auth_code, txn_id, payload_len);
	if( resp == NULL)
		return NULL;
	cgc_pack_and_data_t * pad = cgc_clear_pad();
	if(pad == NULL)
		return NULL;

	resp->pkt_type = t;
	resp->op_code = o;
	resp->status = s;
	if(payload_len == 0){
		pad->data = NULL;
	}else{
		pad->data = resp+1;
	}
	pad->ph = resp;
	pad->pay_data_l = payload_len;
	return pad;
}




cgc_pack_and_data_t * cgc_generate_new_init_and_init_resp(cgc_pack_and_data_t *pin){
	cgc_packet_head_t *ip = pin->ph;
	cgc_packet_data_balance_t *b = pin->data;
	
	if(b == NULL){

		return NULL;
	}


	cgc_uint32_t cid = cgc_register_card_id();
	cgc_uint32_t acd = cgc_register_auth_code();

	
	
	
	
	int acct_ok = cgc_create_account(cid, acd, b);

	if(acct_ok != OK){
		return NULL;
	}



	cgc_account_entry_t *ae = cgc_get_account(cid, acd);
	if(ae == NULL){
		return NULL;
	}


	cgc_open_txn(ae, ip);
	
	// needed for log entry
	pin->ph->card_id = cid;
	pin->ph->auth_code = acd;
	cgc_txn_entry_t * t = cgc_add_transaction_log_entry(ae, pin); 

	if(t == NULL){
		return NULL;
	}

	return cgc_create_basic_response(cid, acd, ip->transaction_id, INIT, ISSUE, OK, 0);
	
}





cgc_pack_and_data_t * cgc_error_out_and_free_txn(cgc_acct_txn_entry_t * aet, cgc_uint32_t erno){
	if(erno != 0){
		ERRNO = erno;	
	}
	
	cgc_free(aet);

	return NULL;
}

cgc_acct_txn_entry_t * cgc_get_acct_txn_reg(cgc_uint32_t cid, cgc_uint32_t acd, cgc_uint32_t txn_id){

	cgc_account_entry_t * ae = cgc_get_account(cid, acd);
	


	if(ae == NULL){
		ERRNO = ERRNO_MP_NOT_FOUND;
		return NULL;
	}

	cgc_txn_entry_t * te = cgc_get_transaction(ae, txn_id);
	if(te == NULL){
		ERRNO = ERRNO_MP_NOT_FOUND;
		return NULL;
	}

	cgc_acct_txn_entry_t *aet = (cgc_acct_txn_entry_t *) cgc_malloc(sizeof(cgc_acct_txn_entry_t));
	if(aet == NULL){
		ERRNO = ERRNO_MP_ALLOC;
		return NULL;
	}
	aet->ae = ae;
	aet->txn = te;

	return aet;


};

size_t cgc_calc_payload_sz(cgc_packet_head_t *ph){
	if(ph->pkt_type == OPS){

		if(ph->op_code == BALANCE)
			return sizeof(cgc_packet_data_balance_t);
	}
	return 0;	
}


void * cgc_get_payload(cgc_packet_head_t *ph){

	return (void *) (ph+1);

}


cgc_pack_and_data_t * cgc_process_client_history(cgc_pack_and_data_t *padi, size_t *ds){
	cgc_packet_head_t *cb_hist = padi->ph;
	cgc_packet_data_history_t *hist = (cgc_packet_data_history_t *) padi->data;
	cgc_uint32_t cid = cb_hist->card_id;
	cgc_uint32_t acd = cb_hist->auth_code;
	cgc_uint32_t txn_id = cb_hist->transaction_id;


	cgc_account_entry_t * ae = cgc_get_account(cid, acd);
	cgc_txn_entry_t * te = cgc_add_transaction_log_entry(ae, padi);
	cgc_acct_txn_entry_t * aet = cgc_get_acct_txn_reg(cid, acd, txn_id);

	if(aet == NULL){
		return NULL;
	}
	


	
	size_t n_transactions = aet->ae->txn_log->n_nodes;
	size_t data_sz =0;
	int i =0;
	if(cb_hist->transaction_id == 0x0002ee11)
		i = 0;

	for(cgc_txn_entry_t *txn = cgc_transaction_iterator(aet->ae); txn != NULL; 
		txn = cgc_transaction_iterator(NULL)){
		if(txn == NULL)
			break;


		if(txn->p->card_id != cid)
			continue;

		data_sz += txn->data_sz;
		if(i >= hist->count){
			break;
		}
		i += 1;

	}
	if(padi->ph->transaction_id == 223352)
		i = i;

	// complex size calculation takes into account void * space for each txn t

	size_t resp_size =  sizeof(cgc_packet_data_history_t) + data_sz + sizeof(cgc_transaction_t)*i - sizeof(void *)*i;
	cgc_pack_and_data_t *resp = cgc_create_basic_response(cid, acd, txn_id, OPS, HISTORY, OK, resp_size);
	if(resp == NULL){

		return NULL;
	}
	cgc_packet_head_t *resp_ph = resp->ph;
	cgc_packet_data_history_t *phptr = (cgc_packet_data_history_t *) (resp->data);
	resp_ph->op_code = HISTORY;
	cgc_transaction_t * txn_ptr = (cgc_transaction_t *) (phptr+1);

	size_t hist_ct = 0;
	te->state = OPS;
	size_t final_data_sz =  0;
	for(cgc_txn_entry_t *txn = cgc_transaction_iterator(aet->ae); txn != NULL; 
		txn = cgc_transaction_iterator(NULL)){

		if(txn == NULL)
			break;

		if(txn->p->card_id != cid)
			continue;
		if(hist_ct >= hist->count)
			break;


		txn_ptr->op_code = txn->p->op_code;
		txn_ptr->state = txn->state;

		txn_ptr->status = txn->p->status;
		txn_ptr->card_id = txn->p->card_id;
		txn_ptr->transaction_id = txn->p->transaction_id;


		size_t dss = txn->data_sz;

#if PATCHED
		if(txn->data == NULL)
			_terminate(26);
#endif
		cgc_memcpy(&(txn_ptr->details), txn->data, dss);
		hist_ct += 1;
		

		// sizeof txn_t already has void ptr which is where we're copying data...
		size_t cp_sz = sizeof(cgc_transaction_t) + dss - sizeof(void *);
		final_data_sz += cp_sz;

		txn_ptr =  ((void *) txn_ptr) +cp_sz;	

		

		
	}


	*ds =  final_data_sz+4;
	resp->pay_data_l = final_data_sz+4;

	phptr->count = i;
	cgc_finalize_transaction(te);



	return resp;

}


cgc_pack_and_data_t * cgc_process_client_balance(cgc_pack_and_data_t *pin){
	cgc_packet_head_t *cb_bal = pin->ph;
	cgc_uint32_t cid = cb_bal->card_id;
	cgc_uint32_t acd = cb_bal->auth_code;
	cgc_uint32_t txn_id = cb_bal->transaction_id;


	cgc_acct_txn_entry_t * aet = cgc_get_acct_txn_reg(cid, acd, txn_id);

	if(aet == NULL){

		return NULL;
	}

	cgc_pack_and_data_t *pad = cgc_create_basic_response(cid, acd, txn_id, OPS, BALANCE, OK, sizeof(cgc_packet_data_balance_t));
	cgc_packet_head_t * ph = pad->ph;
	

	cgc_packet_data_balance_t *inline_bal = cgc_get_payload(ph);

	if(inline_bal == NULL){
		ERRNO = ERRNO_MP_UNK;

		return NULL;
	}
	inline_bal->balance = aet->ae->balance;

	cgc_txn_entry_t * te = cgc_add_transaction_log_entry(aet->ae, pad);
	cgc_free(aet);

	return pad;

}


cgc_pack_and_data_t * cgc_process_client_refund_op(cgc_pack_and_data_t * pin){
	cgc_packet_head_t *rf_pkt = pin->ph;
	cgc_packet_data_refund_t *rcd = pin->data;
	cgc_uint32_t cid = rf_pkt->card_id;
	cgc_uint32_t acd = rf_pkt->auth_code;
	cgc_uint32_t txn_id = rf_pkt->transaction_id;


	cgc_acct_txn_entry_t * aet = cgc_get_acct_txn_reg(cid, acd, txn_id);



	if(aet == NULL){
		return NULL;
	}


	cgc_txn_entry_t * ref_txn = cgc_get_transaction(aet->ae, rcd->transaction_id);
	if(ref_txn == NULL){
		ERRNO = ERRNO_MP_NOT_FOUND;
		return NULL;
	}

	cgc_packet_data_purchase_t *log_pur_data = ref_txn->data;

#if PATCHED
	if(log_pur_data == NULL){
		return NULL;
	}
#endif


	if(log_pur_data->purchase_id != rcd->purchase_id){


		// the purchase id refund request doesn't match up...

		ERRNO = ERRNO_MP_NOT_FOUND;
		return NULL;
	}

	cgc_uint32_t proposed_post_refund = aet->ae->balance + log_pur_data->cost;
	if(proposed_post_refund <= 0xFFFFFFFF && proposed_post_refund >= aet->ae->balance){
		aet->ae->balance = proposed_post_refund;

	}else{
		ERRNO = ERRNO_MP_REFUND_FULL;
		return NULL;
	}

	cgc_add_transaction_log_entry(aet->ae, pin);
	
	cgc_remove_transaction_log_entry(aet->ae,ref_txn);
	cgc_free(aet);
	return cgc_create_basic_response(cid, acd, txn_id, OPS, REFUND, OK, 0);
}


cgc_pack_and_data_t * cgc_process_client_recharge_op(cgc_pack_and_data_t *pad){
	cgc_packet_data_recharge_t *rcd = pad->data;
	cgc_packet_head_t *rc_pkt = pad->ph;
	cgc_uint32_t cid = rc_pkt->card_id;
	cgc_uint32_t acd = rc_pkt->auth_code;
	cgc_uint32_t txn_id = rc_pkt->transaction_id;


	cgc_acct_txn_entry_t * aet = cgc_get_acct_txn_reg(cid, acd, txn_id);
		



	if(aet == NULL){	

		return NULL;
	}
	// check that we have a valid vendor type enum

	if(rcd->v.vendor_type > CAR){
		return cgc_error_out_and_free_txn(aet, ERRNO_MP_INVALID_V_TYPE);
	}



	
	cgc_uint32_t proposed_amt = rcd->amount + aet->ae->balance;
	

	if( proposed_amt  <= 0xFFFFFFFF  && proposed_amt >= aet->ae->balance){
		aet->ae->balance = proposed_amt;

	}else{
		return cgc_error_out_and_free_txn(aet, ERRNO_MP_RECHARGE_FULL);
	}


	cgc_add_transaction_log_entry(aet->ae, pad);

	cgc_free(aet);
	return cgc_create_basic_response(cid, acd, txn_id, OPS, RECHARGE, OK, 0);
}



cgc_pack_and_data_t * cgc_generate_error(cgc_packet_head_t *ph){
	cgc_uint32_t cid = ph->card_id;
	cgc_uint32_t acd = ph->auth_code;
	cgc_uint32_t txn_id = ph->transaction_id;

	if(ERRNO == ERRNO_MP_ALLOC){

		_terminate(1);
	}


	size_t m_sz = cgc_strlen(ERROR_MSGS[ERRNO]);
	
	size_t pay_sz  = sizeof(cgc_packet_data_error_t) + m_sz - sizeof(char *) ;
	cgc_pack_and_data_t *pad = cgc_create_basic_response(cid, acd, txn_id, ph->pkt_type, ph->op_code, ERRNO, pay_sz);
	cgc_packet_head_t * resp = pad->ph;
	
	cgc_packet_data_error_t * e = (cgc_packet_data_error_t *) cgc_get_payload(resp);
	e->length = m_sz;

	cgc_memcpy(&(e->msg), ERROR_MSGS[ERRNO], m_sz);
	
	return pad;

}


cgc_pack_and_data_t * cgc_process_client_purchase_op(cgc_pack_and_data_t * pin){
	// already known to be a purchase ops pkt here...
	// already known to be authorized

	cgc_packet_head_t * buy_pkt = pin->ph;
	cgc_packet_data_purchase_t * pdata = pin->data;
	cgc_uint32_t cid = buy_pkt->card_id;
	cgc_uint32_t acd = buy_pkt->auth_code;
	cgc_uint32_t txn_id = buy_pkt->transaction_id;

	cgc_acct_txn_entry_t * aet = cgc_get_acct_txn_reg(cid, acd, txn_id);



	if(aet == NULL){
		return NULL;
	}

	// check that we have a valid vendor type enum
	if(pdata->v.vendor_type > CAR){
		
		return cgc_error_out_and_free_txn(aet, ERRNO_MP_INVALID_V_TYPE);
	}



	// now we check that the balance is greater than the spend amount, dec balance, and record txn
	if(pdata->cost <= aet->ae->balance){
		aet->ae->balance -= pdata->cost;
		
	}else{
		return cgc_error_out_and_free_txn(aet, ERRNO_MP_PURCHASE_ISF);
	}

	cgc_add_transaction_log_entry(aet->ae, pin);

	cgc_free(aet);
	return cgc_create_basic_response(cid, acd, txn_id, OPS, PURCHASE, OK, 0);

}

cgc_pack_and_data_t * cgc_create_auth_resp(cgc_uint32_t cid, cgc_uint32_t acd, cgc_uint32_t txn_id){
	cgc_packet_head_t *resp = cgc_create_basic_packet_head(cid, acd, txn_id, 0);
	resp->pkt_type = AUTH;

	return cgc_create_basic_response(cid, acd, txn_id, AUTH, 0, OK, 0);
}



cgc_pack_and_data_t * cgc_process_client_auth(cgc_pack_and_data_t * pin){
	cgc_packet_head_t * auth_packet = pin->ph;
	cgc_uint32_t cid = auth_packet->card_id;
	cgc_uint32_t acd =  auth_packet->auth_code;
	
	cgc_account_entry_t * ae = cgc_check_auth_packet(auth_packet);

	if(ae == NULL){
		// errno is set by check auth pkt
		return NULL;
	}

	cgc_txn_entry_t *te = cgc_open_txn(ae, auth_packet);


	if(te == NULL){
		ERRNO = ERRNO_MP_NO_AUTH;
		return NULL;


	}


	cgc_uint32_t txn_id = te->transaction_id;
	auth_packet->transaction_id = txn_id;

	cgc_pack_and_data_t * resp = cgc_create_auth_resp(cid, acd, txn_id);

	resp->ph->op_code = auth_packet->op_code;
	return resp;
}

// come back with a fin response
cgc_pack_and_data_t *cgc_process_client_fin(cgc_pack_and_data_t *pin){
	cgc_packet_head_t *fin_pkt = pin->ph;
	cgc_uint32_t cid = fin_pkt->card_id;
	cgc_uint32_t acd =  fin_pkt->auth_code;
	cgc_uint32_t txn_id = fin_pkt->transaction_id;
	cgc_account_entry_t * ae = cgc_get_account(cid, acd);
	
	if(ae == NULL){
		ERRNO = ERRNO_MP_NOT_FOUND;
		return NULL;
	}

	cgc_txn_entry_t * te = cgc_get_transaction(ae, txn_id);
	if(te == NULL){
		// err no set by get_txn
		return NULL;
	}


	if(cgc_finalize_transaction(te) != OK){
		// finalize transaction sets huge errno for double fin
		return NULL;
	}



	return cgc_create_basic_response(cid, acd, txn_id, FIN, fin_pkt->op_code, OK, 0);

}



