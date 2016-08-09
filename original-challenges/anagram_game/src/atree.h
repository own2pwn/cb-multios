#ifndef ATREE_H_
#define ATREE_H_

#include "alphabet.h"

typedef struct cgc_atree_link_t {
    unsigned int freq;
    struct cgc_atree_node_t *child;
} cgc_atree_link_t;

typedef struct cgc_atree_node_t {
    unsigned int level;
    unsigned short degree;
    unsigned short max_degree;

    struct cgc_atree_node_t *next_leaf;
    struct cgc_atree_node_t *prev_leaf;

    const char *words[0];
    cgc_atree_link_t children[0];
} cgc_atree_node_t;
#define NODE_MIN_DEGREE 2
#define NODE_MIN_SIZE (sizeof(cgc_atree_node_t) + sizeof(cgc_atree_link_t) * NODE_MIN_DEGREE)

typedef struct {
    unsigned int count;

    cgc_atree_node_t *root;
    cgc_atree_node_t *leaf;

    /* map from level to character */
    char levelmap[ALPHABET_SIZE];
} cgc_atree_t;

cgc_atree_t *cgc_atree_init(cgc_freqtab_t *ftab);
int cgc_atree_add(cgc_atree_t *tree, const char *word);
int cgc_atree_remove(cgc_atree_t *tree, const char *word);
char **cgc_atree_query(cgc_atree_t *tree, const char *word);
char **cgc_atree_query_subset(cgc_atree_t *tree, const char *word);
const char *cgc_atree_cgc_random(cgc_atree_t *tree);
static inline unsigned int cgc_atree_count(cgc_atree_t *tree)
{
    return tree->count;
}

#endif /* !ATREE_H_ */
