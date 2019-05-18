#include "logger.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int file_exists(char *path)
{
    if (access(path, F_OK) != -1) //file exists
        return 1;

    return 0; //file not exists
}

logger *logger_new(char *path)
{
    logger *new_logger = calloc(1, sizeof(logger));
    new_logger->path = path;

    FILE *f;
    int exists = file_exists(new_logger->path);
    f = fopen(path, "a+");
    new_logger->fd = fileno(f);
    new_logger->queue = log_queue_new();

    if (!exists)
    {
        char buffer[1024] = {0};
        sprintf(buffer, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",
                "start_time", "end_time",
                "inserts", "time_per_insert",
                "gets", "time_per_get",
                "pops", "time_per_pop",
                "keys", "time_per_keys",
                "all", "time_per_all");
        int ignore = write(new_logger->fd, buffer, strlen(buffer));
        ignore = exists;
        exists = ignore;
    }

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
