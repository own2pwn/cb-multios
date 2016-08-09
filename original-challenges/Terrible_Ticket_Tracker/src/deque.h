#pragma once

#include <cstdint.h>
#include "ticket.h"

class cgc_Deque
{
  public:
    cgc_Deque();
    ~cgc_Deque();
    void cgc_Append(cgc_Ticket *ticket);
    void cgc_AppendLeft(cgc_Ticket *ticket);
    cgc_Ticket *cgc_Find(cgc_uint32_t ID);
    bool cgc_Remove(cgc_Ticket* ticket);
    cgc_Ticket *cgc_Pop();
    cgc_Ticket *cgc_PopLeft();
    cgc_uint32_t cgc_Count();
  private:
    void cgc_Expand();
    void cgc_TryShrink();

    cgc_Ticket **tickets_;
    cgc_uint32_t idx_0_;
    cgc_uint32_t size_;
    cgc_uint32_t cap_;
};
