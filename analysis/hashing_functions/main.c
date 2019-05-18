#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define PRIME_LENGTH 1021

uint16_t hash_0(char *key) //default hash
{
    int len = strlen(key);
    uint32_t hash = 0;

    for (int i = 0; i < len; ++i)
        hash += key[i] * 10ul + i + 1;

    return hash % PRIME_LENGTH;
}

uint16_t hash_1(char *key) //some random function
{
    int len = strlen(key);
    uint32_t hash = 0;

    for (int i = 0; i < len; ++i)
        hash += (key[i] << 16) | ((key[i] << 8) + i + key[i]);

    return hash % PRIME_LENGTH;
}

uint16_t hash_2(char *key) //https://en.wikipedia.org/wiki/Jenkins_hash_function
{
    int len = strlen(key);
    size_t i = 0;
    uint32_t hash = 0;
    while (i != len)
    {
        hash += key[i++];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;

    return hash % PRIME_LENGTH;
}

int main(void)
{
    char buffer[1024] = {0};
    int hash_0_arr[PRIME_LENGTH] = {0},
        hash_1_arr[PRIME_LENGTH] = {0},
        hash_2_arr[PRIME_LENGTH] = {0};

    while (scanf("%s", buffer) == 1)
    {
        ++hash_0_arr[hash_0(buffer)];
        ++hash_1_arr[hash_1(buffer)];
        ++hash_2_arr[hash_2(buffer)];
        memset(buffer, 0, 1024);
    }

    printf("default\trandom\tJenkins\n");

    for (int i = 0; i < PRIME_LENGTH; ++i)
        printf("%d\t%d\t%d\n", hash_0_arr[i], hash_1_arr[i], hash_2_arr[i]);

    return 0;
}