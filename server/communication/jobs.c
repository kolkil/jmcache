#include "jobs.h"
#include "../../shared/debug_print.h"

#include <stdlib.h>

int execute_insert(hash_table *hash, mpocket_request request, int client_fd)
{
    simple_string key,
        data;
    key.content = request.key;
    key.len = request.header.key_len;
    data.content = request.data;
    data.len = request.header.data_len;

    mpocket_response_header header;
    header.info = OK;
    header.items_count = 0;
    header.response_type = NO_DATA;

    mtx_lock(&hash->general_lock);
    int insert_result = hash_table_insert(hash, key, data);
    mtx_unlock(&hash->general_lock);

    if (insert_result)
    {
        header.info = ERROR;
        send_response_header(client_fd, header);
        return 0;
    }

    send_response_header(client_fd, header);

    return 1;
}

int execute_get(hash_table *hash, mpocket_request request, int client_fd)
{
    simple_string key;
    key.content = request.key;
    key.len = request.header.key_len;

    mpocket_response_header header;
    header.info = OK;
    header.response_type = VALUE;
    header.items_count = 1;

    mtx_lock(&hash->general_lock);
    simple_string value = hash_table_get(hash, key);

    if (value.content == NULL)
    {
        header.response_type = NO_DATA;
        header.items_count = 0;
        send_response_header(client_fd, header);
        mtx_unlock(&hash->general_lock);

        return 0;
    }

    send_response_header(client_fd, header);
    int result = send_data_with_length(client_fd, value.content, value.len);
    mtx_unlock(&hash->general_lock);
    free(request.key);

    return result;
}

int execute_pop(hash_table *hash, mpocket_request request, int client_fd)
{
    simple_string key;
    key.content = request.key;
    key.len = request.header.key_len;

    mpocket_response_header header;
    header.info = OK;
    header.response_type = VALUE;
    header.items_count = 1;

    mtx_lock(&hash->general_lock);
    simple_string value = hash_table_get(hash, key);

    if (value.content == NULL)
    {
        header.response_type = NO_DATA;
        header.items_count = 0;
        send_response_header(client_fd, header);
        mtx_unlock(&hash->general_lock);

        return 0;
    }

    send_response_header(client_fd, header);
    int result = send_data_with_length(client_fd, value.content, value.len);
    hash_table_delete(hash, key);
    mtx_unlock(&hash->general_lock);
    free(request.key);

    return result;
}

int execute_keys(hash_table *hash, int client_fd)
{
    mpocket_response_header header;
    header.info = OK;
    header.response_type = RKEYS;
    header.items_count = hash->count;

    mtx_lock(&hash->general_lock);
    simple_string *keys = hash_table_get_keys(hash);
    send_response_header(client_fd, header);

    int result = 1;
    for (uint32_t i = 0; i < hash->count; ++i)
    {
        if (keys[i].content == NULL)
        {
            debug_print_raw_string_int("execute keys error, null-key value given at i = ", i);
            continue;
        }

        if ((result = send_data_with_length(client_fd, keys[i].content, keys[i].len)) == 0)
        {
            debug_print("execute_keys error sending key", 0);
            break;
        }
    }

    free(keys);
    mtx_unlock(&hash->general_lock);

    return result;
}

int execute_all(hash_table *hash, int client_fd)
{
    mpocket_response_header header;
    header.info = OK;
    header.response_type = RALL;
    header.items_count = hash->count;

    mtx_lock(&hash->general_lock);
    simple_string **all_data = hash_table_get_all_data(hash);
    send_response_header(client_fd, header);

    int result = 1;
    for (uint32_t i = 0; i < hash->count; ++i)
    {
        if (all_data[i][0].content == NULL)
        {
            debug_print_raw_string_int("execute keys error, null-key value given at i = ", i);
            continue;
        }

        if ((result = send_data_with_length(client_fd, all_data[i][0].content, all_data[i][0].len)) == 0) //send key
        {
            debug_print("execute_keys error sending key", 0);
            break;
        }

        if ((result = send_data_with_length(client_fd, all_data[i][1].content, all_data[i][1].len)) == 0) //send value
        {
            debug_print("execute_keys error sending key", 0);
            break;
        }
    }

    for (uint32_t c = 0; c < hash->count; ++c)
        free(all_data[c]);

    free(all_data);
    mtx_unlock(&hash->general_lock);

    return 1;
}

int execute_stats(hash_table *hash, int client_fd)
{
    mpocket_response_header header;
    header.info = OK;
    header.response_type = VALUE;
    header.items_count = 2;

    mtx_lock(&hash->general_lock);
    uint32_t filled = hash->filled,
             count = hash->count;
    mtx_unlock(&hash->general_lock);

    send_response_header(client_fd, header);
    send_length(client_fd, filled);
    send_length(client_fd, count);

    return 1;
}

int do_job(hash_table *hash, mpocket_request request, int client_fd, connection_statistics *stats)
{
    long start_time = 0;
    int result = 0;

    switch (request.header.command)
    {
    case INSERT:
        start_time = microtime_now();
        result = execute_insert(hash, request, client_fd);
        ++stats->insert.count;
        stats->insert.time += microtime_now() - start_time;
        break;

    case GET:
        start_time = microtime_now();
        result = execute_get(hash, request, client_fd);
        ++stats->get.count;
        stats->get.time += microtime_now() - start_time;
        break;

    case POP:
        start_time = microtime_now();
        result = execute_pop(hash, request, client_fd);
        ++stats->pop.count;
        stats->pop.time += microtime_now() - start_time;
        break;

    case KEYS:
        start_time = microtime_now();
        result = execute_keys(hash, client_fd);
        ++stats->keys.count;
        stats->keys.time += microtime_now() - start_time;
        break;

    case ALL:
        start_time = microtime_now();
        result = execute_all(hash, client_fd);
        ++stats->all.count;
        stats->all.time += microtime_now() - start_time;
        break;

    case STATS:
        result = execute_stats(hash, client_fd);
        break;

    default:
        break;
    }
    return result;
}

int read_data_send_response(hash_table *hash, int client_fd, connection_statistics *stats)
{
    mpocket_request request = read_request(client_fd);
    if (request.code != 0)
        return 0;

    if (!do_job(hash, request, client_fd, stats))
    {
        if (request.key != NULL)
            free(request.key);

        if (request.data != NULL)
            free(request.data);
    }

    return 1;
}
