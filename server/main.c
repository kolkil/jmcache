#include <stdio.h>

#include "utils/options.h"
#include "utils/config.h"

int main(int argc, char **argv)
{
	input_options opt;
	if (parse_input(&opt, argc, argv) != 0)
		return 1;
	config_values *cnf = read_config(opt.config_path);
	return 0;
}