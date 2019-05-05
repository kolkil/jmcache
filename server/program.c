#include "program.h"
#include "communication/socket.h"
#include "communication/jobs.h"
#include "utils/debug_print.h"

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <threads.h>

typedef struct
{
    hash_table *hash;
    int fd,
        busy;
} thread_data;

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
    int result = deal_with_client(t_data->hash, t_data->fd);
    close(t_data->fd);
    if (result < 0)
        debug_print("Error", 2);
    t_data->busy = 2;
    return 0;
}

int start_program(config_values *cnf)
{
    int client_fd = -1;
    socket_params *params = prepare_socket(cnf->server_address, cnf->server_port);
    hash_table *hash = get_hash_table();
    if (params->fd <= 0)
        return params->fd;

    thrd_t t_ids[THREADS_NUM];
    thread_data threads_data[THREADS_NUM];

    debug_print("main loop", 1);

    for (unsigned long long int i = 0;; ++i)
    {
        debug_print_raw("REQUEST");
        debug_print_raw_int(i);
        client_fd = socket_listen_and_accept(params);
        if (client_fd <= 0)
        {
            debug_print("Could not estabilish connection", 2);
            continue;
        }
        for (int k = 0; k < THREADS_NUM; ++k)
        {
            if (threads_data[k].busy)
                continue;
            threads_data[k].hash = hash;
            threads_data[k].fd = client_fd;
            if (thrd_create(&t_ids[k], dealer_thread, &threads_data[k]) != thrd_success)
            {
                debug_print("Could not create thread", 2);
                close(threads_data[k].fd);
                break;
            }
            threads_data[k].busy = 1;
            break;
        }
        for (int k = 0; k < THREADS_NUM; ++k)
        {
            if (threads_data[k].busy == 2)
            {
                thrd_join(t_ids[k], NULL);
                threads_data[k].busy = 0;
            }
        }
    }

    debug_print("main loop", 0);

    return 0;
}
