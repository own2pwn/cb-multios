#include <libcgc.h>
#include "libc.h"
#include "charter.h"
#include "bars.h"
#include "sparks.h"
#include "rand.h"
#include "data.h"

void cgc_echo();
void cgc_sparks(cgc_data_package);
void cgc_read_data(cgc_uint32);

int main(void) {
  cgc_uint32 datum_count;

  cgc_read(&datum_count, sizeof(datum_count));
  if ((datum_count < 1) || (datum_count > MAX_STACK_SIZE)) return -1;

  cgc_read_data(datum_count);

  return 0;
}

void cgc_read_data(cgc_uint32 datum_count) {
  cgc_uint32 data[datum_count];
  cgc_data_package pack = {datum_count, data};

  for (cgc_uint32 cur = 0; cur < datum_count; cur++) {
    cgc_uint32 datum;
    cgc_read(&datum, sizeof(datum));
    data[cur] = datum;
  }

  while(1) {
    cgc_transmit_all(STDOUT, "CHRT", 4);
    cgc_transmit_all(STDOUT, (char*)(&datum_count), 4);

    cgc_uint32 choice;
    cgc_read(&choice, sizeof(choice));
    
    switch(choice) {
    case 1:
      cgc_sparks(pack);
      break;
    case 3:
      cgc_bars(pack);
      break;
    case 4:
      cgc_echo();
      break;
    case 5:
      cgc_seed();
      break;
    case 6:
      cgc_noise();
      break;
    case 7:
      cgc_replacer(pack);
      break;
    default:
      _terminate(0);
    }
  }
}

void cgc_draw_echo(cgc_uint32 count) {
  cgc_uint32 body[count];

  for (cgc_uint32 cur = 0; cur < count; cur++) {
    cgc_uint32 chr;
    cgc_read(&chr, sizeof(chr));
    body[cur] = chr;
  }

  cgc_transmit_all(STDOUT, (char*)(body), count * sizeof(cgc_uint32));
}

void cgc_echo() {
  cgc_uint32 count;
  cgc_read(&count, sizeof(count));

  if ((count < 1) || (count > MAX_STACK_SIZE)) _terminate(-1);

  cgc_draw_echo(count);
}

size_t cgc_read(void* buf, size_t expected_size) {
  size_t actual_size;
  int status;

  status = receive(STDIN, buf, expected_size, &actual_size);

  if (status != 0) _terminate(-1);
  if (actual_size != expected_size) _terminate(-1);

  return actual_size;
}
