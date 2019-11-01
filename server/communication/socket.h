#ifndef _socket_h_
#define _socket_h_

#include <netinet/in.h>

typedef struct
{
    int fd;
    struct sockaddr_in address;
} socket_params;

socket_params *prepare_socket(char*, int);
void free_socket_params(socket_params *);
int socket_listen_and_accept(socket_params *);

#endif _socket_h_
