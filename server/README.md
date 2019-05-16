### JMCache - server

## Description
Server uses tcp protocol to deal with requests. All data is stored in hash table in the memory.

## Compilation
To compile server in default mode type `make`  
In debug mode server writes to its stdout some useful informations. To compile server in debug mode type `make debug`  
If its necessary to profile the server type `make prof` and then use gprof

## Configuration file
By default server uses file `server_config.csv` wich is in the server directory  
This is example configuration file (at the moment *config reader* does not support comments in configuration file)
```
static_save	yes
static_load	no
static_file	./ssave.tdb

server_address  127.0.0.1
server_port 2137
```
* `static_save` - save add data to `static_file` before exit
* `static_load` - load data from `static_file` before open for connections
* `static_file` - path to file
* `server_address` - address to wich server binds
* `server_port` - port for communication (default 2137)
