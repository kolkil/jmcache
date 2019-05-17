#include "storage/static_file.h"
#include "logs_dealer/logger.h"

#define THREADS_NUM 64

typedef struct
{
    hash_table *hash;
    int fd,
        busy;
} thread_data;

typedef struct
{
    int stop;
    char *path;
    logger *log;
} logger_thread_data;

int deal_with_client(hash_table *, int);
int dealer_thread(void *);
int join_completed_dealer_threads(thrd_t *, thread_data *);
int create_thread_for_request(thrd_t *, thread_data *, int);
int traffic_logger_thread(void *);
int error_logger_thread(void *);
