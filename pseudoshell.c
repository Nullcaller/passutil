#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<termios.h>

#include "pseudoshell.h"

#include "util.h"
#include "facilities.h"

/*** TERMINAL MANAGEMENT FUNCTIONS ***/

int _pseudoshell_terminal_set() {
	if(_pseudoshell_is_terminal_set == true)
		return _PSEUDOSEHLL_TERMINAL_SET_RESET_WRONG_STATE;

	struct termios new;

	if(tcgetattr(fileno(stdin), &_pseudoshell_terminal_settings) != 0)
		return _PSEUDOSHELL_TERMINAL_SET_RESET_FAILED;
	new = _pseudoshell_terminal_settings;
	new.c_lflag &= ~(ICANON | ECHO);
	if(tcsetattr(fileno(stdin), TCSAFLUSH, &new) != 0)
    	return _PSEUDOSHELL_TERMINAL_SET_RESET_FAILED;

	_pseudoshell_is_terminal_set = true;

	return _PSEUDOSHELL_TERMINAL_SET_RESET_OK;
}

int _pseudoshell_terminal_reset() {
	if(_pseudoshell_is_terminal_set == false)
		return _PSEUDOSEHLL_TERMINAL_SET_RESET_WRONG_STATE;

	if(tcsetattr(fileno(stdin), TCSAFLUSH, &_pseudoshell_terminal_settings) != 0)
		return _PSEUDOSHELL_TERMINAL_SET_RESET_FAILED;

	_pseudoshell_is_terminal_set = false;

	return _PSEUDOSHELL_TERMINAL_SET_RESET_OK;
}

/*** INPUT FUNCTIONS ***/

int _pseudoshell_handle_character_input(
	/// Memory
	unsigned int* characters_readp,
	char** strp,
	unsigned int* str_allocated_lengthp,
	unsigned int* str_lengthp,
	bool* vt100_sequence_indicatorp,
	unsigned int* vt100_posp,
	char** vt100_seqp,
	char* characterp,
	/// Settings
	unsigned int piece_length,
	bool echo,
	bool handle_backspace,
	bool handle_vt100_codes,
	bool remove_vt100_codes_from_str,
	char** terminating_vt100_codes,
	unsigned int terminating_vt100_codes_length,
	int eof_return_value,
	bool is_whitelist, // Blacklist otherwise
	char* character_list,
	unsigned int character_list_length,
	bool append_terminating_character,
	bool echo_terminating_character
) {
	int getchar_result = getchar();

	if(getchar_result == EOF)
		return eof_return_value;

	*characterp = getchar_result;
	*characters_readp += 1;

	bool character_allowed = false;
	for(unsigned int it = 0; it < character_list_length; it++)
		if(*characterp == character_list[it]) {
			character_allowed = true;
			break;
		}
	if(!is_whitelist)
		character_allowed = !character_allowed;

	if(handle_backspace && (*characterp == 127 || *characterp == 8)) {
		if(*str_lengthp > 0) {
			*str_lengthp -= 1;
			(*strp)[*str_lengthp] = '\0';
			if(echo)
				fputs("\b \b", stdout);
		}
		
		return _PSEUDOSHELL_INPUT_LOOP_CONTINUE;
	}
	
	if(character_allowed || append_terminating_character || (handle_vt100_codes && *vt100_sequence_indicatorp))
		*strp = strappendcharrealloc(*strp, str_allocated_lengthp, str_lengthp, piece_length, *characterp);

	if(handle_vt100_codes) {
		if(*characterp == '\033') {
			*vt100_sequence_indicatorp = true;
			*vt100_posp = *str_lengthp - 1;
			return _PSEUDOSHELL_INPUT_LOOP_CONTINUE;
		}
		
		if(*vt100_sequence_indicatorp) {
			if(strchr(FORMAT_AZaz09, *characterp) == NULL)
				return _PSEUDOSHELL_INPUT_LOOP_CONTINUE;

			*vt100_sequence_indicatorp = false;
			if(*vt100_seqp != NULL)
				free(*vt100_seqp);
			*vt100_seqp = strcpymalloc(*strp+*vt100_posp);
				
			if(remove_vt100_codes_from_str) {
				*str_lengthp -= strlen(*vt100_seqp);
				(*strp)[*str_lengthp] = '\0';
			}

			if(terminating_vt100_codes_length > 0)
				for(unsigned int it = 0; it < terminating_vt100_codes_length; it++)
					if(strcmp(*vt100_seqp+1, terminating_vt100_codes[it]) == 0)
						return _PSEUDOSHELL_INPUT_LOOP_EXIT;

			return _PSEUDOSHELL_INPUT_LOOP_CONTINUE;
		}
	}

	if(echo || (!character_allowed && echo_terminating_character))
		putchar(*characterp);
	
	return character_allowed ? _PSEUDOSHELL_INPUT_LOOP_CONTINUE : _PSEUDOSHELL_INPUT_LOOP_EXIT;
}

