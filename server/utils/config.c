#include "config.h"

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
    for (int i = 0; fscanf(f, "%s", buffer); ++i)
    {
        if (!strcmp(buffer, "static_save"))
        {
            if (fscanf(f, "%s", buffer) != 1)
                return NULL;
            if (!strcmp(buffer, "yes"))
                config->static_save = 1;
            else
                config->static_save = 0;
            clean_buffer(buffer, 1024);
        }
        else if (!strcmp(buffer, "static_load"))
        {
            if (fscanf(f, "%s", buffer) != 1)
                return NULL;
            if (!strcmp(buffer, "yes"))
                config->static_load = 1;
            else
                config->static_load = 0;
            clean_buffer(buffer, 1024);
        }
        else if (!strcmp(buffer, "save_file"))
        {
            if (fscanf(f, "%s", buffer))
                return NULL;
            config->save_path = malloc(sizeof(strlen(buffer)) + 1);
            strcpy(config->save_path, buffer);
            clean_buffer(buffer, 1024);
        }
        else if (!strcmp(buffer, "server_address"))
        {
            if (fscanf(f, "%s", buffer) != 1)
                return NULL;
            config->server_address = malloc(sizeof(strlen(buffer)) + 1);
            strcpy(config->server_address, buffer);
            clean_buffer(buffer, 1024);
        }
        else if (!strcmp(buffer, "server_port"))
        {
            if (fscanf(f, "%d", &port) != 1)
                return NULL;
            config->server_port = port;
        }
    }
    return config;
}
