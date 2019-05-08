#include "jobs.h"
#include "../utils/debug_print.h"

int execute_insert(hash_table *hash, mcache_request request, int client_fd)
{
    simple_string *key = simple_string_new(request.key, request.header.key_len),
                  *data = simple_string_new(request.data, request.header.data_len);

    mcache_response_header header;
    header.info = OK;
    header.items_count = 0;
    header.response_type = NO_DATA;
    int insert_result = hash_table_insert(hash, key, data);
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
    // debug_print("hash table get", 1);
    ht_data value = hash_table_get(hash, &key);
    // debug_print("hash table get", 0);
    if (value.string == NULL)
    {
        header.response_type = NO_DATA;
        header.items_count = 0;
        send_response_header(client_fd, header);
        return 1;
    }
    send_response_header(client_fd, header);
    send_data(client_fd, (uint8_t *)&value.string->len, sizeof(value.string->len));
    int ret = send_data(client_fd, value.string->content, value.string->len);
    mtx_unlock(value.lock);

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

    ht_data value = hash_table_get(hash, &key);
    send_response_header(client_fd, header);
    send_data(client_fd, (uint8_t *)&value.string->len, sizeof(value.string->len));
    int ret = send_data(client_fd, value.string->content, value.string->len);
    mtx_unlock(value.lock);
    hash_table_delete(hash, &key);

    return ret;
}

int execute_keys(hash_table *hash, int client_fd)
{
    mcache_response_header header;
    header.info = OK;
    header.response_type = RKEYS;
    header.items_count = hash->count;

    ht_data *keys = hash_table_get_keys(hash);
    send_response_header(client_fd, header);

    for (uint32_t i = 0; i < hash->count; ++i)
    {
        if (!send_data(client_fd, (keys + i)->string->content, (keys + i)->string->len))
        {
            mtx_unlock((keys + i)->lock);
            return 0;
        }
        mtx_unlock((keys + i)->lock);
    }

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
        execute_get(hash, request, client_fd);
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