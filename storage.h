#include<stdbool.h>

typedef struct store Store;

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

Store* construct_store();

Password* construct_password();

bool append_password(Store* store, Password* password, char* identifier);

Password* remove_password(Store* store, unsigned long password_index);

int save(Store* store, FILE* metadata_file, FILE* master_file);

int load(FILE* metadata_file, FILE* master_file, Store** store);

Password* find(Store* store, char* identifier);

unsigned char* read_bytes(Password* password, unsigned int* byte_length);

char* read_plain(Password* password);

bool write(Store* store, Password* password, unsigned char* plain_password_bytes, unsigned short byte_length, char* format, unsigned short length);

bool copy_and_insert_key(Store* store, char* shuffled_key, char* shuffle_key, char* shuffle_key_format);

void remove_key_and_dispose(Store* store);
