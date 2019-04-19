#include "protocol.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

mcache_request_header read_request_header(int client_fd)
{
    mcache_request_header header;
    uint8_t buffer[9] = {0};
    if (read(client_fd, buffer, 9) != 9)
    {
        close(client_fd);
        header.command = UNKNOWN;
        return header;
    }
    header.command = buffer[0];
    header.key_len = *(uint32_t *)&buffer[1];
    header.key_len = *(uint32_t *)&buffer[5];
    return header;
}

uint8_t *read_from_client(int clien_fd, uint32_t len)
{
    uint8_t *tmp = calloc(sizeof(uint8_t), len);
    if (read(clien_fd, tmp, len) != len)
    {
        free(tmp);
        return NULL;
    }
    return tmp;
}

int send_response_header(int client_fd, mcache_response_header header)
{
    if (write(client_fd, &header.info, sizeof(uint8_t)) != sizeof(uint8_t))
    {
        close(client_fd);
        return 1;
    }
    if (write(client_fd, &header.response_type, sizeof(uint8_t)) != sizeof(uint8_t))
    {
        close(client_fd);
        return 1;
    }
    if (write(client_fd, &header.items_count, sizeof(uint32_t)) != sizeof(uint32_t))
    {
        close(client_fd);
        return 1;
    }

    return 0;
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

    request.key = read_from_client(client_fd, request.header.key_len);

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

int send_response(int client_fd, uint8_t *data, uint32_t len)
{
    if (write(client_fd, data, len) != len)
        return 1;
    return 0;
}
