#ifndef STACK_H_
#define STACK_H_

typedef struct stack cgc_stack_t;
struct stack {
    char *data;
    cgc_stack_t *next;
};

char *cgc_peek_top(cgc_stack_t *stack);
int cgc_push(cgc_stack_t **stack, char *data);
int cgc_push_copy(cgc_stack_t **stack, char *data, size_t size);
char *cgc_pop_copy(cgc_stack_t **stack);
void cgc_clear_stack();

#endif /* STACK_H */
