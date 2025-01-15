#include<stdlib.h>
#include<string.h>

#include "string.h"

char* strcpymalloc(char* string) {
	char* new_str = malloc(strlen(string)+1);
	strcpy(new_str, string);
	return new_str;
}

char* strappendrealloc(char* destination, unsigned int* destination_allocated_length, unsigned int piece_length, char* source) {
	unsigned int destination_length;

	if(destination == NULL) {
		destination = malloc(sizeof(char)*piece_length);
		*destination_allocated_length = sizeof(char)*piece_length;
		destination_length = 0;
	} else {
		destination_length = strlen(destination);
	}

	unsigned int source_length = strlen(source);

	if((destination_length + source_length + 1) > *destination_allocated_length) {
		unsigned int new_length = ((destination_length + source_length + 1)/piece_length+1)*piece_length;
		destination = realloc(destination, sizeof(char)*new_length);
		*destination_allocated_length = new_length;
	}

	strcpy(destination+destination_length, source);
	return destination;
}

char* strappendcharrealloc(char* destination, unsigned int* destination_allocated_length, unsigned int* current_destination_length, unsigned int piece_length, char source) {
	if(destination == NULL) {
		destination = malloc(sizeof(char)*piece_length);
		*destination_allocated_length = sizeof(char)*piece_length;
		*current_destination_length = 0;
	}

	if((*current_destination_length+2) > *destination_allocated_length) {
		unsigned int new_length = ((*current_destination_length+2)/piece_length+1)*piece_length;
		destination = realloc(destination, sizeof(char)*new_length);
		*destination_allocated_length = new_length;
	}

	*(destination+*current_destination_length) = source;
	*(destination+*current_destination_length+1) = '\0';
	*current_destination_length += 1;

	return destination;
}

char* strtrimrealloc(char* string, unsigned int* allocated_length) {
	unsigned int length = strlen(string);

	string = realloc(string, length+1);
	*allocated_length = length+1;

	return string;
}
