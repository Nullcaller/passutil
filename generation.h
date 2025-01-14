#include "storage.h"

#ifndef GENERATION_H
#define GENERATION_H

void generate_shuffle_key(char** key_var, char* key_format);

unsigned char* generate_bytes(unsigned int length);

Password* generate_password(Store* store, char* format, unsigned int length);

Password* generate_password_and_append(Store* store, char* identifier, char* format, unsigned int length);

#endif
