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
#include <cstdio.h>
#include <cstring.h>
#include "deque.h"
#include "dispatcher.h"
#include "ticket.h"
#include "time.h"
#include "support.h"

#define COMPONENT_DELIM '|'
#define MAX_COMPONENTS 8
#define MAX_COMPONENT MAX_DESC
#define NUM_PRIOS CRITICAL

#define TOSTR(x) #x
#define WITHIN(a, b, v) ((v) >= (a) && (v) <= (b))

class cgc_Scheduler
{
  private:
    cgc_List* workers_;
    cgc_Dispatcher* dispatcher_;

  public:
    cgc_Scheduler(cgc_List* workers, cgc_Dispatcher* dispatcher);
    void cgc_Run(void);
};

cgc_Scheduler::cgc_Scheduler(cgc_List* workers, cgc_Dispatcher* dispatcher)
{
  dispatcher_ = dispatcher;
  workers_ = workers;
}

void cgc_Scheduler::cgc_Run(void)
{
  for (cgc_size_t i = 0; i < workers_->cgc_Length(); ++i)
  {
    cgc_Support* worker = workers_->cgc_Get(i);
    if (worker->cgc_CurrentTicket() == nullptr)
    {
      cgc_Ticket* ticket = dispatcher_->cgc_GetTicket(worker->cgc_max_priority());
      if (ticket != nullptr)
      {
        if (ticket->prev != worker)
        {
          worker->cgc_AssignTicket(ticket);
          ticket->cgc_UpdateStatus(IN_PROGRESS);
        }
        else
        {
          dispatcher_->cgc_AddTicket(ticket);
        }
      }
    }
    else
    {
      cgc_Ticket* ticket = worker->cgc_CurrentTicket();
      ticket->cgc_WorkOn();

      if (ticket->cgc_CheckDone())
      {
        ticket->cgc_UpdateStatus(RESOLVED);
        dispatcher_->cgc_RecordFinished(worker);
      }
      else if (dispatcher_->cgc_GetRandomTicks(10) < 2)
      {
        if (ticket->cgc_priority() > HIGH)
          continue;
        ticket->cgc_UpdateStatus(REJECTED);
        worker->cgc_RemoveTicket();
      }
      else if (dispatcher_->cgc_GetRandomTicks(10) < 3)
      {
        worker->cgc_RemoveTicket();
        dispatcher_->cgc_AddTicket(ticket);
        ticket->cgc_UpdateStatus(OPEN);
      }
    }
  }
}

cgc_List workers;
cgc_Dispatcher* dispatcher;
cgc_Scheduler* scheduler;

typedef enum
{
  COMMAND_ERROR = 0,
  ADD_TICKET,
  CANCEL_TICKET,
  LIST_SUPPORT,
  LIST_FREE_SUPPORT,
  HIRE,
  FIRE,
  VIEW,
  VSTATUS,
  QUIT,
} cgc_COMMAND;

typedef int (*cgc_handler)(cgc_FILE* in, cgc_FILE* out, char** components, cgc_size_t num_components);

int cgc_handle_command_error(cgc_FILE* in, cgc_FILE* out, char** components, cgc_size_t num_components)
{
  cgc_fprintf(out, "Invalid command" EOL);
  return 0;
}

int cgc_handle_add_command(cgc_FILE* in, cgc_FILE* out, char** components, cgc_size_t num_components)
{
  cgc_PRIORITY p = LOW;
  if (components[3])
  {
    p = (cgc_PRIORITY)cgc_strtol(components[3], nullptr, 10);
  }

  if (p < LOW || p > CRITICAL)
  {
    p = LOW;
  }

  cgc_Ticket* new_ticket = cgc_Ticket::cgc_GetTicket(
      components[1], components[2], Time::cgc_GetTime(),
      dispatcher->cgc_GetRandomTicks(100), p
  );

  if (new_ticket)
  {
    dispatcher->cgc_AddTicket(new_ticket);
  }

  return 0;
}

