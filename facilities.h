#include "constants-formats.h"

#ifndef FACILITIES_H
#define FACILITIES_H

/*** CONSTANTS ***/

#define FACILITIES_DISPLAY_DEFAULT_COUNT	20

#define FACILITIES_PEEK_DEFAULT_COUNT		10

static char* mode_short_names[] = {
	"store",
	"password",
	"memorize",
	"transfer"
};

/*** CURRENT APPLICATION STATE DESCRIPTION ***/

#define FACILITIES_MODE_STORE_MANIPULATION 0
#define FACILITIES_MODE_PASSWORD_MANIPULATION 1
#define FACILITIES_MODE_MEMORIZATION 2
#define FACILITIES_MODE_TRANSFER 3

static unsigned short mode = FACILITIES_MODE_STORE_MANIPULATION;

static char *field_name = NULL;

static Store* loaded_store = NULL;
static char* loaded_store_path = NULL;

static char* password_format = FORMAT_AZaz09;
static unsigned int password_length = 20;

#define FACILITIES_MEMORIZE_MODE_WHOLE 0
#define FACILITIES_MEMORIZE_MODE_BY_SYMBOL 1
#define FACILITIES_MEMORIZE_MODE_BY_NTH_SYMBOL 2

static unsigned short memorize_mode = FACILITIES_MEMORIZE_MODE_WHOLE;

static unsigned short memorize_symbol_number = 0;

#define FACILITIES_MEMORIZE_REPEAT_MODE_SINGULAR 0
#define FACILITIES_MEMORIZE_REPEAT_MODE_REPEAT 1

static unsigned short memorize_repeat_mode = FACILITIES_MEMORIZE_REPEAT_MODE_REPEAT;

// State variable
static unsigned long facilities_state = 0;

// Flags
#define FACILITIES_FLAG_STORE_LOADED			(0b1 << 0)
#define FACILITIES_FLAG_KEY_INSERTED			(0b1 << 1)
#define FACILITIES_FLAG_STORE_DIRTY				(0b1 << 2)
#define FACILITIES_FLAG_STORE_INIT_COMPLETE		(0b1 << 3)
#define FACILITIES_FLAG_STORE_UNLOCKED			(0b1 << 4)

// Getters
#define FACILITIES_STORE_LOADED				(facilities_state & FACILITIES_FLAG_STORE_LOADED)
#define FACILITIES_KEY_INSERTED				(facilities_state & FACILITIES_FLAG_KEY_INSERTED)
#define FACILITIES_STORE_DIRTY				(facilities_state & FACILITIES_FLAG_STORE_DIRTY)
#define FACILITIES_STORE_INIT_COMPLETE		(facilities_state & FACILITIES_FLAG_STORE_INIT_COMPLETE)
#define FACILITIES_STORE_UNLOCKED			(facilities_state & FACILITIES_FLAG_STORE_UNLOCKED)

// Setters
#define FACILITIES_SET_STORE_LOADED(VALUE)	 		facilities_state = facilities_state - FACILITIES_STORE_LOADED + FACILITIES_FLAG_STORE_LOADED * VALUE
#define FACILITIES_SET_KEY_INSERTED(VALUE)	 		facilities_state = facilities_state - FACILITIES_KEY_INSERTED + FACILITIES_FLAG_KEY_INSERTED * VALUE
#define FACILITIES_SET_STORE_DIRTY(VALUE)	 		facilities_state = facilities_state - FACILITIES_STORE_DIRTY + FACILITIES_FLAG_STORE_DIRTY * VALUE
#define FACILITIES_SET_STORE_INIT_COMPLETE(VALUE)	facilities_state = facilities_state - FACILITIES_STORE_INIT_COMPLETE + FACILITIES_FLAG_STORE_INIT_COMPLETE * VALUE
#define FACILITIES_SET_STORE_UNLOCKED(VALUE)		facilities_state = facilities_state - FACILITIES_STORE_UNLOCKED + FACILITIES_FLAG_STORE_UNLOCKED * VALUE

/*** UTIL ***/

char* get_facility_error_message(int error);

/*** FACILITIES ***/

#define FACILITIES_OK 			0
#define FACILITIES_WRONG_MODE	1000

#define FACILITIES_SWITCH_MODE_UNKNOWN_MODE		1

int facility_switch_mode(unsigned short new_mode);

#define FACILITIES_SET_WRONG_FIELD_NAME_FOR_MODE	2

int facility_set(char* field_name);

#define FACILITIES_GET_WRONG_FIELD_NAME_FOR_MODE				3
#define FACILITIES_GET_NOT_IMPLEMENTED							4
#define FACILITIES_GET_STORE_MANIPULATION_STORE_NOT_LOADED		5

