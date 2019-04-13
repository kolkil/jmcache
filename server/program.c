#include "program.h"
#include "communication/socket.h"
#include "storage/vector.h"
#include "storage/hash_table.h"
#include "utils/debug_print.h"
#include "utils/parser.h"

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int write_err_close_fd_return_0(int client_fd)
{
    if (write(client_fd, "Err\n", 4))
        close(client_fd);
    else
        close(client_fd);
    return 0;
}

int execute_insert(parsed_data *parsed, hash_table *hash, int client_fd)
{
    debug_print(parsed->first_param.content, 2);
    debug_print(parsed->second_param.content, 2);

    if (strlen(parsed->first_param.content) <= 0)
        return write_err_close_fd_return_0(client_fd);

    int insert_result = hash_table_insert(hash, (uint8_t *)parsed->first_param.content, (uint8_t *)parsed->second_param.content);

    if (insert_result)
        return write_err_close_fd_return_0(client_fd);

    if (write(client_fd, "OK\n", 3) != 3)
        debug_print("error during responding", 2);

    close(client_fd);

    return 0;
}

int execute_get(parsed_data *parsed, hash_table *hash, int client_fd)
{
    if (strlen(parsed->first_param.content) <= 0)
        return write_err_close_fd_return_0(client_fd);

    char *value = (char *)hash_table_get(hash, (uint8_t *)parsed->first_param.content);

    debug_print(value, 2);

    if (value == NULL)
        return write_err_close_fd_return_0(client_fd);

    int value_len = strlen(value);

    if (write(client_fd, value, value_len) != value_len)
        debug_print("error during responding", 2);

    close(client_fd);

    return 0;
}

int execute_pop(parsed_data *data, hash_table *hash, int client_fd)
{
    if (strlen(data->first_param.content) <= 0)
        return write_err_close_fd_return_0(client_fd);

    char *value = (char *)hash_table_pop(hash, (uint8_t *)data->first_param.content);

    debug_print(value, 2);

    if (value == NULL)
        return write_err_close_fd_return_0(client_fd);

    int value_len = strlen(value);

    if (write(client_fd, value, value_len) != value_len)
        debug_print("error during responging", 2);

    close(client_fd);

    return 0;
}

int execute_keys(parsed_data *data, hash_table *hash, int client_fd)
{
    if (strlen(data->first_param.content) <= 0)
        return write_err_close_fd_return_0(client_fd);

    char **value = (char **)hash_table_get_keys(hash);
    for (uint32_t i = 0, len = 0; i < hash->count; ++i)
    {
        len = strlen(*(value + i));
        if (write(client_fd, *(value + i), len) != len || write(client_fd, "\n", 1) != 1)
        {
            debug_print("error during responding", 2);
            break;
        }
    }
    close(client_fd);
    return 0;
}

int react_on_input(parsed_data *parsed, hash_table *hash, int client_fd)
{
    switch (parsed->command)
    {
    case INSERT:
        debug_print("INSERT", 1);
        execute_insert(parsed, hash, client_fd);
        debug_print("INSERT", 0);
        break;
    case GET:
        debug_print("GET", 1);
        execute_get(parsed, hash, client_fd);
        debug_print("GET", 0);
        break;
    case POP:
        debug_print("POP", 1);
        execute_pop(parsed, hash, client_fd);
        debug_print("POP", 0);
        break;
    case KEYS:
        debug_print("KEYS", 1);
        execute_keys(parsed, hash, client_fd);
        debug_print("KEYS", 0);
        break;
    case ALL:
        break;
    default:
        break;
    }
    return 0;
}

int read_data_send_response(hash_table *hash, int client_fd)
{
    debug_print("read_data_send_response", 1);
    char buffer[BUFFER_SIZE] = {0};
    vector *v = get_vector();
    int res = 0,
        data_len = -1;

    while (1)
    {
        res = read(client_fd, buffer, BUFFER_SIZE - 1);
        if (res < 0)
        {
            debug_print("read_data_send_response", 0);
            return -1;
        }
        data_len = strlen(buffer);
        for (int i = 0; i < data_len; ++i)
            vector_push_back(v, buffer[i]);
        if (data_len <= 1022)
            break;
        if (res == 0)
            break;
        memset(buffer, 0, BUFFER_SIZE);
    }
    vector_push_back(v, 0);
    debug_print(v->data, 2);

    parsed_data parsed = parse_data(v->data);

    if (parsed.error)
    {
        debug_print("error", 2);
        debug_print(parsed.error_message, 2);
        debug_print("read_data_send_response", 0);
        write_err_close_fd_return_0(client_fd);
        close(client_fd);
        free_parsed_data_content(&parsed);
        return -1;
    }

    react_on_input(&parsed, hash, client_fd);
    close(client_fd);
    free_parsed_data_content(&parsed);
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
