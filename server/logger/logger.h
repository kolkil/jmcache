#include "queue.h"

typedef struct
{
    char *path;
    int fd;
    log_queue *queue;
} logger;

logger *logger_new(char *);
int logger_log(logger *, char *);
