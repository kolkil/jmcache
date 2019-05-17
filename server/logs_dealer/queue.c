#include "queue.h"

void init_queue(log_queue *queue)
{
    for (int i = 0; i < QUEUE_LEN; ++i)
        queue->element[i] = NULL;

    mtx_init(&queue->lock, mtx_plain);
    queue->first = 0;
    queue->last = 0;
}

int log_enqueue(log_queue *queue, char *value)
{
    mtx_lock(&queue->lock);
    int index_to_enqueue = (queue->last + 1) % QUEUE_LEN;

    if (queue->element[index_to_enqueue] != NULL) // queue full
        return 1;

    queue->element[index_to_enqueue] = value;
    queue->last = index_to_enqueue;
    mtx_unlock(&queue->lock);

    return 0;
}

char *log_dequeue(log_queue *queue)
{
    mtx_lock(&queue->lock);

    if (queue->element[queue->first] == NULL) // queue empty
        return NULL;

    char *to_return = queue->element[queue->first];
    queue->element[queue->first] = NULL;
    queue->first = (queue->first + 1) % QUEUE_LEN;
    mtx_unlock(&queue->lock);

    return to_return;
}
