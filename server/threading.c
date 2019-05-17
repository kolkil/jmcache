#include "threading.h"
#include "utils/debug_print.h"

#include <unistd.h>

int deal_with_client(hash_table *hash, int client_fd)
{
    int result = 1;

    while (result)
    {
        result = read_data_send_response(hash, client_fd);
    }

    return result;
}

int dealer_thread(void *data)
{
    thread_data *t_data = (thread_data *)data;
    deal_with_client(t_data->hash, t_data->fd);
    close(t_data->fd);
    t_data->busy = 2;
    return 0;
}

int join_completed_dealer_threads(thrd_t *threads, thread_data *t_data)
{
    for (int k = 0; k < THREADS_NUM; ++k)
    {
        if (t_data[k].busy == 2)
        {
            thrd_join(threads[k], NULL);
            t_data[k].busy = 0;
        }
    }
    return 0;
}

int create_thread_for_request(thrd_t *threads, thread_data *t_data, int client_fd)
{
    for (int k = 0; k < THREADS_NUM; ++k)
    {
        join_completed_dealer_threads(threads, t_data);
        if (t_data[k].busy)
            continue;

        t_data[k].fd = client_fd;

        if (thrd_create(&threads[k], dealer_thread, &t_data[k]) != thrd_success)
        {
            debug_print("Could not create thread", 2);
            close(t_data[k].fd);
            return -1;
        }
        t_data[k].busy = 1;
        return k;
    }

    return -1;
}
