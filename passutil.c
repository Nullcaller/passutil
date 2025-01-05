/* passutil
 * 
 * A utility to generate, securely store and memorize passwords.
 * For maximum security, should be used on a standalone machine that never connects to the internet.
 * The machine in question should feature full drive encryption to avoid tampering.
 * 
 * by Gleb Salmanov
 */

#include<stdio.h>
#include<stdbool.h>
#include<getopt.h>

#include "passutil.h"

#include "constants-exit-codes.h"
#include "constants-formats.h"

#include "util.c"
#include "shuffler.c"
#include "generation.c"
#include "storage.c"
#include "memorizer.c"

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

	Store* store = store_construct();
	store->algorithm = "AES256";
	store->shuffle_key_format = FORMAT_AZaz09;
	generate_shuffle_key(&store->shuffle_key, store->shuffle_key_format);
	store->shuffled_key = shuffle("HelloWorld", store->shuffle_key, store->shuffle_key_format);

	Password* password = generate_password_and_append(store, "Some Service", FORMAT_AZaz09, 10);
	char* plain_password_1 = password_read_plain(password);
	printf("%s\n", plain_password_1);

	/*unsigned int sm_length;
	char* metadata = store_serialize_metadata(store, &sm_length);
	printf("%d:\n%s\n", sm_length, metadata);

	//length = password->encrypted_byte_length;
	//unsigned char* pseq = password->encrypted_password; 
	unsigned int ps_length;
	unsigned char* pseq = store_serialize_password_sequence(store, &ps_length);
	for(unsigned int it = 0; it < ps_length; it++) {
		if(it % 10 == 0)
			printf("%05d\t", it);
		printf("%02X\t", pseq[it]);
		if(it % 10 == 9)
			printf("\n");
	}

	printf("\n");

	FILE* metadata_file = fopen("test.psmdf", "w+");
	FILE* master_file = fopen("test.psmf", "w+");

	store_save(store, metadata_file, master_file);

	fflush(metadata_file);
	fflush(master_file);

	fclose(metadata_file);
	fclose(master_file);

	metadata_file = fopen("test.psmdf", "r");
	master_file = fopen("test.psmf", "r");

	Store* store_new;
	store_load(metadata_file, master_file, &store_new);

	Password* password_new = store_find_password(store_new, "Some Service");

	store_new->shuffle_key_format = FORMAT_AZaz09;
	generate_shuffle_key(&store_new->shuffle_key, store_new->shuffle_key_format);
	store_new->shuffled_key = shuffle("HelloWorld", store_new->shuffle_key, store_new->shuffle_key_format);

	char* plain_password_2 = password_read_plain(password_new);
	printf("%s\n", plain_password_2);

	metadata = store_serialize_metadata(store, &sm_length);
	printf("%d:\n%s\n", sm_length, metadata);

	pseq = store_serialize_password_sequence(store, &ps_length);
	for(unsigned int it = 0; it < ps_length; it++) {
		if(it % 10 == 0)
			printf("%05d\t", it);
		printf("%02X\t", pseq[it]);
		if(it % 10 == 9)
			printf("\n");
	}*/

	char* shuffle_key_format__ = FORMAT_AZaz09_symb_sp;
	char* shuffle_key__;
	generate_shuffle_key(&shuffle_key__, shuffle_key_format__);

	char* shuffled_password = shuffle(plain_password_1, shuffle_key__, shuffle_key_format__);

	//memorize_direct(shuffled_password, shuffle_key__, shuffle_key_format__);
	memorize_by_symbols(shuffled_password, shuffle_key__, shuffle_key_format__, password->format, 2, true, false);

	return EXIT_CODE_SUCCESS;
}