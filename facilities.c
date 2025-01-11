#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include<unistd.h>

#ifdef WIN32
#include<io.h>
#define F_OK 0
#define access _access
#endif

#include "facilities.h"

#include "constants-formats.h"

#include "passutil.h"
#include "pseudoshell.h"
#include "util.h"
#include "storage.h"
#include "generation.h"

char* get_facility_error_message(int error) {
	switch(error) {
		case FACILITIES_OK:
			return NULL;
		default:
			return NULL;
		// TODO error messages
	}
}

int facility_switch_mode(unsigned short new_mode) {
	unsigned short allowed_mode_count = 4;
	unsigned short allowed_modes[] = {
		FACILITIES_MODE_STORE_MANIPULATION,
		FACILITIES_MODE_PASSWORD_MANIPULATION,
		FACILITIES_MODE_MEMORIZATION,
		FACILITIES_MODE_TRANSFER
	};

	bool mode_allowed = false;
	for(unsigned short it = 0; it < allowed_mode_count; it++)
		if(new_mode == allowed_modes[it]) {
			mode_allowed = true;
			break;
		}

	if(!mode_allowed)
		return FACILITIES_SWITCH_MODE_UNKNOWN_MODE;

	mode = new_mode;
	return FACILITIES_OK;
}

bool is_facility_field_name_allowed(char* field_name_to_validate) {
	unsigned short mode_store_manipulation_allowed_field_name_count = 3;
	char* mode_store_manipulation_allowed_field_names[] = {
		"algorithm",
		"key_verification_algorithm",
		"key_verification_algorithm_rounds"
	};
	unsigned short mode_password_manipulation_allowed_field_name_count = 2;
	char* mode_password_manipulation_allowed_field_names[] = {
		"format",
		"length"
	};
	unsigned short mode_memorization_allowed_field_name_count = 0;
	char* mode_memorization_allowed_field_names[] = {};
	unsigned short mode_transfer_allowed_field_name_count = 0;
	char* mode_transfer_allowed_field_names[] = {};

	unsigned short allowed_field_name_counts[] = {
		mode_store_manipulation_allowed_field_name_count,
		mode_password_manipulation_allowed_field_name_count,
		mode_memorization_allowed_field_name_count,
		mode_transfer_allowed_field_name_count
	};
	char** allowed_field_names[] = {
		mode_store_manipulation_allowed_field_names,
		mode_password_manipulation_allowed_field_names,
		mode_memorization_allowed_field_names,
		mode_transfer_allowed_field_names
	};

	bool field_name_allowed = false;
	for(unsigned short it = 0; it < allowed_field_name_counts[mode]; it++)
		if(strcmp(allowed_field_names[mode][it], field_name_to_validate) == 0) {
			field_name_allowed = true;
			break;
		}
	
	return field_name_allowed;
}

int facility_set(char* new_field_name) {
	if(!is_facility_field_name_allowed(new_field_name))
		return FACILITIES_SET_WRONG_FIELD_NAME_FOR_MODE;

	free(field_name);
	field_name = strcpymalloc(new_field_name);

	return FACILITIES_OK;
}

