#include<stdbool.h>

#ifndef STORAGE_H
#define STORAGE_H

/* CONSTANTS */

#define STORAGE_PIECE_LENGTH 4096

#define STORAGE_EXTENSION_METADATA ".psmdf"
#define STORAGE_EXTENSION_MASTER ".psmf"

static unsigned short storage_supported_algorithm_count = 1;
static char* storage_supported_algorithms[] = {
	"AES256"
};

/* TYPE DECLARATIONS */

typedef struct store Store;

typedef struct password Password;

/*** PASSWORD-RELATED FUNCTIONS ***/

struct password {
	Store* store;
	char* identifier;
	unsigned short length;
	unsigned short byte_length;
	unsigned short encrypted_byte_length;
	char* format;
	char* encrypted_password;
};

Password* password_construct();

void password_destroy(Password* password);

unsigned char* password_read_bytes(Password* password, unsigned int* byte_length);

char* password_read_plain(Password* password);

bool passsword_write(Store* store, Password* password, unsigned char* plain_password_bytes, unsigned short byte_length, char* format, unsigned short length);

/*** STORE-RELATED FUNCTIONS ***/

struct store {
	char* algorithm;
	char* shuffled_key;
	char* shuffle_key;
	char* shuffle_key_format;
	bool key_verifiable;
	char* key_verification_algorithm;
	unsigned long key_verification_algorithm_rounds;
	char* key_verification_salt;
	char* key_verification_text;
	unsigned long password_count;
	Password** passwords;
};

Store* store_construct();

void store_destroy(Store* store);

bool store_append_password(Store* store, Password* password, char* identifier);

Password* store_remove_password(Store* store, unsigned long password_index);

unsigned char* store_serialize_metadata(Store* store, unsigned int* length);

unsigned char* store_serialize_password_sequence(Store* store, unsigned int* length);

bool store_parse_field_forced_len(unsigned char* string, unsigned int max_length, unsigned int* position, char* field_name, char** field_value, long forced_field_length);

bool store_parse_field(unsigned char* string, unsigned int max_length, unsigned int* position, char* field_name, char** field_value);

bool store_parse_metadata_store_string(unsigned char* serialized_metadata, unsigned int length, unsigned int* position, char** algorithm, bool* key_verifiable, char** key_verification_algorithm, unsigned long* key_verification_algorithm_rounds, char** key_verification_salt, char** key_verification_text, unsigned long* password_count);

bool store_parse_metadata_password_string(unsigned char* serialized_metadata, unsigned int length, unsigned int* position, char** identifier, unsigned short* password_length, unsigned short* password_byte_length, unsigned short* password_encrypted_byte_length, char** format);

Store* store_deserialize_metadata_store(unsigned char* serialized_metadata, unsigned int length, unsigned int* position);

Password* store_deserialize_metadata_password(Store* store, unsigned char* serialized_metadata, unsigned int length, unsigned int* position);

Store* store_deserialize(unsigned char* serialized_metadata, unsigned int serialized_metadata_length, unsigned char* serialized_password_sequence, unsigned int serialized_password_sequence_length);

bool store_save(Store* store, FILE* metadata_file, FILE* master_file);

bool store_load(FILE* metadata_file, FILE* master_file, Store** store);

unsigned long store_find_passwords(Store* store, char* identifier, unsigned long** ids);

#define STORE_KEY_OK 0
#define STORE_KEY_VERIFICATION_FAILED 1
#define STORE_KEY_EXISTS_NOT_REPLACING -1

int store_copy_and_insert_key(Store* store, char* shuffled_key, char* shuffle_key, char* shuffle_key_format);

void store_remove_key_and_dispose(Store* store);

#endif
