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

#include <libcgc.h>
#include <libc.h>
#include "malloc.h"
#include "list.h"

#define STARTSTR "INITSTART"
#define DONESTR "done."
#define LOGFMT "LogMessage - Type:@i, Size:@i"
#define RESULTFMT "\nPayments made: @i\nTickets given: @i\nCoupons used: @i\nGross revenue: @i\nOperating costs: @i\nNet revenue: @i\n\n"
#define TICKETPRICE 5

typedef enum {
    ADDTRANS,
    ADDCOUPON,
    STATS,
    LOG,
    FINISH,
} cgc_instrtype;

typedef enum {
    PAY,
    ADDTICKET,
    REMOVETICKET,
    USECOUPON,
} cgc_cmdtype;

typedef enum {
   INIT,
   LOGMSG,
   RESULTS,
   DONE,
} cgc_resptype;

struct parkcmd {
    cgc_cmdtype type;
    size_t size;
    char *cmdbuf; //this is cast to various structs 
} typedef cgc_parkcmd_t;

struct parkinstr {
    cgc_instrtype type;
    cgc_parkcmd_t cmd;
} typedef cgc_parkinstr_t;

struct parkresp {
    cgc_resptype type;
    char resptext[256];
} typedef cgc_parkresp_t;

struct parkstats {
    cgc_uint32_t payments;
    cgc_uint32_t tickets;
    cgc_uint32_t coupons;
    cgc_uint16_t unknown;
    cgc_uint32_t expenses;
    cgc_uint32_t paid_fees;
    cgc_uint16_t paid_fines;
    cgc_uint16_t unpaid_fines;
    cgc_uint32_t paid_fines_value;
    cgc_uint32_t unpaid_fines_value;
    cgc_uint32_t coupon_losses;
    void (*cgc_do_stats)(struct parkstats* stats);
} typedef cgc_parkstats_t;

struct ticket {
    cgc_uint32_t id;
    cgc_uint32_t value;
} typedef cgc_ticket_t;

struct payment {
    cgc_uint32_t id;
    cgc_uint32_t value;
} typedef cgc_payment_t;

struct ticketpay {
    cgc_uint32_t id;
    cgc_uint32_t paid;
} typedef cgc_ticketpay_t;

struct coupon {
    cgc_uint32_t value;
    char cid[128];
} typedef cgc_coupon_t;

cgc_parkstats_t stats;
cgc_parkstats_t *statsp;
struct list parkl;
struct list ticketl;
struct list couponl;
size_t count;

void cgc_free_parking(void *node) {
    cgc_parkcmd_t *cmd = (cgc_parkcmd_t*)node;
    cgc_free(cmd->cmdbuf);
    cgc_free((char*)cmd-sizeof(cgc_instrtype)); //such hack, ow
}

void cgc_dontfree(void *node) {
    //stub function to avoid cgc_free'ing data that's still in use
}

unsigned char cgc_compare_ticket(void *d1, void *d2) {
    cgc_ticket_t *t = (cgc_ticket_t*)d1;
    cgc_uint32_t id = *(cgc_uint32_t*)d2;

    return t->id == id;
}

unsigned char cgc_compare_coupon(void *d1, void *d2) {
    cgc_coupon_t *c = (cgc_coupon_t*)d1;
    char *id = (char *)d2;

    return cgc_streq(c->cid, id);
}

int cgc_send_resp(cgc_resptype type, char *txt) {
    cgc_parkresp_t resp = {0};

    resp.type = type;
    cgc_strcpy(resp.resptext, txt);

    if (!SENDDATA(resp))
        return 1;
    return 0;
}

void cgc_do_pay(void *cmd) {
    cgc_payment_t *p = (cgc_payment_t *)cmd;
    stats.payments++;
    stats.paid_fees += p->value;
    stats.expenses += p->value/2;
}

void cgc_do_add_ticket(void *cmd) {
    cgc_ticket_t *t = (cgc_ticket_t*)cmd;
    stats.tickets++;
    stats.expenses++;
    if (cgc_list_find_node_with_data(&ticketl, cgc_compare_ticket, &t->id))
        return;
    cgc_list_insert_at_end(&ticketl, t);
    stats.unpaid_fines++;
    stats.unpaid_fines_value += t->value;
}

void cgc_do_pay_ticket(void *cmd) {
    cgc_ticketpay_t *pay = (cgc_ticketpay_t*)cmd;
    struct node* n;
    cgc_ticket_t *t;
    stats.paid_fines_value += pay->paid;
    stats.paid_fees += pay->paid;
    stats.unpaid_fines_value -= pay->paid;

    if ((n = cgc_list_find_node_with_data(&ticketl, cgc_compare_ticket, &pay->id))) {
        t = (cgc_ticket_t*)n->data;
        if (t->id == pay->id && t->value <= pay->paid) {
            stats.paid_fines++;
            stats.unpaid_fines--;
            cgc_list_remove_node(&ticketl, n);
        }
    }
}

