#ifndef BN_H_
#define BN_H_

#include <stdint.h>
#include "rng.h"

typedef struct {
    cgc_uint32_t *data;
    unsigned int length;
} cgc_bn_t;

void cgc_bn_init(cgc_bn_t *bn);
int cgc_bn_init_bits(cgc_bn_t *bn, unsigned int bits);
void cgc_bn_destroy(cgc_bn_t *bn);
unsigned int cgc_bn_length(const cgc_bn_t *bn);

int cgc_bn_const(cgc_bn_t *bn, cgc_uint32_t c);
int cgc_bn_copy(cgc_bn_t *bn, const cgc_bn_t *src);
int cgc_bn_from_bytes(cgc_bn_t *bn, const unsigned char *bytes, unsigned int n);
void cgc_bn_to_bytes(const cgc_bn_t *bn, unsigned char *bytes, unsigned int n);
int cgc_bn_cgc_random(cgc_bn_t *bn, const cgc_bn_t *max, cgc_rng_t *rng);

void cgc_bn_slr(cgc_bn_t *dst, unsigned int cnt);
int cgc_bn_mul(cgc_bn_t *dst, const cgc_bn_t *a, const cgc_bn_t *b);
int cgc_bn_div(cgc_bn_t *Q, cgc_bn_t *R, const cgc_bn_t *N, const cgc_bn_t *D);
int cgc_bn_modexp(cgc_bn_t *dst, const cgc_bn_t *b, const cgc_bn_t *e, const cgc_bn_t *m);

int cgc_bn_cmp(const cgc_bn_t *a, const cgc_bn_t *b);

int cgc_bn_self_test();

#endif
