#include "stack.h"

#ifndef ACCELFUNC_H_
#define ACCELFUNC_H_

typedef double (*cgc_handle_op_t)(cgc_stack_t **args, int *is_bad_formula);

typedef struct {
    const char *name;
    cgc_handle_op_t function;
} cgc_operator_t;

double cgc_handle_op_avg(cgc_stack_t **args, int *is_bad_formula);
double cgc_handle_op_count(cgc_stack_t **args, int *is_bad_formula);
double cgc_handle_op_max(cgc_stack_t **args, int *is_bad_formula);
double cgc_handle_op_median(cgc_stack_t **args, int *is_bad_formula);
double cgc_handle_op_min(cgc_stack_t **args, int *is_bad_formula);
double cgc_handle_op_stddev(cgc_stack_t **args, int *is_bad_formula);
double cgc_handle_op_abs(cgc_stack_t **args, int *is_bad_formula);
double cgc_handle_op_add(cgc_stack_t **args, int *is_bad_formula);
double cgc_handle_op_cos(cgc_stack_t **args, int *is_bad_formula);
double cgc_handle_op_ln(cgc_stack_t **args, int *is_bad_formula);
double cgc_handle_op_log10(cgc_stack_t **args, int *is_bad_formula);
double cgc_handle_op_power(cgc_stack_t **args, int *is_bad_formula);
double cgc_handle_op_product(cgc_stack_t **args, int *is_bad_formula);
double cgc_handle_op_quotient(cgc_stack_t **args, int *is_bad_formula);
double cgc_handle_op_sin(cgc_stack_t **args, int *is_bad_formula);
double cgc_handle_op_sqrt(cgc_stack_t **args, int *is_bad_formula);
double cgc_handle_op_subtract(cgc_stack_t **args, int *is_bad_formula);
double cgc_handle_op_sum(cgc_stack_t **args, int *is_bad_formula);

#endif /* ACCELFUNC_H */
