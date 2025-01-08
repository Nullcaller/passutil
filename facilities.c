#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include<unistd.h>

#include "facilities.h"

#include "passutil.h"
#include "pseudoshell.h"
#include "util.h"
#include "storage.h"

int facility_load(char* path) {
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

int facility_close();

int facility_fetch(char* identifier);

int facility_gen(char* identifier, unsigned short length);

int facility_remove(char* identifier);

int facility_memorize(char* identifier);

int facility_memorize_settings_get(char* setting_name);

int facility_memorize_settings_set(char* setting_name, char* setting_value);

int facility_transfer_send(int mode);

int facility_transfer_receive(int mode);
