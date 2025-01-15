#include "storage.h"

#ifndef GENERATION_H
#define GENERATION_H

void generate_shuffle_key(char** key_var, char* key_format);

unsigned char* generate_bytes(unsigned int length);

#endif
