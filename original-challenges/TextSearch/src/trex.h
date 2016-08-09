#ifndef TREP_H_
#define TREP_H_
#include "state.h"

typedef enum {PARTIAL, ALL} cgc_match_type_e;

void cgc_debug_state(cgc_state_t *state);
void cgc_match(cgc_state_t *state, unsigned char *str, cgc_match_type_e match_type);
cgc_state_t *cgc_evalrpn(unsigned char *rpn);

#endif
