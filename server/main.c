#include <stdio.h>

#include "utils/options.h"
#include "utils/debug_print.h"
#include "program.h"

int main(int argc, char **argv)
{
    input_options opt;
    set_default_options(&opt);
    debug_print("parsing input", 1);

    if (parse_input(&opt, argc, argv) != 0)
        return 1;

    debug_print("parsing input", 0);
    debug_print("reading config", 1);
    config_values *cnf = read_config(opt.config_path);

    if (cnf == NULL)
    {
        debug_print("read_config error", 0);
        return 1;
    }

    debug_print("reading config", 0);
    debug_print("start program", 1);
    start_program(cnf);
    free_config_values(cnf);
    clear_default_options(&opt);

    debug_print("start program", 0);

    return 0;
}