typedef struct
{
    int static_save,
        static_load,
        traffic_log,
        error_log,
        server_port,
        limit_access;
    char *save_path,
        *server_address,
        *traffic_file,
        *error_file;
} config_values;

config_values *read_config(char *);
void free_config_values(config_values *);
