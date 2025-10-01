#pragma once
#include <stdbool.h>
#include <assert.h>

#define HANDLES_COUNT_MAX 256

typedef struct Handle {
	int generation;
	int index;
} Handle;

typedef struct Handle_internal {
	int generation;
	bool used;
} Handle_internal;

typedef struct Handle_map {
	Handle_internal handles[HANDLES_COUNT_MAX];
	int count;
} Handle_map;

typedef struct handle_iterator {
	int index;
	Handle_map* map;
} handle_iterator;

void handle_remove(Handle_map* map, Handle handle);
Handle handle_create(Handle_map* map);
int handle_get(const Handle_map* map, Handle handle);
void handle_remove_by_index(Handle_map* map, int index);
bool handle_get_next(const Handle_map* map, int* iterator_index, Handle* result_handle);
bool handle_equal(Handle a, Handle b);