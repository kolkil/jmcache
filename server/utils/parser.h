typedef struct
{
    int length;
    char *content;
} simple_string;

typedef struct
{
    int error,
        command;
    simple_string first_param,
        second_param;
    char *error_message;
} parsed_data;

parsed_data parse_data(char *);
