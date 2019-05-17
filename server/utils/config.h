typedef struct
{
    short static_save,
        static_load,
        traffic_log,
        error_log;
    char *save_path,
        *server_address;
    int server_port;
} config_values;

config_values *read_config(char *);