int pseudoshell_get_string(char** str, unsigned int piece_length) {
	unsigned int characters_read = 0;
	char* _str = NULL;
	char* _vt100_esc = NULL;
	unsigned int str_allocated_length = 0;
	unsigned int str_length = 0;
	char character;
	bool vt100_sequence = false;
	unsigned int vt100_pos;

	if(_pseudoshell_terminal_set() != 0)
		return -1;

	while(true)
		if(_pseudoshell_handle_character_input(
			&characters_read,		// characters_readp
			&_str,					// strp
			&str_allocated_length,	// str_allocated_lengthp
			&str_length,			// str_lengthp
			&vt100_sequence,		// vt100_sequence_indicatorp
			&vt100_pos,				// vt100_posp
			&_vt100_esc,			// vt100_seqp
			&character,				// characterp
			piece_length,			// piece_length
			true,	// echo
			true,	// handle_backspace
			true,	// handle_vt100_codes
			true,	// remove_vt100_codes_from_str
			NULL,	// terminating_vt100_codes
			0,		// terminating_vt100_codes_length
			_PSEUDOSHELL_INPUT_LOOP_EXIT,		// eof_return_value
			false,	// is_whitelist
			_pseudoshell_standard_terminators,				// character_list
			_pseudoshell_standard_terminators_length,		// character_list_length
			false,	// append_terminating_character
			true	// echo_terminating_character
		) == _PSEUDOSHELL_INPUT_LOOP_EXIT)
			break;

	if(_str != NULL)
		_str = strtrimrealloc(_str, &str_allocated_length);
	else {
		_str = malloc(sizeof(char));
		_str[0] = '\0';
	}

	_pseudoshell_terminal_reset();

	*str = _str;
	return characters_read;
}

