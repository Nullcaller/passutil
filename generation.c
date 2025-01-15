#include<sodium.h>
#include<string.h>

#include "generation.h"

#include "storage.h"

void generate_shuffle_key(char** key_var, char* key_format) {
	unsigned int len = strlen(key_format);
	char* key_remainder = malloc(len+1);
	memcpy(key_remainder, key_format, len+1);
	char* key = malloc(len+1);
	key[len] = '\0';

	unsigned int it = 0;
	unsigned int pick;
	while(key_format[it] != '\0') {
		pick = randombytes_uniform(len);
		key[it] = key_remainder[pick];
		for(unsigned int j = pick+1; j <= len; j++)
			key_remainder[j-1] = key_remainder[j];
		len--;
		it++;
	}

	free(key_remainder);
	*key_var = key;
}

unsigned char* generate_bytes(unsigned int length) {
	unsigned char* bytes = malloc(sizeof(unsigned char)*length);
	randombytes_buf(bytes, length);
	return bytes;
}
