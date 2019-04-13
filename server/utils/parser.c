#include "parser.h"
#include "debug_print.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char *commands[5] = {
    "insert",
    "get",
    "pop",
    "keys",
    "all"};

int is_valid(char *data)
{
    int max_command_len = 6;
    if ((int)strlen(data) < max_command_len)
        return 0;
    return 1;
}

int read_command_from_the_beginning(char *data)
{
    char buffer[7] = {0};
    int len = strlen(data);
    for (int i = 0; i < len && i < 7; ++i)
    {
        if (data[i] == ' ')
            break;
        buffer[i] = data[i];
    }
    for (int i = 0; i < 5; ++i)
    {
        if (!strcmp(buffer, commands[i]))
            return i + 1;
    }
    return -1;
}

char *alloc_string(char *data)
{
    int len = strlen(data);
    if (len < 1)
        return NULL;
    char *allocated = calloc(sizeof(char), len + 1);
    memcpy(allocated, data, len);
    return allocated;
}

parsed_data parse_data(char *data)
{
    parsed_data parsed;
    parsed.command = -1;
    parsed.error = 0;
    parsed.first_param.length = 0;
    parsed.second_param.length = 0;

    if (!is_valid(data))
    {
        parsed.error = 1;
        parsed.error_message = alloc_string("invalid input");
        return parsed;
    }
    parsed.command = read_command_from_the_beginning(data);
    if (parsed.command == -1)
    {
        parsed.command = -1;
        parsed.error = 2;
        parsed.error_message = alloc_string("could not parse read command");
        return parsed;
    }
    int len = strlen(data);
    for (int i = strlen(commands[parsed.command]), flag = 0, start = 0, c = 0; i < len; ++i)
    {
        if (data[i] != '"')
            continue;
        if (!flag)
        {
            flag = 1;
            start = i;
            c++; // hehe
            continue;
        }
        else if (data[i - 1] != '\\')
        {
            flag = 0;
            int readed_len = i - start - 1;
            if (readed_len > 0)
            {
                if (c == 1)
                {
                    parsed.first_param.content = calloc(sizeof(char), readed_len + 1);
                    parsed.first_param.length = readed_len;
                    memcpy(parsed.first_param.content, data + start + 1, readed_len);
                }
                else
                {
                    parsed.second_param.content = calloc(sizeof(char), readed_len + 1);
                    parsed.second_param.length = readed_len;
                    memcpy(parsed.second_param.content, data + start + 1, readed_len);
                    break;
                }
            }
        }
    }
    return parsed;
}

void free_parsed_data_content(parsed_data *data)
{
    free(data->first_param.content);
    free(data->second_param.content);
    free(data->error_message);
}

void free_parsed_data(parsed_data *data)
{
    free_parsed_data_content(data);
    free(data);
}
