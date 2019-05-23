#include "socket.h"
#include "../utils/debug_print.h"

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <fcntl.h>

socket_params *prepare_socket(char *address, int port)
{
    debug_print("prepare_socket", 1);
    socket_params *params = calloc(1, sizeof(socket_params));
    int opt = 1;

    if ((params->fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == 0)
    {
        debug_print_raw("socket failed");
        return params;
    }

    if (setsockopt(params->fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(int)))
    {
        debug_print_raw("setsockopt");
        free(params);
        return NULL;
    }

    params->address.sin_family = AF_INET;
    params->address.sin_addr.s_addr = inet_addr(address);
    params->address.sin_port = htons(port);

    if (bind(params->fd, (struct sockaddr *)&(params->address), sizeof(params->address)) < 0)
    {
        debug_print_raw("bind failed");
        free(params);
        return NULL;
    }

    debug_print("prepare_socket", 0);

    return params;
}

int socket_listen_and_accept(socket_params *params)
{
    int new_socket = -1;
    if (listen(params->fd, 8) < 0)
        return -1;
    if ((new_socket = accept(params->fd, (struct sockaddr *)NULL, NULL)) < 1)
    {
        debug_print_int(new_socket);
        return -1;
    }
    return new_socket;
}

void free_socket_params(socket_params *params)
{
    if (params == NULL)
        return;

    free(params);
}
