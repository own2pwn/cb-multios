#include "data.h"

void cgc_each(cgc_data_package data, void (^block)(cgc_uint32 index, cgc_uint32 value)) {
  for(cgc_uint32 cur = 0; cur < data.size; cur++) {
    block(cur, data.contents[cur]);
  }
}
