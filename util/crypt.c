#include<openssl/aes.h>
#include<openssl/sha.h>

#include "crypt.h"

#include "shuffle.h"
#include "generation.h"

unsigned char* encrypt(unsigned int* result_length, unsigned int* result_iv_length, unsigned char** ivp, char* cipher, unsigned char* bytes, unsigned int length, char* shuffled_key, char* shuffle_key, char* shuffle_key_format) {
	char* key = unshuffle(shuffled_key, shuffle_key, shuffle_key_format);
	unsigned char* key_digest;
	unsigned char* encrypted_bytes;

	if(strcmp(cipher, "AES256") == 0) {
		*result_iv_length = AES_BLOCK_SIZE;
		*ivp = generate_bytes(*result_iv_length);
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
		AES_cbc_encrypt(bytes, encrypted_bytes, length, &aes_key, *ivp, AES_ENCRYPT);
		free(key_digest);
		return encrypted_bytes;
	} else if(strcmp(cipher, "AES128") == 0) {
		// TODO
	}
}

unsigned char* decrypt(char* cipher, unsigned char* encrypted_bytes, unsigned int length, char* shuffled_key, char* shuffle_key, char* shuffle_key_format, unsigned int iv_length, unsigned char* iv) {
	char* key = unshuffle(shuffled_key, shuffle_key, shuffle_key_format);
	unsigned char* key_digest;
	unsigned char* bytes;
	unsigned int bytes_length;

	if(strcmp(cipher, "AES256") == 0) {
		if(iv_length != AES_BLOCK_SIZE)
			return NULL;
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
		AES_cbc_encrypt(encrypted_bytes, bytes, length, &aes_key, iv, AES_DECRYPT);
		free(key_digest);
		return bytes;
	} else if(strcmp(cipher, "AES128") == 0) {
		// TODO
	}
}
