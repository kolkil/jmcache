#include "program.h"
#include "communication/socket.h"
#include "storage/vector.h"
#include "storage/hash_table.h"
#include "utils/debug_print.h"

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

char *commands[5] = {
    "insert",
    "get",
    "pop",
    "keys",
    "all"};

int parse_command(uint8_t *buffer)
{
    debug_print("parse_command", 1);
    if (strlen((char *)buffer) < 6)
    {
        debug_print("parse_command", 0);
        return -1;
    }
    char tmp[7] = {0};

    for (int i = 0; i < 6; ++i)
    {
        if ((char)buffer[i] == ' ')
            break;
        tmp[i] = buffer[i];
    }

    for (int i = 0; i < 5; ++i)
    {
        if (!strcmp(tmp, commands[i]))
        {
            debug_print("parse_command", 0);
            return i + 1;
        }
    }
    debug_print("parse_command not", 0);
    return -1;
}

char *get_string(int *offset, char *data)
{
    debug_print("get_string", 1);
    int len = strlen(data);
    char *readed = calloc(sizeof(char), len + 1);
    uint32_t i = *offset;
    for (uint32_t flag = 0; i < (uint32_t)len; ++i)
    {
        if (data[i] == '"')
        {
            if (!flag)
            {
                flag = 1;
                continue;
            }
            else if (data[i - 1] != '\\')
            {
                flag = 0;
                break;
            }
        }
        if (flag)
        {
            readed[i - (*offset) - 1] = data[i];
        }
        else
        {
            (*offset)++;
        }
    }
    debug_print(readed, 2);
    debug_print("get_string", 0);
    *offset = i + 1;
    return readed;
}

int execute_command(int command, hash_table *hash, uint8_t *data, int client_fd)
{
    debug_print("execute_command", 1);
    int offset = strlen(commands[command - 1]),
        tmp = 0;
    char *key,
        *value;
    switch (command)
    {
    case INSERT:
        debug_print("INSERT", 1);
        key = get_string(&offset, (char *)data);
        value = get_string(&offset, (char *)data);
        hash_table_insert(hash, (uint8_t *)key, (uint8_t *)value);
        debug_print((char *)key, 2);
        debug_print((char *)value, 2);
        debug_print("hash_table get", 1);
        debug_print((char *)hash_table_get(hash, (uint8_t *)key), 2);
        debug_print("hash_table get", 0);
        free(key);
        free(value);
        if (write(client_fd, "OK\n", 3) != 3)
        {
            debug_print("Error during responding", 2);
            debug_print("INSERT not", 0);
            return -1;
        }
        debug_print("INSERT", 0);
        break;

    case GET:
        debug_print("GET", 1);
        key = get_string(&offset, (char *)data);
        debug_print((char *)key, 2);
        debug_print("hash_table get", 1);
        value = (char *)hash_table_get(hash, (uint8_t *)key);
        debug_print("hash_table get", 0);

        if (value == NULL)
        {
            if (write(client_fd, "Err\n", 4) != 4)
            {
                debug_print("Error during responding", 2);
                debug_print("GET not", 0);
                return -1;
            }
        }
        else
        {
            tmp = strlen(value);
            if (write(client_fd, value, tmp + 1) != tmp + 1)
            {
                debug_print("Error during responding", 2);
                debug_print("GET not", 0);
                return -1;
            }
        }
        debug_print("GET", 0);
        break;

    default:
        break;
    }
    debug_print("execute_command", 0);
    return 0;
}

int read_data_send_response(hash_table *hash, int client_fd)
{
    debug_print("read_data_send_response", 1);
    uint8_t buffer[BUFFER_SIZE] = {0};
    vector *v = get_vector();
    int res = 0,
        data = -1,
        command = -1;

    while (1)
    {
        res = read(client_fd, buffer, BUFFER_SIZE);
        if (res < 0)
        {
            debug_print("read_data_send_response", 0);
            return -1;
        }
        data = strlen((char *)buffer);
        for (int i = 0; i < data; ++i)
            vector_push_back(v, buffer[i]);
        if (data <= 1023)
            break;
        if (res == 0)
            break;
    }
    debug_print(v->data, 2);
    command = parse_command((uint8_t *)v->data);
    if (command < 1)
    {
        debug_print("read_data_send_response", 0);
        return -1;
    }
    execute_command(command, hash, (uint8_t *)v->data, client_fd);
    close(client_fd);
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

    for(unsigned long long int i = 0; ; ++i)
    {
        debug_print_raw("REQUEST");
        debug_print_raw_int(i);
        client_fd = socket_listen_and_accept(params);
        if (client_fd <= 0)
            return client_fd;
        tmp = read_data_send_response(hash, client_fd);
        if (tmp < 0)
        {
            debug_print("Error", 2);
        }
    }

    debug_print("main loop", 0);

    return 0;
}
