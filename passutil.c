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

#include "storage.c"
#include "generation.c"

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

	Store* store = construct_store();
	store->algorithm = "AES256";
	store->shuffle_key_format = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
	generate_shuffle_key(&store->shuffle_key, store->shuffle_key_format);
	store->shuffled_key = shuffle("HelloWorld", store->shuffle_key, store->shuffle_key_format);

	Password* password = construct_password();
	unsigned int password_length = 50;
	unsigned int password_length_bytes = password_length*(sizeof(unsigned long)/sizeof(char));
	unsigned char* password_bytes = generate_password_bytes(password_length_bytes);
	//unsigned int password_length = 4;
	//unsigned char password_bytes[4] = { 0x0, 0x1, 0x2, 0x3 };
	write(store, password, password_bytes, password_length_bytes, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", password_length);
	append_password(store, password, "Some Service");
	char* plain_password_1 = read_plain(password);

	Password* found_password = find(store, "Some Service");
	char* plain_password_2 = read_plain(found_password);

	printf("%s\n", plain_password_1);
	printf("%s\n", plain_password_2);

	return EXIT_CODE_SUCCESS;
}