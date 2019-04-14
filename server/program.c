#include "program.h"
#include "communication/socket.h"
#include "communication/protocol.h"
#include "storage/vector.h"
#include "storage/hash_table.h"
#include "utils/debug_print.h"

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int execute_insert(hash_table *hash, mcache_request request, int client_fd)
{
    int insert_result = hash_table_insert(hash, request.key, request.data);
    if (insert_result)
    {
        send_response_header(ERROR, NO_DATA, 0);
        return 1;
    }
    send_response_header(OK, NO_DATA, 0);
    return 0;
}

int execute_get(hash_table *hash, mcache_request request, int client_fd)
{
    char *value = (char *)hash_table_get(hash, request.key);
    if (value == NULL)
    {
        send_response_header(OK, NO_DATA, 0);
        return 0;
    }
    return 0;
}

int do_job(hash_table *hash, mcache_request request, int client_fd)
{
    switch (request.header.command)
    {
    case INSERT:
        execute_insert(hash, request, client_fd);
        break;
    case GET:

        break;
    default:
        break;
    }
}

int read_data_send_response(hash_table *hash, int client_fd)
{
    debug_print("read_data_send_response", 1);
    mcache_request request = read_request(client_fd);
    if (request.code != 0)
    {
        debug_print("read_data_send_response", 0);
        return 0;
    }

    int result = do_job(hash, request, client_fd);

    debug_print("read_data_send_response", 0);

    return 0;
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
        tmp = read_data_send_response(hash, client_fd);
        if (tmp < 0)
        {
            debug_print("Error", 2);
        }
    }

    debug_print("main loop", 0);

    return 0;
}
