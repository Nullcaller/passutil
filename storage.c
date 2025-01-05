#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<math.h>

#include "storage.h"

#include "util.h"
#include "shuffler.h"

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

unsigned char* serialize_metadata(Store* store, unsigned int* length) {
	unsigned int piece_length = 4096;
	unsigned int buffer_length = 4096;

	unsigned char* serialized_metadata = NULL;
	unsigned int sml;
	char* buffer = malloc(buffer_length*sizeof(unsigned char));

	sprintf(buffer, "cv=`%s`,algo=`%s`,keyverif=`%s`,", "1.0", store->algorithm, store->key_verifiable ? "true" : "false");
	serialized_metadata = strappendrealloc(serialized_metadata, &sml, piece_length, buffer);
	if(store->key_verifiable) {
		sprintf(buffer, "keyverifalgo=`%s`,keyverifrounds=`%ld`,keyverifsalt=`%s`,keyverifsig=`%s`,", store->key_verification_algorithm, store->key_verification_algorithm_rounds, store->key_verification_salt, store->key_verification_text);
		serialized_metadata = strappendrealloc(serialized_metadata, &sml, piece_length, buffer);
	}
	sprintf(buffer, "passcount=`%ld`\n", store->password_count);
	serialized_metadata = strappendrealloc(serialized_metadata, &sml, piece_length, buffer);

	unsigned long passcount = store->password_count;
	Password* password;
	for(unsigned long it = 0; it < passcount; it++) {
		password = store->passwords[it];
		sprintf(buffer, "identifier=`%s`,encbytelen=`%hd`,bytelen=`%hd`,len=`%hd`,formatlen=`%ld`,format=`%s`\n", password->identifier, password->encrypted_byte_length, password->byte_length, password->length, strlen(password->format), password->format);
		serialized_metadata = strappendrealloc(serialized_metadata, &sml, piece_length, buffer);
	}

	serialized_metadata = strtrimrealloc(serialized_metadata, &sml);
	*length = sml;

	return serialized_metadata;
}

unsigned char* serialize_password_sequence(Store* store, unsigned int* length) {
	Password** passwords = store->passwords;
	unsigned long password_count = store->password_count;

	unsigned int byte_length = 0;
	for(unsigned long it = 0; it < password_count; it++)
		byte_length += passwords[it]->encrypted_byte_length;
	
	unsigned char* serialized_password_sequence = malloc(sizeof(unsigned char)*byte_length);
	byte_length = 0;
	for(unsigned long it = 0; it < password_count; it++) {
		memcpy(serialized_password_sequence+byte_length, passwords[it]->encrypted_password, passwords[it]->encrypted_byte_length);
		byte_length += passwords[it]->encrypted_byte_length;
	}

	*length = byte_length;
	return serialized_password_sequence;
}

bool parse_field_forced_len(unsigned char* string, unsigned int max_length, unsigned int* position, char* field_name, char** field_value, long forced_field_length) {
	char* _field_value = NULL;
	unsigned int allocated_field_length = 0;
	unsigned int current_field_length = 0;
	unsigned int piece_length = 4096;
	char* strp = string+*position;

	unsigned int posadd = 0;
	unsigned int posbuf;

	posbuf = pseudosscanf(strp+posadd, field_name);
	if(strlen(field_name) != posbuf)
		return false;

	posadd += posbuf;

	if(posadd > max_length)
		return false;

	posbuf = pseudosscanf(strp+posadd, "=`");
	if(posbuf != 2)
		return false;
	posadd += posbuf;

	if(posadd > max_length)
		return false;

	if(forced_field_length < 0) {
		posbuf = 0;
		while(*(strp+posadd+posbuf) != '`') {
			if(posadd+posbuf > max_length)
				return false;
			if(*(strp+posadd+posbuf) == '\0')
				return false;
			_field_value = strappendcharrealloc(_field_value, &allocated_field_length, &current_field_length, piece_length, *(strp+posadd+posbuf));
			posbuf++;
		}
		_field_value = strappendcharrealloc(_field_value, &allocated_field_length, &current_field_length, piece_length, '\0');
		posadd += posbuf;
	} else if(forced_field_length == 0) {
		_field_value = malloc(sizeof(char));
		*_field_value = '\0';
	} else {
		if(posadd+forced_field_length > max_length)
			return false;
		_field_value = malloc(sizeof(char)*(forced_field_length+1));
		memcpy(_field_value, strp+posadd, forced_field_length);
		*(_field_value+forced_field_length) = '\0';
		posadd += forced_field_length;
	}

	if(posadd > max_length)
		return false;

	posbuf = pseudosscanf(strp+posadd, "`");
	if(posbuf != 1)
		return false;
	posadd += posbuf;

	if(posadd > max_length)
		return false;

	if(posadd < max_length)
		posadd += pseudosscanf(strp+posadd, ","); // Eat the ',' if available

	*field_value = _field_value;
	*position += posadd;

	return true;
} 

