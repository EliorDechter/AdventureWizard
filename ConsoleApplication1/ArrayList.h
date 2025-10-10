#include <stdbool.h>
#include <assert.h>

typedef struct ArrayListNode
{
	bool occupied;
	int next_node;
} ArrayList32Node;

typedef struct ArrayList32
{
	ArrayList32Node nodes[32];
	int count;
	int first_node, last_node;
} ArrayList32;

int array_list_32_add(ArrayList32* list);
void array_list_32_delete(ArrayList32* list, int node);
int array_list_32_get_next(ArrayList32 *list, int *it);
int array_list_32_modulo(ArrayList32 *list, int index);