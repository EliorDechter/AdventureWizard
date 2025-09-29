#include "HashTable.h"
#include <assert.h>

void hashtable_remove(hashtable* hashtable, str128 key) {
	for (int i = 0; i < MAX_NUM_HASHTABLE_ELEMENTS; ++i) {
		if (str128_equal(hashtable->elements[i].key, key)) {
			hashtable->elements[i] = (hashtable_element){ 0 };
		}
	}
}

void hashtable_add(hashtable* hashtable, str128 key, int value) {

	for (int i = 0; i < MAX_NUM_HASHTABLE_ELEMENTS; ++i) {
		if (strcmp(hashtable->elements[i].key.val, key) == 0) {
			hashtable->elements[i].value = value;
		}
	}

	hashtable_element element = {
		.key = key,
		.value = value
	};

	if (hashtable->count == 0) hashtable->count = 1;

	assert(hashtable->count < MAX_NUM_HASHTABLE_ELEMENTS - 1);
	hashtable->elements[hashtable->count++] = element;
}

int hashtable_get(hashtable hashtable, str128 key) {
	for (int i = 1; i < MAX_NUM_HASHTABLE_ELEMENTS; ++i) {
		if (strcmp(key, hashtable.elements[i].key) == 0)
			return hashtable.elements[i].value;
	}

	return 0;
}