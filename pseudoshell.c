#include<stdio.h>
#include<stdbool.h>

#include "pseudoshell.h"

#include "util.h"

int _getstr(char** str, unsigned int piece_length) {
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

int _present_yesno_prompt(char* prompt) {
	printf(prompt);
	printf(" (Y/n) ");

	char* str;

	if(_getstr(&str, PSEUDOSHELL_BUFFER_SIZE) <= 0)
		return -1;
		
	if(str[0] == 'Y' || str[0] == 'y')
		return 1;
	else if(str[0] == 'N' || str[0] == 'n')
		return 0;
	else
		return -2;
}

bool present_yesno_prompt(char* prompt) {
	int result = -1;
	while(result < 0)
		result = _present_yesno_prompt(prompt);
	return result;
}