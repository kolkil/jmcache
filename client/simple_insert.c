#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/mcache_client.h"

#define BUFFER_SIZE 1024

void print_exit_message(int c)
{
    printf("inserted %d pairs\n", c);
}

int main(int argc, char **argv)
{
    if (argc != 3)
        return 0;

    connection_params params = mcache_connect(argv[1], atoi(argv[2]));

    if (params.server_fd < 1)
    {
        print_exit_message(0);
        return 0;
    }

    char key_buffer[BUFFER_SIZE] = {0},
         value_buffer[BUFFER_SIZE] = {0};

    int i = 0;
    for (; scanf("%s", key_buffer) == 1 && scanf("%s", value_buffer) == 1; ++i)
    {
        query_result qr = mcache_insert_strings(&params, key_buffer, value_buffer);
        if (qr.code != 0)
            break;
        memset(key_buffer, 0, BUFFER_SIZE);
        memset(value_buffer, 0, BUFFER_SIZE);
    }
    print_exit_message(i);
    return 0;
}
