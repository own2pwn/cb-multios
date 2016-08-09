#ifndef MATRIX_H_
#define MATRIX_H_

#define MAX_ROWS 16
#define MAX_COLS 16

#define SUCCESS 0
#define ERROR 1
#define FAIL -1

#define GET_OFFSET(_row, _col, _num_cols) (((_row) * (_num_cols)) + (_col))

typedef enum  { SHORT = 1, INT = 2, FLOAT = 4 } cgc_mtype_e;

typedef struct matrix cgc_matrix_t;
struct matrix {
    int num_rows;
    int num_cols;
    cgc_mtype_e ptype;
    union {
        short *psdata;
        int *pldata;
        float *pfdata;
    };

    int (*get_cell)(cgc_matrix_t *, unsigned int, unsigned int);
    float (*get_fcell)(cgc_matrix_t *, unsigned int, unsigned int);
    int (*set_cell)(cgc_matrix_t *, unsigned int, unsigned int, int);
    int (*set_fcell)(cgc_matrix_t *, unsigned int, unsigned int, float);
    int (*set_rows_cols)(cgc_matrix_t *, unsigned int, unsigned int);
    void (*cgc_print_matrix)(cgc_matrix_t *);

    char data[];
};


cgc_matrix_t *cgc_create_matrix(cgc_mtype_e ptype, char *data);

int cgc_m_add(cgc_matrix_t *m1, cgc_matrix_t *m2, cgc_matrix_t *m_result);
int cgc_m_subtract(cgc_matrix_t *m1, cgc_matrix_t *m2, cgc_matrix_t *m_result);
int cgc_m_multiply(cgc_matrix_t *m1, cgc_matrix_t *m2, cgc_matrix_t *m_result);
int cgc_m_rref(cgc_matrix_t *m, cgc_matrix_t *m_result);
int cgc_m_transpose(cgc_matrix_t *m);
int cgc_swap_row(cgc_matrix_t *m, int row_idx1, int row_idx2);
int cgc_swap_col(cgc_matrix_t *m, int col_idx1, int col_idx2);

#endif
