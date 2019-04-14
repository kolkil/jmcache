#include "data.h"

#include <stdlib.h>

hash_table_content *get_default_content()
{
    hash_table_content *c = malloc(sizeof(hash_table_content));
    c->key->content = NULL;
    c->key->len = 0;
    c->value->content = NULL;
    c->value->len = 0;
    return c;
}

void free_hash_table_content(hash_table_content *a)
{
    free(a->key->content);
    free(a->value->content);
    free(a);
}

void free_simple_string(simple_string *s)
{
    free(s->content);
    free(s);
}
