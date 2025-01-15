#ifndef UTIL_STRING_H
#define UTIL_STRING_H

char* strcpymalloc(char* string);

char* strappendrealloc(char* destination, unsigned int* destination_allocated_length, unsigned int piece_length, char* source);

char* strappendcharrealloc(char* destination, unsigned int* destination_allocated_length, unsigned int* current_destination_length, unsigned int piece_length, char source);

char* strtrimrealloc(char* string, unsigned int* allocated_length);

#endif
