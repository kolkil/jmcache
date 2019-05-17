#include "program.h"
#include "threading.h"
#include "communication/socket.h"
#include "utils/debug_print.h"

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <threads.h>
#include <signal.h>

static volatile int keep_running = 1;

void intHandler(int dummy)
{
    keep_running = dummy = 0;
    return;
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
    thrd_t logger_thread;
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
