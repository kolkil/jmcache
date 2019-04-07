#include "program.h"
#include "communication/socket.h"
#include "storage/vector.h"
#include "storage/hash_table.h"

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int parse_command(uint8_t *buffer)
{
    if (strlen((char *)buffer) < 6)
        return -1;
    char tmp[7] = {0};
    memcpy(tmp, buffer, 6);

    for (int i = 0; i < 5; ++i)
    {
        if (!strcmp(tmp, commands[i]))
            return i + 1;
    }
    return -1;
}

uint8_t *get_string(int offset, uint8_t *data)
{
    int len = strlen(data + offset);

    uint8_t *readed = calloc(sizeof(uint8_t), len + 1);

    for (uint32_t i = offset, flag = 0; i < len; ++i)
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
            readed[i - offset] = data[i];
        }
    }
    return readed;
}

int execute_command(int command, hash_table *hash, uint8_t *data)
{
    int command_len = strlen(commands[command - 1]);
    switch (command)
    {
    case INSERT:
        uint8_t *key = get_string(command_len, data),
                *value = get_string(command_len + strlen(key), data);
#ifdef DEBUG
        printf("INSERT, KEY = %s, VALUE = %s\n", key, value);
#endif
        hash_table_insert(hash, key, value);
        free(key);
        free(value);
        break;

    default:
        break;
    }
}

int read_data_send_response(hash_table *hash, int client_fd)
{
    uint8_t buffer[BUFFER_SIZE] = {0};
    vector *v = get_vector();
    int res = 0,
        data = -1,
        command = -1;

    while (1)
    {
        res = read(client_fd, buffer, BUFFER_SIZE);
        if (res < 0)
            return -1;
        data = strlen(buffer);
        for (int i = 0; i < data; ++i)
            vector_push_back(v, buffer[i]);
        if (data <= 1023)
            break;
        if (res == 0)
            break;
    }
    command = parse_command(v->data);
    execute_command(command, hash, v->data);
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

    while (1)
    {
        client_fd = socket_listen_and_accept(params);
        if (client_fd <= 0)
            return client_fd;
        tmp = read_data_send_response(hash, client_fd);
        if (tmp < 0)
            return tmp;
    }

    return 0;
}
