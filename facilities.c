#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include<unistd.h>

#include "facilities.h"

#include "passutil.h"
#include "pseudoshell.h"
#include "util.h"
#include "storage.h"

int facility_switch_mode(unsigned short new_mode)
{
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

int facility_set(char* new_field_name)
{
	unsigned short mode_store_manipulation_allowed_field_name_count = 3;
	char* mode_store_manipulation_allowed_field_names[] = {
		"algorithm",
		"key_verification_algorithm",
		"key_verification_algorithm_rounds"
	};
	unsigned short mode_password_manipulation_allowed_field_name_count = 0;
	char* mode_password_manipulation_allowed_field_names[] = {};
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
		if(strcmp(allowed_field_names[it], new_field_name) == 0) {
			field_name_allowed = true;
			break;
		}

	if(!field_name_allowed)
		return FACILITIES_SET_WRONG_FIELD_NAME_FOR_MODE;

	free(field_name);
	field_name = strcpymalloc(new_field_name);

	return FACILITIES_OK;
}

int facility_to(char* field_value)
{
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
				}
			} else if(strcmp(field_name, "key_verification_algorithm") == 0) {
				if(!loaded_store->key_verifiable)
					return FACILITIES_TO_STORE_MANIPULATION_KEY_NOT_VERIFIABLE;

				// TODO Change key verification algorithm
			} else if(strcmp(field_name, "key_verification_algorithm_rounds") == 0) {
				if(!loaded_store->key_verifiable)
					return FACILITIES_TO_STORE_MANIPULATION_KEY_NOT_VERIFIABLE;

				// TODO Change key verification algorithm rounds amount
			}

			break;
	}

	return FACILITIES_OK;
}

int facility_load(char* path)
{
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

	if(!FACILITIES_STORE_LOADED)
		return FACILITIES_SAVE_STORE_NOT_LOADED;

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
					printf("Files already exist. Aborting.\n(Use -y option to override)");
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
	
	return FACILITIES_OK;
}

int facility_fetch(unsigned long id);

int facility_gen(char* identifier, unsigned short length);

int facility_remove(unsigned long id);

int facility_memorize(unsigned long id);

int facility_memorize_settings_get(char* setting_name);

int facility_memorize_settings_set(char* setting_name, char* setting_value);

int facility_transfer_send(int mode);

int facility_transfer_receive(int mode);