int pseudoshell_get_password(char** pass, char* prompt, unsigned int piece_length, bool tolerate_eof) {
	unsigned int characters_read = 0;
	char* _str = NULL;
	char* _vt100_esc = NULL;
	unsigned int str_allocated_length = 0;
	unsigned int str_length = 0;
	char character;
	bool vt100_sequence = false;
	unsigned int vt100_pos;

	fputs(prompt, stdout);

	if(_pseudoshell_terminal_set() != 0)
		return -1;

	int character_handle_result;
	while(true)
		if((character_handle_result = _pseudoshell_handle_character_input(
			&characters_read,		// characters_readp
			&_str,					// strp
			&str_allocated_length,	// str_allocated_lengthp
			&str_length,			// str_lengthp
			&vt100_sequence,		// vt100_sequence_indicatorp
			&vt100_pos,				// vt100_posp
			&_vt100_esc,			// vt100_seqp
			&character,				// characterp
			piece_length,			// piece_length
			false,	// echo
			true,	// handle_backspace
			true,	// handle_vt100_codes
			true,	// remove_vt100_codes_from_str
			NULL,	// terminating_vt100_codes
			0,		// terminating_vt100_codes_length
			_PSEUDOSHELL_INPUT_LOOP_EOF,		// eof_return_value
			false,	// is_whitelist
			_pseudoshell_standard_terminators,				// character_list
			_pseudoshell_standard_terminators_length,		// character_list_length
			false,	// append_terminating_character
			true	// echo_terminating_character
		) != _PSEUDOSHELL_INPUT_LOOP_CONTINUE))
			break;

	if(!tolerate_eof && (character_handle_result == _PSEUDOSHELL_INPUT_LOOP_EOF)) {
		free(_str);
		free(_vt100_esc);

		_pseudoshell_terminal_reset();
		
		return -1;
	}

	if(_str != NULL)
		_str = strtrimrealloc(_str, &str_allocated_length);
	else {
		_str = malloc(sizeof(char));
		_str[0] = '\0';
	}

	_pseudoshell_terminal_reset();

	*pass = _str;
	return str_length;
}

int pseudoshell_get_sepcific_hidden_character(char* passchar, char* prompt, char* valid_chars, bool repeat_until_valid, bool tolerate_eof) {
	fputs(prompt, stdout);

	if(_pseudoshell_terminal_set() != 0)
		return -1;

	unsigned int valid_char_count = strlen(valid_chars);
	bool valid_char = false;
	bool vt100_seq = false;
	char c;
	while((c = getchar()) != EOF) {
		if(c == '\033') {
			vt100_seq = true;
			continue;
		}
		if(vt100_seq) {
			if(strchr(FORMAT_AZaz09, c) != NULL)
				vt100_seq = false;
			continue;
		}

		for(unsigned int it = 0; it < valid_char_count; it++)
			if(c == valid_chars[it]) {
				valid_char = true;
				break;
			}
		if(valid_char || !repeat_until_valid)
			break;
	}

	_pseudoshell_terminal_reset();

	if(!tolerate_eof && (c == EOF))
		return -1;

	if(c == '\n' || c == EOF || c == '\r')
		return 0;
	else {
		*passchar = c;
		return 1;
	}
}

unsigned int _pseudoshell_get_command(char** str, char** vt100_esc, int* terminal_set_resultp, int* terminal_reset_resultp, bool* eof_encounteredp, unsigned int piece_length) {
	*eof_encounteredp = false;
	
	unsigned int characters_read = 0;
	char* _str = NULL;
	char* _vt100_esc = NULL;
	unsigned int str_allocated_length = 0;
	unsigned int str_length = 0;
	char character;
	bool vt100_sequence = false;
	unsigned int vt100_pos;

	*terminal_set_resultp = _pseudoshell_terminal_set();
	if(*terminal_set_resultp != _PSEUDOSHELL_TERMINAL_SET_RESET_OK)
		return 0;

	int character_handle_result;
	while(true) {
		character_handle_result = _pseudoshell_handle_character_input(
			&characters_read,		// characters_readp
			&_str,					// strp
			&str_allocated_length,	// str_allocated_lengthp
			&str_length,			// str_lengthp
			&vt100_sequence,		// vt100_sequence_indicatorp
			&vt100_pos,				// vt100_posp
			&_vt100_esc,			// vt100_seqp
			&character,				// characterp
			piece_length,			// piece_length
			true,	// echo
			true,	// handle_backspace
			true,	// handle_vt100_codes
			true,	// remove_vt100_codes_from_str
			_pseudoshell_up_down_arrows,		// terminating_vt100_codes
			_pseudoshell_up_down_arrows_length,	// terminating_vt100_codes_length
			_PSEUDOSHELL_INPUT_LOOP_EOF,		// eof_return_value
			false,	// is_whitelist
			_pseudoshell_standard_terminators,				// character_list
			_pseudoshell_standard_terminators_length,		// character_list_length
			false,	// append_terminating_character
			true	// echo_terminating_character
		);

		if(character_handle_result == _PSEUDOSHELL_INPUT_LOOP_EXIT)
			break;

		if(character_handle_result == _PSEUDOSHELL_INPUT_LOOP_EOF) {
			*eof_encounteredp = true;
			break;
		}
	}

	bool str_null = false;
	if(_str != NULL)
		_str = strtrimrealloc(_str, &str_allocated_length);
	else {
		_str = malloc(sizeof(char));
		_str[0] = '\0';
		str_null = true;
	}

	*terminal_reset_resultp = _pseudoshell_terminal_reset();

	*str = _str;
	*vt100_esc = _vt100_esc;

	return characters_read;
}

