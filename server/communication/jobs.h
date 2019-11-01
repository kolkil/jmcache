#ifndef _jobs_h_
#define _jobs_h_

#include "../storage/hash_table.h"
#include "../../shared/protocol.h"

typedef struct
{
    uint32_t count,
        time;
} number_and_time;

typedef struct
{
    number_and_time insert,
        get,
        pop,
        keys,
        all;
} connection_statistics;

int execute_insert(hash_table *, mpocket_request, int);
int execute_get(hash_table *, mpocket_request, int);
int execute_pop(hash_table *, mpocket_request, int);
int execute_keys(hash_table *, int);
int execute_all(hash_table *, int);
int do_job(hash_table *, mpocket_request, int, connection_statistics *);
int read_data_send_response(hash_table *, int, connection_statistics *, int);

#endif _jobs_h_
