#include<stdbool.h>

#ifndef STORAGE_H
#define STORAGE_H

typedef struct store Store;

typedef struct password Password;

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

struct password {
	Store* store;
	char* identifier;
	unsigned short length;
	unsigned short byte_length;
	unsigned short encrypted_byte_length;
	char* format;
	char* encrypted_password;
};

Store* construct_store();

Password* construct_password();

bool append_password(Store* store, Password* password, char* identifier);

Password* remove_password(Store* store, unsigned long password_index);

unsigned char* serialize_metadata(Store* store, unsigned int* length);

unsigned char* serialize_password_sequence(Store* store, unsigned int* length);

bool parse_field_forced_len(unsigned char* string, unsigned int max_length, unsigned int* position, char* field_name, char** field_value, long forced_field_length);

bool parse_field(unsigned char* string, unsigned int max_length, unsigned int* position, char* field_name, char** field_value);

bool parse_metadata_store_string(unsigned char* serialized_metadata, unsigned int length, unsigned int* position, char** algorithm, bool* key_verifiable, char** key_verification_algorithm, unsigned long* key_verification_algorithm_rounds, char** key_verification_salt, char** key_verification_text, unsigned long* password_count);

bool parse_metadata_password_string(unsigned char* serialize_metadata, unsigned int length, unsigned int* position, char** identifier, unsigned short* password_length, unsigned short* password_byte_length, unsigned short* password_encrypted_byte_length, char** format);

Store* deserialize_metadata_store(unsigned char* serialized_metadata, unsigned int length, unsigned int* position);

Password* deserialize_metadata_password(Store* store, unsigned char* serialized_metadata, unsigned int length, unsigned int* position);

Store* deserialize(unsigned char* serialized_metadata, unsigned int serialized_metadata_length, unsigned char* serialized_password_sequence, unsigned int serialized_password_sequence_length);

int save(Store* store, FILE* metadata_file, FILE* master_file);

int load(FILE* metadata_file, FILE* master_file, Store** store);

Password* find(Store* store, char* identifier);

unsigned char* read_bytes(Password* password, unsigned int* byte_length);

char* read_plain(Password* password);

bool write(Store* store, Password* password, unsigned char* plain_password_bytes, unsigned short byte_length, char* format, unsigned short length);

bool copy_and_insert_key(Store* store, char* shuffled_key, char* shuffle_key, char* shuffle_key_format);

void remove_key_and_dispose(Store* store);

#endif
