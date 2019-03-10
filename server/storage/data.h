#include <stdint.h>

typedef struct
{
	uint32_t len;
	uint8_t *key,
		*data;
} content;

content get_default_conetnet_value();
content *get_default_content();
