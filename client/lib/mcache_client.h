#include <stdint.h>

enum
{
    INSERT = 1,
    GET = 2,
    POP = 3,
    KEYS = 4,
    ALL = 5
} COMMANDS;

enum
{
    OK = 0,
    ERROR = 1,
    UNKNOWN_ERROR = 2
} RESULT_INFO;

enum
{
    NO_DATA = 0,
    VALUE = 1,
    KEYS = 2,
    ALL = 3
} RESPONSE_TYPE;

/*
    mcache_request_header
    _____________
    |_|____|____|

    first byte - command
    next four bytes - key length
    next four bytes - data length
*/

typedef struct
{
    uint8_t command;
    uint32_t key_len,
        data_len;
} mcache_request_header;

/*
    mcache_response_header
    __________
    |_|_|____|
    
    first byte - execution info
    second byte - type of response
    next four bytes - number of items
*/

typedef struct
{
    uint8_t info,
        response_type;
    uint32_t items_count;
} mcache_response_header;

typedef struct
{
    uint8_t *data;
    uint32_t length;
} data_and_length;

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
