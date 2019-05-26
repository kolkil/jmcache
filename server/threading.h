#include "communication/jobs.h"
#include "logger/logger.h"

#define THREADS_NUM 64

typedef struct
{
    hash_table *hash;
    int fd,
        busy;
    logger *traffic_logger,
        *error_logger;
} thread_data;

typedef struct
{
    int stop;
    char *path;
    logger *log;
} logger_thread_data;

int dealer_thread(void *);
int join_completed_dealer_threads(thrd_t *, thread_data *);
int create_thread_for_request(thrd_t *, thread_data *, int);
int logger_thread(void *);
