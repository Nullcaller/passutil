#include<stdbool.h>

#ifndef MEMORIZER_H
#define MEMORIZER_H

#define MEMORIZER_DIRECT_PROMPT "ENTER PASSWORD > "
#define MEMORIZER_CHOICE_PROMPT "SELECT SYMBOL > "

#define MEMORIZER_ERROR -1
#define MEMORIZER_SUCCESS 0
#define MEMORIZER_FAILURE 1

int memorize_direct(char* shuffled_password, char* shuffle_key, char* shuffle_key_format);

int memorize_nth_symbol(char* shuffled_password, char* shuffle_key, char* shuffle_key_format, char* possible_characters, unsigned short symbol_position, unsigned short choice_count);

int memorize_by_symbols(char* shuffled_password, char* shuffle_key, char* shuffle_key_format, char* possible_characters, unsigned short choice_count, bool repeat_on_failure, bool exit_on_failure);

#endif