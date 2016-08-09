#pragma once
#include <cstdint.h>
#include "ticket.h"
#include "support.h"
#include "deque.h"
#include "list.h"

#define NUM_DEQUES 5

class cgc_Dispatcher
{
  public:
    cgc_Dispatcher(cgc_List *pworkers, cgc_uint32_t *secret_page);
    void cgc_AddTicket(cgc_Ticket *ticket);
    void cgc_CancelTicket(cgc_uint32_t cgc_id);
    void cgc_ViewTicket(cgc_uint32_t cgc_id);
    void cgc_ViewTickets(cgc_STATUS cgc_status);
    cgc_Ticket *cgc_GetTicket(cgc_PRIORITY cgc_max_priority);
    void cgc_RecordFinished(cgc_Support* t);

    cgc_uint32_t cgc_GetRandomTicks(cgc_uint32_t modval=1000);
  private:
    cgc_Support *cgc_FindSupportByTicketId(cgc_uint32_t cgc_id);

    cgc_Deque ticket_lists_[5];
    cgc_List *pworkers_;
    cgc_uint32_t *secret_;
};
