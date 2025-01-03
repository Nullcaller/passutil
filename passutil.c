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

#include "generation.c"
#include "util.c"

static int debug = false;
static int interactive = false;

int main(int argc, char* argv[]) {
	/* OPTION PARSING */

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

	printf("THING\n");

	char* thing = "TEST";
	char* key = malloc(strlen(thing)+1);
	strcpy(key, thing);
	fflush(stdout);
	printf("%s\n", key);
	char* shuffle_key;
	char* shuffle_key_format = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
	generate_shuffle_key(&shuffle_key, shuffle_key_format);
	printf("Key: %s\n", shuffle_key);
	char* shuffled_key = shuffle(key, shuffle_key, shuffle_key_format);
	char* text = "Hello world!";
	printf("%s\n", text);
	char* encrypted_text = encrypt("AES256", text, strlen(text)+1, shuffled_key, shuffle_key, shuffle_key_format);
	char* decrypted_text = decrypt("AES256", encrypted_text, strlen(text)+1, shuffled_key, shuffle_key, shuffle_key_format);


	printf("%s\n", encrypted_text);
	printf("%s\n", decrypted_text);

	return EXIT_CODE_SUCCESS;
}