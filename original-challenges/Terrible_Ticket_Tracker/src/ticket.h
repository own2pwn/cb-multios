#pragma once
#define MAX_EMAIL 64
#define MAX_DESC 512
#include <cstdint.h>

typedef enum {
  OPEN = 1,
  IN_PROGRESS = 2,
  REJECTED = 3,
  RESOLVED = 4,
} cgc_STATUS;

typedef enum {
  LOW = 1,
  MEDIUM = 2,
  HIGH = 3,
  SEVERE = 4,
  CRITICAL = 5,
} cgc_PRIORITY;


class cgc_Ticket
{
  public:
    static cgc_Ticket *cgc_GetTicket(char *cgc_email, char *desc, cgc_uint32_t cgc_entry_time, cgc_uint32_t cgc_duration, cgc_PRIORITY cgc_priority = LOW);
    static void cgc_DeleteTicket(cgc_Ticket *ticket);
    void cgc_UpdateStatus(cgc_STATUS cgc_status);
    void cgc_WorkOn();
    bool cgc_CheckDone();
    void cgc_Display();

    cgc_uint32_t cgc_id();
    char *cgc_email();
    char *cgc_description();
    cgc_uint32_t cgc_entry_time();
    cgc_PRIORITY cgc_priority();
    cgc_STATUS cgc_status();
    cgc_uint32_t cgc_duration();
    void* prev;
  private:
    cgc_Ticket(char *cgc_email, char *desc, cgc_uint32_t cgc_entry_time, cgc_uint32_t cgc_duration, cgc_PRIORITY cgc_priority);
    ~cgc_Ticket();

    cgc_uint32_t id_;
    char email_[MAX_EMAIL];
    char desc_[MAX_DESC];
    cgc_uint32_t entry_time_;
    cgc_PRIORITY priority_; //1-5
    cgc_STATUS status_;
    cgc_uint32_t duration_;
};