bool parse_field(unsigned char* string, unsigned int max_length, unsigned int* position, char* field_name, char** field_value) {
	return parse_field_forced_len(string, max_length, position, field_name, field_value, -1);
}

bool parse_metadata_store_string(unsigned char* serialized_metadata, unsigned int length, unsigned int* position, char** algorithm, bool* key_verifiable, char** key_verification_algorithm, unsigned long* key_verification_algorithm_rounds, char** key_verification_salt, char** key_verification_text, unsigned long* password_count) {
	unsigned char* smp = serialized_metadata+*position;

	char* _cv;
	char* _algorithm;
	char* _key_verifiable_string;
	bool _key_verifiable;
	char* _key_verification_algorithm;
	char* _key_verification_algorithm_rounds_string;
	unsigned long _key_verification_algorithm_rounds;
	char* _key_verification_salt;
	char* _key_verification_text;
	char* _password_count_string;
	unsigned long _password_count;

	unsigned int posadd = 0;
	char* read_str;

	if(!parse_field(smp, length-*position-posadd, &posadd, "cv", &_cv))
		return false;
	
	// Check container version
	if(strcmp(_cv, "1.0") != 0)
		return false;

	if(!parse_field(smp, length-*position-posadd, &posadd, "algo", &_algorithm))
		return false;

	if(!parse_field(smp, length-*position-posadd, &posadd, "keyverif", &_key_verifiable_string))
		return false;

	if(strcmp(_key_verifiable_string, "true") == 0)
		_key_verifiable = true;
	else if(strcmp(_key_verifiable_string, "false") == 0)
		_key_verifiable = false;
	else
		return false;

	if(_key_verifiable) {

		if(!parse_field(smp, length-*position-posadd, &posadd, "keyverifalgo", &_key_verification_algorithm))
			return false;

		if(!parse_field(smp, length-*position-posadd, &posadd, "keyverifrounds", &_key_verification_algorithm_rounds_string))
			return false;

		_key_verification_algorithm_rounds = strtoul(_key_verification_algorithm_rounds_string, &read_str, 10);
		if((_key_verification_algorithm_rounds == 0) && ((read_str-_key_verification_algorithm_rounds_string) != strlen(_key_verification_algorithm_rounds_string)))
			return false;

		if(!parse_field(smp, length-*position-posadd, &posadd, "keyverifsalt", &_key_verification_salt))
			return false;

		if(!parse_field(smp, length-*position-posadd, &posadd, "keyverifsig", &_key_verification_text))
			return false;
	
	} else {
		_key_verification_algorithm = NULL;
		_key_verification_algorithm_rounds = 0;
		_key_verification_salt = NULL;
		_key_verification_text = NULL;
	}

	if(!parse_field(smp, length-*position-posadd, &posadd, "passcount", &_password_count_string))
		return false;

	_password_count = strtoul(_password_count_string, &read_str, 10);
	if((_password_count == 0) && ((read_str-_password_count_string) != strlen(_password_count_string)))
		return false;

	if((pseudosscanf(smp+posadd, "\n") != 1) && (_password_count != 0)) // Eat the newline if possible
		return false;
	else
		posadd += 1;

	*algorithm = _algorithm;
	*key_verifiable = _key_verifiable;
	*key_verification_algorithm = _key_verification_algorithm;
	*key_verification_algorithm_rounds = _key_verification_algorithm_rounds;
	*key_verification_salt = _key_verification_salt;
	*key_verification_text = _key_verification_text;
	*password_count = _password_count;
	*position += posadd;

	return true;
}

