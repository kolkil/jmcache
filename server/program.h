#include "utils/config.h"

#define BUFFER_SIZE 1024

enum
{
    INSERT = 1,
    GET = 2,
    POP = 3,
    KEYS = 4,
    ALL = 5
};

typedef struct
{
    int command;
    char *key,
        *data;
} input_container;

int start_program(config_values *);
