#include "ArrayList.h"

int array_list_32_add(ArrayList32 *list)
{
	bool found = false;
	int i = 1;
	for (; i < 32; i++)
	{
		if (!list->nodes[i].valid)
		{
			list->nodes[i].valid = true;
			if (list->last_node)
			{
				list->nodes[list->last_node].next_node = i;
			}
			list->last_node = i;
			list->count++;
			found = true;
			break;
		}
	}

	assert(found);

	return i;
}

void array_list_32_delete(ArrayList32* list, int node)
{
	int next_node = list->nodes[node].next_node;
	list->nodes[node] = (ArrayList32Node){ 0 };

	for (int i = 1; i < 32; ++i)
	{
		if (list->nodes[i].next_node == node)
		{
			list->nodes[i].next_node = next_node;
			list->count--;
			return;
		}
	}

	assert(0);
}

int array_list_32_get_next(ArrayList32 *list, int* it)
{
	int index = *it;
	if (!list->nodes[*it].valid)
		return 0;
	*it = list->nodes[*it].next_node;

	return index;
}