#ifndef STRIO_H_
#define STRIO_H_

typedef struct {
    char *buf;
    int pos;
    int length;
} cgc_strio_t;

int cgc_strio_init(cgc_strio_t *s);
void cgc_strio_free(cgc_strio_t *s);
char *cgc_strio_dup(cgc_strio_t *s);
int cgc_strio_append_char(cgc_strio_t *s, char c);

#endif
