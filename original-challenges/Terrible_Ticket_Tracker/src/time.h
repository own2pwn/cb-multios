#pragma once

#include "cstdint.h"

extern cgc_uint32_t ticks;

class Time
{
  public:
    static void cgc_Reset(void);
    static cgc_uint32_t cgc_GetTime(void);
    static void cgc_Tick(void);
};
