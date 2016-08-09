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
#include <cstdlib.h>
#include <cstring.h>
#include <cstdio.h>
#include "ticket.h"
#include "time.h"

#define MAX_PRIORITY CRITICAL

static cgc_uint32_t g_ticket_id = 1;

cgc_Ticket *cgc_Ticket::cgc_GetTicket(char *cgc_email, char *desc, cgc_uint32_t cgc_entry_time, cgc_uint32_t cgc_duration, cgc_PRIORITY cgc_priority)
{
    if (!cgc_email || !desc || !cgc_entry_time || !cgc_priority || cgc_priority > MAX_PRIORITY)
        return (cgc_Ticket *)NULL;

    return new cgc_Ticket(cgc_email, desc, cgc_entry_time, cgc_duration, cgc_priority);
}

void cgc_Ticket::cgc_DeleteTicket(cgc_Ticket *ticket)
{
    if (ticket)
        delete ticket;
}

cgc_Ticket::cgc_Ticket(char *cgc_email, char *desc, cgc_uint32_t cgc_entry_time, cgc_uint32_t cgc_duration, cgc_PRIORITY cgc_priority)
{
    id_ = g_ticket_id++;
    cgc_uint32_t email_len = cgc_strlen(cgc_email) >= MAX_EMAIL ? MAX_EMAIL - 1 : cgc_strlen(cgc_email);
    cgc_uint32_t desc_len = cgc_strlen(desc) >= MAX_EMAIL ? MAX_EMAIL - 1 : cgc_strlen(desc);
    cgc_memset(email_, 0, sizeof(email_));
    cgc_memset(desc_, 0, sizeof(desc_));
    cgc_memcpy(email_,  cgc_email, email_len);
    cgc_memcpy(desc_,  desc, desc_len);
    entry_time_ = cgc_entry_time;
    status_ = OPEN;
    duration_ = cgc_duration;
    priority_ = cgc_priority;
}

cgc_Ticket::~cgc_Ticket()
{
}

void cgc_Ticket::cgc_UpdateStatus(cgc_STATUS cgc_status)
{
    status_ = cgc_status;
}

void cgc_Ticket::cgc_WorkOn()
{
  if (duration_ > 0)
    --duration_;
}

bool cgc_Ticket::cgc_CheckDone(void)
{
  return duration_ == 0;
}


void cgc_Ticket::cgc_Display()
{
  cgc_printf("%d" EOL, cgc_id());
  cgc_printf("%s" EOL, cgc_email());
  cgc_printf("%s" EOL, cgc_description());
  cgc_printf("%d" EOL, cgc_status());
  cgc_printf("%d" EOL, cgc_priority());
  cgc_printf("%d" EOL, cgc_entry_time());
  cgc_printf("%d" EOL, cgc_duration());
}

cgc_uint32_t cgc_Ticket::cgc_id()
{
    return id_;
}

char *cgc_Ticket::cgc_email()
{
    return email_;
}

char *cgc_Ticket::cgc_description()
{
    return desc_;
}

cgc_uint32_t cgc_Ticket::cgc_entry_time()
{
    return entry_time_;
}

cgc_PRIORITY cgc_Ticket::cgc_priority()
{
    return priority_;
}

cgc_STATUS cgc_Ticket::cgc_status()
{
    return status_;
}

cgc_uint32_t cgc_Ticket::cgc_duration()
{
    return duration_;
}
