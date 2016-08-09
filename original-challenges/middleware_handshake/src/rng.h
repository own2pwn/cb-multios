#ifndef RNG_H_
#define RNG_H_

typedef struct {
    const struct cgc_rng_def_t *def;
    void *priv;
} cgc_rng_t;

typedef struct cgc_rng_def_t {
    const char *name;
    unsigned int id;
    int (*init) (cgc_rng_t *rng);
    int (*get_bytes) (cgc_rng_t *rng, unsigned char *out, unsigned int cnt);
} cgc_rng_def_t;

enum {
    RNG_SYSTEM,
    RNG_LCG,
    RNG_END
};

int cgc_rng_init(cgc_rng_t *, unsigned int id);

static inline int cgc_rng_get_bytes(cgc_rng_t *rng, unsigned char *out, unsigned int cnt)
{
    return rng->def->get_bytes(rng, out, cnt);
}

#endif
