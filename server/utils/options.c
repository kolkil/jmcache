#include "options.h"

#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>

void set_default_options(input_options *opt)
{
    opt->config_path = "./server_config.csv";
    return;
}

int parse_input(input_options *opt, int argc, char **argv)
{
    int c = 0,
        tmp = 0;
    while ((c = getopt(argc, argv, "c:")) != -1)
    {
        switch (c)
        {
        case 'c':
            tmp = strlen(optarg);
            if (tmp == 0)
                return -1;
            free(opt->config_path);
            opt->config_path = malloc(tmp + 1);
            strcpy(opt->config_path, optarg);
            break;

        default:
            return -1;
            break;
        }
    }
    return 0;
}
