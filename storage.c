#include<stdio.h>
#include<stdbool.h>
#include<math.h>

#include "util.c"
//#include "shuffler.c"

typedef struct password Password;

typedef struct store {
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
} Store;

typedef struct password {
	Store* store;
	char* identifier;
	unsigned short length;
	unsigned short byte_length;
	unsigned short encrypted_byte_length;
	char* format;
	char* encrypted_password;
} Password;

Store* construct_store() {
	Store* new_store = malloc(sizeof(Store));
	new_store->algorithm = NULL;
	new_store->shuffled_key = NULL;
	new_store->shuffle_key = NULL;
	new_store->shuffle_key_format = NULL;
	new_store->key_verifiable = false;
	new_store->key_verification_algorithm = NULL;
	new_store->key_verification_algorithm_rounds = 0;
	new_store->key_verification_salt = NULL;
	new_store->key_verification_text = NULL;
	new_store->password_count = 0;
	new_store->passwords = 0;
	return new_store;
}

Password* construct_password() {
	Password* new_password = malloc(sizeof(Password));
	new_password->store = NULL;
	new_password->identifier = NULL;
	new_password->length = 0;
	new_password->byte_length = 0;
	new_password->encrypted_byte_length = 0;
	new_password->format = NULL;
	new_password->encrypted_password = NULL;
	return new_password;
}

bool append_password(Store* store, Password* password, char* identifier) {
	if(password->store != NULL)
		return false;
	password->store = store;
	password->identifier = identifier;

	unsigned long new_password_count = store->password_count+1;
	Password** new_passwords = malloc(sizeof(Password*)*new_password_count);

	unsigned long it = 0;
	while(it < store->password_count) {
		new_passwords[it] = store->passwords[it];
		it++;
	}
	new_passwords[it] = password;

	Password** old_passwords = store->passwords;
	store->passwords = new_passwords;
	store->password_count = new_password_count;
	free(old_passwords);

	return true;
}

Password* remove_password(Store* store, unsigned long password_index) {
	unsigned long new_password_count = store->password_count-1;
	Password** new_passwords = malloc(sizeof(Password*)*new_password_count);

	unsigned long it = 0;
	while(it < password_index) {
		new_passwords[it] = store->passwords[it];
		it++;
	} 
	while(it < new_password_count) {
		new_passwords[it] = store->passwords[it+1];
		it++;
	}

	Password* removed;

	Password** old_passwords = store->passwords;
	removed = old_passwords[password_index];
	store->password_count = new_password_count;
	store->passwords = new_passwords;
	free(old_passwords);

	return removed;
}

int save(Store* store, FILE* metadata_file, FILE* master_file) {

}

int load(FILE* metadata_file, FILE* master_file, Store** store) {

}

Password* find(Store* store, char* identifier) {
	for(unsigned long it = 0; it < store->password_count; it++)
		if(strcmp(identifier, store->passwords[it]->identifier) == 0)
			return store->passwords[it];
	return NULL;
}

unsigned char* read_bytes(Password* password, unsigned int* byte_length) {
	unsigned char* plain_password_bytes = decrypt(password->store->algorithm, password->encrypted_password, password->byte_length, password->store->shuffled_key, password->store->shuffle_key, password->store->shuffle_key_format);
	*byte_length = password->byte_length;
	return plain_password_bytes;
}

char* read_plain(Password* password) {
	char* plain_password = malloc(sizeof(char)*(password->length+1));
	plain_password[password->length] = '\0';

	unsigned int byte_length;
	unsigned char* plain_password_bytes = read_bytes(password, &byte_length);

	unsigned int modulo = strlen(password->format);
	unsigned long symbol;
	size_t symbol_length, one_byte_length;
	symbol_length = sizeof(symbol);
	one_byte_length = sizeof(char);
	unsigned int jmax = symbol_length/one_byte_length;

	for(unsigned int it = 0; it < password->length; it++) {
		symbol = 0;
		for(unsigned int j = 0; j < jmax; j++) {
			symbol += plain_password_bytes[it*jmax+j];
			symbol <<= (one_byte_length*8);
		}
		plain_password[it] = password->format[symbol % modulo];
	}

	return plain_password;
}

bool write(Store* store, Password* password, unsigned char* plain_password_bytes, unsigned short byte_length, char* format, unsigned short length) {
	if(password->encrypted_password != NULL) 
		return false;

	password->format = strcpymalloc(format);
	password->length = length;
	password->byte_length = byte_length;

	unsigned int encrypted_byte_length;
	unsigned char* encrypted_password_bytes = encrypt(&encrypted_byte_length, store->algorithm, plain_password_bytes, byte_length, store->shuffled_key, store->shuffle_key, store->shuffle_key_format);

	password->encrypted_byte_length = encrypted_byte_length;
	password->encrypted_password = encrypted_password_bytes;

	return true;
}

bool insert_key(Store* store, char* shuffled_key, char* shuffle_key, char* shuffle_key_format) {
	if(store->key_verifiable) {
		//char* key = unshuffle(shuffled_key, shuffle_key, shuffle_key_format);

		// TODO KEY VERIFICATION
	}
	
	char* new_shuffled_key = strcpymalloc(shuffled_key);
	char* new_shuffle_key = strcpymalloc(shuffle_key);
	char* new_shuffle_key_format = strcpymalloc(shuffle_key_format);

	store->shuffled_key = new_shuffled_key;
	store->shuffle_key = new_shuffle_key;
	store->shuffle_key_format = new_shuffle_key_format;

	return true;
}

void remove_key_and_dispose(Store* store) {
	char* shuffled_key = store->shuffled_key;
	char* shuffle_key = store->shuffle_key;
	char* shuffle_key_format = store->shuffle_key_format;

	store->shuffled_key = NULL;
	store->shuffle_key = NULL;
	store->shuffle_key_format = NULL;

	free(shuffled_key);
	free(shuffle_key);
	free(shuffle_key_format);
}
