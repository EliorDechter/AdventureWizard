#pragma once

#define MAX_NAMES 128
#define MAX_NAME 32
typedef struct FileNames {
	char names[MAX_NAMES][MAX_NAME];
	int num;
} FileNames;


FileNames foo();