int facility_get(char* field_name_to_get) {
	if(!is_facility_field_name_allowed(field_name_to_get))
		return FACILITIES_GET_WRONG_FIELD_NAME_FOR_MODE;

	switch(mode) {
		case FACILITIES_MODE_STORE_MANIPULATION:
			if(!FACILITIES_STORE_LOADED)
				return FACILITIES_GET_STORE_MANIPULATION_STORE_NOT_LOADED;

			if(strcmp(field_name_to_get, "algorithm") == 0) {
				if(loaded_store->algorithm == NULL) {
					if(!quiet) {
						printf("Algorithm for the loaded store is NULL.\n");
						if(FACILITIES_STORE_INIT_COMPLETE)
							printf("Note: store IS marked as initialized.\nWarning: this state is invalid, and you should probably report this.\n");
						else
							printf("Note: store IS NOT marked as initialized.\n");
					} else
						printf("NULL\n");
				} else
					printf("%s\n", loaded_store->algorithm);
			} else if(strcmp(field_name_to_get, "key_verification_algorithm") == 0) {
				if(loaded_store->key_verifiable) {
					if(quiet) {
						if(loaded_store->key_verification_algorithm == NULL)
							printf("STATE INVALID: NULL\n");
						else
							printf("%s\n", loaded_store->key_verification_algorithm);
					} else {
						if(loaded_store->key_verification_algorithm == NULL) {
							printf("Key verification algorithm for the loaded store is NULL. But the store allows key verifications.\nWarning: this state is invalid, and you should probably report this.\n");
						} else
							printf("%s\n", loaded_store->key_verification_algorithm);
					}
				} else {
					if(quiet) {
						if(loaded_store->key_verification_algorithm == NULL)
							printf("NULL\n");
						else
							printf("STATE INVALID: %s\n", loaded_store->key_verification_algorithm);
					} else {
						if(loaded_store->key_verification_algorithm == NULL)
							printf("Store does not allow key verification.\nHence, key verification algorithm is NULL.\n");
						else
							printf("Store does not allow key verification. But key verification algorithm IS NOT NULL.\nWarning: this state is invalid, and you should probably report this.\n");
					}
				}
			} else if(strcmp(field_name_to_get, "key_verification_algorithm_rounds") == 0) {
				// TODO algorithm rounds tree
				return FACILITIES_GET_NOT_IMPLEMENTED;
			} else
				return FACILITIES_GET_NOT_IMPLEMENTED;

			break;
		case FACILITIES_MODE_PASSWORD_MANIPULATION:
			if(strcmp(field_name_to_get, "format") == 0) {
				if(quiet)
					puts(password_format);
				else
					printf("Current password format: `%s`\n", password_format);
			} else if(strcmp(field_name_to_get, "length") == 0) {
				if(quiet)
					printf("%d\n", password_length);
				else
					printf("Current password length: %d\n", password_length);
			} else
				return FACILITIES_GET_NOT_IMPLEMENTED;

			break;
		default:
			return FACILITIES_GET_NOT_IMPLEMENTED;
	}

	return FACILITIES_OK;
}

int facility_to(char* field_value) {
	switch(mode) {
		case FACILITIES_MODE_STORE_MANIPULATION:
			if(!FACILITIES_STORE_LOADED)
				return FACILITIES_TO_STORE_MANIPULATION_STORE_NOT_LOADED;

			if(strcmp(field_name, "algorithm") == 0) {
				bool algorithm_supported = false;
				for (unsigned short it = 0; it < storage_supported_algorithm_count; it++)
					if (strcmp(field_value, storage_supported_algorithms[it]) == 0) {
						algorithm_supported = true;
						break;
					}
				if (!algorithm_supported)
					return FACILITIES_TO_STORE_MANIPULATION_ALGORITHM_UNSUPPORTED;

				if (!FACILITIES_STORE_INIT_COMPLETE) {
					loaded_store->algorithm = strcpymalloc(field_value);
					FACILITIES_SET_STORE_INIT_COMPLETE(true);
					FACILITIES_SET_STORE_DIRTY(true);
				} else {
					// TODO Change encyrption algorithm
					return FACILITIES_TO_NOT_IMPLEMENTED;
				}
			} else if(strcmp(field_name, "key_verification_algorithm") == 0) {
				if(!loaded_store->key_verifiable)
					return FACILITIES_TO_STORE_MANIPULATION_KEY_NOT_VERIFIABLE;

				// TODO Change key verification algorithm
				return FACILITIES_TO_NOT_IMPLEMENTED;
			} else if(strcmp(field_name, "key_verification_algorithm_rounds") == 0) {
				if(!loaded_store->key_verifiable)
					return FACILITIES_TO_STORE_MANIPULATION_KEY_NOT_VERIFIABLE;

				// TODO Change key verification algorithm rounds amount
				return FACILITIES_TO_NOT_IMPLEMENTED;
			} else
				return FACILITIES_TO_NOT_IMPLEMENTED;

			break;
		case FACILITIES_MODE_PASSWORD_MANIPULATION:
			if(strcmp(field_name, "format") == 0) {
				bool valid_format = false;
				unsigned short valid_format_name_count = 5;
				char* valid_format_names[] = {
					"FORMAT_AZaz09",
					"FORMAT_AZaz09_64",
					"FORMAT_AZaz09_symb",
					"FORMAT_AZaz09_sp",
					"FORMAT_AZaz09_symb_sp"
				};
				char* valid_formats[] = {
					FORMAT_AZaz09,
					FORMAT_AZaz09_64,
					FORMAT_AZaz09_symb,
					FORMAT_AZaz09_sp,
					FORMAT_AZaz09_symb_sp
				};
				unsigned short index;
				for(unsigned short it = 0; it < valid_format_name_count; it++)
					if(strcmp(field_value, valid_format_names[it]) == 0) {
						valid_format = true;
						index = it;
						break;
					}
				if(!valid_format) {
					if(!quiet) {
						printf("Invalid format. Valid formats are:\n");
						for(unsigned short it = 0; it < valid_format_name_count; it++)
							printf("- `%s` (%s)\n", valid_format_names[it], valid_formats[it]);
					}
					return FACILITIES_TO_PASSWORD_MANIPULATION_INVALID_FORMAT;
				}
				password_format = valid_formats[index];
			} else if(strcmp(field_name, "length") == 0) {
				char* read_str;
				unsigned int new_length = strtoul(field_value, &read_str, 10);
				if(new_length == 0 && (read_str-field_value != strlen(field_value)))
					return FACILITIES_TO_PASSWORD_MANIPULATION_INVALID_LENGTH;
				password_length = new_length;
			} else
				return FACILITIES_TO_NOT_IMPLEMENTED;

			break;
		default:
			return FACILITIES_TO_NOT_IMPLEMENTED;
	}

	return FACILITIES_OK;
}

