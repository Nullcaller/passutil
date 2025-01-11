#include<stdlib.h>
#include<string.h>

#include<sodium.h>
#include<openssl/aes.h>
#include<openssl/sha.h>

#include "util.h"

#include "shuffler.h"

int pseudosscanf(char* string, char* check_string) {
	unsigned int length = strlen(check_string);

	int read = 0;
	for(unsigned int it = 0; it < length; it++)
		if(string[it] == check_string[it])
			read++;
		else
			break;

	return read;
}

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
		destination = realloc(destination, sizeof(char)*new_length);
		*destination_allocated_length = new_length;
	}

	strcpy(destination+destination_length, source);
	return destination;
}

char* strappendcharrealloc(char* destination, unsigned int* destination_allocated_length, unsigned int* current_destination_length, unsigned int piece_length, char source) {
	if(destination == NULL) {
		destination = malloc(sizeof(char)*piece_length);
		*destination_allocated_length = sizeof(char)*piece_length;
		*current_destination_length = 0;
	}

	if((*current_destination_length+2) > *destination_allocated_length) {
		unsigned int new_length = ((*current_destination_length+2)/piece_length+1)*piece_length;
		destination = realloc(destination, sizeof(char)*new_length);
		*destination_allocated_length = new_length;
	}

	*(destination+*current_destination_length) = source;
	*(destination+*current_destination_length+1) = '\0';
	*current_destination_length += 1;

	return destination;
}

char* strtrimrealloc(char* string, unsigned int* allocated_length) {
	unsigned int length = strlen(string);

	string = realloc(string, length+1);
	*allocated_length = length+1;

	return string;
}

bool readfile(FILE* file, int max_attempts, unsigned int piece_length, char** data, unsigned int* length) {
	char* _data = malloc(sizeof(char)*piece_length);
	unsigned int allocated_length = piece_length;
	unsigned int _length = 0;
	char* buf = malloc(sizeof(char)*piece_length);
	size_t read;
	int attempts = 0;

	while(!feof(file)) {
		read = fread(buf, sizeof(char), piece_length, file);
		
		if(read == 0)
			attempts++;
		else
			attempts = 0;
		
		if(attempts >= max_attempts) {
			free(buf);
			free(_data);
			return false;
		}
		
		if((_length+read) > allocated_length) {
			allocated_length = ((_length+read)/piece_length+1)*piece_length;
			_data = realloc(_data, allocated_length);
		}

		memcpy(_data+_length, buf, read);
		_length += read;
	}

	if(allocated_length > _length) {
		_data = realloc(_data, _length);
	}

	*data = _data;
	*length = _length;

	return true;
}

bool writefile(FILE* file, int max_attempts, char* data, unsigned int length) {
	unsigned int remaining_length = length;
	size_t result;
	int attempts = 0;
	while(remaining_length > 0) {
		result = fwrite(data+(length-remaining_length), sizeof(char), remaining_length, file);
		remaining_length -= result;
		if(result == 0) {
			attempts++;
		} else
			attempts = 0;
		if(attempts >= max_attempts)
			break;
	}
	if(remaining_length > 0)
		return false;
	return true;
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
		if(length % AES_BLOCK_SIZE == 0)
			*result_length = sizeof(unsigned char)*length;
		else
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
	unsigned int bytes_length;

	if(strcmp(cipher, "AES256") == 0) {
		key_digest = malloc(sizeof(unsigned char)*SHA256_DIGEST_LENGTH);
		SHA256(key, strlen(key), key_digest);
		free(key);
		AES_KEY aes_key;
		AES_set_decrypt_key(key_digest, 256, &aes_key);
		if(length % AES_BLOCK_SIZE == 0)
			bytes_length = sizeof(unsigned char)*length;
		else
			bytes_length = sizeof(unsigned char)*(length/AES_BLOCK_SIZE+1)*AES_BLOCK_SIZE;
		bytes = malloc(bytes_length);
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