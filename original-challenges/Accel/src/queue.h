#ifndef QUEUE_H_
#define QUEUE_H_

typedef struct queue cgc_queue_t;
struct queue {
    char *data;
    cgc_queue_t *next;
};

char *cgc_peek_front(cgc_queue_t *queue);
int cgc_enqueue(cgc_queue_t **queue, char *data);
int cgc_enqueue_copy(cgc_queue_t **queue, char *data, size_t size);
char *cgc_dequeue_copy(cgc_queue_t **queue);
void cgc_clear_queue(cgc_queue_t **queue);

#endif /* QUEUE_H */
