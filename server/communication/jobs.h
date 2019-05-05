#include "../storage/hash_table.h"
#include "protocol.h"

int execute_insert(hash_table *, mcache_request, int);
int execute_get(hash_table *, mcache_request, int);
int execute_pop(hash_table *, mcache_request, int);
int execute_keys(hash_table *, int);
int execute_all(hash_table *, mcache_request, int);
