#include <stdio.h>
#include <unistd.h>

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
    query_result k = mcache_insert_strings(&params, "key", "value");
    printf("result code : %d\nmessage : %s\n", k.code, k.error_message);
    sleep(1);
    get_result g = mcache_get_strings(&params, "key");
    printf("result : %s\n", (char *)g.data.data);
    printf("message : %s\n", (char *)g.result.error_message);
    return 0;
}
