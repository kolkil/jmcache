#include "../storage/hash_table.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void hash_table_test()
{
    hash_table *t = get_hash_table();
    uint8_t key_buffer[1024] = {0},
            dta_buffer[1024] = {0};
    while (scanf("%s %s", key_buffer, dta_buffer) == 2)
    {
        hash_table_insert(t, key_buffer, dta_buffer);
    }
    printf("Test print\n");
    hash_table_print(t);
    printf("Test keys\n");
    uint8_t **keys = hash_table_get_keys(t),
            *tmp;
    for (uint32_t i = 0; i < t->count; ++i)
    {
        tmp = hash_table_pop(t, keys[i]);
        printf("%s\t%s\n", keys[i], tmp);
        free(tmp);
        tmp = hash_table_pop(t, keys[i]);
        printf("%s\t%s\n", keys[i], tmp);
        free(tmp);
        free(keys[i]);
    }
    free(keys);
    free_hash_table(t);
}

void compute_hash()
{
    char buffer[1024] = {0};
    while (scanf("%s", buffer) == 1)
        printf("%d\n", (int)get_hash((uint8_t *)buffer, strlen(buffer)));
}
int menu()
{
    printf("[1]\thash_table\n");
    printf("[2]\tcompute_hash\n");
    int x = 0,
        z = scanf("%d", &x);
    if (z != 1)
        return 0;
    return x;
}

int main(void)
{
    switch (menu())
    {
    case 1:
        hash_table_test();
        break;
    case 2:
        compute_hash();
        break;
    default:
        break;
    }
}