bool parse_metadata_password_string(unsigned char* serialized_metadata, unsigned int length, unsigned int* position, char** identifier, unsigned short* password_length, unsigned short* password_byte_length, unsigned short* password_encrypted_byte_length, char** format) {
	unsigned char* smp = serialized_metadata+*position;
	
	char* _identifier;
	char* _password_length_string;
	unsigned short _password_length;
	char* _password_byte_lenth_string;
	unsigned short _password_byte_length;
	char* _password_encrypted_byte_length_string;
	unsigned short _password_encrypted_byte_length;
	char* format_length_string;
	unsigned long format_length;
	char* _format;

	unsigned int posadd = 0;
	char* read_str;

	if(!parse_field(smp, length-*position-posadd, &posadd, "identifier", &_identifier))
		return false;
	
	if(!parse_field(smp, length-*position-posadd, &posadd, "encbytelen", &_password_encrypted_byte_length_string))
		return false;

	_password_encrypted_byte_length = (unsigned short) strtoul(_password_encrypted_byte_length_string, &read_str, 10);
	if((_password_encrypted_byte_length == 0) && ((read_str-_password_encrypted_byte_length_string) != strlen(_password_encrypted_byte_length_string)))
		return false;

	if(!parse_field(smp, length-*position-posadd, &posadd, "bytelen", &_password_byte_lenth_string))
		return false;

	_password_byte_length = (unsigned short) strtoul(_password_byte_lenth_string, &read_str, 10);
	if((_password_byte_length == 0) && ((read_str-_password_byte_lenth_string) != strlen(_password_byte_lenth_string)))
		return false;

	if(!parse_field(smp, length-*position-posadd, &posadd, "len", &_password_length_string))
		return false;
	
	_password_length = (unsigned short) strtoul(_password_length_string, &read_str, 10);
	if((_password_length == 0) && ((read_str-_password_length_string) != strlen(_password_length_string)))
		return false;
		
	if(!parse_field(smp, length-*position-posadd, &posadd, "formatlen", &format_length_string))
		return false;

	format_length = strtoul(format_length_string, &read_str, 10);
	if((format_length == 0) && ((read_str-format_length_string) != strlen(format_length_string)))
		return false;

	if(!parse_field_forced_len(smp, length-*position-posadd, &posadd, "format", &_format, format_length))
		return false;

	if(length-*position-posadd > 0)
		posadd += pseudosscanf(smp+posadd, "\n"); // Eat the newline if available
		
	*identifier = _identifier;
	*password_length = _password_length;
	*password_byte_length = _password_byte_length;
	*password_encrypted_byte_length = _password_encrypted_byte_length;
	*format = _format;
	*position += posadd;

	return true;
}

Store* deserialize_metadata_store(unsigned char* serialized_metadata, unsigned int length, unsigned int* position) {
	Store* store = construct_store();

	char* algorithm;
	bool key_verifiable;
	char* key_verification_algorithm;
	unsigned long key_verification_algorithm_rounds;
	char* key_verification_salt;
	char* key_verification_text;
	unsigned long password_count;

	if(!parse_metadata_store_string(serialized_metadata, length, position, &algorithm, &key_verifiable, &key_verification_algorithm, &key_verification_algorithm_rounds, &key_verification_salt, &key_verification_text, &password_count))
		return NULL;

	store->algorithm = algorithm;
	store->key_verifiable = key_verifiable;
	store->key_verification_algorithm = key_verification_algorithm;
	store->key_verification_algorithm_rounds = key_verification_algorithm_rounds;
	store->key_verification_salt = key_verification_salt;
	store->key_verification_text = key_verification_text;
	store->password_count = password_count;

	return store;
}

