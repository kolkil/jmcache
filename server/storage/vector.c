#include "vector.h"

#include <stdlib.h>
#include <stdio.h>

#define BASE 2

vector *get_vector()
{
	vector *v = malloc(sizeof(vector));
	v->current = -1;
	v->size = BASE;
	v->data = malloc(sizeof(int64_t) * BASE);
	return v;
}

void free_vector(vector *v)
{
	free(v->data);
	free(v);
}

int64_t vector_push_back(vector *v, char value)
{
	if (v->current >= (int64_t)(v->size - 1))
	{
		char *tmp = realloc(v->data, sizeof(char) * (v->size * 2));
		if (tmp == NULL)
			return -1;
		v->data = tmp;
		v->size *= 2;
	}
	v->data[++v->current] = value;
	return v->current;
}

char vector_pop_back(vector *v)
{
	if (v->current < 0)
		return -1;
	return v->data[v->current--];
}