int cgc_handle_cancel_command(cgc_FILE* in, cgc_FILE* out, char** components, cgc_size_t num_components)
{
  cgc_uint32_t cur_id = cgc_strtol(components[1], nullptr, 10);
  dispatcher->cgc_CancelTicket(cur_id);
  return 0;
}

int cgc_handle_list_command(cgc_FILE* in, cgc_FILE* out, char** components, cgc_size_t num_components)
{
  for (cgc_size_t i = 0; i < workers.cgc_Length(); i++)
  {
    workers.cgc_Get(i)->cgc_Display();
  }

  return 0;
}

int cgc_handle_list_free_command(cgc_FILE* in, cgc_FILE* out, char** components, cgc_size_t num_components)
{
  for (cgc_size_t i = 0; i < workers.cgc_Length(); i++)
  {
    cgc_Support* s = workers.cgc_Get(i);
    if (!s->cgc_CurrentTicket())
    {
      s->cgc_Display();
    }
  }

  return 0;
}

int cgc_handle_hire_command(cgc_FILE* in, cgc_FILE* out, char** components, cgc_size_t num_components)
{
  cgc_PRIORITY p = (cgc_PRIORITY)cgc_strtol(components[1], nullptr, 10);
  if (p < LOW || p > CRITICAL)
    return 0;
  cgc_Support* s = new cgc_Support(0, p);
  workers.cgc_Append(s);
  return 0;
}

int cgc_handle_fire_command(cgc_FILE* in, cgc_FILE* out, char** components, cgc_size_t num_components)
{
  cgc_PRIORITY p = (cgc_PRIORITY)cgc_strtol(components[1], nullptr, 10);
  if (p < LOW || p > CRITICAL)
    return 0;

  for (cgc_size_t i = 0; i < workers.cgc_Length(); i++)
  {
    cgc_Support* s = workers.cgc_Get(i);
    if (s->cgc_max_priority() == p)
    {
      if (s->cgc_CurrentTicket())
      {
        dispatcher->cgc_AddTicket(s->cgc_CurrentTicket());
      }

      workers.cgc_Remove(i);
      break;
    }
  }

  return 0;
}

int cgc_handle_view_command(cgc_FILE* in, cgc_FILE* out, char** components, cgc_size_t num_components)
{
  cgc_STATUS s = (cgc_STATUS)0;
  if (num_components == 2)
  {
    s = (cgc_STATUS)cgc_strtol(components[1], nullptr, 10);
    if (s > RESOLVED)
      s = (cgc_STATUS)0;
  }

  if (s)
  {
    dispatcher->cgc_ViewTickets(s);
  }
  else
  {
    for (cgc_size_t i = OPEN; i < RESOLVED; i++)
    {
      dispatcher->cgc_ViewTickets((cgc_STATUS)i);
    }
  }

  return 0;
}

int cgc_handle_status_command(cgc_FILE* in, cgc_FILE* out, char** components, cgc_size_t num_components)
{
  cgc_uint32_t tid = cgc_strtol(components[1], nullptr, 10);
  dispatcher->cgc_ViewTicket(tid);
  return 0;
}

int cgc_handle_quit(cgc_FILE* in, cgc_FILE* out, char** components, cgc_size_t num_components)
{
  cgc_fprintf(out, "TERMINATING TERRIBLE TICKET TRACKER" EOL);
  return -1;
}

cgc_handler command_handlers[] = {
  cgc_handle_command_error,
  cgc_handle_add_command,
  cgc_handle_cancel_command,
  cgc_handle_list_command,
  cgc_handle_list_free_command,
  cgc_handle_hire_command,
  cgc_handle_fire_command,
  cgc_handle_view_command,
  cgc_handle_status_command,
  cgc_handle_quit,
};


