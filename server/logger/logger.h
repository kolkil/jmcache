#ifndef _logger_h_
#define _logger_h_

#include "queue.h"

#include <stdio.h>

typedef struct
{
    char *path;
    int fd,
        file_existed;
    log_queue *queue;
    FILE *file;
} logger;

logger *logger_new(char *);
void free_logger(logger *);
int logger_log(logger *, char *);
int write_traffic_log_format(logger *);

#endif

