#if 0
/*
 * ===============================================================
 *
 *                          ACTIONS
 *
 * ===============================================================
 */


typedef enum ActionType { ActionType_None, ActionType_AnimateOnce, ActionType_PickupItem, ActionType_Text, ActionType_CreateItem } ActionType;

typedef struct Action {
	//initialization data
	char name[NAME_SIZE];
	ActionType type;
	String entity;
	char depends_on[NAME_SIZE];

	//run time data
	Status status;
	Vector2 pos;
	Vector2 size;
	Animation* animation;
	EntityId entity_id;
} Action;

typedef struct Event {
	char name[NAME_SIZE];
	Action* actions_buff;
} Event;

struct ActionNode;

typedef struct ActionNode {
	Action action;
	struct ActionNode* next;
} ActionNode;

typedef struct ActionSystem {
	struct { char* key; Event value; } *events_map;
	struct { char* key; Action value; } *actions_map;

	EntityId* temp_animation_entities_buff;

	ActionNode* action_list;
} ActionSystem;

ActionSystem action_system;

void AppendAction(Action action) {
	ActionNode* n = action_system.action_list;

	if (!n) {
		action_system.action_list = malloc(sizeof(ActionNode));
		*action_system.action_list = (ActionNode){
		.action = action
		};

		return;
	}

	while (n->next) {
		n = n->next;;
	}

	n->next = malloc(sizeof(ActionNode));
	*n->next = (ActionNode){
		.action = action
	};
}

void RemoveAction(char* name) {
	ActionNode* node = action_system.action_list;

	if (strcmp(name, node->action.name) == 0) {
		action_system.action_list = node->next;
		return;
	}

	node = node->next;
	ActionNode* previous_node = action_system.action_list;

	while (node != 0) {
		if (strcmp(name, node->action.name) != 0) {
			previous_node->next = node->next;
			return;
		}

		node = node->next;
		previous_node = previous_node->next;;
	}

	assert(0);
}

void DoEvent(Event event) {
	for (int i = 0; i < arrlen(event.actions_buff); ++i) {
		AppendAction(event.actions_buff[i]);
	}
}

Action* GetAction(const char* name) {
	int ii = shgeti(action_system.actions_map, name);
	assert(ii != -1);

	Action* action = &action_system.actions_map[ii].value;

	return action;
}

void DoAction(Action* action) {
	if (!action)
		return;

	if (action->status == Status_Not_Running) {
		OldLog("LOG: Started action \"%s\"\n", action->name);
		action->status = Status_Running;
	}

	switch (action->type) {
	case ActionType_PickupItem: {

	}
	case ActionType_AnimateOnce: {
#if 0
		if (!action->animation) {
			EntityId id = AddEntityToScreen(action->name, (PlatformRect) { action->pos.x, action->pos.y, action->size.x, action->size.y }, EntityType_Animation);
			GetEntity(id)->animation = CreateAnimation(action->name, (SpriteSheet) { 0 }, 0, 5);
			action->animation = &GetEntity(id)->animation;
			GetEntity(id)->animation.status = Status_Running;
			action->entity_id = id;

			Log("LOG: Animation \"%s\" started\n", action->animation->name);
		}
		else {
			if (action->animation->status == Status_Ended) {
				Log("LOG: Animation \"%s\" ended\n", action->animation->name);
				DestroyEntity(action->entity_id);
				action->status = Status_Ended;
			}
		}
#endif
		break;
	}
	case ActionType_CreateItem: {
#if FIX_ME
		AddEntityToScreen(action->name, (PlatformRect) { action->pos.x, action->pos.y, action->size.x, action->size.y }, EntityType_Screen);
#endif
		action->status = Status_Ended;
		break;
	}
	}

	if (action->status == Status_Ended)
		OldLog("LOG: Ended action \"%s\"\n", action->name);
}

void DoActions2() {
	for (ActionNode* a = action_system.action_list; a != 0; a = a->next) {

		if (*a->action.depends_on) {
			Action* required_action = 0;
			for (ActionNode* b = action_system.action_list; b != 0; b = b->next) {
				if (strcmp(b->action.name, a->action.depends_on) == 0) required_action = b;
			}

			if (!required_action) {
				DoAction(&a->action);
				if (a->action.status == Status_Ended)
					RemoveAction(a->action.name);
			}
		}
		else {
			DoAction(&a->action);
			if (a->action.status == Status_Ended)
				RemoveAction(a->action.name);
		}
	}
}

void LoadActions() {
	// player interacted with item
	// player says something 
	// open portal
	// add item to screen

	/*Action action0 = (Action){
	.name = "animate_portal",
	.type = ActionType_AnimateOnce,
	.action_object = "portal",
	.pos = (Vector2) {150, 150},
	.size = (Vector2) {100, 100}
	};

	Action action1 = (Action){
	.name = "create_dog",
	.type = ActionType_CreateItem,
	.action_object = "dog",
	.pos = (Vector2) {150, 150},
	.size = (Vector2) {50, 50},
	.depends_on = "animate_portal"
	};

	shput(action_system.actions_map, action0.name, action0);
	shput(action_system.actions_map, action1.name, action1);*/

}

void LoadActions2() {
	// player interacted with item
	// player says something 
	// open portal
	// add item to screen

	/*Action action0 = (Action){
	.name = "animate_portal",
	.type = ActionType_AnimateOnce,
	.action_object = "portal",
	.pos = (Vector2) {150, 150},
	.size = (Vector2) {100, 100}
	};

	Action action1 = (Action){
	.name = "create_dog",
	.type = ActionType_CreateItem,
	.action_object = "dog",
	.pos = (Vector2) {150, 150},
	.size = (Vector2) {50, 50},
	.depends_on = "animate_portal"
	};

	AppendAction(action0);
	AppendAction(action1);*/

}

#define NUM_EVENT_ACTIONS 16


/*typedef struct Action {
	char* name;
	ActionType type;
	char* action_object;
	Vector2 pos;
	Vector2 size;
	char* depends_on;
	bool is_done;

	Animation* animation;
	EntityId entity_id;
} Action;*/

#endif