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
#include "dispatcher.h"
#include "cstdio.h"
#include "cstring.h"

cgc_uint32_t g_rand_idx = 0;

cgc_Dispatcher::cgc_Dispatcher(cgc_List *pworkers, cgc_uint32_t *secret_page)
{
  pworkers_ = pworkers;
  secret_ = secret_page;
}

void cgc_Dispatcher::cgc_AddTicket(cgc_Ticket *ticket)
{
  if(!ticket)
    return;

  if (ticket->cgc_status() == OPEN)
    ticket_lists_[ticket->cgc_priority() - 1].cgc_Append(ticket);
  else
    ticket_lists_[ticket->cgc_priority() - 1].cgc_AppendLeft(ticket);
}

void cgc_Dispatcher::cgc_CancelTicket(cgc_uint32_t cgc_id)
{
  for (int i = 0; i < NUM_DEQUES; i++)
  {
    cgc_Deque *ptickets = &ticket_lists_[i];
    cgc_uint32_t count = ptickets->cgc_Count();
    for (int j = 0; j < count; j++)
    {
      cgc_Ticket *ticket = ptickets->cgc_PopLeft();
      if (ticket->cgc_id() == cgc_id) {
        cgc_Ticket::cgc_DeleteTicket(ticket);
        continue;
      }
      ptickets->cgc_Append(ticket);
    }
  }

  cgc_Support *worker = cgc_FindSupportByTicketId(cgc_id);
  if (worker)
  {
    cgc_Ticket *ticket = worker->cgc_RemoveTicket();
    if (ticket)
      cgc_Ticket::cgc_DeleteTicket(ticket);
  }
}

void cgc_Dispatcher::cgc_ViewTicket(cgc_uint32_t cgc_id)
{
  for (int i = 0; i < NUM_DEQUES; i++)
  {
    cgc_Deque *ptickets = &ticket_lists_[i];
    for (int j = 0; j < ptickets->cgc_Count(); j++)
    {
      cgc_Ticket *ticket = ptickets->cgc_PopLeft();
      if (ticket->cgc_id() == cgc_id)
      {
        ticket->cgc_Display();
      }
      ptickets->cgc_Append(ticket);
    }
  }

  cgc_Support *worker = cgc_FindSupportByTicketId(cgc_id);
  if (worker)
    worker->cgc_CurrentTicket()->cgc_Display();
}

void cgc_Dispatcher::cgc_ViewTickets(cgc_STATUS cgc_status)
{
  for (int i = 0; i < NUM_DEQUES; i++)
  {
    cgc_Deque *ptickets = &ticket_lists_[i];
    for (int j = 0; j < ptickets->cgc_Count(); j++)
    {
      cgc_Ticket *ticket = ptickets->cgc_PopLeft();
      if (ticket->cgc_status() == cgc_status)
        ticket->cgc_Display();
      ptickets->cgc_Append(ticket);
    }
  }

  for (int i = 0; i < pworkers_->cgc_Length(); i++)
  {
    cgc_Support *worker = pworkers_->cgc_Get(i);
    cgc_Ticket *current_ticket = worker->cgc_CurrentTicket();
    if (current_ticket && current_ticket->cgc_status() == cgc_status)
      current_ticket->cgc_Display();
  }

}

void cgc_Dispatcher::cgc_RecordFinished(cgc_Support* worker)
{
  char buf[MAX_DESC + 1];
  cgc_memcpy(buf, worker->cgc_CurrentTicket()->cgc_description(), MAX_DESC);
  cgc_Ticket::cgc_DeleteTicket(worker->cgc_RemoveTicket());

  cgc_printf("cgc_Ticket Finished\n");
  cgc_printf("%d\n", worker->cgc_id());
#ifdef PATCHED_1
  cgc_printf("%s", buf);
#else
  cgc_printf(buf);
#endif
}

cgc_Ticket *cgc_Dispatcher::cgc_GetTicket(cgc_PRIORITY cgc_max_priority)
{
  for (int i = cgc_max_priority; i >= LOW; i--)
  {
    cgc_Deque *ptickets = &ticket_lists_[i - 1];
    if (ptickets->cgc_Count())
      return ptickets->cgc_PopLeft();
  }

  return nullptr;
}

cgc_uint32_t cgc_Dispatcher::cgc_GetRandomTicks(cgc_uint32_t modval)
{
  cgc_uint32_t v =  secret_[g_rand_idx] % modval;
  g_rand_idx = (g_rand_idx + 1) % (0x1000 / sizeof(cgc_uint32_t));
  return v;
}

cgc_Support *cgc_Dispatcher::cgc_FindSupportByTicketId(cgc_uint32_t cgc_id)
{
  for (int i = 0; i < pworkers_->cgc_Length(); i++)
  {
    cgc_Support *worker = pworkers_->cgc_Get(i);
    cgc_Ticket *current_ticket = worker->cgc_CurrentTicket();
    if (current_ticket && current_ticket->cgc_id() == cgc_id)
      return worker;
  }

  return nullptr;
}
