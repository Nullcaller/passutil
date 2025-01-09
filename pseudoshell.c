#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include<termios.h>

#include "pseudoshell.h"

#include "util.h"

int pseudoshell_getpass(char** pass, char* prompt, unsigned int piece_length) {
	struct termios old, new;
	int nread;

	fputs(prompt, stdout);

	if(tcgetattr(fileno(stdin), &old) != 0)
		return -1;
	new = old;
	new.c_lflag &= ~(ICANON | ECHO);
	if(tcsetattr(fileno(stdin), TCSAFLUSH, &new) != 0)
    	return -1;

	nread = getstr(pass, piece_length);

	tcsetattr(fileno(stdin), TCSAFLUSH, &old);

	return nread;
}

int pseudoshell_getpasschar(char* passchar, char* prompt) {
	struct termios old, new;
	char c;

	fputs(prompt, stdout);

	if(tcgetattr(fileno(stdin), &old) != 0)
		return -1;
	new = old;
	new.c_lflag &= ~(ICANON | ECHO);
	if(tcsetattr(fileno(stdin), TCSAFLUSH, &new) != 0)
    	return -1;

	c = getchar();

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