int facility_init() {
	if(mode != FACILITIES_MODE_STORE_MANIPULATION)
		return FACILITIES_WRONG_MODE;

	if(FACILITIES_STORE_LOADED)
		return FACILITIES_INIT_STORE_ALREADY_LOADED;

	Store* store = store_construct();

	loaded_store = store;
	free(loaded_store_path);
	loaded_store_path = NULL;
	FACILITIES_SET_STORE_LOADED(true);
	FACILITIES_SET_STORE_DIRTY(true);
	FACILITIES_SET_STORE_INIT_COMPLETE(false);

	return FACILITIES_OK;
}

int facility_load(char* path) {
	if(mode != FACILITIES_MODE_STORE_MANIPULATION)
		return FACILITIES_WRONG_MODE;

	if(FACILITIES_STORE_LOADED)
		return FACILITIES_LOAD_STORE_ALREADY_LOADED;

	unsigned int path_length = strlen(path);
	unsigned int ext_metadata_length = strlen(STORAGE_EXTENSION_METADATA);
	unsigned int ext_master_length = strlen(STORAGE_EXTENSION_MASTER);
	
	char* path_metadata_file = malloc(sizeof(char)*(path_length+ext_metadata_length+1));
	char* path_master_file = malloc(sizeof(char)*(path_length+ext_master_length+1));

	memcpy(path_metadata_file, path, sizeof(char)*path_length);
	memcpy(path_master_file, path, sizeof(char)*path_length);

	memcpy(path_metadata_file+path_length, STORAGE_EXTENSION_METADATA, ext_metadata_length+1);
	memcpy(path_master_file+path_length, STORAGE_EXTENSION_MASTER, ext_master_length+1);

	FILE* metadata_file = fopen(path_metadata_file, "r");
	FILE* master_file = fopen(path_master_file, "r");

	free(path_metadata_file);
	free(path_master_file);

	if(metadata_file == NULL || master_file == NULL)
		return FACILITIES_LOAD_FILE_ERROR;

	Store* store;
	if(!store_load(metadata_file, master_file, &store))
		return FACILITIES_LOAD_DESERIALIZE_ERROR;

	fclose(metadata_file);
	fclose(master_file);

	loaded_store = store;
	loaded_store_path = strcpymalloc(path);
	FACILITIES_SET_STORE_LOADED(true);
	FACILITIES_SET_STORE_INIT_COMPLETE(true);
	FACILITIES_SET_STORE_UNLOCKED(false);

	return FACILITIES_OK;
}

