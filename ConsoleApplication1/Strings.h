#ifndef STRINGS_H
#define STRINGS_H

#include <stdbool.h>
#include <string.h>

typedef struct str128 {
	char val[128];
	int len;
} str128;

typedef struct Str32 {
	char val[32];
	int size;
} Str32;

str128 str128_create(const char* str);
bool str128_equal(str128 a, str128 b);
void str128_concat(str128* dest, const str128* src);
#endif