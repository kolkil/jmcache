#include "hash_table.h"
#include "../utils/debug_print.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void free_if_not_null(void *ptr)
{
    if (ptr == NULL)
        return;

    free(ptr);
    ptr = NULL;
}

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
    hash_table *table = calloc(1, sizeof(hash_table));

    for (int i = 0; i < PRIME_LENGTH; ++i)
        table->elements[i] = NULL;

    table->count = 0;
    table->filled = 0;

    return table;
}

linked_container *get_linked_container()
{
    linked_container *c = calloc(1, sizeof(linked_container));

    if (c == NULL)
        return NULL;

    c->next = NULL;
    c->prev = NULL;
    c->key = NULL;
    c->value = NULL;
    mtx_init(&c->lock, mtx_plain);

    return c;
}

linked_container *get_and_set_linked_container(simple_string *key, simple_string *data)
{
    linked_container *c = get_linked_container();

    if (c == NULL)
        return NULL;

    c->key = key;
    c->value = data;

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

    mtx_lock(&table->elements[hash]->lock);
    linked_container *c = table->elements[hash];

    for (int i = 0;; ++i)
    {
        if (i > 0)
            mtx_lock(&c->lock);

        if (!sstrings_compare(c->key, key))
        {
            free_simple_string(c->key);
            free_simple_string(c->value);
            c->key = key;
            c->value = data;
            mtx_unlock(&c->lock);

            return 0;
        }

        mtx_unlock(&c->lock);

        if (c->next == NULL)
        {
            mtx_lock(&c->lock);
            break;
        }

        c = c->next;
    }

    c->next = get_and_set_linked_container(key, data);

    if (c->next == NULL)
        return 1;

    c->next->prev = c;
    ++table->count;
    mtx_unlock(&c->lock);

    return 0;
}

ht_data hash_table_get(hash_table *table, simple_string *key)
{
    ht_data response;
    response.string = NULL;
    response.lock = NULL;

    uint16_t hash = get_hash(key->content, key->len);

    if (table->elements[hash] == NULL)
        return response;

    mtx_lock(&table->elements[hash]->lock);
    linked_container *c = table->elements[hash];

    for (int i = 0;; ++i)
    {
        if (i > 0)
            mtx_lock(&c->lock);

        if (!sstrings_compare(c->key, key))
        {
            response.string = c->value;
            response.lock = &c->lock;

            return response;
        }

        mtx_unlock(&c->lock);

        if (c->next == NULL)
            break;

        c = c->next;
    }

    return response;
}

int hash_table_delete(hash_table *table, simple_string *key)
{
    uint16_t hash = get_hash(key->content, key->len);

    if (table->elements[hash] == NULL)
        return 0;

    mtx_lock(&table->elements[hash]->lock);
    linked_container *c = table->elements[hash];

    for (int i = 0;; ++i)
    {
        if (i > 0)
            mtx_lock(&c->lock);

        if (!sstrings_compare(c->key, key))
        {
            if (c->prev == NULL && c->next != NULL) //first but not the only
            {
                linked_container *tmp = c->next;
                free_linked_container(c);
                table->elements[hash] = tmp;
                --table->count;

                return 0;
            }
            else if (c->prev == NULL && c->next == NULL) //first and the only
            {
                free_linked_container(c);
                --table->count;
                --table->filled;
                table->elements[hash] = NULL;

                return 0;
            }
            else if (c->prev != NULL && c->next != NULL) //middle
            {
                linked_container *tmp_prev = c->prev,
                                 *tmp_next = c->next;
                free_linked_container(c);
                tmp_prev->next = tmp_next;
                tmp_next->prev = tmp_prev;
                --table->count;

                return 0;
            }
            //last
            c->prev->next = NULL;
            free_linked_container(c);
            --table->count;
            --table->filled;

            return 0;
        }

        mtx_unlock(&c->lock);

        if (c->next == NULL)
            break;

        c = c->next;
    }

    return 1;
}

ht_data *hash_table_get_keys(hash_table *t)
{
    ht_data *keys = calloc(t->count, sizeof(ht_data));
    uint32_t keys_num = 0;

    for (uint32_t i = 0, k = 0; i < PRIME_LENGTH; ++i)
    {
        if (t->elements[i] == NULL)
            continue;

        if (t->elements[i]->key == NULL)
        {
            t->elements[i] = NULL;
            continue;
        }

        mtx_lock(&t->elements[i]->lock);
        linked_container *c = t->elements[i];

        for (int u = 0; c->next != NULL; ++k, ++u)
        {
            if (u > 0)
                mtx_lock(&c->lock);

            keys[k].string = c->key;
            ++keys_num;
            keys[k].lock = &c->lock;
            c = c->next;
        }

        keys[k].string = c->key;
        ++keys_num;
        keys[k].lock = &c->lock;
        ++k;
    }

    if (keys_num != t->count) //error
    {
        debug_print_raw("hash_table_get_keys error, found less keys than t->count");
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
                printf("%s\t%s\n", (char *)c->key->content, (char *)c->value->content);
                c = c->next;
            }
        }
    }

    return;
}

void free_linked_container(linked_container *c)
{
    free_simple_string(c->key);
    free_simple_string(c->value);
    mtx_unlock(&c->lock);
    mtx_destroy(&c->lock);
    free_if_not_null(c);
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

    free_if_not_null(table);

    return;
}
