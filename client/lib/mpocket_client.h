#include "protocol.h"

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
    data_and_length data;
} get_result;

typedef struct
{
    query_result result;
    data_and_length *keys;
    int count;
} keys_result;

typedef struct
{
    query_result result;
    data_and_length **all_data;
    int count;
} all_result;

typedef struct
{
    query_result result;
    int filled,
        items_count;
} stats_result;

connection_params mpocket_connect(char *, int);
int mpocket_dissconnect(connection_params *);

query_result mpocket_insert(connection_params *, data_and_length, data_and_length);
query_result mpocket_insert_strings(connection_params *, char *, char *);

get_result mpocket_get(connection_params *, data_and_length);
get_result mpocket_get_strings(connection_params *, char *);

get_result mpocket_pop(connection_params *, data_and_length);
get_result mpocket_pop_strings(connection_params *, char *);

keys_result mpocket_keys(connection_params *);

all_result mpocket_all(connection_params *);

stats_result mpocket_stats(connection_params *);
