#pragma once
#include <stdbool.h>
#include <assert.h>

#define HANDLES_COUNT_MAX 256



typedef struct Handle {
	int index_to_handle;
	int generation;
} Handle;

typedef struct Handle_internal {
	//int index_to_item;
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
bool handle_iterator_get_next(Handle_map *map, int *index, int *result_index);
bool handle_iterator_get_next_handle(Handle_map* map, int* index, Handle* result_handle);