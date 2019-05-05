#include "program.h"
#include "communication/socket.h"
#include "communication/jobs.h"
#include "utils/debug_print.h"

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int do_job(hash_table *hash, mcache_request request, int client_fd)
{
    switch (request.header.command)
    {
    case INSERT:
        debug_print("INSERT", 1);
        execute_insert(hash, request, client_fd);
        debug_print("INSERT", 0);
        break;
    case GET:
        debug_print("GET", 1);
        execute_get(hash, request, client_fd);
        debug_print("GET", 0);
        break;
    case POP:
        debug_print("POP", 1);
        execute_pop(hash, request, client_fd);
        debug_print("POP", 0);
        break;
    case KEYS:
        debug_print("KEYS", 1);
        execute_keys(hash, client_fd);
        debug_print("KEYS", 0);
        break;
    case ALL:
        debug_print("ALL", 1);
        execute_get(hash, request, client_fd);
        debug_print("ALL", 0);
        break;
    default:
        break;
    }
    return 0;
}

int read_data_send_response(hash_table *hash, int client_fd)
{
    debug_print("read_data_send_response", 1);
    mcache_request request = read_request(client_fd);
    if (request.code != 0)
    {
        debug_print("Error parsing request", 2);
        debug_print_int((int)request.header.command);
        debug_print("read_data_send_response", 0);
        return 0;
    }
    debug_print_int((int)request.header.command);
    int result = do_job(hash, request, client_fd);

    debug_print("read_data_send_response", 0);

    return !result;
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

int start_program(config_values *cnf)
{
    int client_fd = -1,
        tmp = 0;
    socket_params *params = prepare_socket(cnf->server_address, cnf->server_port);
    hash_table *hash = get_hash_table();
    if (params->fd <= 0)
        return params->fd;

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
        tmp = deal_with_client(hash, client_fd);
        close(client_fd);
        if (tmp < 0)
        {
            debug_print("Error", 2);
        }
    }

    debug_print("main loop", 0);

    return 0;
}
