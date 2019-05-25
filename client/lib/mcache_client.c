#include "mcache_client.h"

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/tcp.h>

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

    int server_socket = 0,
        opt = 1;
    struct sockaddr_in server_address;

    memset(&server_address, '0', sizeof(server_address));

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);

    if ((server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) <= 0)
    {
        params.server_fd = -1;
        return params;
    }

    if (setsockopt(server_socket, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(int)))
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
    if (send(params->server_fd, &header, sizeof(header), MSG_NOSIGNAL) != sizeof(header))
    {
        close_and_reset(params);
        return -1;
    }

    return 0;
}

int send_data(connection_params *params, data_and_length data)
{
    if (send(params->server_fd, data.data, data.length, MSG_NOSIGNAL) != data.length)
    {
        close_and_reset(params);
        return -1;
    }
    return 0;
}

mcache_response_header read_response_header(connection_params *params)
{
    mcache_response_header response;

    if (recv(params->server_fd, &response, sizeof(response), MSG_NOSIGNAL) != sizeof(response))
    {
        close_and_reset(params);
        response.info = UNKNOWN_ERROR;
        return response;
    }
    return response;
}

data_and_length read_data_and_length(connection_params *params)
{
    data_and_length data;
    data.data = NULL;
    data.length = 0;

    uint32_t data_len = 0;
    if (recv(params->server_fd, &data_len, sizeof(uint32_t), MSG_NOSIGNAL) != sizeof(uint32_t))
    {
        return data;
    }

    data.length = data_len;

    data.data = calloc(data_len, sizeof(uint8_t));
    if (recv(params->server_fd, data.data, data_len, MSG_NOSIGNAL) != data_len)
    {
        data.length = 0;
        free(data.data);
        return data;
    }
    return data;
}

get_result read_get_result(connection_params *params)
{
    get_result result;
    result.result.code = 0;
    result.result.error_message = NULL;

    uint32_t data_len = 0;
    if (recv(params->server_fd, &data_len, sizeof(uint32_t), MSG_NOSIGNAL) != sizeof(uint32_t))
    {
        close_and_reset(params);
        result.result.code = 1;
        result.result.error_message = alloc_string("No data len");
        return result;
    }

    uint8_t *read_data = calloc(sizeof(uint8_t), data_len);
    if (recv(params->server_fd, read_data, data_len, MSG_NOSIGNAL) != data_len)
    {
        close_and_reset(params);
        result.result.code = 2;
        result.result.error_message = alloc_string("Invalid data");
        return result;
    }

    result.data.data = read_data;
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

    if (send_data(params, key) == -1)
    {
        result.code = 3;
        result.error_message = alloc_string("Error during sending key");
        return result;
    }

    if (send_data(params, value) == -1)
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

    if (send_data(params, key) == -1)
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

    if (response_header.response_type == NO_DATA)
    {
        get_result result;
        result.data.data = NULL;
        result.data.length = 0;
        result.result.code = 0;
        return result;
    }

    get_result result = read_get_result(params);
    return result;
}

query_result mcache_insert_strings(connection_params *params, char *key, char *value)
{
    data_and_length d_key = {(uint8_t *)key, (uint32_t)strlen(key)};
    data_and_length d_value = {(uint8_t *)value, (uint32_t)strlen(value)};

    query_result result = mcache_insert(params, d_key, d_value);
    return result;
}

get_result mcache_get_strings(connection_params *params, char *key)
{
    data_and_length d_key = {(uint8_t *)key, (uint32_t)strlen(key)};
    return mcache_get(params, d_key);
}

get_result mcache_pop(connection_params *params, data_and_length key)
{
    if (key.length == 0)
    {
        get_result result;
        result.result.code = 1;
        result.result.error_message = alloc_string("Key length cannot be 0");
        return result;
    }

    mcache_request_header header;
    header.command = POP;
    header.key_len = key.length;
    header.data_len = 0;

    if (send_request_header(params, header) != 0)
    {
        get_result result;
        result.result.code = 2;
        result.result.error_message = alloc_string("Error during sending header");
        return result;
    }

    if (send_data(params, key) == -1)
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

    if (response_header.response_type == NO_DATA)
    {
        get_result result;
        result.data.data = NULL;
        result.data.length = 0;
        result.result.code = 0;
        return result;
    }

    get_result result = read_get_result(params);
    return result;
}

get_result mcache_pop_strings(connection_params *params, char *key)
{
    data_and_length d_key = {(uint8_t *)key, (uint32_t)strlen(key)};
    return mcache_pop(params, d_key);
}

keys_result mcache_keys(connection_params *params)
{
    mcache_request_header header;
    header.command = KEYS;
    header.key_len = 0;
    header.data_len = 0;

    if (send_request_header(params, header) != 0)
    {
        keys_result result;
        result.result.code = 2;
        result.result.error_message = alloc_string("Error during sending header");
        return result;
    }

    mcache_response_header response_header = read_response_header(params);

    if (response_header.info != OK)
    {
        keys_result result;
        result.result.code = 5;
        result.result.error_message = alloc_string("Server returned error");
        return result;
    }

    if (response_header.response_type == NO_DATA)
    {
        keys_result result;
        result.result.code = 0;
        return result;
    }

    keys_result result;
    result.keys = calloc(response_header.items_count, sizeof(data_and_length));
    result.count = response_header.items_count;

    for (uint32_t i = 0; i < response_header.items_count; ++i)
    {
        result.keys[i] = read_data_and_length(params);
    }
    return result;
}

all_result mcache_all(connection_params *params)
{
    mcache_request_header header;
    header.command = ALL;
    header.key_len = 0;
    header.data_len = 0;

    all_result result;

    if (send_request_header(params, header) != 0)
    {
        result.result.code = 2;
        result.result.error_message = alloc_string("Error during sending header");
        return result;
    }

    mcache_response_header response_header = read_response_header(params);

    if (response_header.info != OK)
    {
        result.result.code = 5;
        result.result.error_message = alloc_string("Server returned error");
        return result;
    }

    if (response_header.response_type == NO_DATA)
    {
        result.result.code = 0;
        return result;
    }

    result.all_data = calloc(response_header.items_count, sizeof(data_and_length *));

    for (uint32_t i = 0; i < response_header.items_count; ++i)
        result.all_data[i] = calloc(2, sizeof(data_and_length));

    result.count = response_header.items_count;

    for (uint32_t i = 0; i < response_header.items_count; ++i)
    {
        result.all_data[i][0] = read_data_and_length(params);
        result.all_data[i][1] = read_data_and_length(params);
    }
    return result;
}

stats_result mcache_stats(connection_params *params)
{
    mcache_request_header header;
    header.command = STATS;
    header.data_len = 0;
    header.key_len = 0;

    stats_result result;

    if (send_request_header(params, header) != 0)
    {
        result.result.code = 2;
        result.result.error_message = alloc_string("Error during sending header");
        return result;
    }

    mcache_response_header response_header = read_response_header(params);

    if (response_header.info != OK)
    {
        result.result.code = 5;
        result.result.error_message = alloc_string("Server returned error");
        return result;
    }

    if (response_header.response_type == NO_DATA)
    {
        result.result.code = 0;
        return result;
    }

    uint32_t *tmp = (uint32_t *)read_data_and_length(params).data;
    result.filled = (uint32_t)*tmp;
    free(tmp);
    tmp = (uint32_t *)read_data_and_length(params).data;
    result.items_count = (uint32_t)*tmp;
    free(tmp);

    return result;
}
