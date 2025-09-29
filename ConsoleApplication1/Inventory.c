#if 0
/*
 * ===============================================================
 *
 *                          S_INVENTORY
 *
 * ===============================================================
 */
typedef struct ItemMatch {
	char name0[64];
	char name1[64];
} Match;

typedef struct DatabaseItem {
	PlatformTexture inventory_texture;
	PlatformTexture screen_texture;
} DatabaseItem;

typedef struct ItemMix {
	char* name0;
	char* name1;
} ItemMix;

typedef struct ItemId {
	int num;
} ItemId;

typedef struct ItemSystem {
	struct { char* key; Entity value; } *screen_items_map;

	EntityId* inventory_items_buff;

	EntityId* screen_items_buff;

	struct { Match key; char* value; } *item_matches_map;
	struct { Match key; char* value; } *screen_item_matches_map;
	EntityId grabbed_item;
	int grabbed_item_index;

} ItemSystem;

ItemSystem item_system;

bool IsVector2Equal(Vector2 v0, Vector2 v1) {
	return v0.x == v1.x && v0.y == v0.y;
}

char* CheckScreenItemMatch(const char* name0, const char* name1) {


	Match item_match = { 0 };
	strcpy(item_match.name0, name0);
	strcpy(item_match.name1, name1);

	int i = hmgeti(item_system.screen_item_matches_map, item_match);
	if (i == -1) return 0;

	char* result = item_system.screen_item_matches_map[i].value;

	return result;
}

bool DoInventoryItemInteractionWithScreenItem(EntityId entity) {
	//show some text
	char* grabbed_name = GetEntity(item_system.grabbed_item)->name;
	char* s = CheckScreenItemMatch(grabbed_name, GetEntity(entity)->name);

	//AnimateOnce();


	return false;
}

#define INVENTORY_ITEM_ORIGIN_X 50
#define INVENTORY_ITEM_ORIGIN_Y INVENTORY_Y + 50
#define INVENTORY_ITEM_SIZE 80
#define INVENTORY_ITEM_OFFSET 10

Vector2i GetItemPos(int index) {
	Vector2i origin = { INVENTORY_ITEM_ORIGIN_X, INVENTORY_ITEM_ORIGIN_Y };
	Vector2i size = { INVENTORY_ITEM_SIZE, INVENTORY_ITEM_SIZE };
	Vector2i offset = { INVENTORY_ITEM_OFFSET, INVENTORY_ITEM_OFFSET };

	Vector2i pos = { origin.x + index * (size.x + offset.x),
				origin.y, };

	return pos;
}

EntityId CreateItem(const char* name) {
	//TODO: check we don't add the same item twice

	EntityId id = CreateEntity(name, EntityType_InventoryItem, GetItemPos(arrlen(item_system.inventory_items_buff)), (Vector2) { INVENTORY_ITEM_SIZE, INVENTORY_ITEM_SIZE }, GetTextureFromMap(texture_system.inventory_map, name));
	GetEntity(id)->is_grabbable = true;

	return id;
}

void AddItemToInventory(EntityId id) {
	arrput(item_system.inventory_items_buff, id);
}

char* CheckItemMatch(const char* name0, const char* name1) {

	int x = grab_system.num_hovered_entities;

	//for (int i = 0; i < arrlen(game_state.hovered_entities_buff); ++i) {
	//	printf("entity name: %s\n", game_state.hovered_entities_buff[i]);
	//}

	Match item_match = { 0 };
	strcpy(item_match.name0, name0);
	strcpy(item_match.name1, name1);

	int i = hmgeti(item_system.item_matches_map, item_match);
	if (i == -1) return 0;

	char* result = item_system.item_matches_map[i].value;

	return result;
}

void RemoveInventoryItem(EntityId id) {
	for (int i = 0; i < arrlen(item_system.inventory_items_buff); ++i) {
		if (IsIdEqual(item_system.inventory_items_buff[i], id)) {
			arrdel(item_system.inventory_items_buff, i);
			return;
		}
	}

	assert(0);
}

bool DoInventoryItemInteractionWithInventoryItem(EntityId id) {

	const char* name0 = GetEntity(id)->name;
	const char* name1 = GetEntity(item_system.grabbed_item)->name;

	const char* match = CheckItemMatch(name0, name1);

	if (match) {
		RemoveInventoryItem(id);
		RemoveInventoryItem(item_system.grabbed_item);

		DestroyEntity(id);
		DestroyEntity(item_system.grabbed_item);

		CreateItem(match);

		return true;
	}

	OldLog("LOG: No match found between \"%s\" and \"%s\"", name0, name1);

	return false;

}

void AddMatch(const char* name0, const char* name1, const char* result) {
	Match item_match = { 0 };
	strcpy(item_match.name0, name0);
	strcpy(item_match.name1, name1);

	hmput(item_system.item_matches_map, item_match, result);

	item_match = (Match){ 0 };
	strcpy(item_match.name0, name1);
	strcpy(item_match.name1, name0);

	hmput(item_system.item_matches_map, item_match, result);
}

void AddScreenMatch(const char* name0, const char* name1, const char* result) {
	Match item_match = { 0 };
	strcpy(item_match.name0, name0);
	strcpy(item_match.name1, name1);

	hmput(item_system.screen_item_matches_map, item_match, result);

	item_match = (Match){ 0 };
	strcpy(item_match.name0, name1);
	strcpy(item_match.name1, name0);

	hmput(item_system.screen_item_matches_map, item_match, result);
}

void AddScreenItem(const char* str, Rect rect) {
#if 0

	assert(0);
	int i = shgeti(item_system.item_database_map, str);
	assert(i != 1);
	Entity item = {
		.type = EntityType_ScreenItem,
		.name = CreateString(str),
		.pixel_pos = {rect.x, rect.y},
		.size = {rect.width, rect.height},
		.texture = item_system.item_database_map[i].value.screen_texture
	};

	shput(item_system.screen_items_map, str, item);
#endif
}

void AddScreenItem2(const char* str, Rect rect) {
	assert(0);
#if 0
	int i = shgeti(item_system.item_database_map, str);
	assert(i != 1);
	Entity item = {
		.type = EntityType_ScreenItem,
		//.name = str,
		.pixel_pos = {rect.x, rect.y},
		.size = {rect.width, rect.height},
		.texture = item_system.item_database_map[i].value.screen_texture
	};

	AddEntity(item);
	arrput(item_system.screen_items_buff, item.id);
#endif
}

void LoadScreenItems2() {
	//AddScreenItem2("ketchup", (PlatformRect) { 400, 400, 50, 50 });
}

void LoadInventory() {
	CreateItem("tool");
	CreateItem("tool2");
}

void LoadMatches() {
	AddMatch("tool", "tool2", "dog");
}

void LoadScreenMatches() {
	AddScreenMatch("ketchup", "arm", "wow much interaction");
}

void GrabItemToInventory() {
	//animate 
	//move to inventory
	//show some text
}
#endif