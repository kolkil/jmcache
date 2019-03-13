#include "socket.h"

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>

socket_params *prepare_socket(char address, int port)
{
    socket_params *params = calloc(1, sizeof(socket_params));
    // int server_fd, new_socket, valread;
    // struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(params->address);
    char buffer[1024] = {0};

    if ((params->fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        printf("socket failed");
        return params;
    }

    if (setsockopt(params->fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        printf("setsockopt");
        return params;
    }
    params->address.sin_family = AF_INET;
    params->address.sin_addr.s_addr = inet_addr(address);
    params->address.sin_port = htons(port);

    if (bind(params->fd, (struct sockaddr *)&(params->address), sizeof(params->address)) < 0)
    {
        printf("bind failed");
        return params;
    }
    // if (listen(server_fd, 3) < 0)
    // {
    // printf("listen");
    // return -1;
    // }
    // if ((new_socket = accept(server_fd, (struct sockaddr *)&(params->address), (socklen_t *)&addrlen)) < 0)
    // {
    // printf("accept");
    // return -1;
    // }
    // valread = read(new_socket, buffer, 1024);
    // printf("%s\n", buffer);
    // send(new_socket, hello, strlen(hello), 0);
    // printf("Hello message sent\n");
    return params;
}

int socket_listen_and_accept(socket_params *params)
{
    int new_socket = -1;
    if (listen(params->fd, 3) < 0)
        return -1;
    if ((new_socket = accept(params->fd, (struct sockaddr *)&(params->address), sizeof(params->address))))
        return -1;
    return new_socket;
}
