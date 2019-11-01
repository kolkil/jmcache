#include "program.h"
#include "threading.h"
#include "communication/socket.h"
#include "../shared/debug_print.h"

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
        hash_table_load_from_file(hash, fileno(f));
        fclose(f);
    }

    socket_params *params = prepare_socket(cnf->server_address, cnf->server_port);

    if (params == NULL)
        return -1;

    if (params->fd <= 0)
        return params->fd;

    thrd_t t_ids[THREADS_NUM];
    thrd_t error_logger_thread,
        traffic_logger_thread;
    thread_data threads_data[THREADS_NUM];
    logger_thread_data traffic_data,
        error_data;

    traffic_data.log = logger_new(cnf->traffic_file);
    traffic_data.stop = 0;

    if (!traffic_data.log->file_existed)
        write_traffic_log_format(traffic_data.log);

    error_data.log = logger_new(cnf->error_file);
    error_data.stop = 0;

    for (int i = 0; i < THREADS_NUM; ++i) //set default values
    {
        threads_data[i].hash = hash;
        threads_data[i].fd = -1;
        threads_data[i].busy = 0;
        threads_data[i].error_logger = error_data.log;
        threads_data[i].traffic_logger = traffic_data.log;
    }

    if (cnf->traffic_log && thrd_create(&traffic_logger_thread, logger_thread, &traffic_data) != thrd_success)
    {
        debug_print_raw("could not create traffic_thread");
        return 1;
    }

    if (cnf->error_log && thrd_create(&error_logger_thread, logger_thread, &error_data) != thrd_success)
    {
        debug_print_raw("could not create error_thread");
        return 1;
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

        for (int flag = -1; flag == -1;) // try to create thread for client
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

    error_data.stop = 1;
    traffic_data.stop = 1;

    thrd_join(traffic_logger_thread, NULL);
    thrd_join(error_logger_thread, NULL);

    free_socket_params(params);

    if (cnf->static_save)
    {
        FILE *f = fopen(cnf->save_path, "w+b");
        hash_table_save_to_file(hash, fileno(f));
        fclose(f);
    }

    debug_print("main loop", 0);
    free_hash_table(hash);
    free_logger(error_data.log);
    free_logger(traffic_data.log);

    return 0;
}
