/*
 * Copyright (c) 2016 Kaprica Security, Inc.
 *
 * Permission is hereby granted, cgc_free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */
#include "support.h"

#include "cstdio.h"

cgc_uint32_t g_support_id = 1;

cgc_Support::cgc_Support(cgc_uint32_t cgc_contact_info, cgc_PRIORITY cgc_max_priority)
{
    support_id_ = g_support_id++;
    contact_info_ = cgc_contact_info;
    max_priority_ = cgc_max_priority;
}

bool cgc_Support::cgc_AssignTicket(cgc_Ticket *ticket)
{
    if (!ticket)
        return false;

    ticket_ = ticket;
    return true;
}

bool cgc_Support::cgc_UpdateTicket(cgc_STATUS cgc_status)
{
    if (!ticket_)
        return false;

    ticket_->cgc_UpdateStatus(cgc_status);
    return true;
}

cgc_Ticket *cgc_Support::cgc_CurrentTicket()
{
    return ticket_;
}

void cgc_Support::cgc_Display(void)
{
  cgc_printf("%d" EOL, cgc_id());
  if (cgc_CurrentTicket())
    cgc_printf("%d" EOL, cgc_CurrentTicket()->cgc_id());
  else
    cgc_printf("%s" EOL, "None");
  cgc_printf("%d" EOL, cgc_contact_info());
  cgc_printf("%d" EOL, cgc_max_priority());
}

cgc_Ticket *cgc_Support::cgc_RemoveTicket()
{
    cgc_Ticket *ticket = nullptr;
    if (ticket_)
    {
        ticket = ticket_;
        ticket_->prev = this;
        ticket_ = nullptr;
    }

    return ticket;
}

cgc_uint32_t cgc_Support::cgc_id()
{
    return support_id_;
}

cgc_uint32_t cgc_Support::cgc_contact_info()
{
    return contact_info_;
}

cgc_PRIORITY cgc_Support::cgc_max_priority()
{
    return max_priority_;
}
