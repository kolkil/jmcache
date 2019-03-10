#include "data.h"

#include <stdlib.h>

content get_default_conetnet_value()
{
    content c;
    c.data = NULL;
    c.key = NULL;
    c.len = 0;
    return c;
}

content *get_default_content()
{
    content *c = malloc(sizeof(content));
    c->data = NULL;
    c->key = NULL;
    c->len = 0; 
    return c;
}
