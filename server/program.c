#include "program.h"
#include "communication/socket.h"
#include "storage/vector.h"

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

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

int read_data_send_response(int client_fd)
{
    uint8_t buffer[BUFFER_SIZE] = {0};
    vector *v = get_vector();
    int res = 0,
        comm = -1;

    while (1)
    {
        res = read(client_fd, buffer, BUFFER_SIZE);
        if (res < 0)
            return -1;
        comm = strlen(buffer);
        for (int i = 0; i < comm; ++i)
            vector_push_back(v, buffer[i]);
        if (comm <= 1023)
            break;
        if (res == 0)
            break;
    }

    return 0;
}

int start_program(config_values *cnf)
{
    int client_fd = -1,
        tmp = 0;
    socket_params *params = prepare_socket(cnf->server_address, cnf->server_port);
    if (params->fd <= 0)
        return params->fd;

    while (1)
    {
        client_fd = socket_listen_and_accept(params);
        if (client_fd <= 0)
            return client_fd;
        tmp = read_data_send_response(client_fd);
        if (tmp < 0)
            return tmp;
    }

    return 0;
}
