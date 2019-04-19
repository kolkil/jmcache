#include <stdio.h>

#include "lib/mcache_client.h"

int main(void)
{
    connection_params params = mcache_connect("127.0.0.1", 2137);
    if (params.server_fd < 1)
    {
        printf("server fd: %d\n", params.server_fd);
        printf("quit\n");
        return 1;
    }
    query_result k = mcache_insert_strings(&params, "klucz", "wartość");
    printf("result code : %d\n message : %s\n", k.code, k.error_message);
}
