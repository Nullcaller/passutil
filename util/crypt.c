#include<openssl/aes.h>
#include<openssl/evp.h>
#include<openssl/sha.h>

#include "crypt.h"

#include "shuffle.h"
#include "generation.h"

unsigned char* encrypt(unsigned int* result_length, unsigned int* result_iv_length, unsigned char** ivp, char* cipher, unsigned char* bytes, unsigned int length, char* shuffled_key, char* shuffle_key, char* shuffle_key_format) {
	char* key = unshuffle(shuffled_key, shuffle_key, shuffle_key_format);
	unsigned char* key_digest;
	unsigned char* encrypted_bytes;

	if(strcmp(cipher, "AES256") == 0) {
		EVP_CIPHER_CTX* context = EVP_CIPHER_CTX_new();
		
		*result_iv_length = AES_BLOCK_SIZE;
		*ivp = generate_bytes(*result_iv_length);
		key_digest = malloc(sizeof(unsigned char)*SHA256_DIGEST_LENGTH);
		SHA256(key, strlen(key), key_digest);
		free(key);
		
		EVP_EncryptInit_ex(context, EVP_aes_256_cbc(), NULL, key_digest, *ivp);
		
		if(length % AES_BLOCK_SIZE == 0)
			*result_length = sizeof(unsigned char)*length;
		else
			*result_length = sizeof(unsigned char)*(length/AES_BLOCK_SIZE+1)*AES_BLOCK_SIZE;
		encrypted_bytes = malloc(*result_length);
		
		int rl = 0, sl = 0;
		EVP_EncryptUpdate(context, encrypted_bytes, &sl, bytes, length);
		rl += sl;
		EVP_EncryptFinal_ex(context, bytes+sl, &sl);
		rl += sl;
		
		EVP_CIPHER_CTX_free(context);
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
		
		EVP_CIPHER_CTX* context = EVP_CIPHER_CTX_new();

		key_digest = malloc(sizeof(unsigned char)*SHA256_DIGEST_LENGTH);
		SHA256(key, strlen(key), key_digest);
		free(key);
		
		EVP_DecryptInit_ex(context, EVP_aes_256_cbc(), NULL, key_digest, iv);
		
		if(length % AES_BLOCK_SIZE == 0)
			bytes_length = sizeof(unsigned char)*length;
		else
			bytes_length = sizeof(unsigned char)*(length/AES_BLOCK_SIZE+1)*AES_BLOCK_SIZE;
		bytes = malloc(bytes_length);
		
		int rl = 0, sl = 0;
		EVP_DecryptUpdate(context, bytes, &sl, encrypted_bytes, length);
		rl += sl;
		EVP_DecryptFinal_ex(context, bytes+sl, &sl);
		rl += sl;
		
		EVP_CIPHER_CTX_free(context);
		free(key_digest);
		
		return bytes;
	} else if(strcmp(cipher, "AES128") == 0) {
		// TODO
	}
}