int facility_unlock() {
	if(mode != FACILITIES_MODE_STORE_MANIPULATION)
		return FACILITIES_WRONG_MODE;

	if(!FACILITIES_STORE_LOADED)
		return FACILITIES_UNLOCK_STORE_NOT_LOADED;

	if(!FACILITIES_STORE_INIT_COMPLETE)
		return FACILITIES_UNLOCK_STORE_INIT_NOT_COMPLETE;

	if(FACILITIES_STORE_UNLOCKED)
		return FACILITIES_UNLOCK_STORE_ALREADY_UNLOCKED;

	char* key = NULL;
	int length = 0;

	if(interactive)
		length = pseudoshell_getpass(&key, "Enter key: ", PSEUDOSHELL_BUFFER_SIZE);
	else
		length = getstr(&key, PSEUDOSHELL_BUFFER_SIZE);

	if(length <= 0) {
		free(key);
		return FACILITIES_UNLOCK_NO_KEY_PROVIDED;
	}

	char* shuffle_key_format = FORMAT_AZaz09_symb_sp;
	for(unsigned short it = 0; it < strlen(key); it++)
		if(strchr(shuffle_key_format, key[it]) == NULL)
			return FACILITIES_UNLOCK_UNABLE_TO_GENERATE_SHUFFLE_KEY;
	
	char* shuffle_key;
	generate_shuffle_key(&shuffle_key, shuffle_key_format);
	char* shuffled_key = shuffle(key, shuffle_key, shuffle_key_format);

	int insert_result = store_copy_and_insert_key(loaded_store, shuffled_key, shuffle_key, shuffle_key_format);

	free(shuffle_key);
	free(shuffled_key);

	// TODO Display passwords and ask if they are correct, redo if not

	switch(insert_result) {
		default:
			return FACILITIES_UNLOCK_UNKNOWN_ERROR;
		case STORE_KEY_OK:
			FACILITIES_SET_STORE_UNLOCKED(true);
			return FACILITIES_OK;
		case STORE_KEY_EXISTS_NOT_REPLACING:
			return FACILITIES_UNLOCK_KEY_EXISTS_NOT_REPLACING;
		case STORE_KEY_VERIFICATION_FAILED:
			return FACILITIES_UNLOCK_VERIFICATION_FAILED;
	}
}

int facility_lock() {
	if(mode != FACILITIES_MODE_STORE_MANIPULATION)
		return FACILITIES_WRONG_MODE;

	if(!FACILITIES_STORE_LOADED)
		return FACILITIES_LOCK_STORE_NOT_LOADED;

	if(!FACILITIES_STORE_INIT_COMPLETE)
		return FACILITIES_LOCK_STORE_INIT_NOT_COMPLETE;

	if(!FACILITIES_STORE_UNLOCKED)
		return FACILITIES_LOCK_STORE_ALREADY_LOCKED;

	store_remove_key_and_dispose(loaded_store);

	FACILITIES_SET_STORE_UNLOCKED(false);

	return FACILITIES_OK;
}

int facility_save() {
	if(!FACILITIES_STORE_DIRTY)
		return FACILITIES_SAVE_NOTHING_TO_SAVE;

	return facility_save_as(loaded_store_path);
}

int facility_save_as(char* path) {
	if(mode != FACILITIES_MODE_STORE_MANIPULATION)
		return FACILITIES_WRONG_MODE;

	if(path == NULL)
		return FACILITIES_SAVE_NO_PATH_PROVIDED;

	if(strlen(path) <= 0)
		return FACILITIES_SAVE_AS_EMPTY_PATH;

	if(!FACILITIES_STORE_LOADED)
		return FACILITIES_SAVE_STORE_NOT_LOADED;

	if(!FACILITIES_STORE_INIT_COMPLETE)
		return FACILITIES_SAVE_STORE_INIT_NOT_COMPLETE;

	unsigned int path_length = strlen(path);
	unsigned int ext_metadata_length = strlen(STORAGE_EXTENSION_METADATA);
	unsigned int ext_master_length = strlen(STORAGE_EXTENSION_MASTER);

	char* path_metadata_file = malloc(sizeof(char)*(path_length+ext_metadata_length+1));
	char* path_master_file = malloc(sizeof(char)*(path_length+ext_master_length+1));

	memcpy(path_metadata_file, path, sizeof(char)*path_length);
	memcpy(path_master_file, path, sizeof(char)*path_length);

	memcpy(path_metadata_file+path_length, STORAGE_EXTENSION_METADATA, ext_metadata_length+1);
	memcpy(path_master_file+path_length, STORAGE_EXTENSION_MASTER, ext_master_length+1);

	FILE* metadata_file;
	FILE* master_file;

	if((loaded_store_path == NULL || !(strcmp(path, loaded_store_path) == 0)) && (access(path_metadata_file, F_OK) == 0 || access(path_master_file, F_OK) == 0)) {
		if(interactive) {
			if(!present_yesno_prompt("Files already exist. Overwrite?", true)) {
				free(path_metadata_file);
				free(path_master_file);
				return FACILITIES_SAVE_OVERWRITE_DENIED;
			}
		} else {
			if(!yes) {
				if(!quiet)
					printf("Files already exist. Aborting.\n(Use -y option to override)\n");
				free(path_metadata_file);
				free(path_master_file);
				return FACILITIES_SAVE_OVERWRITE_DENIED;
			}
		}
	}

	metadata_file = fopen(path_metadata_file, "w+");
	master_file = fopen(path_master_file, "w+");

	if(metadata_file == NULL || master_file == NULL)
		return FACILITIES_SAVE_FILE_ERROR;

	if(!store_save(loaded_store, metadata_file, master_file))
		return FACILITIES_SAVE_SERIALIZE_ERROR;

	free(path_metadata_file);
	free(path_master_file);

	fclose(metadata_file);
	fclose(master_file);

	FACILITIES_SET_STORE_DIRTY(false);

	if(loaded_store_path != NULL)
		free(loaded_store_path);
	loaded_store_path = strcpymalloc(path);

	return FACILITIES_OK;
}

