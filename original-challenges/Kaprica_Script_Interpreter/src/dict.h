#ifndef DICT_H_
#define DICT_H_

typedef void (*cgc_record_free_t) (void *);

typedef struct cgc__record_t {
    struct cgc__record_t *next, *prev;
    struct cgc__record_t *tbl_next, *tbl_prev;
    char *name;
} cgc__record_t;

typedef struct {
    cgc_record_free_t free_fn;
    cgc__record_t *head;
    cgc__record_t **tbl;

    unsigned int count;
    unsigned int length;
} cgc_dict_t;

int cgc_dict_init(cgc_dict_t *dict, cgc_record_free_t);
void cgc_dict_free(cgc_dict_t *dict);
void *cgc_dict_get(cgc_dict_t *dict, const char *name);
int cgc_dict_add(cgc_dict_t *dict, const char *name, void *value);

#endif
