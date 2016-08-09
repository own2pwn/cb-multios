#ifndef LEX_H_
#define LEX_H_

#include "io.h"

#define PATTERN_INVALID NULL
#define PATTERN_EMPTY ((void *)(-1))
#define PATTERN_BEGIN ((void *)(-2))
#define PATTERN_END ((void *)(-3))

enum {
    STMT_INVALID = 0,
    STMT_IF,
    STMT_WHILE,
    STMT_FOR,
    STMT_FOREACH,
    STMT_CONTINUE,
    STMT_BREAK,
    STMT_NEXT,
    STMT_EXIT,
    STMT_PRINT,
    STMT_EXPR
};

enum {
    OP_INVALID = 0,
    OP_CONST_STRING,
    OP_CONST_INT,
    OP_CONST_REGEXP,
    OP_FIELD, OP_FIELD_VAR,
    OP_VAR,
    OP_ASSIGN, OP_ASSIGN_ADD, OP_ASSIGN_SUB,
    OP_ASSIGN_MUL, OP_ASSIGN_DIV, OP_ASSIGN_MOD,
    OP_CONDITIONAL,
    OP_OR, OP_AND, // logical
    OP_MATCH, OP_NOT_MATCH,
    OP_LT, OP_GT, OP_LTE, OP_GTE, OP_EQ, OP_NEQ,
    OP_CONCAT,
    OP_ADD, OP_SUB,
    OP_MUL, OP_DIV, OP_MOD,
    OP_NEGATE,
    OP_NOT,
    OP_INC_PRE, OP_INC_POST,
    OP_DEC_PRE, OP_DEC_POST,
    OP_END,

    // used internally in cgc_parse_expression
    OP_DEC, OP_INC,
    OP_NEGATE_OR_SUB,
};

typedef struct cgc_expr_t {
    unsigned int op;
    struct cgc_expr_t *next, *prev;
    union {
        struct {
            char *value; 
        } e_cstring;
        struct {
            int value;
        } e_cint;
        struct {
            char *value;
        } e_cregexp;
        struct {
            char *name;
        } e_var;
        struct {
            struct cgc_expr_t *expr;
        } e_unop;
        struct {
            struct cgc_expr_t *lhs, *rhs;
        } e_binop;
        struct {
            struct cgc_expr_t *cond;
            struct cgc_expr_t *vtrue;
            struct cgc_expr_t *vfalse;
        } e_cond;
    };
} cgc_expr_t;

typedef struct cgc_stmt_t {
    unsigned int type;
    struct cgc_stmt_t *next;

    union {
        struct {
            cgc_expr_t *cond;
            struct cgc_stmt_t *child;
        } s_if;
        struct {
            unsigned int post;
            cgc_expr_t *cond;
            struct cgc_stmt_t *child;
        } s_while;
        struct {
            cgc_expr_t *init;
            cgc_expr_t *cond;
            cgc_expr_t *post;
            struct cgc_stmt_t *child;
        } s_for;
        struct {
            char *var;
            char *array;
            struct cgc_stmt_t *child;
        } s_foreach;
        struct {
            int value;
        } s_exit;
        struct {
            cgc_expr_t *fmt;
            cgc_expr_t *expr;
        } s_print;
        struct {
            cgc_expr_t *expr;
        } s_expr;
    };
} cgc_stmt_t;

typedef struct cgc_pattern_t {
    void *pattern;
    cgc_stmt_t *stmt;

    struct cgc_pattern_t *next;
} cgc_pattern_t;

typedef struct {
    cgc_io_t *io;
    cgc_pattern_t *patterns;
} cgc_program_t;

void cgc_program_init(cgc_program_t *prog, cgc_io_t *io);
int cgc_program_parse(cgc_program_t *prog);

#endif
