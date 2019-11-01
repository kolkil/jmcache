#ifndef _debug_print_h_
#define _debug_print_h_

#include <stdint.h>
#include <stddef.h>

void debug_print(char *, int);
void debug_print_int(int);
void debug_print_raw(char *);
void debug_print_raw_int(int);
void debug_print_raw_string_int(char *, int);
void debug_print_content_as_hex(char *, void *, size_t);
void debug_print_content_as_dec(char *, void *, size_t);

long int microtime_now();
float micro_to_seconds(long int, long int);
float msd(long int, long int); // micro to seconds divide

#endif _debug_print_h_
