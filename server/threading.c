#include "threading.h"
#include "utils/debug_print.h"

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

int logger_thread(void *data)
{
    debug_print("logger_thread", 1);
    logger_thread_data *ltd = (logger_thread_data *)data;
    logger *log = logger_new(ltd->path);

    if (log->fd <= 0)
        return 1;

    while (ltd->stop != 1)
    {
        debug_print_int(ltd->stop);
        char *data;

        while ((data = log_dequeue(ltd->log->queue)) != NULL)
        {
            debug_print(data, 2);
            logger_log(ltd->log, data);
            free(data);
        }
        sleep(1);
    }

    debug_print("logger_thread", 0);

    return 0;
}

int deal_with_client(thread_data *data)
{
    int result = 1;
    connection_statistics stats;
    memset(&stats, 0, sizeof(connection_statistics));
    char buffer[1024] = {0};
    int income_time = time(NULL);

    while (result)
        result = read_data_send_response(data->hash, data->fd, &stats);

    int end_time = time(NULL);

    sprintf(buffer, "%d\t%d\t%d\t%f\t%d\t%f\t%d\t%f\t%d\t%f\t%d\t%f\n", income_time, end_time,
            stats.insert.count, msd(stats.insert.time, stats.insert.count),
            stats.get.count, msd(stats.get.time, stats.get.count),
            stats.pop.count, msd(stats.pop.time, stats.pop.count),
            stats.keys.count, msd(stats.keys.time, stats.keys.count),
            stats.all.count, msd(stats.all.time, stats.all.count));

    log_enqueue(data->traffic_logger->queue, buffer);

    return result;
}

int dealer_thread(void *data)
{
    thread_data *t_data = (thread_data *)data;
    deal_with_client(t_data);
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
