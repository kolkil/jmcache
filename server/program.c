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
    simple_string *key = simple_string_new(request.key, request.header.key_len),
                  *data = simple_string_new(request.data, request.header.data_len);

    mcache_response_header header;
    header.info = OK;
    header.items_count = 0;
    header.response_type = NO_DATA;
    debug_print("hash_table_insert", 1);
    int insert_result = hash_table_insert(hash, key, data);
    debug_print("hash_table_insert", 0);
    if (insert_result)
    {
        header.info = ERROR;
        send_response_header(client_fd, header);
        return 1;
    }
    send_response_header(client_fd, header);
    return 0;
}

int execute_get(hash_table *hash, mcache_request request, int client_fd)
{
    simple_string key;
    key.content = request.key;
    key.len = request.header.key_len;

    mcache_response_header header;
    header.info = OK;
    header.response_type = VALUE;
    header.items_count = 1;

    simple_string *value = hash_table_get(hash, &key);
    debug_print((char *)value->content, 2);
    send_response_header(client_fd, header);
    send_data(client_fd, (uint8_t *)&value->len, sizeof(value->len));
    return send_data(client_fd, value->content, value->len);
}

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
