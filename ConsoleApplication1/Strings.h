#ifndef STRINGS_H
#define STRINGS_H

#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

typedef struct str1024 {
	char val[1024];
	size_t len;
} str1024;

typedef struct str128 {
	char val[128];
	int len;
} str128;

typedef struct Str32 {
	char val[32];
	int size;
} Str32;

str1024 str1024_create(const char* format, ...);
void str1024_concat(str1024* dest, str1024 src);
str128 str128_create(const char* format, ...);
bool str128_equal(str128 a, str128 b);
void str128_concat(str128* dest, str128 src);
#endif