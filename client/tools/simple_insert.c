#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "../lib/mpocket_client.h"

#define BUFFER_SIZE 1024

long getMicrotime()
{
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);
    return currentTime.tv_sec * (int)1e6 + currentTime.tv_usec;
}

void print_exit_message(int c)
{
    printf("inserted %d pairs\n", c);
}

int main(int argc, char **argv)
{
    if (argc != 3)
        return 0;

    connection_params params = mpocket_connect(argv[1], atoi(argv[2]));

    if (params.server_fd < 1)
    {
        print_exit_message(0);
        return 0;
    }

    char key_buffer[BUFFER_SIZE] = {0},
         value_buffer[BUFFER_SIZE] = {0};

    int i = 0;
    long long int total_time = 0;
    for (; scanf("%s", key_buffer) == 1 && scanf("%s", value_buffer) == 1; ++i)
    {
        long long int start_time = getMicrotime();
        query_result qr = mpocket_insert_strings(&params, key_buffer, value_buffer);
        if (qr.code != 0)
            break;
        long long int stop_time = getMicrotime();
        total_time += stop_time - start_time;
        memset(key_buffer, 0, BUFFER_SIZE);
        memset(value_buffer, 0, BUFFER_SIZE);
    }
    print_exit_message(i);
    printf("avg time for insert = %fs\n", (float)(total_time / i + 1) / (1000 * 1000));
    return 0;
}
