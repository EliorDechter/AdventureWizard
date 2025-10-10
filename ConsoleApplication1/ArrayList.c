#include "ArrayList.h"

int array_list_32_add(ArrayList32* list)
{
	bool found = false;
	int i = 1;
	int last_node = list->last_node;

	for (; i < 32; i++)
	{
		if (!list->nodes[i].occupied)
		{
			list->nodes[i].occupied = true;
			list->nodes[i].next_node= list->first_node;
			list->last_node = i;
			list->count++;
			found = true;
			break;
		}
	}

	if (list->count == 1)
	{
		list->first_node = i;
	}

	list->nodes[last_node].next_node = i;

	assert(found);

	return i;
}

void array_list_32_delete(ArrayList32* list, int node)
{
	int next_node = list->nodes[node].next_node;
	list->nodes[node] = (ArrayList32Node){ 0 };

	if (list->first_node == node)
	{
		list->first_node = next_node;
		list->count--;


		if (node == list->last_node)
		{
			list->last_node = next_node;
		}

		return;
	}

	//  Find previous node
	for (int i = 1; i < 32; ++i)
	{
		if (list->nodes[i].next_node == node)
		{
			list->nodes[i].next_node = next_node;
			list->count--;

			if (node == list->last_node)
			{
				list->last_node = i;
			}

			return;
		}
	}

	assert(0);
}

int array_list_32_get_next(ArrayList32* list, int* it)
{
	if (!list->count)
	{
		return 0;
	}

	int index = *it;
	/*if (!list->nodes[*it].occupied)
		return 0;*/

	if (*it == list->last_node)
	{
		*it = 0;
	}
	else
	{
		*it = list->nodes[*it].next_node;
	}

	return index;
}

int array_list_32_modulo(ArrayList32* list, int index)
{
	int result = index;
	if (index - 1 == list->count)
		result = 1;
	return result;
}