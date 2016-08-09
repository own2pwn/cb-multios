#ifndef MODES_H_
#define MODES_H_

#include "codes.h"

typedef struct {
    cgc_code_t *code;
    unsigned int mode;
    union {
        unsigned char xm[MAX_BSIZE / 8];
        cgc_uint64_t cm;
    } state;
} cgc_mode_t;

enum {
    MODE_NULL,
    MODE_BCM,
    MODE_XIM,
    MODE_XOM,
    MODE_END
};

int cgc_modes_init(cgc_mode_t *mode, unsigned int mode_id, cgc_code_t *code);
int cgc_modes_encode(cgc_mode_t *mode, const unsigned char *inb, unsigned int inlen, 
    unsigned char **outb, unsigned int *outlen);
int cgc_modes_decode(cgc_mode_t *mode, const unsigned char *inb, unsigned int inlen, 
    unsigned char **outb, unsigned int *outlen);

int cgc_modes_self_test();

#endif
