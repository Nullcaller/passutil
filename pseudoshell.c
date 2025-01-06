#include<stdio.h>
#include<stdbool.h>

#include "pseudoshell.h"

int _present_yesno_prompt(char* prompt) {
	printf(prompt);
	printf(" (Y/n) ");

	char* buf = malloc(sizeof(char)*PSEUDOSHELL_BUFFER_SIZE);
	char* scanfstr = malloc(sizeof(char)*PSEUDOSHELL_BUFFER_SIZE);

	sprintf(scanfstr, "%%%d[^\\n]", PSEUDOSHELL_BUFFER_SIZE-1);
	if(scanf(scanfstr, buf) <= 0) {
		free(buf);
		free(scanfstr);
		return -1;
	}
	free(scanfstr);

	if(buf[0] == 'Y' || buf[0] == 'y') {
		free(buf);
		return 1;
	} else if(buf[0] == 'N' || buf[0] == 'n') {
		free(buf);
		return 0;
	} else {
		free(buf);
		return -2;
	}
}

bool present_yesno_prompt(char* prompt) {
	int result = -1;
	while(result < 0)
		_present_yesno_prompt(prompt);
	return result;
}