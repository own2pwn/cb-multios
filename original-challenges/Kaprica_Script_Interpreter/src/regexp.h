#ifndef REGEXP_H_
#define REGEXP_H_

#include <stdlib.h>
#include <stdint.h>

enum {
    RFLAG_ONE = 0,
    RFLAG_ONE_OR_MORE,
    RFLAG_ZERO_OR_MORE,
    RFLAG_ZERO_OR_ONE,
    RFLAG_FREED
};

enum {
    ROP_FRONT,
    ROP_BACK,
    ROP_WILDCARD,
    ROP_SEQ,
    ROP_CLASS,
    ROP_GROUP,
    ROP_OR
};

typedef struct cgc_rop_t {
    unsigned char op, flag;
    struct cgc_rop_t *next, *parent;

    union {
        struct {
            struct cgc_rop_t *e[0];
        } r_or;
        struct {
            cgc_uint8_t data[0];
        } r_class;
        struct {
            struct cgc_rop_t *e;
        } r_group;
        struct {
            char seq;
        } r_seq;
    };
} cgc_rop_t;

typedef struct {
    char *input;
    cgc_rop_t *tree;

    cgc_rop_t **states;
    unsigned int num_states;
    unsigned int max_states;

    // loop detection (only an issue for begin and end)
    cgc_rop_t **prev_states;
    unsigned int num_prev_states;
} cgc_regexp_t;

int cgc_regexp_init(cgc_regexp_t *r, const char *str);
int cgc_regexp_free(cgc_regexp_t *r);
int cgc_regexp_match(cgc_regexp_t *r, const char *input);

#endif
