typedef struct
{
    int static_save,
        static_load,
        traffic_log,
        error_log,
        server_port;
    char *save_path,
        *server_address;
} config_values;

config_values *read_config(char *);
