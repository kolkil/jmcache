#include "storage/static_file.h"

#define THREADS_NUM 64

typedef struct
{
    hash_table *hash;
    int fd,
        busy;
} thread_data;

int deal_with_client(hash_table *, int);
int dealer_thread(void *);
int join_completed_dealer_threads(thrd_t *, thread_data *);
int create_thread_for_request(thrd_t *, thread_data *, int);
