#include<sodium.h>
#include<stdbool.h>
#include<unistd.h>
#include<string.h>

#include "memorizer.h"

#include "shuffler.h"

// TODO getpass implementation with ^C termination and exit

int memorize_direct(char* shuffled_password, char* shuffle_key, char* shuffle_key_format) {
	char* entered_password = getpass(MEMORIZER_DIRECT_PROMPT);
	if(entered_password == NULL)
		return MEMORIZER_FAILURE;

	char* shuffled_entered_passdword = shuffle(entered_password, shuffle_key, shuffle_key_format);

	int ret;
	if(strcmp(shuffled_password, shuffled_entered_passdword) == 0) {
		printf("\t\t\t\t\tSUCCESS\n");
		ret = MEMORIZER_SUCCESS;
	} else {
		printf("\t\t\t\t\t\tFAILURE\n");
		ret = MEMORIZER_FAILURE;
	}

	free(shuffled_entered_passdword);
	return ret;
}

int memorize_nth_symbol(char* shuffled_password, char* shuffle_key, char* shuffle_key_format, char* possible_characters, unsigned short symbol_position, unsigned short choice_count) {
	char character = unshuffle_char(shuffled_password[symbol_position], shuffle_key, shuffle_key_format);
	unsigned short position = randombytes_uniform(choice_count);
	unsigned short possible_character_count = strlen(possible_characters)-1;
	char* possible_characters_actual = malloc(sizeof(char)*possible_character_count);
	unsigned short character_pos_in_possible_characters = strchr(possible_characters, character)-possible_characters;
	memcpy(possible_characters_actual, possible_characters, character_pos_in_possible_characters);
	memcpy(possible_characters_actual+character_pos_in_possible_characters, possible_characters+character_pos_in_possible_characters+1, possible_character_count-character_pos_in_possible_characters);

	char* characters = malloc(sizeof(char)*choice_count);
	characters[position] = character;

	unsigned short pick;
	for(unsigned short it = 0; it < choice_count; it++) {
		if(it == position)
			continue;
		pick = randombytes_uniform(possible_character_count);
		characters[it] = possible_characters_actual[pick];
		strncpy(possible_characters_actual+pick, possible_characters_actual+(pick+1), possible_character_count-pick);
		possible_character_count--;
	}
	free(possible_characters_actual);

	for(unsigned short choice = 0; choice < choice_count; choice++)
		printf("%hd) %c    ", choice+1, characters[choice]);
	printf("\n");

	free(characters);

	char* selection_string = getpass(MEMORIZER_CHOICE_PROMPT);

	if(selection_string == NULL) {
		return MEMORIZER_ERROR;
	}

	char* read_str;
	unsigned short selection = strtoul(selection_string, &read_str, 10);
	if(selection == 0 && strlen(selection_string) != (read_str-selection_string)) {
		return MEMORIZER_ERROR;
	}

	//printf("SELECTION: %hd\n", selection);

	if(position+1 == selection) {
		printf("\t\t\t\t\tSUCCESS\n");
		return MEMORIZER_SUCCESS;
	} else {
		printf("\t\t\t\t\t\tFAILURE\n");
		return MEMORIZER_FAILURE;
	}
}

int memorize_by_symbols(char* shuffled_password, char* shuffle_key, char* shuffle_key_format, char* possible_characters, unsigned short choice_count, bool repeat_on_failure, bool exit_on_failure) {
	unsigned short length = strlen(shuffled_password);

	int ret;
	for(unsigned short it = 0; it < length; it++) {
		ret = MEMORIZER_FAILURE;
		while(ret == MEMORIZER_FAILURE) {
			printf("SELECT THE %d", it+1);
			switch(it+1) {
				case 1:
					printf("st ");
					break;
				case 2:
					printf("nd ");
					break;
				case 3:
					printf("rd ");
					break;
				default:
					printf("th ");
					break;
			}
			printf("SYMBOL:\n");
			ret = memorize_nth_symbol(shuffled_password, shuffle_key, shuffle_key_format, possible_characters, it, choice_count);
			if(exit_on_failure && ret == MEMORIZER_FAILURE)
				return ret;
			if(ret == MEMORIZER_ERROR)
				return ret;
			if(!repeat_on_failure)
				break;
		}
	}

	return MEMORIZER_SUCCESS;
}