cgc_COMMAND cgc_read_command(cgc_FILE* f, char*** components, cgc_size_t* num_components)
{
  cgc_COMMAND c = COMMAND_ERROR;
  char* component = nullptr;
  char** comps = *components;
  *num_components = 0;

  for (cgc_size_t i = 0; i < MAX_COMPONENTS; ++i)
  {
    component = new char[MAX_COMPONENT + 1];
    cgc_memset(component, 0, MAX_COMPONENT + 1);
    c = COMMAND_ERROR;

    int ret = cgc_freaduntil(component, MAX_COMPONENT, COMPONENT_DELIM, f);

    if (ret > 1) // So two successive COMPONENT_DELIM ends command
    {
      comps[i] = component;
      *num_components = i + 1;
    }
    else
    {
      delete component;
      break;
    }
  }

  if (comps[0] == nullptr)
  {
    goto done;
  }

  if (cgc_strcmp(comps[0], TOSTR(ADD_TICKET)) == 0)
  {
    if (!WITHIN(3, 4, *num_components))
      goto done;

    c = ADD_TICKET;
  }
  else if (cgc_strcmp(comps[0], TOSTR(CANCEL_TICKET)) == 0)
  {
    if (!WITHIN(2, 2, *num_components))
      goto done;

    c = CANCEL_TICKET;
  }
  else if (cgc_strcmp(comps[0], TOSTR(LIST_SUPPORT)) == 0)
  {
    if (!WITHIN(1, 1, *num_components))
      goto done;

    c = LIST_SUPPORT;
  }
  else if (cgc_strcmp(comps[0], TOSTR(LIST_FREE_SUPPORT)) == 0)
  {
    if (!WITHIN(1, 1, *num_components))
      goto done;

    c = LIST_FREE_SUPPORT;
  }
  else if (cgc_strcmp(comps[0], TOSTR(HIRE)) == 0)
  {
    if (!WITHIN(2, 2, *num_components))
      goto done;

    c = HIRE;
  }
  else if (cgc_strcmp(comps[0], TOSTR(FIRE)) == 0)
  {
    if (!WITHIN(2, 2, *num_components))
      goto done;

    c = FIRE;
  }
  else if (cgc_strcmp(comps[0], TOSTR(VIEW)) == 0)
  {
    if (!WITHIN(1, 2, *num_components))
      goto done;

    c = VIEW;
  }
  else if (cgc_strcmp(comps[0], TOSTR(VSTATUS)) == 0)
  {
    if (!WITHIN(2, 2, *num_components))
      goto done;

    c = VSTATUS;
  }
  else if (cgc_strcmp(comps[0], TOSTR(QUIT)) == 0)
  {
    c = QUIT;
  }

done:
  return c;
}

void cgc_run_server(cgc_FILE* in, cgc_FILE* out, unsigned char* secrets)
{
  cgc_COMMAND command;
  cgc_size_t num_components;
  char** components = new char*[MAX_COMPONENTS];

  Time::cgc_Reset();
  dispatcher = new cgc_Dispatcher(&workers, (cgc_uint32_t*)(secrets));
  scheduler = new cgc_Scheduler(&workers, dispatcher);


  // Setup two starter workers
  char *commands[2] = {(char *)"HIRE", (char *)"3"};
  cgc_handle_hire_command(in, out, commands, 2);
  commands[1] = (char *)"5";
  cgc_handle_hire_command(in, out, commands, 2);

  cgc_fprintf(out, "Welcome to the terrible ticket tracker" EOL);
  for (;;)
  {
    // cgc_Tick
    Time::cgc_GetTime();
    scheduler->cgc_Run();

    // Clear previous components
    for (cgc_size_t i = 0; i < MAX_COMPONENTS; ++i)
    {
      if (components[i] != nullptr)
      {
        delete components[i];
        components[i] = nullptr;
      }
    }

    // Read components
    cgc_fprintf(out, "$ ");
    command = cgc_read_command(in, &components, &num_components);

    // Dispatch command
    int ret = command_handlers[command](in, out, components, num_components);
    if (ret != 0)
    {
      break;
    }

    cgc_fprintf(out, "OK" EOL);
  }
}

extern "C" int __attribute__((fastcall)) main(int secret_page_i, char *unused[])
{
    unsigned char *secret_page = (unsigned char *)secret_page_i;
    cgc_fxlat(stdin, "EREH_EULAV_MODNAR");
    cgc_fxlat(stdout, "EREH_EULAV_MODNAR");
    cgc_run_server(stdin, stdout, secret_page);
    return 0;
}