/*** PROMPTING (NOT THE LLM KIND) FUNCTIONS ***/

int pseudoshell_present_prompt(char* prompt, char* options_LOWERCASE, bool repeat_until_valid) {
	char* str;
	int res = -2;

	while(res < 0) {
		fputs(prompt, stdout);
		printf(" (");

		unsigned int options_length = strlen(options_LOWERCASE);
		for(unsigned int it = 0; it < options_length; it++) {
			if(it == 0)
				printf("%c", options_LOWERCASE[it] + 'A'-'a');
			else
				printf("%c", options_LOWERCASE[it]);
			if(it+1 != options_length)
				printf("/");
		}

		printf(") ");

		if(pseudoshell_get_string(&str, PSEUDOSHELL_BUFFER_SIZE) <= 0)
			return -1;
			
		for(unsigned int it = 0; it < options_length; it++)
			if((options_LOWERCASE[it] == str[0]) || (str[0] == (options_LOWERCASE[it]-('a'-'A')) && (str[0] >= 'A' && str[0] <= 'Z')))
				res = it;

		if(!repeat_until_valid)
			break;
	}
	
	return res;
}

bool pseudoshell_present_yesno_prompt(char* prompt, bool repeat_until_valid) {
	return pseudoshell_present_prompt(prompt, "yn", repeat_until_valid) == 0 ? true : false;
}

/*** COMMAND-RELATED FUNCTIONS ***/

int _pseudoshell_handle_command_input(char** str, char** vt100_esc, unsigned int piece_length) {
	unsigned int characters_read;
	int terminal_set_result;
	int terminal_reset_result;
	bool eof_encountered;

	characters_read = _pseudoshell_get_command(str, vt100_esc, &terminal_set_result, &terminal_reset_result, &eof_encountered, piece_length);

	if(eof_encountered)
		return _PSEUDOSHELL_HANDLE_COMMAND_INPUT_EOF;

	if(terminal_set_result != _PSEUDOSHELL_TERMINAL_SET_RESET_OK) {
		printf("%s\n", _PSEUDOSHELL_HANDLE_COMMAND_INPUT_TERMINAL_SET_FAIL_MESSAGE);
		return _PSEUDOSHELL_HANDLE_COMMAND_INPUT_TERMINAL_SET_FAIL;
	}

	if(terminal_reset_result != _PSEUDOSHELL_TERMINAL_SET_RESET_OK)
		printf("%s\n", _PSEUDOSHELL_HANDLE_COMMAND_INPUT_TERMINAL_RESET_FAIL_MESSAGE);

	return characters_read;
}

