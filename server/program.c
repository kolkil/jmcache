#include "program.h"
#include "communication/socket.h"
#include "utils/debug_print.h"
#include "storage/static_file.h"

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <threads.h>
#include <signal.h>

typedef struct
{
    hash_table *hash;
    int fd,
        busy;
} thread_data;

static volatile int keep_running = 1;

void intHandler(int dummy)
{
    keep_running = dummy = 0;
    return;
}

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

int conditional_stop()
{
    if (!keep_running)
    {
        debug_print_raw("SIGINT captured, shutting down\n");
        return 1;
    }
    return 0;
}

int start_program(config_values *cnf)
{
    signal(SIGINT, intHandler);
    int client_fd = -1;
    hash_table *hash = get_hash_table();

    if (cnf->static_load)
    {
        FILE *f = fopen(cnf->save_path, "rb");
        load_from_file(fileno(f), hash);
        fclose(f);
    }

    socket_params *params = prepare_socket(cnf->server_address, cnf->server_port);

    if (params->fd <= 0)
        return params->fd;

    thrd_t t_ids[THREADS_NUM];
    thread_data threads_data[THREADS_NUM];

    for (int i = 0; i < THREADS_NUM; ++i) //set default values
    {
        threads_data[i].hash = hash;
        threads_data[i].fd = -1;
        threads_data[i].busy = 0;
    }

    debug_print("main loop", 1);

    for (unsigned long long int request_counter = 0;; ++request_counter)
    {
        if (conditional_stop())
        {
            join_completed_dealer_threads(t_ids, threads_data);
            break;
        }

        debug_print_raw("REQUEST");
        debug_print_raw_int(request_counter);

        client_fd = socket_listen_and_accept(params);

        if (client_fd <= 0)
        {
            debug_print("Could not estabilish connection", 2);
            continue;
        }

        for (int flag = -1; flag == -1;) //try to create thread for client
        {
            flag = create_thread_for_request(t_ids, threads_data, client_fd);

            if (flag != -1)
            {
                debug_print_raw_int(flag);
                debug_print_raw("\n");
            }
        }
    }

    join_completed_dealer_threads(t_ids, threads_data);

    if (cnf->static_save)
    {
        FILE *f = fopen(cnf->save_path, "w+b");
        save_to_file(fileno(f), hash);
        fclose(f);
    }

    debug_print("main loop", 0);
    free_hash_table(hash);

    return 0;
}
