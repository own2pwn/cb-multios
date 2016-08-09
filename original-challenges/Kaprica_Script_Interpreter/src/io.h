#ifndef IO_H_
#define IO_H_

typedef struct {
    int fd;
    char *buf;
    int pos;
    int length;
    int mark;
} cgc_io_t;

void cgc_io_init_fd(cgc_io_t *io, int fd);
void cgc_io_init_bytes(cgc_io_t *io, const char *str, unsigned int length);
void cgc_io_init_string(cgc_io_t *io, const char *str);
void cgc_io_mark(cgc_io_t *io);
int cgc_io_rewind(cgc_io_t *io);
int cgc_io_tell(cgc_io_t *io);
int cgc_io_seek(cgc_io_t *io, int pos);
int cgc_io_getc(cgc_io_t *io);
int cgc_io_ungetc(cgc_io_t *io);
int cgc_io_peek(cgc_io_t *io);
int cgc_io_read(cgc_io_t *io, char *buf, unsigned int cnt);

#endif
