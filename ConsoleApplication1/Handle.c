#include "Handle.h"

struct Handle_node;

void handle_remove(Handle_map* map, Handle handle) {
	Handle_internal* handle_internal = map->handles + handle.index;

	//assert(handle_internal->generation == handle.generation);

	handle_internal->used = false;
	handle_internal->generation++;

	map->count--;
}

Handle handle_create(Handle_map* map) {
	bool found = false;

	int index = 0;
	for (; index < HANDLES_COUNT_MAX; ++index) {
		if (!map->handles[index].used) {
			map->handles[index].used = true;
			found = true;
			break;
		}
	}

	assert(found);

	Handle result = (Handle){
		//.generation = generation,
		.index = index
	};

	map->count++;

	return result;
}

int handle_get(const Handle_map* map, Handle handle) {
	Handle_internal handle_internal = map->handles[handle.index];

	int result = 0;

	if (handle_internal.used)
		//&& handle_internal.generation == handle.generation)
	{
		result = handle.index;
	}

	return result;
}

void handle_remove_by_index(Handle_map* map, int index) {
	Handle_internal* handle_internal = map->handles + index;

	handle_internal->used = false;
	handle_internal->generation++;

	map->count--;
}

bool handle_get_next(const Handle_map* map, int* iterator_index, Handle* result_handle) {
	bool result = false;

	for (; *iterator_index < HANDLES_COUNT_MAX; *iterator_index = *iterator_index + 1) {
		if (result) break;
		if (map->handles[*iterator_index].used) {
			*result_handle = (Handle) {
				//.generation = map->handles[*iterator_index].generation,
				.index = *iterator_index
			};
			result = true;
		}
	}

	return result;
}

bool handle_is_equal(Handle a, Handle b) {
	bool result = a.index == b.index;
	//&& a.generation == b.generation;

	return result;
}

