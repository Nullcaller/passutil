#ifndef UTIL_CRYPT_H
#define UTIL_CRYPT_H

unsigned char* encrypt(unsigned int* result_length, unsigned int* result_iv_length, unsigned char** ivp, char* cipher, unsigned char* bytes, unsigned int length, char* shuffled_key, char* shuffle_key, char* shuffle_key_format);

unsigned char* decrypt(char* cipher, unsigned char* encrypted_bytes, unsigned int length, char* shuffled_key, char* shuffle_key, char* shuffle_key_format, unsigned int iv_length, unsigned char* iv);

#endif
