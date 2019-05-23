typedef struct
{
    char *config_path;
} input_options;

void set_default_options(input_options *);
void clear_default_options(input_options *);
int parse_input(input_options *, int, char **);
