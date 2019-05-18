typedef struct
{
    int static_save,
        static_load,
        traffic_log,
        error_log,
        server_port;
    char *save_path,
<<<<<<< HEAD
        *server_address;
=======
        *server_address,
        *traffic_file,
        *error_file;
    int server_port;
>>>>>>> logger
} config_values;

config_values *read_config(char *);
