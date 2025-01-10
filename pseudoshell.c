#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<termios.h>

#include "pseudoshell.h"

#include "util.h"
#include "facilities.h"

int pseudoshell_getpass(char** pass, char* prompt, unsigned int piece_length) {
	struct termios old, new;

	fputs(prompt, stdout);

	if(tcgetattr(fileno(stdin), &old) != 0)
		return -1;
	new = old;
	new.c_lflag &= ~(ICANON | ECHO);
	if(tcsetattr(fileno(stdin), TCSAFLUSH, &new) != 0)
    	return -1;

	char* _str = NULL;
	unsigned int str_allocated_length = 0;
	unsigned int str_length = 0;

	char character;
	while((character = getchar()) != '\n' && character != '\r' && character != EOF) {
		if(character == 127 || character == 8) {
			if(str_length != 0) {
				str_length--;
				_str[str_length] = '\0';
			}
		} else
			_str = strappendcharrealloc(_str, &str_allocated_length, &str_length, piece_length, character);
	}

	if(_str != NULL)
		_str = strtrimrealloc(_str, &str_allocated_length);
	else {
		_str = malloc(sizeof(char));
		_str[0] = '\0';
	}

	tcsetattr(fileno(stdin), TCSAFLUSH, &old);

	*pass = _str;
	return str_length;
}

int pseudoshell_getpasschar(char* passchar, char* prompt, char* valid_chars, bool repeat_until_valid) {
	struct termios old, new;
	char c;

	fputs(prompt, stdout);

	if(tcgetattr(fileno(stdin), &old) != 0)
		return -1;
	new = old;
	new.c_lflag &= ~(ICANON | ECHO);
	if(tcsetattr(fileno(stdin), TCSAFLUSH, &new) != 0)
    	return -1;

	unsigned int valid_char_count = strlen(valid_chars);
	bool valid_char = false;
	while((c = getchar()) != EOF) {
		for(unsigned int it = 0; it < valid_char_count; it++)
			if(c == valid_chars[it]) {
				valid_char = true;
				break;
			}
		if(valid_char || !repeat_until_valid)
			break;
	}

	tcsetattr(fileno(stdin), TCSAFLUSH, &old);

	if(c == '\n' || c == EOF || c == '\r')
		return 0;
	else {
		*passchar = c;
		return 1;
	}
}

int getstr(char** str, unsigned int piece_length) {
	char* _str = NULL;
	unsigned int str_allocated_length = 0;
	unsigned int str_length = 0;

	char character;
	while((character = getchar()) != '\n' && character != '\r' && character != EOF)
		_str = strappendcharrealloc(_str, &str_allocated_length, &str_length, piece_length, character);

	if(_str != NULL)
		_str = strtrimrealloc(_str, &str_allocated_length);
	else {
		_str = malloc(sizeof(char));
		_str[0] = '\0';
		*str = _str;
		return 0;
	}

	*str = _str;
	return str_length+1;
}

int present_prompt(char* prompt, char* options_LOWERCASE, bool repeat_until_valid) {
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

		if(getstr(&str, PSEUDOSHELL_BUFFER_SIZE) <= 0)
			return -1;
			
		for(unsigned int it = 0; it < options_length; it++)
			if((options_LOWERCASE[it] == str[0]) || (str[0] == (options_LOWERCASE[it]-('a'-'A')) && (str[0] >= 'A' && str[0] <= 'Z')))
				res = it;

		if(!repeat_until_valid)
			break;
	}
	
	return res;
}

bool present_yesno_prompt(char* prompt, bool repeat_until_valid) {
	return !present_prompt(prompt, "yn", repeat_until_valid);
}

