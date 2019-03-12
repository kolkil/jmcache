#include "hash_table.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

uint16_t get_hash(uint8_t *key, uint8_t len)
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
	c->next = NULL;
	c->data = get_default_content();
	return c;
}

void linked_container_set_data(linked_container *c, uint8_t *data)
{
	uint32_t d_len = strlen((char *)data);
	c->data->data = calloc(d_len + 2, sizeof(uint8_t));
	for (uint32_t i = 0; i < d_len; ++i)
		c->data->data[i] = data[i];
	c->data->len = d_len;
}

void linked_container_set_key(linked_container *c, uint8_t *key)
{
	uint32_t k_len = strlen((char *)key);
	c->data->key = calloc(k_len + 1, sizeof(uint8_t));
	for (uint32_t i = 0; i < k_len; ++i)
		c->data->key[i] = key[i];
}

void set_linked_container_content(linked_container *c, uint8_t *key, uint8_t *data)
{
	linked_container_set_key(c, key);
	linked_container_set_data(c, data);
}

linked_container *get_and_set_linked_container(uint8_t *key, uint8_t *data)
{
	linked_container *c = get_linked_container();
	set_linked_container_content(c, key, data);
	return c;
}

int hash_table_insert(hash_table *table, uint8_t *key, uint8_t *data)
{
	uint16_t hash = get_hash(key, strlen((char *)key));

	if (table->elements[hash] == NULL)
	{
		table->elements[hash] = get_and_set_linked_container(key, data);
		++table->filled;
		++table->count;
	}
	else
	{
		linked_container *c = table->elements[hash];
		for (; c->next != NULL;)
		{
			if (!strcmp((char *)c->data->key, (char *)key))
			{
				free(c->data->data);
				linked_container_set_data(c, data);
				return 0;
			}
			c = c->next;
		}
		if (!strcmp((char *)c->data->key, (char *)key))
		{
			free(c->data->data);
			linked_container_set_data(c, data);
			return 0;
		}
		c->next = get_and_set_linked_container(key, data);
		++table->count;
	}
	return 0;
}

uint8_t *hash_table_get(hash_table *table, uint8_t *key)
{
	uint16_t hash = get_hash(key, strlen((char *)key));

	if (table->elements[hash] == NULL)
	{
		return NULL;
	}
	else
	{
		linked_container *c = table->elements[hash];
		for (; c->next != NULL;)
		{
			if (!strcmp((char *)c->data->key, (char *)key))
			{
				return c->data->data;
			}
			c = c->next;
		}
		return c->data->data;
	}
	return NULL;
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
				printf("%s\t%s\n", c->data->key, c->data->data);
				c = c->next;
			}
		}
	}
	return;
}

void free_linked_container(linked_container *c)
{
	free(c->data->data);
	free(c->data->key);
	free(c->data);
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
