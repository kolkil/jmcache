#include "data.h"

#include <stdlib.h>

simple_string *simple_string_new(uint8_t *str, uint32_t len)
{
    simple_string *s = calloc(1, sizeof(simple_string));
    s->content = str;
    s->len = len;
    
    return s;
}

void free_simple_string(simple_string *s)
{
    if (s == NULL)
        return;
    if (s->content == NULL)
        return;

    free(s->content);
    free(s);
}
