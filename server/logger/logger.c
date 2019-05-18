#include "logger.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

logger *logger_new(char *path)
{
    logger *new_logger = calloc(1, sizeof(logger));
    new_logger->path = path;
    FILE *f = fopen(path, "a+");
    new_logger->fd = fileno(f);
    new_logger->queue = log_queue_new();

    return new_logger;
}

int logger_log(logger *log, char *value)
{
    int l = write(log->fd, value, strlen(value));

    return l;
}

int log_whole_queue(logger *log)
{
    char *data = NULL;
    int i = 0;

    for (i = 0;; ++i)
    {
        data = log_dequeue(log->queue);
        if (data == NULL)
            return i;

        free(data);
        logger_log(log, data);
    }

    return i;
}
