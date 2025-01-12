#include<stdbool.h>
#include<termios.h>

#ifndef PSEUDOSHELL_H
#define PSEUDOSHELL_H

#define PSEUDOSHELL_BUFFER_SIZE 4096

static struct termios pseudoshell_terminal_settings;

int pseudoshell_getpass(char** pass, char* prompt, unsigned int piece_length);

int pseudoshell_getpasschar(char* passchar, char* prompt, char* valid_chars, bool repeat_until_valid);

int getstr(char** str, unsigned int piece_length);

int present_prompt(char* prompt, char* options_LOWERCASE, bool repeat_until_valid);

bool present_yesno_prompt(char* prompt, bool repeat_until_valid);

void parse_command(char* str, char** command, int* argc, char*** argv);

#define PSEUDOSHELL_OK			0
#define PSEUDOSHELL_CONTINUE	1

#define PSEUDOSHELL_LOOP_PROMPT_START		"passutil("
#define PSEUDOSHELL_LOOP_PROMPT_END			")> "

int enter_pseudoshell_loop();

#endif