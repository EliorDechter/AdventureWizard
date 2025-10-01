#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "Strings.h"
#include <string.h>

#define MAX_NUM_HASHTABLE_ELEMENTS 32

typedef struct hashtable_element {
	str128 key;
	int value;
} hashtable_element;

typedef struct hashtable {
	hashtable_element elements[MAX_NUM_HASHTABLE_ELEMENTS];
	int count;
} hashtable;

void hashtable_add(hashtable* hashtable, str128 key, int value);
int hashtable_get(hashtable hashtable, str128 key);
#endif