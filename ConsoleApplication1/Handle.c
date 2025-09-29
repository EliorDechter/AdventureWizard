#include "Handle.h"

struct Handle_node;

void handle_remove(Handle_map* map, Handle handle) {
	Handle_internal* handle_internal = map->handles + handle.index_to_handle;

	assert(handle_internal->generation == handle.generation);

	handle_internal->used = false;
	handle_internal->generation++;

	map->count--;
}

Handle handle_create(Handle_map* map) {
	int handle_internal_index = -1;

	for (int i = 0; i < HANDLES_COUNT_MAX; ++i) {
		if (!map->handles[i].used) {
			map->handles[i].used = true;
			handle_internal_index = i;
			break;
		}
	}

	assert(handle_internal_index >= 0 && handle_internal_index < 256);

	Handle_internal* handle_internal = map->handles + handle_internal_index;

	Handle result = (Handle){
		.generation = handle_internal->generation,
		.index_to_handle = handle_internal_index
	};

	map->count++;

	return result;
}

int handle_get(const Handle_map* map, Handle handle) {
	Handle_internal handle_internal = map->handles[handle.index_to_handle];

	assert(handle_internal.generation == handle.generation);

	int result = handle.index_to_handle;

	return result;
}

void handle_remove_by_index(Handle_map* map, int index) {
	Handle_internal* handle_internal = map->handles + index;

	handle_internal->used = false;
	handle_internal->generation++;

	map->count--;
}

bool handle_iterator_get_next(Handle_map* map, int* index, int* result_index) {

	bool result = false;

	for (; *index < HANDLES_COUNT_MAX; *index = *index + 1) {
		if (map->handles[*index].used) {
			*result_index = *index;
			result = true;
		}
	}

	return result;
}

bool handle_iterator_get_next_handle(Handle_map* map, int* index, Handle* result_handle) {

	bool result = false;

	for (; *index < HANDLES_COUNT_MAX; *index = *index + 1) {
		if (map->handles[*index].used) {
			*result_handle = (Handle){
				.generation = map->handles[*index].generation,
				.index_to_handle = *index
			};
			result = true;
		}
	}

	return result;
}
