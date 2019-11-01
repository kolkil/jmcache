#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "../lib/mpocket_client.h"

#define MAX_KEY_SIZE 128
#define MAX_VALUE_SIZE 128

int menu_and_initialization(connection_params *params)
{
    char address[10];
    printf("server address: ");
    if (scanf("%s", address) != 1)
        return 0;
    int port = -1;
    printf("server port: ");
    if (scanf("%d", &port) != 1)
        return 0;
    connection_params t = mpocket_connect(address, port);
    params->address = t.address;
    params->port = t.port;
    params->server_fd = t.server_fd;
    return 1;
}

int select_command()
{
    printf("[1] insert\n");
    printf("[2] get\n");
    printf("[3] pop\n");
    printf("[4] keys\n");
    printf("[5] all\n");
    printf("[6] stats\n");
    printf("[7] access\n");
    printf(": ");
    int choi = -1;
    if (scanf("%d", &choi) != 1)
        return -1;
    return choi;
}

void print_l_and_d(length_and_data ld)
{
    for(int i = 0; i < ld.length; ++i)
    {
        if(i != 0)
            printf(" ");
        printf("%d", ld.data[i]);
    }
}

int main(int argc, char **argv)
{
    connection_params params;

    if (argc == 3)
    {
        params = mpocket_connect(argv[1], atoi(argv[2]));
    }
    else if (menu_and_initialization(&params) != 1)
    {
        printf("error\n");
        return 1;
    }
    if (params.server_fd < 1)
    {
        printf("server fd: %d\n", params.server_fd);
        printf("quit\n");
        return 1;
    }

    while (1)
    {
        char key[MAX_KEY_SIZE + 1] = {0},
                                value[MAX_VALUE_SIZE + 1] = {0};
        int command = select_command();
        query_result qr;
        get_result gr;
        keys_result kr;
        all_result ar;
        stats_result sr;
        access_result acr;
        length_and_data access_key;

        switch (command)
        {
        case INSERT:
            printf("put key: ");
            if (scanf("%s", key) != 1)
                return 1;
            printf("put value: ");
            if (scanf("%s", value) != 1)
                return 1;
            qr = mpocket_insert_strings(&params, key, value);
            printf("code %d\nmessage %s\n", qr.code, qr.error_message);
            break;

        case GET:
            printf("put key: ");
            if (scanf("%s", key) != 1)
                return 1;
            gr = mpocket_get_strings(&params, key);
            printf("\"%.*s\"\n", gr.data.length, gr.data.data);
            printf("code %d\nmessage %s\n", gr.result.code, gr.result.error_message);
            break;

        case POP:
            printf("put key: ");
            if (scanf("%s", key) != 1)
                return 1;
            gr = mpocket_pop_strings(&params, key);
            printf("\"%.*s\"\n", gr.data.length, gr.data.data);
            printf("code %d\nmessage %s\n", gr.result.code, gr.result.error_message);
            break;

        case KEYS:
            kr = mpocket_keys(&params);
            for (int i = 0; i < kr.count; ++i)
            {
                if (kr.keys[i].length == 0 || kr.keys[i].data == NULL)
                    continue;
                printf("%.*s\n", kr.keys[i].length, kr.keys[i].data);
                // free(kr.keys[i].data);
            }
            free(kr.keys);
            break;

        case ALL:
            ar = mpocket_all(&params);
            for (int i = 0; i < ar.count; ++i)
            {
                printf("%.*s\t%.*s\n", ar.all_data[i][0].length, ar.all_data[i][0].data, ar.all_data[i][1].length, ar.all_data[i][1].data);
                free(ar.all_data[i]);
            }
            free(ar.all_data);
            break;

        case STATS:
            sr = mpocket_stats(&params);
            printf("filled\t%d\ncount\t%d\n", sr.filled, sr.items_count);
            break;

        case ACCESS:
            acr = mpocket_access(&params, access_key);
            printf("access\t%s\n", acr.ok ? "granted": "denied");
            printf("access_key\t");
            print_l_and_d(acr.access_key);
            printf("\n");
            break;

        default:
            return 1;
            break;
        }
        memset(key, 0, MAX_KEY_SIZE);
        memset(value, 0, MAX_VALUE_SIZE);
    }

    return 0;
}
