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

#include "constants/exit-codes.h"
#include "constants/formats.h"

#include "util/string.c"
#include "util/io.c"
#include "util/crypt.c"
#include "util/shuffle.c"
#include "util/generation.c"
#include "util/draw.c"
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

	if(interactive) {
		int ret;
		if((ret = enter_pseudoshell_loop()) == PSEUDOSHELL_OK)
			return EXIT_CODE_SUCCESS;
		return ret;
	}

	// TODO Deny any more arguments after "--interactive" for security reasons, add a `manual override` preprocessor define

	unsigned short line = 0, column = 0;
	char character;

	unsigned short input_char_num = 8, separator_char_num = 2, checksum_char_num = 3, blocks_per_line = 5, total_block_num = 27, target_line_width = 120;

	fputs(drawer_input_page_draw_empty(&line, &column, input_char_num, separator_char_num, checksum_char_num, blocks_per_line, total_block_num, target_line_width), stdout);
	for(int inp = 0; inp < total_block_num*(input_char_num+checksum_char_num); inp++) {
		fputs(drawer_input_page_continue_input_at_index(&line, &column, inp, input_char_num, separator_char_num, checksum_char_num, blocks_per_line, total_block_num, target_line_width), stdout);
		pseudoshell_get_specific_hidden_character(&character, "", "1234567890ABCDEF", true, false);
		fputs(drawer_input_page_draw_input_at_index(&line, &column, character, inp, input_char_num, separator_char_num, checksum_char_num, blocks_per_line, total_block_num, target_line_width), stdout);
	}
	putc('\n', stdout);

	return EXIT_CODE_SUCCESS;
}