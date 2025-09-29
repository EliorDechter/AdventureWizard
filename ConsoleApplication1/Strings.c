#include "Strings.h"

str128 str128_create(const char* str) {
	str128 result = { 0 };
	strcpy(result.val, str);
	result.len = strlen(str);

	return result;
}

bool str128_equal(str128 a, str128 b) {
	bool result = false;
	if (strcmp(a.val, b.val) == 0) result = true;

	return result;
}


void str128_concat(str128* dest, const str128* src) {
	strcat(dest->val, src->val);
	dest->len = strlen(dest->val);
}