#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>

#include "shuffle.h"

bool validate_shuffle_key(char* key, char* key_format) {
	unsigned int it = 0;
	bool isValid = true;
	char test_char;

	while(key_format[it] != '\0') { // Iterate over all possible characters
		test_char = key[it]; // Shuffle character
		test_char = key_format[strchr(key, test_char)-key]; // Unshuffle character
		if(test_char != key_format[it]) { // Compare
			isValid = false;
			break;
		}
		it++;
	}

	return isValid;
}

char shuffle_char(char character, char* key, char* key_format) {
	return key[strchr(key_format, character)-key_format];
}

char unshuffle_char(char shuffled_character, char* key, char* key_format) {
	return key_format[strchr(key, shuffled_character)-key];
}

void shuffle_in_place(char* plain_text, char* key, char* key_format) {
	unsigned int it = 0;
	while(plain_text[it] != '\0') {
		plain_text[it] = shuffle_char(plain_text[it], key, key_format);
		it++;
	}
}

void unshuffle_in_place(char* shuffled_text, char* key, char* key_format) {
	unsigned int it = 0;
	while(shuffled_text[it] != '\0') {
		shuffled_text[it] = unshuffle_char(shuffled_text[it], key, key_format);
		it++;
	}
}

char* shuffle(char* plain_text, char* key, char* key_format) {
	size_t length = strlen(plain_text)+1;
	char* shuffled_text = malloc(length);
	memcpy(shuffled_text, plain_text, length);
	shuffle_in_place(shuffled_text, key, key_format);
	return shuffled_text;
}

char* unshuffle(char* shuffled_text, char* key, char* key_format) {
	size_t length = strlen(shuffled_text)+1;
	char* plain_text = malloc(length);
	memcpy(plain_text, shuffled_text, length);
	unshuffle_in_place(plain_text, key, key_format);
	return plain_text;
}
