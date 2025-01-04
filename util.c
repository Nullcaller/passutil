#include<stdlib.h>
#include<string.h>

#include<sodium.h>
#include<openssl/aes.h>
#include<openssl/sha.h>

#include "util.h"

#include "shuffler.h"

char* strcpymalloc(char* string) {
	char* new_str = malloc(strlen(string)+1);
	strcpy(new_str, string);
	return new_str;
}

char* strappendrealloc(char* destination, unsigned int* destination_allocated_length, unsigned int piece_length, char* source) {
	unsigned int destination_length;

	if(destination == NULL) {
		destination = malloc(sizeof(char)*piece_length);
		*destination_allocated_length = sizeof(char)*piece_length;
		destination_length = 0;
	} else {
		destination_length = strlen(destination);
	}

	unsigned int source_length = strlen(source);

	if((destination_length + source_length + 1) > *destination_allocated_length) {
		unsigned int new_length = ((destination_length + source_length + 1)/piece_length+1)*piece_length;
		destination = realloc(destination, new_length);
		*destination_allocated_length = new_length;
	}

	strcpy(destination+destination_length, source);
	return destination;
}

char* strtrimrealloc(char* string, unsigned int* allocated_length) {
	unsigned int length = strlen(string);

	string = realloc(string, length+1);
	*allocated_length = length+1;

	return string;
}

unsigned char* encrypt(unsigned int* result_length, char* cipher, unsigned char* bytes, unsigned int length, char* shuffled_key, char* shuffle_key, char* shuffle_key_format) {
	char* key = unshuffle(shuffled_key, shuffle_key, shuffle_key_format);
	unsigned char* key_digest;
	unsigned char* encrypted_bytes;
	unsigned char* iv;

	if(strcmp(cipher, "AES256") == 0) {
		key_digest = malloc(sizeof(unsigned char)*SHA256_DIGEST_LENGTH);
		SHA256(key, strlen(key), key_digest);
		free(key);
		AES_KEY aes_key;
		AES_set_encrypt_key(key_digest, 256, &aes_key);
		*result_length = sizeof(unsigned char)*(length/AES_BLOCK_SIZE+1)*AES_BLOCK_SIZE;
		encrypted_bytes = malloc(*result_length);
		iv = malloc(sizeof(unsigned char)*AES_BLOCK_SIZE);
		memset(iv, 0, AES_BLOCK_SIZE);
		AES_cbc_encrypt(bytes, encrypted_bytes, length, &aes_key, iv, AES_ENCRYPT);
		free(iv);
		free(key_digest);
		return encrypted_bytes;
	} else if(strcmp(cipher, "AES128") == 0) {
		// TODO
	}
}

unsigned char* decrypt(char* cipher, unsigned char* encrypted_bytes, unsigned int length, char* shuffled_key, char* shuffle_key, char* shuffle_key_format) {
	char* key = unshuffle(shuffled_key, shuffle_key, shuffle_key_format);
	unsigned char* key_digest;
	unsigned char* bytes;
	unsigned char* iv;

	if(strcmp(cipher, "AES256") == 0) {
		key_digest = malloc(sizeof(unsigned char)*SHA256_DIGEST_LENGTH);
		SHA256(key, strlen(key), key_digest);
		free(key);
		AES_KEY aes_key;
		AES_set_decrypt_key(key_digest, 256, &aes_key);
		bytes = malloc(sizeof(unsigned char)*(length/AES_BLOCK_SIZE+1)*AES_BLOCK_SIZE);
		iv = malloc(sizeof(unsigned char)*AES_BLOCK_SIZE);
		memset(iv, 0, AES_BLOCK_SIZE);
		AES_cbc_encrypt(encrypted_bytes, bytes, length, &aes_key, iv, AES_DECRYPT);
		free(iv);
		free(key_digest);
		return bytes;
	} else if(strcmp(cipher, "AES128") == 0) {
		// TODO
	}
}