#include<stdbool.h>

#ifndef PSEUDOSHELL_H
#define PSEUDOSHELL_H

#define PSEUDOSHELL_BUFFER_SIZE 4096

int present_prompt(char* prompt, char* options_LOWERCASE, bool repeat_until_valid);

bool present_yesno_prompt(char* prompt, bool repeat_until_valid);

#endif