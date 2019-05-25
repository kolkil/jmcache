#include "protocol.h"
#include "../utils/debug_print.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>

int send_data(int client_fd, uint8_t *data, uint32_t len)
{
    for (int sent = 0;;)
    {
        sent = send(client_fd, data + sent, len - (uint32_t)sent, 0);
        if (sent == -1)
            return 0;
        if ((uint32_t)sent == len)
            return 1;
    }
    return 0;
}

mcache_request_header read_request_header(int client_fd)
{
    mcache_request_header header;
    
    if (recv(client_fd, &header, sizeof(header), MSG_NOSIGNAL) != sizeof(header))
        header.command = UNKNOWN;

    return header;
}

uint8_t *read_from_client(int clien_fd, uint32_t len)
{
    uint8_t *tmp = malloc(len * sizeof(uint8_t));

    if (recv(clien_fd, tmp, len, MSG_NOSIGNAL) != len)
    {
        free(tmp);
        return NULL;
    }
    
    return tmp;
}

int send_response_header(int client_fd, mcache_response_header header)
{
    return send(client_fd, &header, sizeof(header), 0) != sizeof(header);
}

mcache_request read_request(int client_fd)
{
    mcache_request request;
    request.header = read_request_header(client_fd);
    request.code = 0;

    if (request.header.command == UNKNOWN)
    {
        request.code = 1;
        return request;
    }

    if (request.header.key_len == 0)
        return request;

    uint8_t *received_data = read_from_client(client_fd, request.header.key_len);

    request.key = received_data;

    if (request.key == NULL)
    {
        mcache_response_header header;
        header.info = ERROR;
        header.items_count = 0;
        header.response_type = NO_DATA;
        send_response_header(client_fd, header);
        request.code = 1;
        return request;
    }

    if (request.header.data_len == 0)
        return request;

    request.data = read_from_client(client_fd, request.header.data_len);

    if (request.data == NULL)
    {
        mcache_response_header header;
        header.info = ERROR;
        header.items_count = 0;
        header.response_type = NO_DATA;
        send_response_header(client_fd, header);
        request.code = 1;
        return request;
    }
    return request;
}
