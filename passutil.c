/* passutil
 * 
 * A utility to generate, securely store and memorize passwords.
 * For maximum security, should be used on a standalone machine that never connects to the internet.
 * The machine in question should feature full drive encryption to avoid tampering.
 * 
 * Copyright (c) 2025 Gleb Salmanov
 * All rights reserved.
 */

#include<stdio.h>
#include<stdbool.h>
#include<getopt.h>

#include "exit-codes.h"

static int debug = false;
static int interactive = false;

int main(int argc, char* argv[]) {
	int option, option_index;

	static struct option long_options[] = {
    	{ "debug",			no_argument,	&debug,			true },
		{ "interactive",	no_argument,	&interactive,	true },
    	{ 0, 0, 0, 0 }
    };

	while(true) {
		option_index = 0;

		option = getopt_long(argc, argv, "di", long_options, &option_index);

		if(debug && option != 0 && option != -1)
			printf("Parsing option: %c (%d)\n", option, option);

		if(option == -1)
			break;

		switch(option)
		{
			case 0:
				if(debug)
					printf("Parsing long option: %s\n", long_options[option_index].name);
				if(long_options[option_index].flag != NULL)
					break;
				
				break;
			case 'd':
				debug = true;
				break;
			case 'i':
				interactive = true;
				break;
			case '?':
				if(debug)
					printf("Argument error while parsing option: %d\n", option);
				return EXIT_CODE_ARGUMENT_ERROR;
			default:
				if(debug)
					printf("Unknown error while parsing option: %d\n", option);
				return EXIT_CODE_UNKNOWN_ERROR;
		}
	}

	return EXIT_CODE_SUCCESS;
}