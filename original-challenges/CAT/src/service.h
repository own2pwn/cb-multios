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

#ifndef SERVICE_H
#define SERVICE_H

#include "libc.h"
#include "static.h"

#define SZ_SCRATCH SZ_PAGE
cgc_byte_t *buf_scratch = NULL;

// If we significantly change the size of the CB, this will need revisiting.
#define TEXT_BGN (cgc_byte_t *)0x08048000
#define TEXT_END (cgc_byte_t *)0x08146000 // We don't want leeway here.
#define DATA_BGN (cgc_byte_t *)0x08146000
#define DATA_END (cgc_byte_t *)0x0815b000+0x10000 // We can have some leeway here.
#define FLAG_PAGE_BGN (cgc_byte_t *)0x4347C000
#define FLAG_PAGE_END (cgc_byte_t *)(FLAG_PAGE_BGN + SZ_PAGE)

#define MAGIC_DATA_BEFORE 0xCAFEBABE
#define MAGIC_DATA_AFTER 0xBBBBBBBB
#define MAGIC_PTR_BEFORE 0xDEADBEEF
#define MAGIC_PTR_AFTER 0xAAAAAAAA


// We force these enums to be int via last assignment to INT_MAX, so their size 
// is not ambiguous.
typedef enum {
	S_REQ,
	S_RESP,
	S_ERROR,
	S_EXIT,
	S_LAST = INT_MAX
} cgc_status_t;

typedef enum {
	X_NORMAL,
	X_SCRATCH,
	X_STATIC,
	X_PREV,
	X_LAST = INT_MAX
} cgc_access_t;

typedef enum {
	A_READ,
	A_WRITE,
	A_LAST = INT_MAX
} cgc_action_t;

typedef enum {
	OP2_IMM,
	OP2_MEM,
	OP2_LAST = INT_MAX
} cgc_op2type_t;

typedef struct __attribute__((packed)) {
	cgc_action_t action; // 4
	cgc_op2type_t op2type; // 8
	cgc_uint32_t dst; // 12
	cgc_uint32_t src; // 16
} cgc_instruction_t;

typedef struct __attribute__((packed)) {
	cgc_status_t status; // 4
	// The sequence number.  Used for correlating requests & responses.
	cgc_uint32_t seq; // 8
	// Access type (NORMAL, STATIC, SCRATCH)
	cgc_access_t type; // 12
	// Operation
	cgc_instruction_t inst; // 12 + 16 = 28
} cgc_packet_t;

// NOTE: originally were going to recv packets in groups of 10, changed mind.
#define BUF_RX_SZ (sizeof(cgc_packet_t))
cgc_byte_t rx_buf[BUF_RX_SZ];
cgc_byte_t *rx_buf_cursor;

/**
 * Verify whether requested operation falls within acceptable boundaries.  
 * Assume R/Ws are 4B (sizeof(cgc_size_t)) long and prevent offcut accesses as well.
 *
 * @param loc Where we've been requested to R/W.
 * @param bgn Beginning of the allowed access region.
 * @param end End of the allowed access region.
 * @return A boolean indicating whether the access should be allowed.
 */
cgc_bool cgc_access_check(cgc_byte_t *loc, cgc_byte_t *bgn, cgc_byte_t *end);

/**
 * Read a packet off the wire.
 *
 * @param pkt Pointer the packet struct to be populated.
 * @param resp Pointer to a cgc_packet_t that is to be populated with the response.
 * @return An int indicating SUCCESS (0) or an error condition (!=0).
 */
int cgc_packet_recv(cgc_packet_t *pkt);

/**
 * Write a packet to the wire.
 *
 * @param pkt Pointer the packet struct to be serialized & sent.
 * @param resp Pointer to a cgc_packet_t that is to be populated with the response.
 * @return An int indicating SUCCESS (0) or an error condition (!=0).
 */
int cgc_packet_send(cgc_packet_t *pkt);

/**
 * Query against the "normal" region.
 *
 * @param req Pointer to a cgc_packet_t containing the normal request.
 * @param resp Pointer to a cgc_packet_t that is to be populated with the response.
 * @return An int indicating SUCCESS (0) or an error condition (!=0).
 */
int cgc_do_normal(cgc_packet_t *req, cgc_packet_t *resp);

/**
 * Query against the "scratch" region.
 *
 * @param req Pointer to a cgc_packet_t containing the scratch request.
 * @param resp Pointer to a cgc_packet_t that is to be populated with the response.
 * @return An int indicating SUCCESS (0) or an error condition (!=0).
 */
int cgc_do_scratch(cgc_packet_t *req, cgc_packet_t *resp);

/**
 * Query against the "static" region.
 * Involves unnecessarily complex code.
 *
 * @param req Pointer to a cgc_packet_t containing the static request.
 * @param resp Pointer to a cgc_packet_t that is to be populated with the response.
 * @return An int indicating SUCCESS (0) or an error condition (!=0).
 */
int cgc_do_static(cgc_packet_t *req, cgc_packet_t *resp);

/**
 * Generate a "bad request" response packet for the corresponding request.
 *
 * @param req Pointer to a cgc_packet_t containing the bad request.
 * @param resp Pointer to a cgc_packet_t that is to be populated with the response.
 * @return An int indicating SUCCESS (0) or an error condition (!=0).
 */
int cgc_bad_request(cgc_packet_t *req, cgc_packet_t *resp);

/**
 * Allocate heap space for SCRATCH queries.
 *
 * @return An int indicating SUCCESS (0) or an error condition (!=0).
 */
int cgc_init(void);

/**
 * The main loop.
 *
 * @return (Should never return).
 */
int main(void);


// We will clobber one of these during exploitation.  
// Make it easy to find by bookending with known values.
int (*volatile func_ptr[3])(cgc_packet_t *req, cgc_packet_t *resp);


#endif
