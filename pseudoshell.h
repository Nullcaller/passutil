#include<stdbool.h>

#ifndef PSEUDOSHELL_H
#define PSEUDOSHELL_H

#define PSEUDOSHELL_BUFFER_SIZE 4096

int pseudoshell_getpass(char** pass, char* prompt, unsigned int piece_length);

int pseudoshell_getpasschar(char* passchar, char* prompt, char* valid_chars, bool repeat_until_valid);

int getstr(char** str, unsigned int piece_length);

int present_prompt(char* prompt, char* options_LOWERCASE, bool repeat_until_valid);

bool present_yesno_prompt(char* prompt, bool repeat_until_valid);

#endif