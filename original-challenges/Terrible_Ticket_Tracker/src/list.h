#pragma once

#include "cstdint.h"
#include "support.h"

#define MAX_WORKERS 128
class cgc_List
{
  public:
    cgc_List();
    bool cgc_Append(cgc_Support* s);
    cgc_Support* cgc_Remove(cgc_uint32_t idx);
    cgc_Support* cgc_Get(cgc_uint32_t idx);
    cgc_uint32_t cgc_Length();
  private:
    cgc_Support* data[MAX_WORKERS];
    cgc_uint32_t size;
    cgc_uint32_t cap;
};
