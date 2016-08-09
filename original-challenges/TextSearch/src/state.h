#ifndef STATE_H_
#define STATE_H_

#include "constants.h"

typedef struct state cgc_state_t;
struct state {
    unsigned int id;
    unsigned char input;
    unsigned char is_accepting_state;

    cgc_state_t *t1;
    cgc_state_t *t2;
    cgc_state_t *end_state;
};

void cgc_init_regex();
void cgc_clear_regex();
cgc_state_t *cgc_create_state(unsigned char input);
cgc_state_t *cgc_op_concat(cgc_state_t *s1, cgc_state_t *s2);
cgc_state_t *cgc_op_union(cgc_state_t *s1, cgc_state_t *s2);
cgc_state_t *cgc_op_star(cgc_state_t *s1);
cgc_state_t *cgc_op_qmark(cgc_state_t *s1);
cgc_state_t *cgc_op_plus(cgc_state_t *s1);

#endif
