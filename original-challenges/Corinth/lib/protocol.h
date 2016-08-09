#pragma once

#include "types.h"

typedef struct {
  cgc_uint8 id;
  cgc_uint16 length;
  void* value;
} cgc_protocol_frame;

void cgc_protocol_send(cgc_protocol_frame* frame);
void cgc_protocol_with_recv_frame(cgc_uint8 (^block)(cgc_protocol_frame* frame));