void parse_command(char* str, char** command, int* argc, char*** argv) {
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

void check_res_print_err_or_success_msg(int res, char* success_msg) {
	if(res != FACILITIES_OK) {
		char* err_str = get_facility_error_message(res);
		if(err_str != NULL)
			printf("%s\n", err_str);
		else
			printf("Unknown error occured (%d).\n", res);
	} else
		fputs(success_msg, stdout);
}

int find_first_non_empty_argument_after_command(char* command, int argc, char** argv) {
	if(argc <= 1)
		return -1;
	int first_non_empty_arg_after_cmd = -1;
	for(int it = 1; it < argc; it++)
		if(strlen(argv[it]) > 0) {
			first_non_empty_arg_after_cmd = it;
			break;
		}
	return first_non_empty_arg_after_cmd;
}

char* get_everything_after_command(char* command_str, char* command) {
	char* strp = command_str+strlen(command);
	while(*strp == ' ')
		strp++;
	return strp;
}

char* get_specific_success_message(char* start, char* specific, char* end) {
	char* success_msg = NULL;
	unsigned int success_msg_allocated_length = 0;
	success_msg = strappendrealloc(success_msg, &success_msg_allocated_length, PSEUDOSHELL_BUFFER_SIZE, start);
	success_msg = strappendrealloc(success_msg, &success_msg_allocated_length, PSEUDOSHELL_BUFFER_SIZE, specific);
	success_msg = strappendrealloc(success_msg, &success_msg_allocated_length, PSEUDOSHELL_BUFFER_SIZE, end);
	success_msg = strtrimrealloc(success_msg, &success_msg_allocated_length);
	return success_msg;
}

void protest_command_requires_agument(char* command) {
	printf("'%s' command requires an argument.\n", command);
}

int execute_command(char* command_str, char* command, int argc, char** argv) {
	int first_non_empty_argument_after_command;

	if(strcmp(command, "exit") == 0 || strcmp(command, "quit") == 0)
		return facility_exit() == FACILITIES_OK ? PSEUDOSHELL_OK : PSEUDOSHELL_CONTINUE;
	else if(strcmp(command, "store") == 0)
		check_res_print_err_or_success_msg(facility_switch_mode(FACILITIES_MODE_STORE_MANIPULATION), "");
	else if(strcmp(command, "password") == 0)
		check_res_print_err_or_success_msg(facility_switch_mode(FACILITIES_MODE_PASSWORD_MANIPULATION), "");
	else if(strcmp(command, "memorize") == 0)
		check_res_print_err_or_success_msg(facility_switch_mode(FACILITIES_MODE_MEMORIZATION), "");
	else if(strcmp(command, "transfer") == 0)
		check_res_print_err_or_success_msg(facility_switch_mode(FACILITIES_MODE_TRANSFER), "");
	else if(strcmp(command, "init") == 0)
		check_res_print_err_or_success_msg(facility_init(), "Store initialized.\n");
	else if(strcmp(command, "set") == 0) {
		first_non_empty_argument_after_command = find_first_non_empty_argument_after_command(command, argc, argv);
		if(first_non_empty_argument_after_command < 0) {
			protest_command_requires_agument(command);
			return PSEUDOSHELL_OK;
		}
		
		char* success_msg = get_specific_success_message("Set current field name to '", argv[first_non_empty_argument_after_command], "'.\n");
		check_res_print_err_or_success_msg(facility_set(argv[first_non_empty_argument_after_command]), success_msg);
		free(success_msg);
	}
	else if(strcmp(command, "to") == 0) {
		first_non_empty_argument_after_command = find_first_non_empty_argument_after_command(command, argc, argv);
		if(first_non_empty_argument_after_command < 0) {
			protest_command_requires_agument(command);
			return PSEUDOSHELL_OK;
		}

		char* success_msg = get_specific_success_message("Set field value to '", argv[first_non_empty_argument_after_command], "'.\n");
		check_res_print_err_or_success_msg(facility_to(argv[first_non_empty_argument_after_command]), success_msg);
		free(success_msg);
	}
	else if(strcmp(command, "get") == 0) {
		first_non_empty_argument_after_command = find_first_non_empty_argument_after_command(command, argc, argv);
		if(first_non_empty_argument_after_command < 0) {
			protest_command_requires_agument(command);
			return PSEUDOSHELL_OK;
		}

		check_res_print_err_or_success_msg(facility_get(argv[first_non_empty_argument_after_command]), "");
	}
	else if(strcmp(command, "load") == 0)
		check_res_print_err_or_success_msg(facility_load(get_everything_after_command(command_str, command)), "Store loaded.\n");
	else if(strcmp(command, "save") == 0)
		check_res_print_err_or_success_msg(facility_save(), "Store saved.\n");
	else if(strcmp(command, "save-as") == 0)
		check_res_print_err_or_success_msg(facility_save_as(get_everything_after_command(command_str, command)), "Store saved.\n");
	else if(strcmp(command, "unlock") == 0)
		check_res_print_err_or_success_msg(facility_unlock(), "Store unlocked.\n");
	else if(strcmp(command, "lock") == 0)
		check_res_print_err_or_success_msg(facility_lock(), "Store locked.\n");
	else if(strcmp(command, "close") == 0)
		check_res_print_err_or_success_msg(facility_close(), "Store closed.\n");
	else if(strcmp(command, "fetch") == 0) {
		char* read_str;
		char* str = get_everything_after_command(command_str, command);

		unsigned long id = strtoul(str, &read_str, 10);
		if(id == 0 && (read_str-str) != strlen(str)) {
			printf("Couldn't parse the password id.\n");
			return PSEUDOSHELL_OK;
		}

		check_res_print_err_or_success_msg(facility_fetch(id), "");
	}
	else if(strcmp(command, "find") == 0)
		check_res_print_err_or_success_msg(facility_find(get_everything_after_command(command_str, command)), "");
	else if(strcmp(command, "generate") == 0)
		check_res_print_err_or_success_msg(facility_generate(get_everything_after_command(command_str, command)), "Password generated.\n");
	else if(strcmp(command, "remove") == 0) {
		char* read_str;
		char* str = get_everything_after_command(command_str, command);

		unsigned long id = strtoul(str, &read_str, 10);
		if(id == 0 && (read_str-str) != strlen(str)) {
			printf("Couldn't parse the password id.\n");
			return PSEUDOSHELL_OK;
		}

		if(!present_yesno_prompt("Are you sure? Removed passwords cannot be restored.", true))
			return PSEUDOSHELL_OK;

		check_res_print_err_or_success_msg(facility_remove(id), "");
	}
	else
		printf("Unknown command '%s'.\nUse 'help' to view the list of available commands.\n", command);

	return PSEUDOSHELL_OK;
}

void free_and_reset_command_variables(char** command_str, char** command, int* argc, char*** argv) {
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
	char* command = NULL;
	int argc = 0;
	char** argv = NULL;
	int ret = PSEUDOSHELL_OK;

	while(true) {
		free_and_reset_command_variables(&command_str, &command, &argc, &argv);
		fputs(PSEUDOSHELL_LOOP_PROMPT_START, stdout);
		fputs(mode_short_names[mode], stdout);
		fputs(PSEUDOSHELL_LOOP_PROMPT_END, stdout);
		getstr(&command_str, PSEUDOSHELL_BUFFER_SIZE);

		parse_command(command_str, &command, &argc, &argv);
		ret = execute_command(command_str, command, argc, argv);

		if(ret == PSEUDOSHELL_CONTINUE)
			continue;

		if(ret != PSEUDOSHELL_OK) {
			free_and_reset_command_variables(&command_str, &command, &argc, &argv);
			return ret;
		}

		if(strcmp(command, "exit") == 0 || strcmp(command, "quit") == 0) {
			free_and_reset_command_variables(&command_str, &command, &argc, &argv);
			break;
		}
	}

	return PSEUDOSHELL_OK;
}