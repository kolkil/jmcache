#include <stdint.h>

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
