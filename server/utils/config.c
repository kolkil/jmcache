#include "config.h"
#include "../utils/debug_print.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int compare_yes_and_set_int(char *target, char *key, char *value, int *to_set)
{
    if (strcmp(target, key))
        return 1;

    debug_print(target, 1);
    debug_print(value, 2);

    if (strcmp("yes", value))
    {
        *to_set = 0;
        debug_print(target, 0);
        return 1;
    }

    *to_set = 1;
    debug_print(target, 0);

    return 0;
}

int compare_and_set_int(char *target, char *key, char *value, int *to_set)
{
    if (strcmp(target, key))
        return 1;

    debug_print(target, 1);
    debug_print(value, 2);

    *to_set = atoi(value);
    debug_print(target, 0);

    return 0;
}

int compare_and_set_string(char *target, char *key, char *value, char **to_set)
{
    if (strcmp(target, key))
        return 1;

    debug_print(target, 1);
    debug_print(value, 2);

    int len = strlen(value);
    *to_set = calloc(len + 1, sizeof(char));
    memcpy(*to_set, value, len);
    debug_print(target, 0);

    return 0;
}

config_values *read_config(char *path)
{
    FILE *f = fopen(path, "r");
    char key_buffer[1024] = {0},
         value_buffer[1024] = {0};

    if (f == NULL)
        return NULL;

    config_values *config = calloc(1, sizeof(config_values));
    debug_print("file reading loop", 1);

    for (int i = 0; fscanf(f, "%s", key_buffer) == 1 && fscanf(f, "%s", value_buffer) == 1; ++i)
    {
        compare_yes_and_set_int("static_save", key_buffer, value_buffer, &config->static_save);
        compare_yes_and_set_int("static_load", key_buffer, value_buffer, &config->static_load);
        compare_yes_and_set_int("traffic_log", key_buffer, value_buffer, &config->traffic_log);
        compare_yes_and_set_int("error_log", key_buffer, value_buffer, &config->error_log);

        compare_and_set_int("server_port", key_buffer, value_buffer, &(config->server_port));

        compare_and_set_string("server_address", key_buffer, value_buffer, &config->server_address);
        compare_and_set_string("static_file", key_buffer, value_buffer, &config->save_path);
        compare_and_set_string("traffic_log_file", key_buffer, value_buffer, &config->traffic_file);
        compare_and_set_string("error_log_file", key_buffer, value_buffer, &config->error_file);

        memset(key_buffer, 0, 1024);
        memset(value_buffer, 0, 1024);
    }

    fclose(f);
    debug_print("file reading loop", 0);

    return config;
}

void free_config_values(config_values *values)
{
    free(values->error_file);
    free(values->save_path);
    free(values->traffic_file);
    free(values->server_address);
    free(values);
}