void _pseudoshell_parse_command(char* str, char** command, int* argc, char*** argv) {
	char* strp = str;
	char* whitespace; 
	char* next_whitespace;

	int _argc = 1;
	for(unsigned long it = 0; it < strlen(str); it++)
		if(str[it] == ' ')
			_argc++;
	char** _argv = malloc(sizeof(char*)*_argc);
	
	whitespace = strchr(strp, ' ');
	if(whitespace == NULL)
		whitespace = strp + strlen(strp);
	int len = whitespace-strp;
	_argv[0] = malloc(sizeof(char)*(len+1));
	memcpy(_argv[0], strp, sizeof(char)*len);
	_argv[0][len] = '\0';
	
	int it = 1;
	while((whitespace = strchr(strp, ' ')) != NULL) {
		next_whitespace = strchr(whitespace+1, ' ');
		if(next_whitespace == NULL)
			next_whitespace = whitespace + strlen(whitespace);
		len = next_whitespace-whitespace-1;
		_argv[it] = malloc(sizeof(char)*(len+1));
		memcpy(_argv[it], whitespace+1, len);
		_argv[it][len] = '\0';
		strp = whitespace+1;
		it++;
	}

	*command = strcpymalloc(_argv[0]);
	*argc = _argc;
	*argv = _argv;
}

void _pseudoshell_remove_empty_arguments(int* argc, char*** argv) {
	int empty_count = 0;
	char** argvp = *argv;

	for(int it = 0; it < *argc; it++)
		if(argvp[it][0] == '\0')
			empty_count++;

	int _argc = *argc - empty_count;
	char** _argv = malloc(sizeof(char*)*_argc);

	int pos = 0;
	for(int it = 0; it < *argc; it++)
		if(argvp[it][0] == '\0') {
			free(argvp[it]);
			continue;
		} else {
			_argv[pos] = (*argv)[it];
			pos++;
		}

	free(argvp);
	*argv = _argv;
	*argc = _argc;
}

void _pseudoshell_print_error_for_result(int result) {
	char* error_string = get_facility_error_message(result);
	if(error_string != NULL)
		printf("%s\n", error_string);
	else
		printf("Unknown error occured (%d).\n", result);
}

void _pseudoshell_check_result_print_error(int result) {
	if(result != FACILITIES_OK)
		_pseudoshell_print_error_for_result(result);
}

void _pseudoshell_check_result_print_error_or_success_message(int result, char* success_msg) {
	if(result != FACILITIES_OK)
		_pseudoshell_print_error_for_result(result);
	else
		fputs(success_msg, stdout);
}

char* _pseudoshell_get_everything_after_command(char* command_str, char* command) {
	char* strp = command_str+strlen(command);
	while(*strp == ' ')
		strp++;
	return strp;
}

char* _pseudoshell_get_specific_success_message(char* start, char* specific, char* end) {
	char* success_msg = NULL;
	unsigned int success_msg_allocated_length = 0;
	success_msg = strappendrealloc(success_msg, &success_msg_allocated_length, PSEUDOSHELL_BUFFER_SIZE, start);
	success_msg = strappendrealloc(success_msg, &success_msg_allocated_length, PSEUDOSHELL_BUFFER_SIZE, specific);
	success_msg = strappendrealloc(success_msg, &success_msg_allocated_length, PSEUDOSHELL_BUFFER_SIZE, end);
	success_msg = strtrimrealloc(success_msg, &success_msg_allocated_length);
	return success_msg;
}

void _pseudoshell_protest_command_requires_agument(char* command) {
	printf("'%s' command requires an argument.\n", command);
}