int facility_close() {
	if(mode != FACILITIES_MODE_STORE_MANIPULATION)
		return FACILITIES_WRONG_MODE;

	if(!FACILITIES_STORE_LOADED)
		return FACILITIES_CLOSE_STORE_NOT_LOADED;

	if(FACILITIES_STORE_DIRTY) {
		if(interactive) {
			if(!present_yesno_prompt("There are unsaved changes to the store. If the store is closed, THEY WILL BE LOST. Close regardless?", true))
				return FACILITIES_CLOSE_DIRTY_DISCARD_DENIED;
		} else {
			if(!yes) {
				if(!quiet)
					printf("The store contains unsaved changes. Aborting store close.\n(Use -y option to override)\n");
				return FACILITIES_CLOSE_DIRTY_DISCARD_DENIED;
			}
		}
	}

	store_destroy(loaded_store);
	loaded_store = NULL;
	free(loaded_store_path);
	loaded_store_path = NULL;
	FACILITIES_SET_STORE_DIRTY(false);
	FACILITIES_SET_STORE_LOADED(false);
	FACILITIES_SET_STORE_INIT_COMPLETE(false);
	FACILITIES_SET_STORE_UNLOCKED(false);

	return FACILITIES_OK;
}

int facility_fetch(unsigned long id) { 
	if(mode != FACILITIES_MODE_PASSWORD_MANIPULATION)
		return FACILITIES_WRONG_MODE;
	
	if(!FACILITIES_STORE_LOADED)
		return FACILITIES_FETCH_STORE_NOT_LOADED;

	if(!FACILITIES_STORE_INIT_COMPLETE)
		return FACILITIES_FETCH_STORE_INIT_NOT_COMPLETE;
	
	if(!FACILITIES_STORE_UNLOCKED)
		return FACILITIES_FETCH_STORE_LOCKED;

	if(loaded_store->password_count <= id)
		return FACILITIES_FETCH_ID_OUT_OF_BOUNDS;

	Password* password = loaded_store->passwords[id];

	if(password == NULL)
		return FACILITIES_FETCH_PASSWORD_ENTRY_IS_NULL;

	char* plain_password = password_read_plain(password);
	
	if(plain_password == NULL)
		return FACILITIES_FETCH_PASSWORD_STRING_IS_NULL;

	int res = puts(plain_password);
	res -= 1; // account for newline

	int len = strlen(plain_password);

	free(plain_password);
	
	if(len == res)
		return FACILITIES_OK;
	else
		return FACILITIES_FETCH_PRINT_FAILED;
}

int facility_find(char* identifier) {
	if(mode != FACILITIES_MODE_PASSWORD_MANIPULATION)
		return FACILITIES_WRONG_MODE;

	if(!FACILITIES_STORE_LOADED)
		return FACILITIES_FIND_STORE_NOT_LOADED;

	if(!FACILITIES_STORE_INIT_COMPLETE)
		return FACILITIES_FIND_STORE_INIT_NOT_COMPLETE;

	if(strlen(identifier) <= 0)
		return FACILITIES_FIND_EMPTY_IDENTIFIER;

	unsigned long count;
	unsigned long* ids;
	count = store_find_passwords(loaded_store, identifier, &ids);

	if(!quiet) {
		if(count == 0) {
			printf("No matching passwords found.\n");
			free(ids);
			return FACILITIES_FIND_NO_PASSWORDS_FOUND;
		} else
			printf("IDs of matching passwords:\n");
	}

	for(unsigned long it = 0; it < count; it++)
		printf("%ld\n", ids[it]);

	free(ids);

	return FACILITIES_OK;
}

