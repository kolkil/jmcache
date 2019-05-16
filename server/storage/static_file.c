#include "static_file.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int save_to_file(int fd, hash_table *t)
{
    if (fd <= 0)
        return 0;

    simple_string **all_data = hash_table_get_all_data(t);

    for (uint32_t i = 0, k; i < t->count; ++i)
    {
        k = write(fd, &all_data[i][0].len, sizeof(uint32_t));
        k = write(fd, all_data[i][0].content, all_data[i][0].len);
        k = write(fd, &all_data[i][1].len, sizeof(uint32_t));
        k = write(fd, all_data[i][1].content, all_data[i][1].len);
        k = k;
    }

    return 1;
}

int load_from_file(int fd, hash_table *t)
{
    if (fd <= 0)
        return 0;

    for (int32_t i = 0;; ++i)
    {
        simple_string key;

        if (read(fd, &key.len, sizeof(uint32_t)) != sizeof(uint32_t))
            return 0;

        key.content = malloc(key.len * sizeof(uint8_t));

        if (read(fd, key.content, key.len) != key.len)
            return 0;

        simple_string value;

        if (read(fd, &value.len, sizeof(uint32_t)) != sizeof(uint32_t))
            return 0;

        value.content = malloc(value.len * sizeof(uint8_t));

        if (read(fd, value.content, value.len) != value.len)
            return 0;

        hash_table_insert(t, key, value);
    }
    return 0;
}
