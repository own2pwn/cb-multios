#ifndef KX_H_
#define KX_H_

#include "bn.h"
#include "rng.h"

typedef struct {
    cgc_bn_t P;
    cgc_bn_t Q;
    cgc_bn_t G;
    cgc_bn_t a;
    cgc_bn_t b;
    cgc_bn_t S;
} cgc_kx_t;

enum {
    KX_GROUP_1024_160,
    KX_GROUP_2048_224,
    KX_GROUP_2048_256,
    KX_GROUP_3072_3072,
    KX_GROUP_END
};

int cgc_kx_init_std(cgc_kx_t *kx, unsigned int group);
int cgc_kx_init(cgc_kx_t *kx, cgc_bn_t *P, cgc_bn_t *Q, cgc_bn_t *G);
void cgc_kx_destroy(cgc_kx_t *kx);
int cgc_kx_gen_a(cgc_kx_t *kx, cgc_rng_t *rng);
int cgc_kx_get_A(cgc_kx_t *kx, cgc_bn_t *A);
int cgc_kx_set_b(cgc_kx_t *kx, cgc_bn_t *b);
int cgc_kx_get_sk(cgc_kx_t *kx, unsigned char *out, unsigned int n);

int cgc_kx_self_test();

#endif
