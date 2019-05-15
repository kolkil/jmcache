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
