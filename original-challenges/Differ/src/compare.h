#ifndef COMPARE_H_
#define COMPARE_H_

#include "hash.h"
#include "sfile.h"

typedef struct linecmp cgc_lc_t;
struct linecmp {
    char *pline;
    char *no_ws_line;

    cgc_lhash_t lhash;
    cgc_lhash_t no_ws_lhash;

}; // line comparator

typedef struct linecmp_list cgc_lcll_t;
struct linecmp_list {
    cgc_lc_t *lc;
    cgc_lcll_t *next;
};

cgc_lcll_t *cgc_pre_process(cgc_SFILE *sfp, cgc_size_t *wordc, cgc_size_t *linec);
cgc_lc_t *cgc_new_linecmp(char *line);
void cgc_free_linecmp_list(cgc_lcll_t **head);

#endif
