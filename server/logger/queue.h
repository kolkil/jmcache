#include <threads.h>

#define QUEUE_LEN 2048

typedef struct
{
    int last,
        first,
        empty;
    char *element[QUEUE_LEN];
    mtx_t lock;
} log_queue;

log_queue *log_queue_new();
void free_log_queue(log_queue *);
int log_enqueue(log_queue *, char *);
char *log_dequeue(log_queue *);
