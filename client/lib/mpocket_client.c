#include "mpocket_client.h"
#include "../../shared/debug_print.h"

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

connection_params mpocket_connect(char *address, int port)
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

get_result read_get_result(connection_params *params)
{
    get_result result;
    result.result.code = 0;
    result.result.error_message = NULL;
    if ((result.data.length = read_length(params->server_fd)) == 0)
    {
        close_and_reset(params);
        result.result.code = 1;
        result.result.error_message = alloc_string("No data len");
        return result;
    }
    if ((result.data.data = read_data(params->server_fd, result.data.length)) == NULL)
    {
        close_and_reset(params);
        result.result.code = 2;
        result.result.error_message = alloc_string("Invalid data");
        return result;
    }
    return result;
}

query_result mpocket_insert(connection_params *params, length_and_data key, length_and_data value)
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

    mpocket_request_header header;
    header.command = INSERT;
    header.key_len = key.length;
    header.data_len = value.length;

    if (send_request_header(params->server_fd, header) != 0)
    {
        close_and_reset(params);
        result.code = 2;
        result.error_message = alloc_string("Error during sending header");
        return result;
    }

    if (send_data(params->server_fd, key.data, key.length) == 0)
    {
        close_and_reset(params);
        result.code = 3;
        result.error_message = alloc_string("Error during sending key");
        return result;
    }

    if (send_data(params->server_fd, value.data, value.length) == 0)
    {
        close_and_reset(params);
        result.code = 4;
        result.error_message = alloc_string("Error during sending value");
        return result;
    }

    mpocket_response_header response = read_response_header(params->server_fd);

    if (response.info != OK)
    {
        close_and_reset(params);
        result.code = 5;
        result.error_message = alloc_string("Server returned error");
        return result;
    }

    return result;
}

