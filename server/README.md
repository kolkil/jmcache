## JMCache - server

### Description
Server uses tcp protocol to deal with requests. All data is stored in hash table in the memory.

### Compilation
To compile server in default mode type `make`  
In debug mode server writes to its stdout some useful informations. To compile server in debug mode type `make debug`.  
If its necessary to profile the server type `make prof` and then use gprof.

### Configuration file
By default server uses file `server_config.csv` wich is in the server directory.  
You can use other config files by passing path as command line argument `./mcache-server.out -c path_to_config_file`.   
This is example configuration file (at the moment *config reader* does not support comments in configuration file).
```
static_save	yes
static_load	no
static_file	./ssave.tdb

server_address	127.0.0.1
server_port	2137

traffic_log	yes
traffic_log_file	traffic_log.log

error_log	yes
error_log_file error_log.log
```
* `static_save` - save data to `static_file` before exit
* `static_load` - load data from `static_file` before open for connections
* `static_file` - path to file
* `server_address` - address to wich server binds
* `server_port` - port for communication (default 2137)
* `traffic_log` - save statistical data
* `traffic_log_file` - path to file
* `error_log` - save error logs
* `error_log_file` - path to file