void cgc_do_coupon(void *cmd) {
    cgc_coupon_t *coupon = (cgc_coupon_t*)cmd;
    struct node* n;
    cgc_coupon_t *match; 
    stats.coupons++;
    stats.paid_fees += coupon->value;
    
    if ((n = cgc_list_find_node_with_data(&couponl, cgc_compare_coupon, &coupon->cid))) {
        match = (cgc_coupon_t*)n->data;
        stats.coupon_losses -= match->value;
        stats.expenses += match->value;
    }
}

void cgc_do_unknown(void *cmd) {
    stats.unknown += 1;
}

void cgc_add_to_stats(void *d) {
    cgc_parkcmd_t *cmd = (cgc_parkcmd_t*)d;
    switch(cmd->type) { 
        case PAY:
            cgc_do_pay(cmd->cmdbuf);
            break;
        case ADDTICKET:
            cgc_do_add_ticket(cmd->cmdbuf);
            break;
        case REMOVETICKET:
            cgc_do_pay_ticket(cmd->cmdbuf);
            break;
        case USECOUPON:
            cgc_do_coupon(cmd->cmdbuf);
            break;
        default:
            cgc_do_unknown(cmd->cmdbuf);
    }
}

void cgc_do_stats(cgc_parkstats_t *stats) {
    cgc_parkresp_t resp = {0};

    cgc_memset(stats, '\0', sizeof(*stats));
    stats->cgc_do_stats = cgc_do_stats;

    cgc_list_init(&ticketl, cgc_dontfree);
    cgc_list_foreach(&parkl, cgc_add_to_stats);
    cgc_list_destroy(&ticketl);

    resp.type = RESULTS;
    cgc_sprintf(resp.resptext, RESULTFMT, stats->payments, stats->tickets, stats->coupons,
            stats->paid_fees, stats->expenses, stats->paid_fees-stats->expenses);

    if (!SENDDATA(resp))
        _terminate(35);
}

void cgc_print_cmd(void *d) {
    cgc_parkresp_t resp = {0};
    cgc_parkcmd_t *cmd = (cgc_parkcmd_t*)d;

    resp.type = LOGMSG;
    cgc_sprintf(resp.resptext, LOGFMT, cmd->type, cmd->size);
    if (!SENDDATA(resp))
        _terminate(34);
}


cgc_parkinstr_t *cgc_read_instr() {
    size_t res;
    cgc_parkinstr_t *instr = cgc_calloc(sizeof(cgc_parkinstr_t));

    if (!instr)
        return NULL;

    res = cgc_fread(instr, sizeof(*instr) - sizeof(char*), stdin);
    if (res != sizeof(*instr)-sizeof(char*)) {
        cgc_free(instr);
        return NULL;
    }

    if (instr->cmd.size == UINT_MAX || !(instr->cmd.cmdbuf = cgc_malloc(instr->cmd.size + 1))) {
        cgc_free(instr);
        return NULL;
    }

    int ret = 0;
    if ((ret = cgc_fread_until(instr->cmd.cmdbuf, NEWLINE, instr->cmd.size + 1, stdin)) != instr->cmd.size) {
        cgc_free(instr->cmd.cmdbuf);
        cgc_free(instr);
        return NULL;
    }
    return instr;
}

int main(void) {
    stats.cgc_do_stats = cgc_do_stats;
    statsp = &stats;
    cgc_parkinstr_t *instr;

    cgc_list_init(&parkl, cgc_free_parking);
    cgc_list_init(&couponl, cgc_free);

    if (cgc_send_resp(INIT,STARTSTR))
        return 1;

    while ((instr = cgc_read_instr())) {
        switch(instr->type) {
            case ADDTRANS:
                if (instr->cmd.size < sizeof(cgc_ticket_t)) {
                    break;
                }
                cgc_list_insert_at_end(&parkl, &instr->cmd);
                break;
            case ADDCOUPON:
                if (instr->cmd.size < sizeof(cgc_coupon_t))
                    break;
                *((char*)(instr->cmd.cmdbuf)+sizeof(cgc_coupon_t)-1) = '\0';
                cgc_list_insert_at_end(&couponl, instr->cmd.cmdbuf);
                cgc_free(instr);
                break;
            case STATS:
                //this is silly, but intended to make proving somewhat less difficult
                stats.cgc_do_stats(statsp);
                cgc_free(instr->cmd.cmdbuf);
                cgc_free(instr);
                break;
            case LOG:
                cgc_list_foreach(&parkl, cgc_print_cmd);
                cgc_free(instr->cmd.cmdbuf);
                cgc_free(instr);
                break;
            case FINISH:
                cgc_free(instr->cmd.cmdbuf);
                cgc_free(instr);
                goto done;
        }
    }

    done:
        stats.cgc_do_stats(statsp);
        cgc_list_destroy(&parkl);
        cgc_list_destroy(&couponl);

        if (cgc_send_resp(DONE,DONESTR))
            return 1;

        return 0;
}