get_result mpocket_get(connection_params *params, length_and_data key)
{
    if (key.length == 0)
    {
        get_result result;
        result.result.code = 1;
        result.result.error_message = alloc_string("Key length cannot be 0");
        return result;
    }

    mpocket_request_header header;
    header.command = GET;
    header.key_len = key.length;
    header.data_len = 0;

    if (send_request_header(params->server_fd, header) != 0)
    {
        close_and_reset(params);
        get_result result;
        result.result.code = 2;
        result.result.error_message = alloc_string("Error during sending header");
        return result;
    }

    if (send_data(params->server_fd, key.data, key.length) == 0)
    {
        close_and_reset(params);
        get_result result;
        result.result.code = 4;
        result.result.error_message = alloc_string("Error during sending value");
        return result;
    }

    mpocket_response_header response_header = read_response_header(params->server_fd);

    if (response_header.info != OK)
    {
        close_and_reset(params);
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

query_result mpocket_insert_strings(connection_params *params, char *key, char *value)
{
    length_and_data d_key = {(uint32_t)strlen(key), (uint8_t *)key};
    length_and_data d_value = {(uint32_t)strlen(value), (uint8_t *)value};

    query_result result = mpocket_insert(params, d_key, d_value);
    return result;
}

get_result mpocket_get_strings(connection_params *params, char *key)
{
    length_and_data d_key = {(uint32_t)strlen(key), (uint8_t *)key};
    return mpocket_get(params, d_key);
}

get_result mpocket_pop(connection_params *params, length_and_data key)
{
    if (key.length == 0)
    {
        get_result result;
        result.result.code = 1;
        result.result.error_message = alloc_string("Key length cannot be 0");
        return result;
    }

    mpocket_request_header header;
    header.command = POP;
    header.key_len = key.length;
    header.data_len = 0;

    if (send_request_header(params->server_fd, header) != 0)
    {
        close_and_reset(params);
        get_result result;
        result.result.code = 2;
        result.result.error_message = alloc_string("Error during sending header");
        return result;
    }

    if (send_data(params->server_fd, key.data, key.length) == 0)
    {
        close_and_reset(params);
        get_result result;
        result.result.code = 4;
        result.result.error_message = alloc_string("Error during sending value");
        return result;
    }

    mpocket_response_header response_header = read_response_header(params->server_fd);

    if (response_header.info != OK)
    {
        close_and_reset(params);
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

get_result mpocket_pop_strings(connection_params *params, char *key)
{
    length_and_data d_key = {(uint32_t)strlen(key), (uint8_t *)key};
    return mpocket_pop(params, d_key);
}

keys_result mpocket_keys(connection_params *params)
{
    mpocket_request_header header;
    header.command = KEYS;
    header.key_len = 0;
    header.data_len = 0;

    if (send_request_header(params->server_fd, header) != 0)
    {
        close_and_reset(params);
        keys_result result;
        result.result.code = 2;
        result.result.error_message = alloc_string("Error during sending header");
        return result;
    }

    mpocket_response_header response_header = read_response_header(params->server_fd);
    debug_print_raw_int(response_header.items_count);

    if (response_header.info != OK)
    {
        close_and_reset(params);
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

    keys_result result = {.count = response_header.items_count};
    result.keys = calloc(response_header.items_count, sizeof(length_and_data));

    for (uint32_t i = 0; i < response_header.items_count; ++i)
        result.keys[i] = read_length_and_data(params->server_fd);

    return result;
}

all_result mpocket_all(connection_params *params)
{
    mpocket_request_header header;
    header.command = ALL;
    header.key_len = 0;
    header.data_len = 0;

    all_result result;

    if (send_request_header(params->server_fd, header) != 0)
    {
        close_and_reset(params);
        result.result.code = 2;
        result.result.error_message = alloc_string("Error during sending header");
        return result;
    }

    mpocket_response_header response_header = read_response_header(params->server_fd);

    if (response_header.info != OK)
    {
        close_and_reset(params);
        result.result.code = 5;
        result.result.error_message = alloc_string("Server returned error");
        return result;
    }

    if (response_header.response_type == NO_DATA)
    {
        result.result.code = 0;
        return result;
    }

    result.all_data = calloc(response_header.items_count, sizeof(length_and_data *));

    for (uint32_t i = 0; i < response_header.items_count; ++i)
        result.all_data[i] = calloc(2, sizeof(length_and_data));

    result.count = response_header.items_count;

    for (uint32_t i = 0; i < response_header.items_count; ++i)
    {
        result.all_data[i][0] = read_length_and_data(params->server_fd);
        result.all_data[i][1] = read_length_and_data(params->server_fd);
    }
    return result;
}

stats_result mpocket_stats(connection_params *params)
{
    mpocket_request_header header;
    header.command = STATS;
    header.data_len = 0;
    header.key_len = 0;

    stats_result result;

    if (send_request_header(params->server_fd, header) != 0)
    {
        close_and_reset(params);
        result.result.code = 2;
        result.result.error_message = alloc_string("Error during sending header");
        return result;
    }

    mpocket_response_header response_header = read_response_header(params->server_fd);

    if (response_header.info != OK)
    {
        close_and_reset(params);
        result.result.code = 5;
        result.result.error_message = alloc_string("Server returned error");
        return result;
    }

    if (response_header.response_type == NO_DATA)
    {
        result.result.code = 0;
        return result;
    }

    result.filled = read_length(params->server_fd);
    result.items_count = read_length(params->server_fd);

    return result;
}

access_result mpocket_access(connection_params *params, length_and_data access_key)
{
    mpocket_request_header header;
    header.command = ACCESS;
    header.key_len = access_key.length;
    header.data_len = 0;

    if (send_request_header(params->server_fd, header) != 0)
    {
        close_and_reset(params);
        access_result result;
        result.result.code = 2;
        result.result.error_message = alloc_string("Error during sending header");
        return result;
    }

    if (send_data(params->server_fd, access_key.data, access_key.length) == 0)
    {
        close_and_reset(params);
        access_result result;
        result.result.code = 4;
        result.result.error_message = alloc_string("Error during sending value");
        return result;
    }

    mpocket_response_header response_header = read_response_header(params->server_fd);

    if (response_header.info != OK)
    {
        close_and_reset(params);
        access_result result;
        result.result.code = 5;
        result.result.error_message = alloc_string("Server returned error");
        return result;
    }

    access_result result;
    result.ok = response_header.info == OK;
    result.access_key = read_length_and_data(params->server_fd);

    return result;
}
