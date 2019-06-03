#include <stdint.h>
#include <stddef.h>

enum
{
    UNKNOWN = 0,
    INSERT = 1,
    GET = 2,
    POP = 3,
    KEYS = 4,
    ALL = 5,
    STATS = 6
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
    RKEYS = 2,
    RALL = 3
} RESPONSE_TYPE;

typedef struct
{
    uint8_t command;
    uint32_t key_len,
        data_len;
} mpocket_request_header;

typedef struct
{
    uint8_t info,
        response_type;
    uint32_t items_count;
} mpocket_response_header;

typedef struct
{
    uint32_t length;
    uint8_t *data;
} data_and_length;

typedef struct
{
    uint32_t code;
    mpocket_request_header header;
    uint8_t *key,
        *data;
} mpocket_request;

typedef struct
{
    uint32_t position;
    uint8_t data[12];
} data_buffer;

mpocket_request read_request(int);

mpocket_request_header read_request_header(int);
int32_t send_request_header(int, mpocket_request_header);

mpocket_response_header read_response_header(int);
void send_response_header(int, mpocket_response_header);

uint8_t read_uint8(data_buffer *);
uint32_t read_uint32(data_buffer *);

void write_uint8(data_buffer *, uint8_t);
void write_uint32(data_buffer *, uint32_t);

uint8_t *read_data(int, uint32_t);
int send_data(int, uint8_t *, uint32_t);

void debug_print_content_as_hex(char *, void *, size_t);