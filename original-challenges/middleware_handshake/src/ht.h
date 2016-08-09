#ifndef HT_H_
#define HT_H_

typedef struct cgc_ht_node_t {
    struct cgc_ht_node_t *next;
    struct cgc_ht_node_t *prev;
    char *k;
    void *value;
} cgc_ht_node_t;

typedef struct {
    struct cgc_ht_node_t *first;
    struct cgc_ht_node_t **tbl;

    unsigned int tbl_size;
    unsigned int tbl_count;
} cgc_ht_t;

int cgc_ht_init(cgc_ht_t *tbl);
int cgc_ht_compare(const char *a, const char *b);
int cgc_ht_insert(cgc_ht_t *tbl, const char *k, void *v);
int cgc_ht_lookup(cgc_ht_t *tbl, const char *k, cgc_ht_node_t **node);
int cgc_ht_delete(cgc_ht_t *tbl, const char *k, void **v);
cgc_ht_node_t *cgc_ht_first(cgc_ht_t *tbl);
cgc_ht_node_t *cgc_ht_next(cgc_ht_t *tbl, cgc_ht_node_t *);
static inline void *cgc_ht_node_value(cgc_ht_node_t *node)
{
    return node->value;
}

#endif
