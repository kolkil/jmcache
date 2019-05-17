#include "config.h"
#include "../utils/debug_print.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void clean_buffer(char *buffer, int len)
{
    for (int i = 0; i < len; ++i)
        buffer[i] = 0;
}

config_values *read_config(char *path)
{
    FILE *f = fopen(path, "r");
    char buffer[1024] = {0};
    int port = 0;
    if (f == NULL)
        return NULL;
    config_values *config = malloc(sizeof(config_values));

    debug_print("file reading loop", 1);

    for (int i = 0; fscanf(f, "%s", buffer); ++i)
    {
        if (!strcmp(buffer, "static_save"))
        {
            debug_print("static_save", 1);

            if (fscanf(f, "%s", buffer) != 1)
                return NULL;
            if (!strcmp(buffer, "yes"))
                config->static_save = 1;
            else
                config->static_save = 0;

            debug_print(buffer, 2);
            debug_print("static_save", 0);

            clean_buffer(buffer, 1024);
        }
        else if (!strcmp(buffer, "static_load"))
        {
            debug_print("static_load", 1);

            if (fscanf(f, "%s", buffer) != 1)
                return NULL;
            if (!strcmp(buffer, "yes"))
                config->static_load = 1;
            else
                config->static_load = 0;

            debug_print(buffer, 2);
            debug_print("static_load", 0);

            clean_buffer(buffer, 1024);
        }
        else if (!strcmp(buffer, "traffic_log"))
        {
            debug_print("traffic_log", 1);

            if (fscanf(f, "%s", buffer) != 1)
                return NULL;
            if (!strcmp(buffer, "yes"))
                config->traffic_log = 1;
            else
                config->traffic_log = 0;

            debug_print(buffer, 2);
            debug_print("traffic_log", 0);

            clean_buffer(buffer, 1024);
        }
        else if (!strcmp(buffer, "error_log"))
        {
            debug_print("error_log", 1);

            if (fscanf(f, "%s", buffer) != 1)
                return NULL;
            if (!strcmp(buffer, "yes"))
                config->error_log = 1;
            else
                config->error_log = 0;

            debug_print(buffer, 2);
            debug_print("error_log", 0);

            clean_buffer(buffer, 1024);
        }
        else if (!strcmp(buffer, "static_file"))
        {
            debug_print("static_file", 1);
            debug_print("reading file", 1);

            clean_buffer(buffer, 1024);

            if (!fscanf(f, "%s", buffer))
                return NULL;

            debug_print("reading file", 0);

            config->save_path = malloc(strlen(buffer) + 1);
            strcpy(config->save_path, buffer);
            clean_buffer(buffer, 1024);

            debug_print(config->save_path, 2);
            debug_print("static_file", 0);
        }
        else if (!strcmp(buffer, "server_address"))
        {
            debug_print("server_address", 1);

            if (fscanf(f, "%s", buffer) != 1)
                return NULL;

            debug_print(buffer, 2);

            config->server_address = malloc(strlen(buffer) + 1);
            strcpy(config->server_address, buffer);
            clean_buffer(buffer, 1024);

            debug_print("server_address", 0);
        }
        else if (!strcmp(buffer, "server_port"))
        {
            debug_print("server_port", 1);

            if (fscanf(f, "%d", &port) != 1)
                return NULL;
            config->server_port = port;
            clean_buffer(buffer, 1024);
            debug_print_int(port);
            debug_print("server_port", 0);
        }
        else
        {
            debug_print("no key", 1);
            debug_print(buffer, 2);
            debug_print("no key", 0);
            break;
        }
    }
    debug_print("file reading loop", 0);
    return config;
}
