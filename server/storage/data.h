#include <stdint.h>

typedef struct
{
    uint32_t len;
    uint8_t *content;
} simple_string;

typedef struct
{
    simple_string *key,
        *value;
} hash_table_content;

hash_table_content *get_default_content();
void free_hash_table_content(hash_table_content *);
void free_simple_string(simple_string *);
