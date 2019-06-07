#include "protocol.h"
#include "debug_print.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>

mpocket_request_header mpocket_request_header_hton(mpocket_request_header header)
{
    header.key_len = htonl(header.key_len);
    header.data_len = htonl(header.data_len);

    return header;
}

mpocket_response_header mpocket_response_header_hton(mpocket_response_header header)
{
    header.items_count = htonl(header.items_count);
    header.response_type = htonl(header.response_type);

    return header;
}

mpocket_request_header mpocket_request_header_ntoh(mpocket_request_header header)
{
    header.key_len = ntohl(header.key_len);
    header.data_len = ntohl(header.data_len);

    return header;
}

mpocket_response_header mpocket_response_header_ntoh(mpocket_response_header header)
{
    header.response_type = ntohl(header.response_type);
    header.items_count = ntohl(header.items_count);

    return header;
}

mpocket_request read_request(int client_fd)
{
    mpocket_request request = {.key = NULL, .data = NULL};
    request.header = read_request_header(client_fd);
    request.code = 0;

    if (request.header.command == UNKNOWN)
    {
        request.code = 1;
        return request;
    }

    if (request.header.key_len == 0)
        return request;

    request.key = read_data(client_fd, request.header.key_len);

    if (request.key == NULL)
    {
        mpocket_response_header header;
        header.info = ERROR;
        header.items_count = 0;
        header.response_type = NO_DATA;
        send_response_header(client_fd, header);
        request.code = 1;
        return request;
    }

    if (request.header.data_len == 0)
        return request;

    request.data = read_data(client_fd, request.header.data_len);

    if (request.data == NULL)
    {
        mpocket_response_header header;
        header.info = ERROR;
        header.items_count = 0;
        header.response_type = NO_DATA;
        send_response_header(client_fd, header);
        request.code = 1;
        return request;
    }

    return request;
}

mpocket_request_header read_request_header(int client_fd)
{
    mpocket_request_header header;
    recv(client_fd, &header, sizeof(header), MSG_NOSIGNAL);

    return mpocket_request_header_ntoh(header);
}

int32_t send_request_header(int server_fd, mpocket_request_header header)
{
    header = mpocket_request_header_hton(header);

    return send(server_fd, &header, sizeof(header), MSG_NOSIGNAL) != sizeof(header) ? -1 : 0;
}

mpocket_response_header read_response_header(int server_fd)
{
    mpocket_response_header header = {.info = UNKNOWN_ERROR};

    if (recv(server_fd, &header, sizeof(header), MSG_NOSIGNAL) != 6)
        return header;

    return mpocket_response_header_ntoh(header);
}

void send_response_header(int client_fd, mpocket_response_header header)
{
    header = mpocket_response_header_hton(header);
    send(client_fd, &header, sizeof(header), MSG_NOSIGNAL);
}

uint8_t *read_data(int fd, uint32_t len)
{
    uint8_t *tmp = malloc(len * sizeof(uint8_t));

    for (uint32_t read = 0; read != len;)
    {
        int32_t result = recv(fd, tmp, len - read, MSG_NOSIGNAL);
        if (result == -1)
        {
            free(tmp);
            return NULL;
        }
        read += result;
    }

    return tmp;
}

int send_data(int fd, uint8_t *data, uint32_t len)
{
    for (uint32_t sent = 0; sent != len;)
    {
        int32_t result = send(fd, data + sent, len - (uint32_t)sent, MSG_NOSIGNAL);
        if (result == -1)
            return 0;
        sent += result;
    }

    return 1;
}

int send_length_and_data(int fd, length_and_data value)
{
    return send_length(fd, value.length) && send_data(fd, value.data, value.length);
}

length_and_data read_length_and_data(int fd)
{
    length_and_data result =
        {
            .length = read_length(fd),
            .data = read_data(fd, result.length)};

    return result;
}

uint32_t read_length(int fd)
{
    uint32_t len = 0;
    if (recv(fd, &len, sizeof(len), MSG_NOSIGNAL) != sizeof(len))
        return 0;

    return ntohl(len);
}

int send_length(int fd, uint32_t len)
{
    len = htonl(len);

    return send(fd, &len, sizeof(len), MSG_NOSIGNAL) == sizeof(len);
}