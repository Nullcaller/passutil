#include<stdio.h>
#include<stdbool.h>

#ifndef UTIL_IO_H
#define UTIL_IO_H

int pseudosscanf(char* string, char* check_string);

bool readfile(FILE* file, int max_attempts, unsigned int piece_length, char** data, unsigned int* length);

bool writefile(FILE* file, int max_attempts, char* data, unsigned int length);

#endif
