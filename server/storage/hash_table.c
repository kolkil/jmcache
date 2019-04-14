#include "hash_table.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int sstrings_compare(simple_string *a, simple_string *b)
{
    if (a->len != b->len)
        return 1;
    for (uint32_t i = 0; i < a->len; ++i)
        if (a->content[i] != b->content[i])
            return 1;
    return 0;
}

uint16_t get_hash(uint8_t *key, uint32_t len)
{
    uint16_t hash = 0;
    for (uint8_t i = 0; i < len; ++len)
        hash += key[i] * 10 + i + 1;
    return hash % PRIME_LENGTH;
}

hash_table *get_hash_table()
{
    hash_table *table = malloc(sizeof(hash_table));
    for (int i = 0; i < PRIME_LENGTH; ++i)
        table->elements[i] = NULL;
    table->count = 0;
    table->filled = 0;
    return table;
}

linked_container *get_linked_container()
{
    linked_container *c = malloc(sizeof(linked_container));
    if (c == NULL)
        return NULL;
    c->next = NULL;
    c->prev = NULL;
    c->data = get_default_content();
    return c;
}

uint8_t *get_new_copy(uint8_t *source)
{
    uint32_t len = strlen((char *)source);
    uint8_t *new = calloc(len + 1, sizeof(uint8_t));
    memcpy(new, source, len);
    return new;
}

void linked_container_set_data(linked_container *c, simple_string *data)
{
    c->data->value = data;
}

void set_linked_container_content(linked_container *c, simple_string *key, simple_string *data)
{
    c->data->key = key;
    c->data->value = data;
}

linked_container *get_and_set_linked_container(simple_string *key, simple_string *data)
{
    linked_container *c = get_linked_container();
    if (c == NULL)
        return NULL;
    set_linked_container_content(c, key, data);
    return c;
}

int hash_table_insert(hash_table *table, simple_string *key, simple_string *data)
{
    uint16_t hash = get_hash(key->content, key->len);

    if (table->elements[hash] == NULL)
    {
        table->elements[hash] = get_and_set_linked_container(key, data);
        if (table->elements[hash] == NULL)
            return 1;
        ++table->filled;
        ++table->count;
        return 0;
    }
    linked_container *c = table->elements[hash];
    for (;;)
    {
        if (!sstrings_compare(c->data->key, key))
        {
            free_hash_table_content(c->data);
            free_simple_string(key);
            linked_container_set_data(c, data);
            return 0;
        }
        if (c->next == NULL)
            break;
        c = c->next;
    }
    c->next = get_and_set_linked_container(key, data);
    if (c->next == NULL)
        return 1;
    c->next->prev = c;
    ++table->count;
    return 0;
}

simple_string *hash_table_get(hash_table *table, simple_string *key)
{
    uint16_t hash = get_hash(key->content, key->len);

    if (table->elements[hash] == NULL)
        return NULL;
    linked_container *c = table->elements[hash];
    for (;;)
    {
        if (!sstrings_compare(c->data->key, key))
        {
            return c->data->value;
        }
        if (c->next == NULL)
            break;
        c = c->next;
    }
    return NULL;
}

simple_string *hash_table_pop(hash_table *table, simple_string *key)
{
    uint16_t hash = get_hash(key->content, key->len);
    if (table->elements[hash] == NULL)
        return NULL;
    linked_container *c = table->elements[hash];
    for (;;)
    {
        if (!strcmp((char *)c->data->key, (char *)key))
        {
            simple_string *tmp = c->data->value;
            if (c->prev == NULL)
            {
                linked_container *tmp_lc = c->next;
                free_linked_container(c);
                table->elements[hash] = tmp_lc;
                c = table->elements[hash];
                if (c != NULL)
                    c->prev = NULL;
            }
            else if (c->next == NULL)
            {
                free_linked_container(c);
            }
            else
            {
                linked_container *tmp_lcn = c->next,
                                 *tmp_lcp = c->prev;
                free_linked_container(c);
                tmp_lcp->next = tmp_lcn;
                tmp_lcn->prev = tmp_lcp;
            }
            return tmp;
        }
        if (c->next == NULL)
            break;
        c = c->next;
    }
    return NULL;
}

simple_string **hash_table_get_keys(hash_table *t)
{
    simple_string **keys = calloc(t->count, sizeof(uint8_t *));
    for (uint32_t i = 0, k = 0; i < PRIME_LENGTH; ++i)
    {
        if (t->elements[i] == NULL)
            continue;
        linked_container *c = t->elements[i];
        for (; c->next != NULL; ++k)
        {
            keys[k] = c->data->key;
            c = c->next;
        }
        keys[k] = c->data->key;
        ++k;
    }
    return keys;
}

void hash_table_print(hash_table *table)
{
    for (uint32_t i = 0; i < PRIME_LENGTH; ++i)
    {
        if (table->elements[i] != NULL)
        {
            linked_container *c = table->elements[i];
            for (; c != NULL;)
            {
                printf("%s\t%s\n", (char *)c->data->key->content, (char *)c->data->value->content);
                c = c->next;
            }
        }
    }
    return;
}

void free_linked_container(linked_container *c)
{
    free_hash_table_content(c->data);
    free(c);
}

void free_hash_table(hash_table *table)
{
    for (uint32_t i = 0; i < PRIME_LENGTH; ++i)
    {
        if (table->elements[i] == NULL)
            continue;
        linked_container *c = table->elements[i];
        for (linked_container *tmp = NULL; c != NULL;)
        {
            tmp = c->next;
            free_linked_container(c);
            c = tmp;
        }
    }
    free(table);
    return;
}
