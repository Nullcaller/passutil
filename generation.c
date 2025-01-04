#include<sodium.h>
#include<string.h>

#include "generation.h"

#include "storage.h"

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

Password* generate_password(Store* store, char* format, unsigned int length) {
	Password* password = construct_password();

	unsigned int length_bytes = length*(sizeof(unsigned long long)/sizeof(char));
	unsigned char* plain_password_bytes = generate_password_bytes(length_bytes);
	
	write(store, password, plain_password_bytes, length_bytes, format, length);
	
	free(plain_password_bytes);
	return password;
}

void generate_password_and_append(Store* store, char* identifier, char* format, unsigned int length) {
	Password* password = generate_password(store, format, length);

	append_password(store, password, identifier);
}
