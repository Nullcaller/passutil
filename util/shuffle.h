#include<stdbool.h>

#ifndef UTIL_SHUFFLE_H
#define UTIL_SHUFFLE_H

bool validate_shuffle_key(char* key, char* key_format);

char shuffle_char(char character, char* key, char* key_format);

char unshuffle_char(char shuffled_character, char* key, char* key_format);

void shuffle_in_place(char* plain_text, char* key, char* key_format);

void unshuffle_in_place(char* shuffled_text, char* key, char* key_format);

char* shuffle(char* plain_text, char* key, char* key_format);

char* unshuffle(char* shuffled_text, char* key, char* key_format);

#endif
