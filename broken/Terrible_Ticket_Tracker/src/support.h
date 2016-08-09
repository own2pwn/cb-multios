#pragma once

#include <cstdint.h>
#include "ticket.h"

class cgc_Support
{
  public:
    cgc_Support(cgc_uint32_t cgc_contact_info, cgc_PRIORITY cgc_max_priority = HIGH);
    ~cgc_Support();

    bool cgc_AssignTicket(cgc_Ticket *ticket);
    bool cgc_UpdateTicket(cgc_STATUS cgc_status);
    cgc_Ticket *cgc_CurrentTicket();
    cgc_Ticket *cgc_RemoveTicket();
    void cgc_Display(void);

    cgc_uint32_t cgc_id();
    cgc_uint32_t cgc_contact_info();
    cgc_PRIORITY cgc_max_priority();
  private:
    cgc_Ticket *ticket_;
    cgc_uint32_t support_id_;
    cgc_uint32_t contact_info_;
    cgc_PRIORITY max_priority_;
};
