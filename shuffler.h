#include<stdbool.h>

#ifndef SHUFFLER_H
#define SHUFFLER_H

bool validate_shuffle_key(char* key, char* key_format);

void shuffle_in_place(char* plain_text, char* key, char* key_format);

void unshuffle_in_place(char* shuffled_text, char* key, char* key_format);

char* shuffle(char* plain_text, char* key, char* key_format);

char* unshuffle(char* shuffled_text, char* key, char* key_format);

#endif
