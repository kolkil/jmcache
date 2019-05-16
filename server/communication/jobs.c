#include "jobs.h"
#include "../utils/debug_print.h"

#include <stdlib.h>

int execute_insert(hash_table *hash, mcache_request request, int client_fd)
{
    simple_string key,
        data;
    key.content = request.key;
    key.len = request.header.key_len;
    data.content = request.data;
    data.len = request.header.data_len;

    mcache_response_header header;
    header.info = OK;
    header.items_count = 0;
    header.response_type = NO_DATA;

    mtx_lock(&hash->general_lock);
    debug_print("hash_table_insert", 1);
    int insert_result = hash_table_insert(hash, key, data);
    debug_print("hash_table_insert", 0);
    mtx_unlock(&hash->general_lock);

    if (insert_result)
    {
        header.info = ERROR;
        send_response_header(client_fd, header);
        return 1;
    }

    send_response_header(client_fd, header);

    return 0;
}

int execute_get(hash_table *hash, mcache_request request, int client_fd)
{
    simple_string key;
    key.content = request.key;
    key.len = request.header.key_len;

    mcache_response_header header;
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

        return 1;
    }

    send_response_header(client_fd, header);
    send_data(client_fd, (uint8_t *)&value.len, sizeof(value.len));
    int ret = send_data(client_fd, value.content, value.len);
    mtx_unlock(&hash->general_lock);

    return ret;
}

int execute_pop(hash_table *hash, mcache_request request, int client_fd)
{
    simple_string key;
    key.content = request.key;
    key.len = request.header.key_len;

    mcache_response_header header;
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

        return 1;
    }

    send_response_header(client_fd, header);
    send_data(client_fd, (uint8_t *)&value.len, sizeof(value.len));
    int ret = send_data(client_fd, value.content, value.len);
    hash_table_delete(hash, key);
    mtx_unlock(&hash->general_lock);

    return ret;
}

int execute_keys(hash_table *hash, int client_fd)
{
    mcache_response_header header;
    header.info = OK;
    header.response_type = RKEYS;
    header.items_count = hash->count;

    mtx_lock(&hash->general_lock);
    simple_string *keys = hash_table_get_keys(hash);
    send_response_header(client_fd, header);

    for (uint32_t i = 0; i < hash->count; ++i)
    {
        if (keys[i].content == NULL)
        {
            debug_print_raw_string_int("execute keys error, null-key value given at i = ", i);
            continue;
        }

        if (!send_data(client_fd, (uint8_t *)&(keys[i].len), sizeof(uint32_t)))
        {
            free(keys);
            debug_print("execute_keys error sending key size", 0);
            mtx_unlock(&hash->general_lock);

            return 0;
        }

        if (keys[i].content != NULL && !send_data(client_fd, keys[i].content, keys[i].len))
        {
            free(keys);
            debug_print("execute_keys error sending key", 0);
            mtx_unlock(&hash->general_lock);

            return 0;
        }
    }

    free(keys);
    mtx_unlock(&hash->general_lock);

    return 1;
}

int execute_all(hash_table *hash, int client_fd)
{
    mcache_response_header header;
    header.info = OK;
    header.response_type = RALL;
    header.items_count = hash->count;

    mtx_lock(&hash->general_lock);
    simple_string **all_data = hash_table_get_all_data(hash);
    send_response_header(client_fd, header);

    for (uint32_t i = 0; i < hash->count; ++i)
    {
        if (all_data[i][0].content == NULL)
        {
            debug_print_raw_string_int("execute keys error, null-key value given at i = ", i);
            continue;
        }

        if (!send_data(client_fd, (uint8_t *)&(all_data[i][0].len), sizeof(uint32_t))) //send key len
        {
            for (uint32_t c = 0; c < hash->count; ++c)
                free(all_data[c]);

            free(all_data);
            debug_print("execute_keys error sending key size", 0);
            mtx_unlock(&hash->general_lock);

            return 0;
        }

        if (!send_data(client_fd, all_data[i][0].content, all_data[i][0].len)) //send key
        {
            for (uint32_t c = 0; c < hash->count; ++c)
                free(all_data[c]);

            free(all_data);
            debug_print("execute_keys error sending key", 0);
            mtx_unlock(&hash->general_lock);

            return 0;
        }

        if (!send_data(client_fd, (uint8_t *)&(all_data[i][1].len), sizeof(uint32_t))) //send key len
        {
            for (uint32_t c = 0; c < hash->count; ++c)
                free(all_data[c]);

            free(all_data);
            debug_print("execute_keys error sending key size", 0);
            mtx_unlock(&hash->general_lock);

            return 0;
        }

        if (!send_data(client_fd, all_data[i][1].content, all_data[i][1].len)) //send value
        {
            for (uint32_t c = 0; c < hash->count; ++c)
                free(all_data[c]);

            free(all_data);
            debug_print("execute_keys error sending key", 0);
            mtx_unlock(&hash->general_lock);

            return 0;
        }
    }

    for (uint32_t c = 0; c < hash->count; ++c)
        free(all_data[c]);

    free(all_data);
    mtx_unlock(&hash->general_lock);

    return 1;
}

int do_job(hash_table *hash, mcache_request request, int client_fd)
{
    switch (request.header.command)
    {
    case INSERT:
        execute_insert(hash, request, client_fd);
        break;
    case GET:
        execute_get(hash, request, client_fd);
        break;
    case POP:
        execute_pop(hash, request, client_fd);
        break;
    case KEYS:
        execute_keys(hash, client_fd);
        break;
    case ALL:
        execute_all(hash, client_fd);
        break;
    default:
        break;
    }
    return 0;
}

int read_data_send_response(hash_table *hash, int client_fd)
{
    mcache_request request = read_request(client_fd);
    if (request.code != 0)
    {
        return 0;
    }

    int result = do_job(hash, request, client_fd);

    return !result;
}
