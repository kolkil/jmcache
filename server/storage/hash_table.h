#include "data.h"

#include <threads.h>

#define PRIME_LENGTH 1021

typedef struct linked_container linked_container;

struct linked_container
{
    linked_container *next,
        *prev;
    simple_string key,
        value;
};

typedef struct
{
    linked_container *elements[PRIME_LENGTH];
    uint32_t filled,
        count;
    mtx_t general_lock;
} hash_table;

hash_table *get_hash_table();
void free_hash_table(hash_table *);
linked_container *get_linked_container();
void free_linked_container();
uint16_t get_hash(uint8_t *, uint32_t);
int hash_table_insert(hash_table *, simple_string , simple_string );
simple_string hash_table_get(hash_table *, simple_string );
int hash_table_delete(hash_table *, simple_string );
simple_string *hash_table_get_keys(hash_table *);
simple_string **hash_table_get_all_data(hash_table*);
void hash_table_print(hash_table *);
int hash_table_load_from_file(hash_table *, int);
int hash_table_save_to_file(hash_table *, int);