int facility_get(char* field_name_to_get);

#define FACILITIES_TO_STORE_MANIPULATION_STORE_NOT_LOADED		6
#define FACILITIES_TO_STORE_MANIPULATION_KEY_NOT_VERIFIABLE		7
#define FACILITIES_TO_STORE_MANIPULATION_ALGORITHM_UNSUPPORTED	8
#define FACILITIES_TO_PASSWORD_MANIPULATION_INVALID_LENGTH		9
#define FACILITIES_TO_PASSWORD_MANIPULATION_INVALID_FORMAT		10
#define	FACILITIES_TO_NOT_IMPLEMENTED							11

int facility_to(char* field_value);

#define FACILITIES_INIT_STORE_ALREADY_LOADED	12

int facility_init();

#define FACILITIES_LOAD_STORE_ALREADY_LOADED	13
#define FACILITIES_LOAD_FILE_ERROR				-1
#define FACILITIES_LOAD_DESERIALIZE_ERROR		-2

int facility_load(char* path);

#define FACILITIES_UNLOCK_STORE_NOT_LOADED					14
#define FACILITIES_UNLOCK_STORE_INIT_NOT_COMPLETE			15
#define FACILITIES_UNLOCK_UNABLE_TO_GENERATE_SHUFFLE_KEY	16
#define FACILITIES_UNLOCK_UNKNOWN_ERROR						17
#define FACILITIES_UNLOCK_KEY_EXISTS_NOT_REPLACING			18
#define FACILITIES_UNLOCK_VERIFICATION_FAILED				19
#define FACILITIES_UNLOCK_NO_KEY_PROVIDED					20
#define FACILITIES_UNLOCK_STORE_ALREADY_UNLOCKED			21

int facility_unlock();

#define FACILITIES_LOCK_STORE_NOT_LOADED			22
#define FACILITIES_LOCK_STORE_INIT_NOT_COMPLETE		23
#define FACILITIES_LOCK_STORE_ALREADY_LOCKED		24

int facility_lock();

#define FACILITIES_SAVE_STORE_INIT_NOT_COMPLETE		25
#define FACILITIES_SAVE_NO_PATH_PROVIDED			26
#define FACILITIES_SAVE_NOTHING_TO_SAVE				27
#define FACILITIES_SAVE_STORE_NOT_LOADED			28
#define FACILITIES_SAVE_FILE_ERROR					-3
#define FACILITIES_SAVE_OVERWRITE_DENIED			-4
#define FACILITIES_SAVE_SERIALIZE_ERROR				-5

int facility_save();

#define FACILITIES_SAVE_AS_EMPTY_PATH	50

int facility_save_as(char* path);

#define FACILITIES_CLOSE_STORE_NOT_LOADED		29
#define FACILITIES_CLOSE_DIRTY_DISCARD_DENIED	30

int facility_close();

#define FACILITIES_DISPLAY_STORE_NOT_LOADED				51
#define FACILITIES_DISPLAY_STORE_INIT_NOT_COMPLETE		52
#define FACILITIES_DISPLAY_START_OUT_OF_BOUNDS			53

int facility_display(unsigned long start, unsigned long count);

#define FACILITIES_PEEK_STORE_NOT_LOADED				54
#define FACILITIES_PEEK_STORE_INIT_NOT_COMPLETE			55
#define FACILITIES_PEEK_STORE_LOCKED					56
#define FACILITIES_PEEK_START_OUT_OF_BOUNDS				57

int facility_peek(unsigned long start, unsigned long count, bool present_yn_prompt, char* prompt, bool* prompt_result);

#define FACILITIES_FETCH_STORE_NOT_LOADED			31
#define FACILITIES_FETCH_STORE_INIT_NOT_COMPLETE	32
#define FACILITIES_FETCH_STORE_LOCKED				33
#define FACILITIES_FETCH_ID_OUT_OF_BOUNDS			34
#define FACILITIES_FETCH_PASSWORD_ENTRY_IS_NULL		35
#define FACILITIES_FETCH_PASSWORD_STRING_IS_NULL	36
#define FACILITIES_FETCH_PRINT_FAILED				37

int facility_fetch(unsigned long id);

#define FACILITIES_FIND_STORE_NOT_LOADED			38
#define FACILITIES_FIND_STORE_INIT_NOT_COMPLETE		39
#define FACILITIES_FIND_NO_PASSWORDS_FOUND			40
#define FACILITIES_FIND_EMPTY_IDENTIFIER			49

