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
    data_and_length **keys;
} keys_result;

typedef struct
{
    query_result result;
    data_and_length ***all_data;
} all_result;

connection_params mcache_connect(char *, int);
int mcache_dissconnect(connection_params *);

query_result mcache_insert(connection_params *, data_and_length, data_and_length);
query_result mcache_insert_strings(connection_params *, char *, char *);

get_result mcache_get(connection_params *, data_and_length);
get_result mcache_get_strings(connection_params *, char *);

get_result mcache_pop(connection_params *, data_and_length);
get_result mcache_pop_strings(connection_params *, char *);

keys_result mcache_keys(connection_params *);

all_result mcache_all(connection_params *);
