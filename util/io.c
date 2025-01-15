#include<stdio.h>
#include<stdbool.h>

#include "io.h"

int pseudosscanf(char* string, char* check_string) {
	unsigned int length = strlen(check_string);

	int read = 0;
	for(unsigned int it = 0; it < length; it++)
		if(string[it] == check_string[it])
			read++;
		else
			break;

	return read;
}

bool readfile(FILE* file, int max_attempts, unsigned int piece_length, char** data, unsigned int* length) {
	char* _data = malloc(sizeof(char)*piece_length);
	unsigned int allocated_length = piece_length;
	unsigned int _length = 0;
	char* buf = malloc(sizeof(char)*piece_length);
	size_t read;
	int attempts = 0;

	while(!feof(file)) {
		read = fread(buf, sizeof(char), piece_length, file);
		
		if(read == 0)
			attempts++;
		else
			attempts = 0;
		
		if(attempts >= max_attempts) {
			free(buf);
			free(_data);
			return false;
		}
		
		if((_length+read) > allocated_length) {
			allocated_length = ((_length+read)/piece_length+1)*piece_length;
			_data = realloc(_data, allocated_length);
		}

		memcpy(_data+_length, buf, read);
		_length += read;
	}

	if(allocated_length > _length) {
		_data = realloc(_data, _length);
	}

	*data = _data;
	*length = _length;

	return true;
}

bool writefile(FILE* file, int max_attempts, char* data, unsigned int length) {
	unsigned int remaining_length = length;
	size_t result;
	int attempts = 0;
	while(remaining_length > 0) {
		result = fwrite(data+(length-remaining_length), sizeof(char), remaining_length, file);
		remaining_length -= result;
		if(result == 0) {
			attempts++;
		} else
			attempts = 0;
		if(attempts >= max_attempts)
			break;
	}
	if(remaining_length > 0)
		return false;
	return true;
}
