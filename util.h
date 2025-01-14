#include<stdbool.h>

#ifndef UTIL_H
#define UTIL_H

int pseudosscanf(char* string, char* check_string);

char* strcpymalloc(char* string);

char* strappendrealloc(char* destination, unsigned int* destination_allocated_length, unsigned int piece_length, char* source);

char* strappendcharrealloc(char* destination, unsigned int* destination_allocated_length, unsigned int* current_destination_length, unsigned int piece_length, char source);

char* strtrimrealloc(char* string, unsigned int* allocated_length);

bool readfile(FILE* file, int max_attempts, unsigned int piece_length, char** data, unsigned int* length);

bool writefile(FILE* file, int max_attempts, char* data, unsigned int length);

unsigned char* encrypt(unsigned int* result_length, unsigned int* result_iv_length, unsigned char** ivp, char* cipher, unsigned char* bytes, unsigned int length, char* shuffled_key, char* shuffle_key, char* shuffle_key_format);

unsigned char* decrypt(char* cipher, unsigned char* encrypted_bytes, unsigned int length, char* shuffled_key, char* shuffle_key, char* shuffle_key_format, unsigned int iv_length, unsigned char* iv);

#endif
