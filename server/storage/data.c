#include "data.h"

#include <stdlib.h>

simple_string *simple_string_new(uint8_t *str, uint32_t len)
{
    simple_string *s = calloc(sizeof(simple_string), 1);
    s->content = str;
    s->len = len;
    return s;
}

void free_simple_string(simple_string *s)
{
    free(s->content);
    free(s);
}
