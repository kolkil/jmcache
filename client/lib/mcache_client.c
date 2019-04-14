#include "mcache_client.h"
#include "vector.h"

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

char *alloc_string(char *str)
{
    int len = strlen(str);
    char *tmp = calloc(sizeof(char), len + 1);
    memcpy(tmp, str, len + 1);
    return tmp;
}

connection_params mcache_connect(char *address, int port)
{
    connection_params params;
    params.address = calloc(sizeof(char), strlen(address) + 1);
    params.port = port;

    memcpy(params.address, address, strlen(address));

    int server_socket = 0;
    struct sockaddr_in server_address;

    memset(&server_address, '0', sizeof(server_address));

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);

    if (server_socket = socket(AF_INET, SOCK_STREAM, 0) <= 0)
    {
        params.server_fd = -1;
        return params;
    }

    if (inet_pton(AF_INET, address, &server_address.sin_addr) <= 0)
    {
        params.server_fd = -2;
        return params;
    }

    if (connect(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        params.server_fd = -3;
        return params;
    }
    params.server_fd = server_socket;
    return params;
}

void close_and_reset(connection_params *params)
{
    close(params->server_fd);
    params->server_fd = -1;
}

int send_request_header(connection_params *params, mcache_request_header header)
{
    if (write(params->server_fd, header.command, sizeof(uint8_t) != sizeof(uint8_t)))
    {
        close_and_reset(params);
        return -1;
    }

    if (write(params->server_fd, header.key_len, sizeof(uint32_t)) != sizeof(uint32_t))
    {
        close_and_reset(params);
        return -1;
    }

    if (write(params->server_fd, header.data_len, sizeof(uint32_t)) != sizeof(uint32_t))
    {
        close_and_reset(params);
        return -1;
    }
    return 0;
}

int send_data(connection_params *params, data_and_length data)
{
    if (write(params->server_fd, data.data, data.length) != data.length)
    {
        close_and_reset(params);
        return -1;
    }
    return 0;
}

mcache_response_header read_response_header(connection_params *params)
{
    mcache_response_header response;
    response.info = OK;
    response.response_type = NO_DATA;
    response.items_count = 0;

    uint8_t header[6] = {0};
    if (read(params->server_fd, header, 6) != 6)
    {
        close_and_reset(params);
        response.info = UNKNOWN_ERROR;
        return response;
    }

    response.info = header[0];
    response.response_type = header[1];
    response.items_count = *(uint32_t *)&header[2];

    return response;
}

get_result read_get_result(connection_params *params, mcache_response_header header)
{
    get_result result;
    result.result.code = 0;
    result.result.error_message = NULL;

    uint32_t data_len = 0;
    if (read(params->server_fd, &data_len, sizeof(uint32_t)) != sizeof(uint32_t))
    {
        close_and_reset(params);
        result.result.code = 1;
        result.result.error_message = alloc_string("No data len");
        return result;
    }

    uint8_t *readed_data = calloc(sizeof(uint8_t), data_len);
    if (read(params->server_fd, readed_data, data_len) != data_len)
    {
        close_and_reset(params);
        result.result.code = 2;
        result.result.error_message = alloc_string("Invalid data");
        return result;
    }

    result.data.data = readed_data;
    result.data.length = data_len;
    return result;
}

query_result mcache_insert(connection_params *params, data_and_length key, data_and_length value)
{
    query_result result;
    result.code = 0;
    result.error_message = NULL;

    if (key.length == 0)
    {
        result.code = 1;
        result.error_message = alloc_string("Key length cannot be 0");
        return result;
    }

    mcache_request_header header;
    header.command = INSERT;
    header.key_len = key.length;
    header.data_len = value.length;

    if (send_request_header(params, header) != 0)
    {
        result.code = 2;
        result.error_message = alloc_string("Error during sending header");
        return result;
    }

    if (send_data(params, key) != key.length)
    {
        result.code = 3;
        result.error_message = alloc_string("Error during sending key");
        return result;
    }

    if (send_data(params, value) != value.length)
    {
        result.code = 4;
        result.error_message = alloc_string("Error during sending value");
        return result;
    }

    mcache_response_header response = read_response_header(params);

    if (response.info != OK)
    {
        result.code = 5;
        result.error_message = alloc_string("Server returned error");
        return result;
    }

    return result;
}

get_result mcache_get(connection_params *params, data_and_length key)
{
    if (key.length == 0)
    {
        get_result result;
        result.result.code = 1;
        result.result.error_message = alloc_string("Key length cannot be 0");
        return result;
    }

    mcache_request_header header;
    header.command = GET;
    header.key_len = key.length;
    header.data_len = 0;

    if (send_request_header(params, header) != 0)
    {
        get_result result;
        result.result.code = 2;
        result.result.error_message = alloc_string("Error during sending header");
        return result;
    }

    if (send_data(params, key) != key.length)
    {
        get_result result;
        result.result.code = 4;
        result.result.error_message = alloc_string("Error during sending value");
        return result;
    }

    mcache_response_header response_header = read_response_header(params);

    if (response_header.info != OK)
    {
        get_result result;
        result.result.code = 5;
        result.result.error_message = alloc_string("Server returned error");
        return result;
    }

    get_result result = read_get_result(params, response_header);
    return result;
}
