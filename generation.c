#include<sodium.h>
#include<string.h>

#include "generation.h"

void generate_shuffle_key(char** key_var, char* key_format) {
	unsigned int len = strlen(key_format);
	char* key_remainder = malloc(len+1);
	memcpy(key_remainder, key_format, len+1);
	char* key = malloc(len);

	unsigned int it = 0;
	unsigned int pick;
	while(key_format[it] != '\0') {
		pick = randombytes_uniform(len);
		key[it] = key_remainder[pick];
		strncpy(key_remainder+pick, key_remainder+pick+1, len-pick);
		len--;
		it++;
	}

	free(key_remainder);
	*key_var = key;
}

unsigned char* generate_password_bytes(unsigned int length) {
	unsigned char* bytes = malloc(sizeof(unsigned char)*length);
	randombytes_buf(bytes, length);
	return bytes;
}