int _pseudoshell_execute_command(char* command_str, char** commandp, int* argcp, char*** argvp) {
	if(strcmp(*commandp, "exit") == 0 || strcmp(*commandp, "quit") == 0)
		return facility_exit() == FACILITIES_OK ? PSEUDOSHELL_OK : PSEUDOSHELL_CONTINUE;
	else if(strcmp(*commandp, "store") == 0)
		_pseudoshell_check_result_print_error(facility_switch_mode(FACILITIES_MODE_STORE_MANIPULATION));
	else if(strcmp(*commandp, "password") == 0)
		_pseudoshell_check_result_print_error(facility_switch_mode(FACILITIES_MODE_PASSWORD_MANIPULATION));
	else if(strcmp(*commandp, "memorize") == 0)
		_pseudoshell_check_result_print_error(facility_switch_mode(FACILITIES_MODE_MEMORIZATION));
	else if(strcmp(*commandp, "transfer") == 0)
		_pseudoshell_check_result_print_error(facility_switch_mode(FACILITIES_MODE_TRANSFER));
	else if(strcmp(*commandp, "init") == 0)
		_pseudoshell_check_result_print_error(facility_init());
	else if(strcmp(*commandp, "set") == 0) {
		_pseudoshell_remove_empty_arguments(argcp, argvp);

		if(*argcp != 3) {
			_pseudoshell_protest_command_requires_agument(*commandp);
			return PSEUDOSHELL_OK;
		}

		int res = facility_set((*argvp)[1]);
		_pseudoshell_check_result_print_error(res);

		if(res != FACILITIES_OK)
			return PSEUDOSHELL_OK;

		_pseudoshell_check_result_print_error(facility_to((*argvp)[2]));
	}
	else if(strcmp(*commandp, "get") == 0) {
		_pseudoshell_remove_empty_arguments(argcp, argvp);

		if(*argcp < 0) {
			_pseudoshell_protest_command_requires_agument(*commandp);
			return PSEUDOSHELL_OK;
		}

		_pseudoshell_check_result_print_error(facility_get((*argvp)[1]));
	}
	else if(strcmp(*commandp, "load") == 0)
		_pseudoshell_check_result_print_error(facility_load(_pseudoshell_get_everything_after_command(command_str, *commandp)));
	else if(strcmp(*commandp, "save") == 0)
		_pseudoshell_check_result_print_error(facility_save());
	else if(strcmp(*commandp, "save-as") == 0)
		_pseudoshell_check_result_print_error(facility_save_as(_pseudoshell_get_everything_after_command(command_str, *commandp)));
	else if(strcmp(*commandp, "unlock") == 0)
		_pseudoshell_check_result_print_error(facility_unlock());
	else if(strcmp(*commandp, "lock") == 0)
		_pseudoshell_check_result_print_error(facility_lock());
	else if(strcmp(*commandp, "close") == 0)
		_pseudoshell_check_result_print_error(facility_close());
	else if(strcmp(*commandp, "fetch") == 0) {
		char* read_str;
		char* str = _pseudoshell_get_everything_after_command(command_str, *commandp);

		unsigned long id = strtoul(str, &read_str, 10);
		if(id == 0 && (read_str-str) != strlen(str)) {
			printf("Couldn't parse the password id.\n");
			return PSEUDOSHELL_OK;
		}

		_pseudoshell_check_result_print_error(facility_fetch(id));
	}
	else if(strcmp(*commandp, "find") == 0)
		_pseudoshell_check_result_print_error(facility_find(_pseudoshell_get_everything_after_command(command_str, *commandp)));
	else if(strcmp(*commandp, "display") == 0) {
		_pseudoshell_remove_empty_arguments(argcp, argvp);

		unsigned long start, count;

		if(*argcp < 2) {
			start = 0;
			count = 0;
		} else {
			char* read_str;

			start = strtoul((*argvp)[1], &read_str, 10);
			if(start == 0 && (read_str-(*argvp)[1]) != strlen((*argvp)[1])) {
				printf("Couldn't parse start id.\n");
				return PSEUDOSHELL_OK;
			}

			if(*argcp == 2)
				count = 0;
			else {
				count = strtoul((*argvp)[2], &read_str, 10);
				if(count == 0 && (read_str-(*argvp)[2]) != strlen((*argvp)[2])) {
					printf("Couldn't parse count.\n");
					return PSEUDOSHELL_OK;
				}
			}
		}

		_pseudoshell_check_result_print_error(facility_display(start, count));
	}
	else if(strcmp(*commandp, "peek") == 0) {
		_pseudoshell_remove_empty_arguments(argcp, argvp);

		unsigned long start, count;

		if(*argcp < 2) {
			start = 0;
			count = 0;
		} else {
			char* read_str;

			start = strtoul((*argvp)[1], &read_str, 10);
			if(start == 0 && (read_str-(*argvp)[1]) != strlen((*argvp)[1])) {
				printf("Couldn't parse start id.\n");
				return PSEUDOSHELL_OK;
			}

			if(*argcp == 2)
				count = 0;
			else {
				count = strtoul((*argvp)[2], &read_str, 10);
				if(count == 0 && (read_str-(*argvp)[2]) != strlen((*argvp)[2])) {
					printf("Couldn't parse count.\n");
					return PSEUDOSHELL_OK;
				}
			}
		}

		_pseudoshell_check_result_print_error(facility_peek(start, count, false, NULL, NULL));
	}
	else if(strcmp(*commandp, "generate") == 0)
		_pseudoshell_check_result_print_error(facility_generate(_pseudoshell_get_everything_after_command(command_str, *commandp)));
	else if(strcmp(*commandp, "remove") == 0) {
		char* read_str;
		char* str = _pseudoshell_get_everything_after_command(command_str, *commandp);

		unsigned long id = strtoul(str, &read_str, 10);
		if(id == 0 && (read_str-str) != strlen(str)) {
			printf("Couldn't parse the password id.\n");
			return PSEUDOSHELL_OK;
		}

		if(!pseudoshell_present_yesno_prompt("Are you sure? Removed passwords cannot be restored.", true))
			return PSEUDOSHELL_OK;

		_pseudoshell_check_result_print_error(facility_remove(id));
	}
	else
		printf("Unknown command '%s'.\nUse 'help' to view the list of available commands.\n", *commandp);

	return PSEUDOSHELL_OK;
}

