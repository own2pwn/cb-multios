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

#ifndef TAXPAYER_H
#define TAXPAYER_H 1

#include <libcgc.h>

#include "comms.h"

typedef enum {NONE, BLUE, GREEN, RED, PURPLE, PARTY_COUNT} cgc_PARTY_T;
typedef struct tenfourdee cgc_TenFourD;
typedef struct taxpayer cgc_TaxPayer;
typedef struct ident cgc_Ident;
typedef struct daterange cgc_DateRange;

struct tenfourdee {
	cgc_uint16_t tax_year;
	struct {
		char fname[128];
		char mname[128];
		char lname[128];
		char addy[128];		// street addy
		char csz[128]; 		// city state zip
		cgc_uint32_t id_num;
	} PACKED ident;
	struct {
		cgc_uint8_t donate;
		cgc_uint32_t amount;
		cgc_PARTY_T party;
	} PACKED funding;
	struct {
		cgc_uint32_t wages;
		cgc_uint32_t interest;
		cgc_uint32_t biz_income;
		cgc_uint32_t retirement_income;
	} PACKED income;
	struct {
		cgc_uint32_t biz_expenses;
		cgc_uint32_t edu_expenses;
		cgc_uint32_t self_employ_expenses;
	} PACKED expenses;
	struct {
		cgc_uint32_t edu_credits;
		cgc_uint32_t child_credits;
		cgc_uint32_t retirement_credits;
		cgc_uint32_t home_buyer_credits;
	} PACKED credits;
	struct {
		cgc_uint32_t tax_withheld;
		cgc_uint32_t tax_paid_non_taxable_income;
	} PACKED payments;
	char digital_signature[24];
	char submission_date[8];
	cgc_uint32_t tax_due;
	cgc_uint32_t tax_refund;
	cgc_uint32_t tax_paid_refunded;
	cgc_TenFourD *next;
} PACKED;

struct ident {
	char fname[128];
	char mname[128];
	char lname[128];
	char addy[128];		// street addy
	char csz[128]; 		// city state zip
	cgc_uint32_t dob;
	cgc_uint32_t id_num;
} PACKED;

struct taxpayer {
	cgc_Ident ident;
	struct {
		char username[12];
		char password[12];
	} PACKED auth;
	cgc_TenFourD *tax_forms;
	cgc_TaxPayer *next;
} PACKED;

struct daterange {
	cgc_uint16_t start_date;
	cgc_uint16_t end_date;
};

typedef void (*cgc_copy_yr_fn)(cgc_Response *, char *, cgc_size_t);

void cgc_taxpayer_new(cgc_Session *s, cgc_Response *r, cgc_TaxPayer **tp_list);
cgc_TaxPayer *cgc_taxpayer_get_by_username(cgc_TaxPayer *tp_list, cgc_Session *s);
int cgc_taxpayer_compare_creds(cgc_TaxPayer *tp, cgc_Session *s);
int cgc_taxpayer_add_tenfourdee(cgc_TaxPayer *tp, cgc_Session *s, cgc_size_t data_sz);
cgc_int32_t cgc_taxpayer_sum_taxes_due(cgc_TaxPayer *tp, cgc_Session *s, cgc_int32_t *sum);
cgc_int32_t cgc_taxpayer_list_submitted_tax_years(cgc_TaxPayer *tp, cgc_Session *s, char *year_buf, cgc_size_t *bytes_written);
cgc_int32_t cgc_taxpayer_pay_taxes(cgc_TaxPayer *tp, cgc_Session *s, char *year_buf, cgc_size_t *bytes_written);
cgc_int32_t cgc_taxpayer_get_refund(cgc_TaxPayer *tp, cgc_Session *s, char *year_buf, cgc_size_t *bytes_written);
cgc_uint32_t cgc_taxpayer_count_submitted_tax_years(cgc_TaxPayer *tp);

void cgc_tenfourd_append(cgc_TenFourD **t4d_list, cgc_TenFourD *t4d);
void cgc_tenfourd_get_last_three_from_list(cgc_TenFourD *t4d_list, cgc_uint32_t list_length, cgc_uint16_t tax_years[3]);
int cgc_tenfourd_validate(cgc_TenFourD *t4d, cgc_TaxPayer *tp);
cgc_TenFourD *cgc_tenfourd_ingest(cgc_Session *s, cgc_size_t data_sz);

#endif