#include "protocol.h"
//#include "../../server/utils/debug_print.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>

mpocket_request read_request(int client_fd)
{
    mpocket_request request = { .key = NULL, .data = NULL };
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
    data_buffer buffer = { .position = 0 };

    recv(client_fd, &buffer.data, 9, MSG_NOSIGNAL);
    
    mpocket_request_header header;
    header.command = read_uint8(&buffer);
    header.key_len = read_uint32(&buffer);
    header.data_len = read_uint32(&buffer);

    debug_print_content_as_hex("Incoming request header", &header, sizeof(header));

    return header;
}

int32_t send_request_header(int server_fd, mpocket_request_header header)
{
    data_buffer buffer = { .position = 0 };

    write_uint8(&buffer, header.command);
    write_uint32(&buffer, header.key_len);
    write_uint32(&buffer, header.data_len);

    debug_print_content_as_hex("Outgoing request header", &header, sizeof(header));

    return send(server_fd, &buffer.data, 9, MSG_NOSIGNAL) != 9 ? -1 : 0;
}

mpocket_response_header read_response_header(int server_fd)
{
    data_buffer buffer = { .position = 0 };

    mpocket_response_header header = { .info = UNKNOWN_ERROR };

    if (recv(server_fd, &buffer.data, 6, MSG_NOSIGNAL) != 6)
        return header;
    
    header.info = read_uint8(&buffer);
    header.response_type = read_uint8(&buffer);
    header.items_count = read_uint32(&buffer);

    debug_print_content_as_hex("Incoming response header", &header, sizeof(header));

    return header;
}

void send_response_header(int client_fd, mpocket_response_header header)
{
    data_buffer buffer = { .position = 0 };

    write_uint8(&buffer, header.info);
    write_uint8(&buffer, header.response_type);
    write_uint32(&buffer, header.items_count);

    debug_print_content_as_hex("Outgoing response header", &header, sizeof(header));

    send(client_fd, &buffer.data, 6, 0);
}

uint8_t read_uint8(data_buffer *buffer)
{
    return buffer->data[buffer->position++];
}

uint32_t read_uint32(data_buffer *buffer)
{
    uint32_t ret = 0;
    for (uint32_t i = 0; i < sizeof(uint32_t); ++i)
    {
        ret <<= 8;
        ret += buffer->data[buffer->position++];
    }
    return ret;
}

void write_uint8(data_buffer *buffer, uint8_t value)
{
    buffer->data[buffer->position++] = value;
}

void write_uint32(data_buffer *buffer, uint32_t value)
{
    for (uint32_t i = 0; i < sizeof(uint32_t); ++i)
        buffer->data[buffer->position++] = (value >> ((3-i) * 8)) & 0xFF;
}

uint8_t *read_data(int clien_fd, uint32_t len)
{
    uint8_t *tmp = malloc(len * sizeof(uint8_t));

    if (recv(clien_fd, tmp, len, MSG_NOSIGNAL) != len)
    {
        free(tmp);
        return NULL;
    }
    
    debug_print_content_as_hex("Incoming data", tmp, len);

    return tmp;
}

int send_data(int fd, uint8_t *data, uint32_t len)
{
    uint32_t sent = 0;
    while (sent != len)
    {
        int32_t result = send(fd, data + sent, len - (uint32_t)sent, 0);
        if (result == -1)
            return 0;
        sent += result;
    }

    debug_print_content_as_hex("Outgoing data", data, len);

    return 1;
}