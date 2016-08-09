#ifndef INTERP_H_
#define INTERP_H_

#include "dict.h"
#include "io.h"
#include "lex.h"

enum {
    VAR_NULL,
    VAR_NUMBER,
    VAR_STRING,
    VAR_ARRAY
};

typedef struct {
    struct cgc__record_t _record;

    unsigned int type;
    union {
        struct {
            char *value;
        } v_string;
        struct {
            int value;
            char *strvalue;
        } v_number;
        struct {
            cgc_dict_t *value;
        } v_array;
    };
} cgc_var_t;

int cgc_program_run(cgc_program_t *prog, cgc_io_t *io);

#endif
