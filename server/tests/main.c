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
        simple_string key, value;
        key.content = calloc(sizeof(uint8_t), strlen((char *)key_buffer));
        memcpy(key.content, key_buffer, strlen((char *)key_buffer));
        key.len = strlen((char *)key_buffer);
        value.content = calloc(sizeof(uint8_t), strlen((char *)dta_buffer));
        memcpy(key.content, dta_buffer, strlen((char *)dta_buffer));
        value.len = strlen((char *)dta_buffer);
        hash_table_insert(t, &key, &value);
    }
    printf("Test print\n");
    hash_table_print(t);
    printf("Test keys\n");
    simple_string *keys = hash_table_get_keys(t),
                  *tmp;
    for (uint32_t i = 0; i < t->count; ++i)
    {
        tmp = hash_table_pop(t, &keys[i]);
        printf("%s\t%s\n", keys[i], tmp);
        free(tmp);
        tmp = hash_table_pop(t, &keys[i]);
        printf("%s\t%s\n", keys[i].content, tmp);
        free(tmp);
        free(&keys[i]);
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

void print_golden_ratio()
{
    unsigned long long int fib_num = 50,
                           a = 1,
                           b = 1,
                           c = a + b;
    if (scanf("%llu", &fib_num) != 1)
        fib_num = 50;

    for (int i = 0; i < (int)fib_num; ++i)
    {
        c = a + b;
        a = b;
        b = c;
    }
    printf("a = %llu, b = %llu, c = %llu\n", a, b, c);
    printf("%lf\n", (double)(c) / (double)(a));
}

void print_string(char *str)
{
    printf("%s, ", str);
    for (int i = 0; i < strlen(str); ++i)
    {
        printf("%d", (int)str[i]);
        if (i < strlen(str) - 1)
            printf(" ");
    }
    printf("\n");
}

void strings()
{
    char *str1 = "k\tk\"",
         *str2 = "k\\t\\",
         str3[4] = "\"";
    if (str3[0] == '\"')
    {
        str3[0] = '\\';
        str3[1] = '\"';
    }

    print_string(str1);
    print_string(str2);
    print_string(str3);
}

int menu()
{
    printf("[1]\thash_table\n");
    printf("[2]\tcompute_hash\n");
    printf("[3]\tprint_golden_ratio\n");
    printf("[4]\tstrings\n");
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
    case 3:
        print_golden_ratio();
        break;
    case 4:
        strings();
        break;
    default:
        break;
    }
}