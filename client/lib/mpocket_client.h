#ifndef _mpocket_client_h
#define _mpocket_client_h

#include "../../shared/protocol.h"

typedef struct
{
    char *address;
    int port,
        server_fd;
} connection_params;

typedef struct
{
    int code;
    char *error_message;
} query_result;

typedef struct
{
    query_result result;
    length_and_data data;
} get_result;

typedef struct
{
    query_result result;
    length_and_data *keys;
    int count;
} keys_result;

typedef struct
{
    query_result result;
    length_and_data **all_data;
    int count;
} all_result;

typedef struct
{
    query_result result;
    int filled,
        items_count;
} stats_result;

typedef struct
{
    query_result result;
    length_and_data access_key;
    int ok;
} access_result;

connection_params mpocket_connect(char *, int);
int mpocket_dissconnect(connection_params *);

query_result mpocket_insert(connection_params *, length_and_data, length_and_data);
query_result mpocket_insert_strings(connection_params *, char *, char *);

get_result mpocket_get(connection_params *, length_and_data);
get_result mpocket_get_strings(connection_params *, char *);

get_result mpocket_pop(connection_params *, length_and_data);
get_result mpocket_pop_strings(connection_params *, char *);

keys_result mpocket_keys(connection_params *);

all_result mpocket_all(connection_params *);

stats_result mpocket_stats(connection_params *);

/*  send access key to server to estabilish connection */
access_result mpocket_access(connection_params *params, length_and_data access_key);

#endif
