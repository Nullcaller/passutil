#include<stdbool.h>
#include<termios.h>

#ifndef PSEUDOSHELL_H
#define PSEUDOSHELL_H

/*** CONSTANTS ***/

#define PSEUDOSHELL_BUFFER_SIZE 4096

static char* _pseudoshell_up_down_arrows[] = {
	"[A",
	"[B"
};
static unsigned int _pseudoshell_up_down_arrows_length = 2;

static char* _pseudoshell_newline = "\n";
static unsigned int _pseudoshell_newline_length = 1;

/*** STATE DESCRIPTION ***/

// Terminal management
static bool _pseudoshell_is_terminal_set = false;

static struct termios _pseudoshell_terminal_settings;

/*** TERMINAL MANAGEMENT FUNCTIONS ***/

#define _PSEUDOSHELL_TERMINAL_SET_RESET_OK 0
#define _PSEUDOSHELL_TERMINAL_SET_RESET_FAILED -1
#define _PSEUDOSEHLL_TERMINAL_SET_RESET_WRONG_STATE -2

/*** INPUT FUNCTIONS ***/

#define _PSEUDOSHELL_INPUT_LOOP_CONTINUE 0
#define _PSEUDOSHELL_INPUT_LOOP_EXIT 1
#define _PSEUDOSHELL_INPUT_LOOP_EOF 2

int pseudoshell_get_string(char** str, unsigned int piece_length);

int pseudoshell_get_password(char** pass, char* prompt, unsigned int piece_length, bool tolerate_eof);

int pseudoshell_get_sepcific_hidden_character(char* passchar, char* prompt, char* valid_chars, bool repeat_until_valid, bool tolerate_eof);

/*** PROMPTING (NOT THE LLM KIND) FUNCTIONS ***/

int pseudoshell_present_prompt(char* prompt, char* options_LOWERCASE, bool repeat_until_valid);

bool pseudoshell_present_yesno_prompt(char* prompt, bool repeat_until_valid);

/*** COMMAND-RELATED FUNCTIONS ***/

#define _PSEUDOSHELL_HANDLE_COMMAND_INPUT_TERMINAL_SET_FAIL		-2
#define _PSEUDOSHELL_HANDLE_COMMAND_INPUT_EOF					-1

#define _PSEUDOSHELL_HANDLE_COMMAND_INPUT_TERMINAL_SET_FAIL_MESSAGE "ERROR: Couldn't set terminal state"
#define _PSEUDOSHELL_HANDLE_COMMAND_INPUT_TERMINAL_RESET_FAIL_MESSAGE "WARNING: Couldn't reset terminal state"

#define PSEUDOSHELL_OK			0
#define PSEUDOSHELL_CONTINUE	1

#define PSEUDOSHELL_LOOP_PROMPT_START		"passutil("
#define PSEUDOSHELL_LOOP_PROMPT_END			")> "

int enter_pseudoshell_loop();

#endif