int facility_find(char* identifier);

#define FACILITIES_GENERATE_STORE_NOT_LOADED			41
#define FACILITIES_GENERATE_STORE_INIT_NOT_COMPLETE		42
#define FACILITIES_GENERATE_STORE_LOCKED				43
#define FACILITIES_GENERARE_EMPTY_IDENTIFIER			48

int facility_generate(char* identifier);

#define FACILITIES_REMOVE_STORE_NOT_LOADED			44
#define FACILITIES_REMOVE_STORE_INIT_NOT_COMPLETE	45
#define FACILITIES_REMOVE_REMOVED_PASSWORD_IS_NULL	46

int facility_remove(unsigned long id);

int facility_memorize(unsigned long id);

int facility_send();

int facility_receive();

#define FACILITIES_EXIT_DIRTY_DISCARD_DENIED		47

int facility_exit();

/*** ERROR MESSAGES ***/

#define FACILITIES_WRONG_MODE_MESSAGE						"Wrong mode for selected facility"

#define FACILITIES_SWITCH_MODE_UNKNOWN_MODE_MESSAGE			"switch_mode: Unknown mode"

#define FACILITIES_SET_WRONG_FIELD_NAME_FOR_MODE_MESSAGE	"set: Wrong field name for selected mode"

#define FACILITIES_GET_WRONG_FIELD_NAME_FOR_MODE_MESSAGE			"get: Wrong field name for selected mode"
#define FACILITIES_GET_NOT_IMPLEMENTED_MESSAGE						"get: Getter not currently implemented (sorry)"
#define FACILITIES_GET_STORE_MANIPULATION_STORE_NOT_LOADED_MESSAGE	"get: Store not loaded"

#define FACILITIES_TO_STORE_MANIPULATION_STORE_NOT_LOADED_MESSAGE		"to: Store not loaded"
#define FACILITIES_TO_STORE_MANIPULATION_KEY_NOT_VERIFIABLE_MESSAGE		"to: Key not verifiable"
#define FACILITIES_TO_STORE_MANIPULATION_ALGORITHM_UNSUPPORTED_MESSAGE	"to: Algorithm unsupported"
#define FACILITIES_TO_PASSWORD_MANIPULATION_INVALID_LENGTH_MESSAGE		"to: Invalid password length"
#define FACILITIES_TO_PASSWORD_MANIPULATION_INVALID_FORMAT_MESSAGE		"to: Invalid password format"
#define	FACILITIES_TO_NOT_IMPLEMENTED_MESSAGE							"to: Not implemented"

#define FACILITIES_INIT_STORE_ALREADY_LOADED_MESSAGE		"init: Store already loaded"

#define FACILITIES_LOAD_STORE_ALREADY_LOADED_MESSAGE		"load: Store already loaded"
#define FACILITIES_LOAD_FILE_ERROR_MESSAGE					"load: File I/O error"
#define FACILITIES_LOAD_DESERIALIZE_ERROR_MESSAGE			"load: Deserialization error"

#define FACILITIES_UNLOCK_STORE_NOT_LOADED_MESSAGE					"unlock: Store not loaded"
#define FACILITIES_UNLOCK_STORE_INIT_NOT_COMPLETE_MESSAGE			"unlock: Store init not complete"
#define FACILITIES_UNLOCK_UNABLE_TO_GENERATE_SHUFFLE_KEY_MESSAGE	"unlock: Unable to generate shuffle key"
#define FACILITIES_UNLOCK_UNKNOWN_ERROR_MESSAGE						"unlock: Unknown error while inserting key"
#define FACILITIES_UNLOCK_KEY_EXISTS_NOT_REPLACING_MESSAGE			"unlock: Error while unlocking the store - key already inserted"
#define FACILITIES_UNLOCK_VERIFICATION_FAILED_MESSAGE				"unlock: Error while unlocking the store - verification failed"
#define FACILITIES_UNLOCK_NO_KEY_PROVIDED_MESSAGE					"unlock: No key was provided"
#define FACILITIES_UNLOCK_STORE_ALREADY_UNLOCKED_MESSAGE			"unlock: Store is already UNLOCKED"

#define FACILITIES_LOCK_STORE_NOT_LOADED_MESSAGE					"lock: Store not loaded"
#define FACILITIES_LOCK_STORE_INIT_NOT_COMPLETE_MESSAGE				"lock: Store init not complete"
#define FACILITIES_LOCK_STORE_ALREADY_LOCKED_MESSAGE				"lock: Store is already LOCKED"