int facility_display(unsigned long start, unsigned long count) {
	if(mode != FACILITIES_MODE_PASSWORD_MANIPULATION)
		return FACILITIES_WRONG_MODE;

	if(!FACILITIES_STORE_LOADED)
		return FACILITIES_DISPLAY_STORE_NOT_LOADED;

	if(!FACILITIES_STORE_INIT_COMPLETE)
		return FACILITIES_DISPLAY_STORE_INIT_NOT_COMPLETE;
	
	if(loaded_store->password_count <= start)
		return FACILITIES_DISPLAY_START_OUT_OF_BOUNDS;

	if(count == 0)
		count = FACILITIES_DISPLAY_DEFAULT_COUNT;

	if(loaded_store->password_count-start <= count)
		count = loaded_store->password_count-start;

	if(interactive) {
		printf("Displaying identifiers for passwords with ids %ld..%ld out of total %ld passwords:\n", start, start+count-1, loaded_store->password_count);

		char number_format_str[128];
		number_format_str[0] = '%';
		number_format_str[1] = '0';
		char length_str[64];
		char count_str[64];
		sprintf(count_str, "%ld", loaded_store->password_count);
		sprintf(length_str, "%ld", strlen(count_str));
		sprintf(number_format_str+2, "%sld", length_str);

		int cnt = 0;
		fputs("ID", stdout);
		cnt += 2;
		for(int it = strlen(count_str); it > 0; it--) {
			putchar(' ');
			cnt++;
		}
		fputs("|  IDENTIFIER\n", stdout);
		cnt += 3;
		int max = 10;
		int cur;
		for(unsigned long it = start; it < start+count; it++)
			if((cur = strlen(loaded_store->passwords[it]->identifier)) > max)
				max = cur;
		cnt += max;

		for(int it = 0; it < cnt; it++)
			putchar('=');
		putchar('\n');

		for(unsigned long it = start; it < start+count; it++) {
			printf(number_format_str, it);
			fputs("  |  ", stdout);
			fputs(loaded_store->passwords[it]->identifier, stdout);
			putchar('\n');
		}
	} else
		for(unsigned long it = start; it < start+count; it++)
			printf("%ld\t%s\n", it, loaded_store->passwords[it]->identifier);
	
	return FACILITIES_OK;
}

