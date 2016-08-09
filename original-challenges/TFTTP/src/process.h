#ifndef PROCESS_H
#define PROCESS_H
#include "proto.h"

#define MAXQS 2048

typedef struct cgc_qs {
    char key[128];
    char data[256];
} cgc_qs;

int cgc_load_data();
int cgc_process_next_dgram();
cgc_tfttp_pkt* cgc_do_put();
cgc_tfttp_pkt* cgc_do_get();
cgc_tfttp_pkt* cgc_do_rand();
#endif
