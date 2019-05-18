#include "queue.h"

#include <string.h>
#include <stdlib.h>

log_queue *log_queue_new()
{
    log_queue *new = calloc(1, sizeof(log_queue));

    for (int i = 0; i < QUEUE_LEN; ++i)
        new->element[i] = NULL;

    mtx_init(&new->lock, mtx_plain);
    new->first = 0;
    new->last = 0;
    new->empty = 1;

    return new;
}

int log_enqueue(log_queue *queue, char *value)
{
    mtx_lock(&queue->lock);
    int index_to_enqueue = queue->empty ? 0 : (queue->last + 1) % QUEUE_LEN;

    if (queue->element[index_to_enqueue] != NULL) // queue full
        return 1;

    queue->empty = 0;
    int value_len = strlen(value);
    queue->element[index_to_enqueue] = calloc(value_len + 1, sizeof(char));
    memcpy(queue->element[index_to_enqueue], value, value_len);
    queue->last = index_to_enqueue;
    mtx_unlock(&queue->lock);

    return 0;
}

char *log_dequeue(log_queue *queue)
{
    mtx_lock(&queue->lock);

    if (queue->element[queue->first] == NULL) // queue empty
    {
        queue->empty = 1;
        return NULL;
    }

    char *to_return = queue->element[queue->first];
    queue->element[queue->first] = NULL;
    queue->first = (queue->first + 1) % QUEUE_LEN;
    mtx_unlock(&queue->lock);

    return to_return;
}