int facility_peek(unsigned long start, unsigned long count, bool present_yn_prompt, char* prompt, bool* prompt_result) {
	if(mode != FACILITIES_MODE_PASSWORD_MANIPULATION)
		return FACILITIES_WRONG_MODE;

	if(!FACILITIES_STORE_LOADED)
		return FACILITIES_PEEK_STORE_NOT_LOADED;

	if(!FACILITIES_STORE_INIT_COMPLETE)
		return FACILITIES_PEEK_STORE_INIT_NOT_COMPLETE;

	if(!FACILITIES_STORE_UNLOCKED)
		return FACILITIES_PEEK_STORE_LOCKED;
	
	if(loaded_store->password_count <= start)
		return FACILITIES_PEEK_START_OUT_OF_BOUNDS;

	if(count == 0)
		count = FACILITIES_PEEK_DEFAULT_COUNT;

	if(loaded_store->password_count-start <= count)
		count = loaded_store->password_count-start;

	if(interactive) {
		if(!quiet)
			printf("Displaying passwords with ids %ld..%ld out of total %ld passwords:\n", start, start+count-1, loaded_store->password_count);

		char number_format_str[128];
		number_format_str[0] = '%';
		number_format_str[1] = '0';
		char length_str[64];
		char count_str[64];
		sprintf(count_str, "%ld", loaded_store->password_count);
		sprintf(length_str, "%ld", strlen(count_str));
		sprintf(number_format_str+2, "%sld", length_str);

		int cnt = 0;
		fputs("ID", stdout);
		cnt += 2;
		for(int it = strlen(count_str); it > 0; it--) {
			putchar(' ');
			cnt++;
		}
		fputs("|  IDENTIFIER", stdout);
		cnt += 3;
		int max_i = 10;
		int cur;
		for(unsigned long it = start; it < start+count; it++)
			if((cur = strlen(loaded_store->passwords[it]->identifier)) > max_i)
				max_i = cur;
		cnt += max_i;
		for(int it = 10; max_i-it > 0; it++)
			putchar(' ');
		fputs("  |  PASSWORD", stdout);
		putchar('\n');
		cnt += 5;
		int max_p = 8;
		for(unsigned long it = start; it < start+count; it++)
			if((cur = loaded_store->passwords[it]->length) > max_p)
				max_p = cur;
		cnt += max_p;


		for(int it = 0; it < cnt; it++)
			putchar('=');
		putchar('\n');
		cnt += 1;

		char* plain_password;
		for(unsigned long it = start; it < start+count; it++) {
			printf(number_format_str, it);
			fputs("  |  ", stdout);
			fputs(loaded_store->passwords[it]->identifier, stdout);
			cur = strlen(loaded_store->passwords[it]->identifier);
			for(int it = max_i-cur; it > 0; it--)
				putchar(' ');
			plain_password = password_read_plain(loaded_store->passwords[it]);
			fputs("  |  ", stdout);
			fputs(plain_password, stdout);
			cur = strlen(plain_password);
			for(int it = max_p-cur; it > 0; it--)
				putchar(' ');
			putchar('\n');
			free(plain_password);
		}

		if(!quiet)
			printf("Press any key to continue.\n");
		char passchar;
		pseudoshell_getpasschar(&passchar, "", "", false);

		if(!quiet) {
			printf("\033[A\r");
			printf("\33[2K");
		}

		for(unsigned long it = start; it < start+count+2; it++) {
			printf("\033[A\r");
			printf("\33[2K");
		}

		if(!quiet) {
			printf("\033[A\r");
			printf("\33[2K");
		}
	} else {
		char* plain_password;
		for(unsigned long it = start; it < start+count; it++) {
			plain_password = password_read_plain(loaded_store->passwords[it]);
			printf("%ld\t%s\t%s\n", it, loaded_store->passwords[it]->identifier, plain_password);
			free(plain_password);
		}
	}
	
	return FACILITIES_OK;
}

int facility_generate(char* identifier) {
	if(mode != FACILITIES_MODE_PASSWORD_MANIPULATION)
		return FACILITIES_WRONG_MODE;

	if(!FACILITIES_STORE_LOADED)
		return FACILITIES_GENERATE_STORE_NOT_LOADED;

	if(!FACILITIES_STORE_INIT_COMPLETE)
		return FACILITIES_GENERATE_STORE_INIT_NOT_COMPLETE;
	
	if(!FACILITIES_STORE_UNLOCKED)
		return FACILITIES_GENERATE_STORE_LOCKED;

	if(strlen(identifier) <= 0)
		return FACILITIES_GENERARE_EMPTY_IDENTIFIER;

	generate_password_and_append(loaded_store, identifier, password_format, password_length);

	FACILITIES_SET_STORE_DIRTY(true);

	return FACILITIES_OK;
}

int facility_remove(unsigned long id) {
	if(mode != FACILITIES_MODE_PASSWORD_MANIPULATION)
		return FACILITIES_WRONG_MODE;

	if(!FACILITIES_STORE_LOADED)
		return FACILITIES_REMOVE_STORE_NOT_LOADED;

	if(!FACILITIES_STORE_INIT_COMPLETE)
		return FACILITIES_REMOVE_STORE_INIT_NOT_COMPLETE;

	Password* removed = store_remove_password(loaded_store, id);

	if(removed == NULL)
		return FACILITIES_REMOVE_REMOVED_PASSWORD_IS_NULL;

	FACILITIES_SET_STORE_DIRTY(true);

	return FACILITIES_OK;
}

int facility_memorize(unsigned long id);

int facility_send();

int facility_receive();

int facility_exit() {
	if(!FACILITIES_STORE_LOADED)
		return FACILITIES_OK;
	
	if(FACILITIES_STORE_DIRTY) {
		if(interactive) {
			if(!present_yesno_prompt("Loaded store has unsaved changes. Exit regardless?", true))
				return FACILITIES_EXIT_DIRTY_DISACRD_DENIED;
		} else {
			if(!yes) {
				if(!quiet)
					printf("The store contains unsaved changes. Aborting exit.\n(Use -y option to override)\n");
				return FACILITIES_EXIT_DIRTY_DISACRD_DENIED;
			}
		}
	}

	return FACILITIES_OK;
}
