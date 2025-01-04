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
#include "formats.h"

#include "util.c"
#include "shuffler.c"
#include "generation.c"
#include "storage.c"

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
	store->shuffle_key_format = FORMAT_AZaz09;
	generate_shuffle_key(&store->shuffle_key, store->shuffle_key_format);
	store->shuffled_key = shuffle("HelloWorld", store->shuffle_key, store->shuffle_key_format);

	Password* password = generate_password_and_append(store, "Some Service", FORMAT_AZaz09, 50);
	char* plain_password_1 = read_plain(password);

	Password* found_password = find(store, "Some Service");
	char* plain_password_2 = read_plain(found_password);

	printf("%s\n", plain_password_1);
	printf("%s\n", plain_password_2);

	unsigned int length;
	char* metadata = serialize_metadata(store, &length);
	printf("%d, %s\n", length, metadata);

	//length = password->encrypted_byte_length;
	//unsigned char* pseq = password->encrypted_password; 
	unsigned char* pseq = serialize_password_sequence(store, &length);
	for(unsigned int it = 0; it < length; it++) {
		if(it % 10 == 0)
			printf("%05d\t", it);
		printf("%02X\t", pseq[it]);
		if(it % 10 == 9)
			printf("\n");
	}

	printf("\n");

	return EXIT_CODE_SUCCESS;
}