#define FACILITIES_SAVE_STORE_INIT_NOT_COMPLETE_MESSAGE		"save: Store init not complete"
#define FACILITIES_SAVE_NO_PATH_PROVIDED_MESSAGE			"save: No path was provided"
#define FACILITIES_SAVE_NOTHING_TO_SAVE_MESSAGE				"save: Nothing to save (no changes)"
#define FACILITIES_SAVE_STORE_NOT_LOADED_MESSAGE			"save: Store not loaded"
#define FACILITIES_SAVE_FILE_ERROR_MESSAGE					"save: File I/O error"
#define FACILITIES_SAVE_OVERWRITE_DENIED_MESSAGE			"save: Not overwriting (denied)"
#define FACILITIES_SAVE_SERIALIZE_ERROR_MESSAGE				"save: Serialization error"

#define FACILITIES_SAVE_AS_EMPTY_PATH_MESSAGE				"save-as: No path was provided"

#define FACILITIES_CLOSE_STORE_NOT_LOADED_MESSAGE			"close: Store not loaded"
#define FACILITIES_CLOSE_DIRTY_DISCARD_DENIED_MESSAGE		"close: Unsaved changes detected, discard denied"

#define FACILITIES_DISPLAY_STORE_NOT_LOADED_MESSAGE					"display: Store not loaded"
#define FACILITIES_DISPLAY_STORE_INIT_NOT_COMPLETE_MESSAGE			"display: Store init not complete"
#define FACILITIES_DISPLAY_START_OUT_OF_BOUNDS_MESSAGE				"display: Selected start index is out of bounds"

#define FACILITIES_PEEK_STORE_NOT_LOADED_MESSAGE			"peek: Store not loaded"
#define FACILITIES_PEEK_STORE_INIT_NOT_COMPLETE_MESSAGE		"peek: Store init not complete"
#define FACILITIES_PEEK_STORE_LOCKED_MESSAGE				"peek: Store is LOCKED"
#define FACILITIES_PEEK_START_OUT_OF_BOUNDS_MESSAGE			"peek: Selected start index is out of bounds"

#define FACILITIES_FETCH_STORE_NOT_LOADED_MESSAGE			"fetch: Store not loaded"
#define FACILITIES_FETCH_STORE_INIT_NOT_COMPLETE_MESSAGE	"fetch: Store init not complete"
#define FACILITIES_FETCH_STORE_LOCKED_MESSAGE				"fetch: Store is LOCKED"
#define FACILITIES_FETCH_ID_OUT_OF_BOUNDS_MESSAGE			"fetch: Selected index is out of bounds"
#define FACILITIES_FETCH_PASSWORD_ENTRY_IS_NULL_MESSAGE		"fetch: Fetch succeeded, but password object is NULL (this is bad)"
#define FACILITIES_FETCH_PASSWORD_STRING_IS_NULL_MESSAGE	"fetch: Fetch succeeded, but password string is NULL (this is bad)"
#define FACILITIES_FETCH_PRINT_FAILED_MESSAGE				"fetch: Failed to print password (this is weird)"

#define FACILITIES_FIND_STORE_NOT_LOADED_MESSAGE			"find: Store not loaded"
#define FACILITIES_FIND_STORE_INIT_NOT_COMPLETE_MESSAGE		"find: Store init not complete"
#define FACILITIES_FIND_NO_PASSWORDS_FOUND_MESSAGE			"find: No passwords found"
#define FACILITIES_FIND_EMPTY_IDENTIFIER_MESSAGE			"find: Find succeeded, but password identifier is NULL (this is weird)"

#define FACILITIES_GENERATE_STORE_NOT_LOADED_MESSAGE				"generate: Store not loaded"
#define FACILITIES_GENERATE_STORE_INIT_NOT_COMPLETE_MESSAGE			"generate: Store init not complete"
#define FACILITIES_GENERATE_STORE_LOCKED_MESSAGE					"generate: Store is LOCKED"
#define FACILITIES_GENERARE_EMPTY_IDENTIFIER_MESSAGE				"generate: No identifier provided"

#define FACILITIES_REMOVE_STORE_NOT_LOADED_MESSAGE					"remove: Store not loaded"
#define FACILITIES_REMOVE_STORE_INIT_NOT_COMPLETE_MESSAGE			"remove: Store init not complete"
#define FACILITIES_REMOVE_REMOVED_PASSWORD_IS_NULL_MESSAGE			"remove: Remove succeeded, but removed password object is NULL (this is weird)"

#define FACILITIES_EXIT_DIRTY_DISCARD_DENIED_MESSAGE		"exit: Unsaved changes detected, discard denied"

#endif