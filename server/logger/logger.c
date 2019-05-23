#include "logger.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

char *copy_string(char *str)
{
    int len = strlen(str);
    if (len <= 0)
        return NULL;

    char *new = calloc(sizeof(char), len + 1);
    memcpy(new, str, len);

    return new;
}

int file_exists(char *path)
{
    if (access(path, F_OK) != -1) //file exists
        return 1;

    return 0; //file not exists
}

logger *logger_new(char *path)
{
    logger *new_logger = calloc(1, sizeof(logger));
    new_logger->path = copy_string(path);

    FILE *f;
    new_logger->file_existed = file_exists(new_logger->path);
    f = fopen(path, "a+");
    new_logger->fd = fileno(f);
    new_logger->queue = log_queue_new();

    return new_logger;
}

void free_logger(logger *log)
{
    free(log->path);
    free_log_queue(log->queue);
    free(log);
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

int write_traffic_log_format(logger *log)
{
    char buffer[1024] = {0};
    sprintf(buffer, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",
            "start_time", "end_time",
            "inserts", "time_per_insert",
            "gets", "time_per_get",
            "pops", "time_per_pop",
            "keys", "time_per_keys",
            "all", "time_per_all");

    int ignore = write(log->fd, buffer, strlen(buffer));

    return ignore;
}
