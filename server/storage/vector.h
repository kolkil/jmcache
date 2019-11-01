#ifndef _vector_h_
#define _vector_h_

#include <stdint.h>

#define GOLDEN_RATIO 1.618034

typedef struct
{
	uint32_t size;
	int64_t current;
	char *data;
} vector;

vector *get_vector();
void free_vector();

int64_t vector_push_back(vector *, char);
char vector_pop_back(vector *);

#endif
