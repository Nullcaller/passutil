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
#include "facilities.c"
#include "pseudoshell.c"

int main(int argc, char* argv[]) {
	/* OPTION PARSING */

	int option, option_index;

	static struct option long_options[] = {
    	{ "debug",			no_argument,	&debug,			true	},
		{ "interactive",	no_argument,	&interactive,	true	},
		{ "quiet",			no_argument,	&quiet,			true	},
		{ "yes",			no_argument,	&yes,			true	},
		{ "no",				no_argument,	&yes,			false	},
    	{ 0, 0, 0, 0 }
    };

	while(true) {
		option_index = 0;

		option = getopt_long(argc, argv, "diqyn", long_options, &option_index);

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
			case 'q':
				quiet = true;
				break;
			case 'y':
				yes = true;
				break;
			case 'n':
				yes = false;
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

	enter_pseudoshell_loop();

	// TODO Deny any more arguments after "--interactive" for security reasons, add a `manual override` preprocessor define

	facility_init();
	facility_set("algorithm");
	facility_to("AES256");
	//char* shuffle_key;
	//char* shuffle_key_format = FORMAT_AZaz09;
	//generate_shuffle_key(&shuffle_key, shuffle_key_format);
	//char* shuffled_key = shuffle("HelloWorld", shuffle_key, shuffle_key_format);
	//store_copy_and_insert_key(loaded_store, shuffled_key, shuffle_key, shuffle_key_format);
	//free(shuffle_key);
	//free(shuffled_key);

	//Password* password = generate_password_and_append(loaded_store, "Some_Service", FORMAT_AZaz09, 10);
	facility_unlock();
	facility_switch_mode(FACILITIES_MODE_PASSWORD_MANIPULATION);
	facility_set("format");
	facility_to("FORMAT_AZaz09_64");
	facility_set("length");
	facility_to("10");
	facility_generate("Some_Service");
	facility_switch_mode(FACILITIES_MODE_STORE_MANIPULATION);
	
	//Password* password = loaded_store->passwords[0];
	//char* plain_password_1 = password_read_plain(password);
	//printf("%s\n", plain_password_1);

	facility_switch_mode(FACILITIES_MODE_PASSWORD_MANIPULATION);
	facility_fetch(0);
	facility_switch_mode(FACILITIES_MODE_STORE_MANIPULATION);

	unsigned int sm_length;
	char* metadata = store_serialize_metadata(loaded_store, &sm_length);
	printf("%d:\n%s\n", sm_length, metadata);

	//length = password->encrypted_byte_length;
	//unsigned char* pseq = password->encrypted_password; 
	unsigned int ps_length;
	unsigned char* pseq = store_serialize_password_sequence(loaded_store, &ps_length);
	for(unsigned int it = 0; it < ps_length; it++) {
		if(it % 10 == 0)
			printf("%05d\t", it);
		printf("%02X\t", pseq[it]);
		if(it % 10 == 9)
			printf("\n");
	}

	printf("\n");

	int save_debug;
	if((save_debug = facility_save_as("test")) != FACILITIES_OK) {
		printf("WTF SAVE: %d\n", save_debug);
		return -1;
	}

	//FILE* metadata_file = fopen("test.psmdf", "w+");
	//FILE* master_file = fopen("test.psmf", "w+");

	//store_save(store, metadata_file, master_file);

	//fflush(metadata_file);
	//fflush(master_file);

	//fclose(metadata_file);
	//fclose(master_file);

	//metadata_file = fopen("test.psmdf", "r");
	//master_file = fopen("test.psmf", "r");

	//Store* store_new;
	//store_load(metadata_file, master_file, &store_new);

	facility_close();

	int load_debug;
	if((load_debug = facility_load("test")) != FACILITIES_OK) {
		printf("WTF LAOD: %d\n", load_debug);
		return 1;
	}

	Store* store_new = loaded_store;
	Password* password_new = loaded_store->passwords[0];

	facility_switch_mode(FACILITIES_MODE_PASSWORD_MANIPULATION);
	facility_find("Some_Service");
	facility_switch_mode(FACILITIES_MODE_STORE_MANIPULATION);

	//store_new->shuffle_key_format = FORMAT_AZaz09;
	//generate_shuffle_key(&store_new->shuffle_key, store_new->shuffle_key_format);
	//store_new->shuffled_key = shuffle("HelloWorld", store_new->shuffle_key, store_new->shuffle_key_format);

	facility_unlock();

	char* plain_password_2 = password_read_plain(password_new);
	printf("%s\n", plain_password_2);

	metadata = store_serialize_metadata(loaded_store, &sm_length);
	printf("%d:\n%s\n", sm_length, metadata);

	pseq = store_serialize_password_sequence(loaded_store, &ps_length);
	for(unsigned int it = 0; it < ps_length; it++) {
		if(it % 10 == 0)
			printf("%05d\t", it);
		printf("%02X\t", pseq[it]);
		if(it % 10 == 9)
			printf("\n");
	}

	char* shuffle_key_format__ = FORMAT_AZaz09_symb_sp;
	char* shuffle_key__;
	generate_shuffle_key(&shuffle_key__, shuffle_key_format__);

	char* shuffled_password = shuffle(plain_password_2, shuffle_key__, shuffle_key_format__);

	//memorize_direct(shuffled_password, shuffle_key__, shuffle_key_format__);
	memorize_by_symbols(shuffled_password, shuffle_key__, shuffle_key_format__, password_new->format, 2, true, false);

	return EXIT_CODE_SUCCESS;
}