Password* deserialize_metadata_password(Store* store, unsigned char* serialized_metadata, unsigned int length, unsigned int* position) {
	Password* password = construct_password();
	password->store = store;

	char* identifier;
	unsigned short password_length;
	unsigned short password_byte_length;
	unsigned short password_encrypted_byte_length;
	char* format;

	if(!parse_metadata_password_string(serialized_metadata, length, position, &identifier, &password_length, &password_byte_length, &password_encrypted_byte_length, &format))
		return NULL;

	password->identifier = identifier;
	password->length = password_length;
	password->byte_length = password_byte_length;
	password->encrypted_byte_length = password_encrypted_byte_length;
	password->format = format;

	return password;
}

Store* deserialize(unsigned char* serialized_metadata, unsigned int serialized_metadata_length, unsigned char* serialized_password_sequence, unsigned int serialized_password_sequence_length) {
	unsigned int position = 0;

	Store* store = deserialize_metadata_store(serialized_metadata, serialized_metadata_length, &position);

	if(store == NULL)
		return NULL;

	Password** passwords = malloc(sizeof(Password*)*(store->password_count));
	Password* password = NULL;
	unsigned int bytepos = 0;

	for(unsigned long it = 0; it < store->password_count; it++) {
		password = deserialize_metadata_password(store, serialized_metadata, serialized_metadata_length, &position);

		if(password == NULL)
			return NULL;

		passwords[it] = password;

		if(bytepos+password->encrypted_byte_length > serialized_password_sequence_length)
			return NULL;

		char* encrypted_password = malloc(sizeof(char)*password->encrypted_byte_length);
		memcpy(encrypted_password, serialized_password_sequence+bytepos, password->encrypted_byte_length);
		bytepos += password->encrypted_byte_length;

		password->encrypted_password = encrypted_password;
	}

	store->passwords = passwords;

	return store;
}

bool save(Store* store, FILE* metadata_file, FILE* master_file) {
	int max_attempts = 128;

	unsigned char* serialized_metadata;
	unsigned int serialized_metadata_length;

	serialized_metadata = serialize_metadata(store, &serialized_metadata_length);

	unsigned char* serialized_password_sequence;
	unsigned int serialized_password_sequence_length;

	serialized_password_sequence = serialize_password_sequence(store, &serialized_password_sequence_length);
	
	if(!writefile(metadata_file, max_attempts, serialized_metadata, serialized_metadata_length)) {
		free(serialized_metadata);
		free(serialized_password_sequence);
		return false;
	}

	if(!writefile(master_file, max_attempts, serialized_password_sequence, serialized_password_sequence_length)) {
		free(serialized_metadata);
		free(serialized_password_sequence);
		return false;
	}

	return true;
}

bool load(FILE* metadata_file, FILE* master_file, Store** store) {
	int max_attempts = 128;
	unsigned int piece_length = 4096;

	unsigned char* serialized_metadata;
	unsigned int serialized_metadata_length;

	if(!readfile(metadata_file, max_attempts, piece_length, &serialized_metadata, &serialized_metadata_length))
		return false;

	unsigned char* serialized_password_sequence;
	unsigned int serialized_password_sequence_length;

	if(!readfile(master_file, max_attempts, piece_length, &serialized_password_sequence, &serialized_password_sequence_length)) {
		free(serialized_metadata);
		return false;
	}

	printf("%d\n", serialized_password_sequence_length);

	Store* _store = deserialize(serialized_metadata, serialized_metadata_length, serialized_password_sequence, serialized_password_sequence_length);

	free(serialized_metadata);
	free(serialized_password_sequence);

	if(_store == NULL)
		return false;

	*store = _store;

	return true;
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
	unsigned long long symbol;
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

bool copy_and_insert_key(Store* store, char* shuffled_key, char* shuffle_key, char* shuffle_key_format) {
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
