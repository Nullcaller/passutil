#ifndef FACILITIES_H
#define FACILITIES_H

/*** CURRENT APPLICATION STATE DESCRIPTION ***/

#define FACILITIES_MODE_STORE_MANIPULATION 0
#define FACILITIES_MODE_PASSWORD_MANIPULATION 1
#define FACILITIES_MODE_MEMORIZATION 2
#define FACILITIES_MODE_TRANSFER 3

static unsigned short mode = FACILITIES_MODE_STORE_MANIPULATION;

static char *field_name = NULL;

static Store* loaded_store = NULL;
static char* loaded_store_path = NULL;

// State variable
static unsigned long facilities_state = 0;

// Flags
#define FACILITIES_FLAG_STORE_LOADED	(0b1 << 0)
#define FACILITIES_FLAG_KEY_INSERTED	(0b1 << 1)
#define FACILITIES_FLAG_STORE_DIRTY		(0b1 << 2)

// Getters
#define FACILITIES_STORE_LOADED			(facilities_state & FACILITIES_FLAG_STORE_LOADED)
#define FACILITIES_KEY_INSERTED			(facilities_state & FACILITIES_FLAG_KEY_INSERTED)
#define FACILITIES_STORE_DIRTY			(facilities_state & FACILITIES_FLAG_STORE_DIRTY)

// Setters
#define FACILITIES_SET_STORE_LOADED(VALUE)	 	facilities_state = facilities_state - FACILITIES_STORE_LOADED + FACILITIES_FLAG_STORE_LOADED * VALUE
#define FACILITIES_SET_KEY_INSERTED(VALUE)	 	facilities_state = facilities_state - FACILITIES_KEY_INSERTED + FACILITIES_FLAG_KEY_INSERTED * VALUE
#define FACILITIES_SET_STORE_DIRTY(VALUE)	 	facilities_state = facilities_state - FACILITIES_STORE_DIRTY + FACILITIES_FLAG_STORE_DIRTY * VALUE

/*** FACILITIES ***/

#define FACILITIES_OK 			0
#define FACILITIES_WRONG_MODE	-100

#define FACILITIES_SWITCH_MODE_UNKNOWN_MODE		1

int facility_switch_mode(unsigned short new_mode);

#define FACILITIES_LOAD_STORE_ALREADY_LOADED	1
#define FACILITIES_LOAD_FILE_ERROR				-1
#define FACILITIES_LOAD_DESERIALIZE_ERROR		-2

int facility_load(char* path);

#define FACILITIES_SAVE_NO_PATH_PROVIDED		3
#define FACILITIES_SAVE_NOTHING_TO_SAVE			2
#define FACILITIES_SAVE_STORE_NOT_LOADED		1
#define FACILITIES_SAVE_FILE_ERROR				-1
#define FACILITIES_SAVE_OVERWRITE_DENIED		-2
#define FACILITIES_SAVE_SERIALIZE_ERROR			-3

int facility_save();

int facility_save_as(char* path);

int facility_close();

int facility_fetch(unsigned long id);

int facility_gen(char* identifier, unsigned short length);

int facility_remove(unsigned long id);

int facility_memorize(unsigned long id);

int facility_memorize_settings_get(char* setting_name);

int facility_memorize_settings_set(char* setting_name, char* setting_value);

int facility_transfer_send(int mode);

int facility_transfer_receive(int mode);

#endif