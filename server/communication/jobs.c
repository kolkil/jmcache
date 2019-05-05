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
    debug_print("hash_table_insert", 1);
    int insert_result = hash_table_insert(hash, key, data);
    debug_print("hash_table_insert", 0);
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

    simple_string *value = hash_table_get(hash, &key);
    debug_print((char *)value->content, 2);
    send_response_header(client_fd, header);
    send_data(client_fd, (uint8_t *)&value->len, sizeof(value->len));
    return send_data(client_fd, value->content, value->len);
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

    simple_string *value = hash_table_pop(hash, &key);
    debug_print((char *)value->content, 2);

    send_response_header(client_fd, header);
    send_data(client_fd, (uint8_t *)&value->len, sizeof(value->len));
    int r = send_data(client_fd, value->content, value->len);
    free_simple_string(value);

    return r;
}

int execute_keys(hash_table *hash, int client_fd)
{
    mcache_response_header header;
    header.info = OK;
    header.response_type = RKEYS;
    header.items_count = hash->count;

    simple_string *keys = hash_table_get_keys(hash);
    debug_print_int((int)hash->count);
    send_response_header(client_fd, header);

    for (uint32_t i = 0; i < hash->count; ++i)
        if (!send_data(client_fd, (keys + i)->content, (keys + i)->len))
            return 0;

    return 1;
}

int do_job(hash_table *hash, mcache_request request, int client_fd)
{
    switch (request.header.command)
    {
    case INSERT:
        debug_print("INSERT", 1);
        execute_insert(hash, request, client_fd);
        debug_print("INSERT", 0);
        break;
    case GET:
        debug_print("GET", 1);
        execute_get(hash, request, client_fd);
        debug_print("GET", 0);
        break;
    case POP:
        debug_print("POP", 1);
        execute_pop(hash, request, client_fd);
        debug_print("POP", 0);
        break;
    case KEYS:
        debug_print("KEYS", 1);
        execute_keys(hash, client_fd);
        debug_print("KEYS", 0);
        break;
    case ALL:
        debug_print("ALL", 1);
        execute_get(hash, request, client_fd);
        debug_print("ALL", 0);
        break;
    default:
        break;
    }
    return 0;
}

int read_data_send_response(hash_table *hash, int client_fd)
{
    debug_print("read_data_send_response", 1);
    mcache_request request = read_request(client_fd);
    if (request.code != 0)
    {
        debug_print("Error parsing request", 2);
        debug_print_int((int)request.header.command);
        debug_print("read_data_send_response", 0);
        return 0;
    }
    debug_print_int((int)request.header.command);
    int result = do_job(hash, request, client_fd);

    debug_print("read_data_send_response", 0);

    return !result;
}
