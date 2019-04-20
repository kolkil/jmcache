#include <stdint.h>

typedef struct
{
    uint32_t len;
    uint8_t *content;
} simple_string;

void free_simple_string(simple_string *);
simple_string *simple_string_new(uint8_t *, uint32_t);