void _pseudoshell_free_and_reset_command_variables(char** command_str, char** command, int* argc, char*** argv) {
	free(*command_str);
	free(*command);
	for(int it = 0; it < *argc; it++)
		free(*((*argv)+it));
	free(*argv);
	*command_str = NULL;
	*command = NULL;
	*argc = 0;
	*argv = NULL;
}

int enter_pseudoshell_loop() {
	char* command_str = NULL;
	char* vt100_esc = NULL;
	char* command = NULL;
	int argc = 0;
	char** argv = NULL;
	int ret = PSEUDOSHELL_OK;

	while(true) {
		_pseudoshell_free_and_reset_command_variables(&command_str, &command, &argc, &argv);
		fputs(PSEUDOSHELL_LOOP_PROMPT_START, stdout);
		fputs(mode_short_names[mode], stdout);
		fputs(PSEUDOSHELL_LOOP_PROMPT_END, stdout);
		if(_pseudoshell_handle_command_input(&command_str, &vt100_esc, PSEUDOSHELL_BUFFER_SIZE) < 0)
			break;

		if(strlen(command_str) == 0 && vt100_esc == NULL)
			continue;

		if(vt100_esc != NULL) {
			printf("\33[2K\r");

			if(strcmp(vt100_esc+1, "[A") == 0) {
				// TODO Command history
				continue;
			} else if(strcmp(vt100_esc+1, "[B") == 0) {
				// TODO Command history
				continue;
			}

			free(vt100_esc);
		}

		_pseudoshell_parse_command(command_str, &command, &argc, &argv);
		ret = _pseudoshell_execute_command(command_str, &command, &argc, &argv);

		if(ret == PSEUDOSHELL_CONTINUE)
			continue;

		if(ret != PSEUDOSHELL_OK) {
			_pseudoshell_free_and_reset_command_variables(&command_str, &command, &argc, &argv);
			return ret;
		}

		if(strcmp(command, "exit") == 0 || strcmp(command, "quit") == 0) {
			_pseudoshell_free_and_reset_command_variables(&command_str, &command, &argc, &argv);
			break;
		}
	}

	return PSEUDOSHELL_OK;
}