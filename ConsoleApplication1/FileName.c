#include "raylib.h"
#include <assert.h>
#include <stdio.h>
#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"
#include <float.h>
#include "json.h"
#include "raymath.h"
#include <stdarg.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define GAME_ORIGIN_X  5
#define GAME_ORIGIN_Y  5
#define GAME_SCREEN_WIDTH 900
#define GAME_SCREEN_HEIGHT  450

#define BACKGROUND_WIDTH 900
#define BACKGROUND_HEIGHT 350

#define INVENTORY_ORIGIN_X 0
#define INVENTORY_ORIGIN_Y BACKGROUND_HEIGHT
#define INVENTORY_WIDTH GAME_SCREEN_WIDTH
#define INVENTORY_HEIGHT 100

#define LOG_ORIGIN_X GAME_ORIGIN_X
#define LOG_ORIGIN_Y GAME_ORIGIN_Y + GAME_SCREEN_HEIGHT + INVENTORY_HEIGHT + 5
#define LOG_WIDTH GAME_SCREEN_WIDTH
#define LOG_HEIGHT 200

#define EDITOR_ORIGIN_X  GAME_ORIGIN_X + GAME_SCREEN_WIDTH + 5
#define EDITOR_ORIGIN_Y  GAME_ORIGIN_Y
#define EDITOR_WIDTH  300
#define EDITOR_HEIGHT  LOG_ORIGIN_Y + LOG_HEIGHT - EDITOR_ORIGIN_Y

#define WINDOW_SIZE_X GAME_ORIGIN_X + GAME_SCREEN_WIDTH + 5 + EDITOR_WIDTH + 5
#define WINDOW_SIZE_Y EDITOR_ORIGIN_Y + EDITOR_HEIGHT + 5

const int NUM_NODES_X = 90;
const int NUM_NODES_Y = 50;

bool is_playing;
bool run_slice_spritesheet;

bool g_start_drawing;
bool g_is_drawing;

RenderTexture2D target;
RenderTexture2D drop_down_panel_target;


/*
 * ===============================================================
 *
 *                          S_MISC
 *
 * ===============================================================
 */

#define MAX_NUM_HASHTABLE_ELEMENTS 32

typedef struct HashTableElement {
	char* key;
	int value;
} HashTableElement;

typedef struct HashTable {
	HashTableElement elements[MAX_NUM_HASHTABLE_ELEMENTS];
	int num;
} HashTable;

void AddToHashTable(HashTable* hashtable, char* key, int value) {
	HashTableElement element = {
		.key = key,
		.value = value
	};

	if (hashtable->num == 0) hashtable->num = 1;
	
	assert(hashtable->num < MAX_NUM_HASHTABLE_ELEMENTS - 1);
	hashtable->elements[hashtable->num++] = element;
}

int GetHashTableElement(HashTable hashtable, char* key) {
	for (int i = 1; i < hashtable.num; ++i) {
		if (strcmp(key,hashtable.elements[i].key) == 0)
			return hashtable.elements[i].value;
	}

	return 0;
}

void DrawTargetTexture(RenderTexture2D target, Vector2 position) {
	DrawTextureRec(target.texture, (Rectangle) {
		0, 0, (float)target.texture.width,
			(float)-target.texture.height
	}, position, WHITE);
}
char log_buffer[10000];
int log_buffer_size;
bool is_log_dirty;

void Log(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);

	char buffer[128 * 8] = { 0 };
	vsprintf(buffer, fmt, args);

	assert(log_buffer_size + strlen(buffer) < 10000);
	strcpy(log_buffer + log_buffer_size, buffer);
	log_buffer_size += strlen(buffer);

	//log_buffer[log_buffer_size - 1] = '\n';
	printf(buffer);

	va_end(args);

	is_log_dirty = true;
}

typedef enum Status { Status_Not_Running, Status_Running, Status_Ended } Status;

typedef struct Vector2i {
	int x, y;
} Vector2i;


Vector2 LerpPos(Vector2 pos, Vector2 end_pos) {
	float lerp_amount = 0.05;
	float delta = 5;
	if (abs(end_pos.x - pos.x) > delta) {
		pos.x = pos.x + lerp_amount * (end_pos.x - pos.x);
	}

	if (abs(end_pos.y - pos.y) > delta) {
		pos.y = pos.y + lerp_amount * (end_pos.y - pos.y);
	}

	return pos;
}

int GetSign(int n) {
	if (n < 0) return -1;
	else if (n > 0) return 1;
	return 0;
}

int pnpoly(int nvert, Vector2* vertices, Vector2 test)
{
	int i, j, c = 0;

	for (i = 0, j = nvert - 1; i < nvert; j = i++) {
		if (((vertices[i].y > test.y) != (vertices[j].y > test.y)) &&
			(test.x < (vertices[j].x - vertices[i].x) * (test.y - vertices[i].y) / (vertices[j].y - vertices[i].y) + vertices[i].x))
			c = !c;
	}

	return c;
}

bool IsInsidePolygon(Vector2* vertices_buff, Vector2 test) {
	if (pnpoly(arrlen(vertices_buff), vertices_buff, test)) {
		return true;
	}

	return false;
}

typedef struct PixelPos {
	Vector2 pos;
} PixelPos;

typedef struct NodePos {
	Vector2 pos;
} NodePos;

/*
 * ===============================================================
 *
 *                          S_STRINGS
 *
 * ===============================================================
 */

#define NAME_SIZE 30
#define LINE_SIZE 256
#define PARAGRAPH_SIZE LINE_SIZE * 8

typedef struct String {
	int size;
	int index;
	char* name;
} String;

typedef struct StringSystem {
	char* big_string;
	int size;
	int index;
} StringSystem;

StringSystem string_system;

void InitStringSystem() {
	string_system.size = 10000;
	string_system.big_string = (char*)malloc(sizeof(char) * 10000);
}

String CreateString(const char* str) {
	String result = (String){
		.size = strlen(str),
		.index = string_system.index,
		.name = string_system.index + string_system.big_string
	};
	strcpy(string_system.big_string + string_system.index, str);
	string_system.index += (strlen(str) + 1);

	return result;
}

char* GetString(String s) {
	if (!s.name) return 0;
	char* result = &string_system.big_string[s.index];

	return result;
}

/*
 * ===============================================================
 *
 *                          IO
 *
 * ===============================================================
 */
typedef enum InputEvent { InputEvent_None, InputEvent_LeftClickPressed, InputEvent_LeftClickReleased } InputEvent;

typedef struct InputSystem {
	InputEvent input_event;
	Vector2 mouse_pos;
	Vector2 previous_mouse_pos;
} InputSystem;

InputSystem input_system;

const char* LoadFile(const char* file_name) {
	FILE* file = fopen(file_name, "r");
	assert(file);

	fseek(file, 0, SEEK_END);
	int size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* data = malloc(size + 1);
	fread(data, size, 1, file);

	data[size] = '\0';

	fclose(file);

	return data;
}

void HandleInput() {
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		input_system.input_event = InputEvent_LeftClickPressed;
	}
	else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
		input_system.input_event = InputEvent_LeftClickReleased;
	}
	else {
		input_system.input_event = InputEvent_None;
	}

	input_system.previous_mouse_pos = input_system.mouse_pos;
	input_system.mouse_pos = GetMousePosition();

	input_system.mouse_pos.x -= 5;
	input_system.mouse_pos.y -= 5;
}

/*
 * ===============================================================
 *
 *                          TIMER
 *
 * ===============================================================
 */


typedef struct Timer {
	double time;
	double time_elapsed;
} Timer;

double total_time;

struct { char* key; Timer value; } *timer_map;

bool DoTimer(char* name, float time) {
	int i = shgeti(timer_map, name);
	if (i == -1) {
		Timer timer = {
			.time = time
		};

		shput(timer_map, name, timer);

		return true;
	}

	Timer* timer = &timer_map[i].value;
	timer->time_elapsed += 0.016;

	if (timer->time_elapsed <= timer->time) {
		return true;
	}
	else {
		shdel(timer_map, name);
		return false;
	}
}

void CountTime() {
	total_time += 0.016;
	assert(total_time <= DBL_MAX);
}
/*
 * ===============================================================
 *
 *                          S_TEXTURES
 *
 * ===============================================================
 */

typedef struct Texture2 {
	char name[NAME_SIZE];
	Texture2D texture;
} Texture2;

typedef struct SpriteSheet {
	Vector2i num_sprites;
	char texture_name[NAME_SIZE];
} SpriteSheet;

typedef struct TextureMap { char* key; Texture2 value; } TextureMap;

typedef struct TextureSystem {
	TextureMap* textures_map;

	TextureMap* spritesheet_map;
	TextureMap* inventory_map;

	char* texture_cache;
} TextureSystem;

TextureSystem texture_system;

typedef struct MyTexture {
	Vector2 num_sprites;
	Texture2D texture;
} MyTexture;

static void DrawTextureRect(Texture2D texture, Rectangle dest) {

	//draw red rectangle on error
	if (!texture.id) {
		DrawRectangleRec(dest, RED);
		return;
	}

	// draw
	DrawTexturePro(texture,
		(Rectangle) {
		0.0f, 0.0f, (float)texture.width, (float)texture.height
	},
		dest, (Vector2) { 0 }, (float)0, WHITE);
}

#if 1
static void DrawTextureRectCenter(Texture2D texture, Rectangle dest) {
	dest.x -= dest.width / 2;
	dest.y -= dest.height / 2;
	DrawTextureRect(texture, dest);
}
#endif

static void DrawTextureRectFlipped(Texture2D texture, Rectangle dest) {
#if 0
	dest.x += game_origin_x;
	dest.y += game_origin_y;
#endif
	DrawTexturePro(texture,
		(Rectangle) {
		0.0f, 0.0f, -1 * (float)texture.width, (float)texture.height
	},
		dest, (Vector2) { 0 }, (float)0, WHITE);
}

static void AddTextureToMap(TextureMap** map, const char* name, Texture2 texture) {

	if (shgeti(*map, name) != -1) {
		UnloadTexture(shget(*map, name).texture);
		shdel(*map, name);
	}

	//Log("Hello: %s %d %d\n", texture.name, texture.texture.width, texture.texture.width);

	shput(*map, name, texture);
}

static void AddTexture(const char* name, Texture2 texture) {
	AddTextureToMap(&texture_system.textures_map, name, texture);
}

static Texture2 GetTextureFromMap(TextureMap* map, const char* str) {
	if (!str || strcmp(str, "") == 0) return (Texture2) { 0 };
	int i = shgeti(map, str);
	if (i == -1) {
		Log("LOG: Failed to find the texture \"%s\"\n", str);
		return (Texture2) { 0 };
	}

	Texture2 t = map[i].value;

	return t;
}

Texture2 GetTexture(const char* str) {
	Texture2 res = GetTextureFromMap(texture_system.textures_map, str);

	return res;
}

static Texture2D LoadTexture1(const char* name) {
	//NOTE: breaks if not initialized to zero
	char str[256] = { 0 };
	strcat(str, "C:\\Users\\elior\\OneDrive\\Desktop\\ConsoleApplication1\\Resoruces\\Textures");
	strcat(str, name);

	Image image = LoadImage(str);
	if (!image.data) {
		printf("failed to load %s\n", name);
		return (Texture2D) { 0 };
	}
	Texture2D texture = LoadTextureFromImage(image);
	UnloadImage(image);

	return texture;
}

static void LoadTextures() {
	FilePathList files = LoadDirectoryFiles("C:\\Users\\elior\\OneDrive\\Desktop\\ConsoleApplication1\\Resoruces\\Textures");
	for (int i = 0; i < files.count; ++i) {
		char* name = GetFileNameWithoutExt(files.paths[i]);
		Texture2 texture = {
			.texture = LoadTexture(files.paths[i])
		};
		strcpy(texture.name, name);
		String s = CreateString(name);
		Log("LOG: Loaded texture \"%s\" size:%dx%d\n", s.name, texture.texture.width, texture.texture.height);
		AddTexture(s.name, texture);
	}

#if 1
	files = LoadDirectoryFiles("C:\\Users\\elior\\OneDrive\\Desktop\\ConsoleApplication1\\Resoruces\\Inventory");
	for (int i = 0; i < files.count; ++i) {
		char* name = GetFileNameWithoutExt(files.paths[i]);
		Texture2 texture = {
					.texture = LoadTexture(files.paths[i])
		};
		strcpy(texture.name, name);
		String s = CreateString(name);
		Log("LOG: Loaded texture \"%s\"\n", s.name);
		AddTextureToMap(&texture_system.inventory_map, s.name, texture);
	}
#endif
}


#if 0
static Animation LoadAnimation() {
	Animation animation = {
		.time_per_frame = 0.016,
		.valid = true,
		.is_spritesheet = true,
		.num_sprites = (Vector2){8,3},
		.row = 0,
		.num_frames = 8
	};

	arrput(animation.textures_buff, GetTexture("portal"));

	return animation;
}
#endif

/*
 * ===============================================================
 *
 *                          S_ANIMATION
 *
 * ===============================================================
 */

typedef struct Animation {
	bool valid;
	char name[NAME_SIZE];
	SpriteSheet sprite_sheet;
	double time_per_sprite;
	int current_frame;
	int row;
	Status status;
} Animation;

struct {
	int key;
	Animation* value;
} *animations_map;


void BeginAnimation(Animation* animation) {
	animation->status = Status_Running;
}

void EndAnimation(Animation* animation) {
	animation->status = Status_Not_Running;
}


void Animate(Animation* animation) {
	assert(animation->valid);
	assert(animation);
	assert(animation->sprite_sheet.num_sprites.x > 0 && animation->sprite_sheet.num_sprites.y > 0);

	if (DoTimer(animation->name, animation->time_per_sprite)) {
		return;
	}

	animation->current_frame = (animation->current_frame + 1) %
		(int)animation->sprite_sheet.num_sprites.y;
}

void DrawAnimation(Animation animation, Rectangle dest, bool flip_flag) {

	// handle bad input
	if (!animation.sprite_sheet.num_sprites.x || !animation.sprite_sheet.num_sprites.y) {
		DrawTextureRect((Texture2D) { 0 }, dest);
		return;
	}

	Texture2D texture = GetTexture(animation.sprite_sheet.texture_name).texture;
	Vector2i num_sprites = animation.sprite_sheet.num_sprites;
	int row = animation.row;

	float width = texture.width / num_sprites.y;
	float height = texture.height / num_sprites.x;
	float x = width * animation.current_frame;
	float y = height * row;

	Rectangle source = { x, y, width, height };

	int flip = 1;
	if (flip_flag) flip = -1;

	Rectangle rect = { x, y, width, height };
	DrawTexturePro(texture,
		source,
		dest, (Vector2) { 0 }, (float)0, WHITE);
}

Animation CreateAnimation(const char* name, SpriteSheet sprite_sheet, int row) {
	assert(row < sprite_sheet.num_sprites.x);

	Animation animation = {
			.valid = true,
			.time_per_sprite = 0.016,
			.sprite_sheet = sprite_sheet,
			.row = row,
	};

	strcpy(animation.name, name);

	return animation;
}


/*
 * ===============================================================
 *
 *                          S_Entities
 *
 * ===============================================================
 */

typedef enum EntityType { EntityType_None, EntityType_Background, EntityType_Screen, EntityType_ScreenItem, EntityType_Character, EntityType_InventoryItem, EntityType_Animation } EntityType;

typedef struct EntityId {
	char name[NAME_SIZE];
	int index;
	int generation;
} EntityId;


typedef struct Entity {
	char name[NAME_SIZE];
	NodePos node_pos;
	//char texture_name[NAME_SIZE];
	Texture2 texture;

	EntityId id;

	EntityType type;
	PixelPos pixel_pos;

	Vector2 size;
	//Rectangle rect;

	//Vector2 center;
	bool flipped;
	struct {
		char* key; Animation value;
	}  *animations_map;
	char* event;
	char* current_animation;

	bool is_grabbable;
} Entity;

typedef enum GrabStatus { GrabStatus_None, GrabStatus_Grabbing, GrabStatus_Releasing } GrabStatus;

#define MAX_NUM_ENTITIES 32

typedef struct EntitySystem {
	bool valid;

	Entity entities[MAX_NUM_ENTITIES];
	int num_entities;

	HashTable entities_map;

	EntityId hovered_entities[MAX_NUM_ENTITIES];
	int num_hovered_entities;

	EntityId hovered_entity;
	EntityId grabbed_entity;

	GrabStatus grab_status;
} EntitySystem;

EntitySystem entity_system;

#define MAX_NUM_STACK 64

typedef struct UndoSystem {
	EntitySystem entity_system_stack[MAX_NUM_STACK];
	int num_stack;
	size_t index;
} UndoSystem;

UndoSystem undo_system;

void Do() {
	undo_system.entity_system_stack[undo_system.num_stack] = entity_system;
	undo_system.index = undo_system.num_stack;
	undo_system.num_stack = (undo_system.num_stack + 1) % MAX_NUM_STACK;
}

EntitySystem Redo() {
	if (undo_system.index == undo_system.num_stack) return undo_system.entity_system_stack[undo_system.num_stack];
	undo_system.index = (undo_system.index) % MAX_NUM_STACK;
	return undo_system.entity_system_stack[undo_system.index];
}

EntitySystem Undo() {
	//if (undo_system.index == undo_system.num_stack) return undo_system.entity_system_stack[undo_system.num_stack];
	EntitySystem result = undo_system.entity_system_stack[undo_system.index];

	undo_system.index--;
	if (undo_system.index < 0) undo_system.index = MAX_NUM_STACK;

	return result;
}

void AddEntity(Entity entity) {
	assert(entity_system.num_entities < MAX_NUM_ENTITIES - 1);
	entity_system.entities[entity_system.num_entities++] = entity;
}

void AddHoveredEntity(EntityId entity) {
	assert(entity_system.num_hovered_entities < MAX_NUM_ENTITIES - 1);
	entity_system.hovered_entities[entity_system.num_hovered_entities++] = entity;
}

static Entity* GetEntity(EntityId id) {

	Entity* entity = &entity_system.entities[id.index];

	return entity;
}

static void DestroyEntity(EntityId id) {
	if (GetEntity(id)->type == EntityType_None) {
		Log("LOG: Failed to destroy entity\n");
		return;
	}
	Log("LOG: Entity \"%s\" destroyed\n", GetEntity(id)->name);

	Do();

	entity_system.entities[id.index] = (Entity){ 0 };
	//entity_system.num_entities--;
}

int CompareEntities(const void* element1, const void* element2) {
	EntityId* id1 = (EntityId*)element1;
	EntityId* id2 = (EntityId*)element2;
	Entity* e1 = GetEntity(*id1);
	Entity* e2 = GetEntity(*id2);

	if (e1->type > e2->type) return 1;
	if (e1->type < e2->type) return -1;
	return 0;
}

EntityId* GetOrderedEntities() {
	EntityId* ordered_entities = 0;
	for (int i = 0; i < entity_system.num_entities; ++i) {
		arrput(ordered_entities, entity_system.entities[i].id);
	}

	int _x = arrlen(ordered_entities);
	qsort(ordered_entities, arrlen(ordered_entities), sizeof(EntityId), CompareEntities);

	return ordered_entities;
}


void DestroyEntityByName(const char* str) {
	//TODO: delete from entity map too!
	for (int i = 0; i < entity_system.num_entities; ++i) {
		if (strcmp(str, entity_system.entities[i].name) == 0) {
			DestroyEntity(entity_system.entities[i].id);
		}
	}
}

void InitEntitySystem() {
	Entity e = { 0 };
	AddEntity(e);
	entity_system.valid = true;
}

Entity *GetEntityByName(char* name) {
	int index = GetHashTableElement(entity_system.entities_map, name);

	if (!index) {
		Log("LOG: Could not find entity \"%s\"\n", name);
	}

	Entity* result = &entity_system.entities[index];

	return result;
}

void ResetEntitySystem() {
	entity_system.num_entities = 0;
	entity_system.entities_map.num = 0;
	Entity nop = { 0 };
	AddEntity(nop);
	entity_system.num_hovered_entities = 0;
}

bool IsHovered(Rectangle r) {
	Vector2 mouse_pos = GetMousePosition();

	if (mouse_pos.x < r.x + r.width &&
		mouse_pos.x > r.x &&
		mouse_pos.y < r.y + r.height &&
		mouse_pos.y > r.y) {
		return true;
	}

	return false;
}

bool IsClicked(Rectangle r) {
	return IsHovered(r) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

PixelPos GetEntityPixelPos(EntityId id) {

	return  GetEntity(id)->pixel_pos;
}

PixelPos MovePos(PixelPos current_pos, PixelPos dest) {
	//TODO: warning! broken code!
	float speed = 1;

	Vector2 distance = Vector2Subtract(dest.pos, current_pos.pos);
	Vector2 direction = Vector2Normalize(distance);

	current_pos.pos.x += direction.x * speed;
	current_pos.pos.y += direction.y * speed;

	assert(current_pos.pos.x < GAME_SCREEN_WIDTH);
	assert(current_pos.pos.y < GAME_SCREEN_HEIGHT);

	return current_pos;
}

bool IsEntityNone(EntityId id) {
	Entity* e = GetEntity(id);
	return e->type == EntityType_None;
}
#pragma optimize( "", off )


static EntityId CreateEntity(Entity entity) {
	if (!*entity.name) return (EntityId) { 0 };

	entity.id = (EntityId){ .index = entity_system.num_entities };
	strcpy(entity.id.name, entity.name);
	AddEntity(entity);

	Entity* e = entity_system.entities + (entity_system.num_entities - 1);
	AddToHashTable(&entity_system.entities_map, e->name, entity_system.num_entities - 1);

	Log("LOG: Added the entity \"%s\"\n", entity.name);

	return entity.id;
}

Vector2 ScreenPosToCenterPos(Vector2 pos, Vector2 size) {
	Vector2 result = { pos.x + size.x / 2, pos.y + size.y / 2 };

	return result;
}

EntityId CreateBackground() {
	Vector2 size = (Vector2){ BACKGROUND_WIDTH, BACKGROUND_HEIGHT };
	Vector2 pos = ScreenPosToCenterPos((Vector2) { 0, 0 }, size);
	Entity entity = {
		.type = EntityType_Background,
		.pixel_pos = pos,
		.size = size,
		.texture = GetTexture("background"),
	};

	strcpy(entity.name, "background");

	EntityId id = CreateEntity(entity);

	return id;
}



bool IsIdEqual(EntityId id0, EntityId id1) {
	return id0.index == id1.index;
}

void InitEntities() {
}

Entity CreateCharacter(const char* name, Vector2 pos, Vector2 size) {
	//TODO: RETURN ID!!!
	Entity entity = {
		.type = EntityType_Character,
		.pixel_pos = { pos.x, pos.y},
		.size = {size.x, size.y},
		.texture = GetTexture(name),
	};

	strcpy(entity.name, name);

	EntityId id = CreateEntity(entity);

	return entity;
}

EntityId AddEntityToScreen(const char* name, Rectangle rect, EntityType type) {
	Entity entity = {
		//.rect = rect,
		.type = type,
		.pixel_pos = { rect.x, rect.y},
		.size = {rect.width, rect.height},
		//.center = { rect.x - rect.x / 2, rect.y - rect.y / 2},
		.texture = GetTexture(name)
	};

	strcpy(entity.name, name);

	EntityId id = CreateEntity(entity);

	return id;
}



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
		Log("LOG: Started action \"%s\"\n", action->name);
		action->status = Status_Running;
	}

	switch (action->type) {
	case ActionType_PickupItem: {

	}
	case ActionType_AnimateOnce: {
#if 0
		if (!action->animation) {
			EntityId id = AddEntityToScreen(action->name, (Rectangle) { action->pos.x, action->pos.y, action->size.x, action->size.y }, EntityType_Animation);
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
		AddEntityToScreen(action->name, (Rectangle) { action->pos.x, action->pos.y, action->size.x, action->size.y }, EntityType_Screen);
		action->status = Status_Ended;
		break;
	}
	}

	if (action->status == Status_Ended)
		Log("LOG: Ended action \"%s\"\n", action->name);
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
	Texture2D inventory_texture;
	Texture2D screen_texture;
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

static char* CheckScreenItemMatch(const char* name0, const char* name1) {


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
#define INVENTORY_ITEM_ORIGIN_Y INVENTORY_ORIGIN_Y + 50
#define INVENTORY_ITEM_SIZE 80
#define INVENTORY_ITEM_OFFSET 10

Vector2 GetItemPos(int index) {
	Vector2 origin = { INVENTORY_ITEM_ORIGIN_X, INVENTORY_ITEM_ORIGIN_Y };
	Vector2 size = { INVENTORY_ITEM_SIZE, INVENTORY_ITEM_SIZE };
	Vector2 offset = { INVENTORY_ITEM_OFFSET, INVENTORY_ITEM_OFFSET };

	Vector2 pos = { origin.x + index * (size.x + offset.x),
				origin.y, };

	return pos;
}

static void CreateItem(const char* name) {
	//TODO: check we don't add the same item twice

	Entity inventory_item = (Entity){
		.type = EntityType_InventoryItem,
		.pixel_pos = GetItemPos(arrlen(item_system.inventory_items_buff)),
		.size = {
				INVENTORY_ITEM_SIZE,
				INVENTORY_ITEM_SIZE
			},
		.texture = GetTextureFromMap(texture_system.inventory_map, name),
		.is_grabbable = true
	};

	strcpy(inventory_item.name, name);

	EntityId id = CreateEntity(inventory_item);

}

void AddItemToInventory(EntityId id) {
	arrput(item_system.inventory_items_buff, id);
}

static char* CheckItemMatch(const char* name0, const char* name1) {

	int x = entity_system.num_hovered_entities;

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


static void RemoveInventoryItem(EntityId id) {
	for (int i = 0; i < arrlen(item_system.inventory_items_buff); ++i) {
		if (IsIdEqual(item_system.inventory_items_buff[i], id)) {
			arrdel(item_system.inventory_items_buff, i);
			return;
		}
	}

	assert(0);
}

static bool DoInventoryItemInteractionWithInventoryItem(EntityId id) {

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

	Log("LOG: No match found between \"%s\" and \"%s\"", name0, name1);

	return false;

}

static void AddMatch(const char* name0, const char* name1, const char* result) {
	Match item_match = { 0 };
	strcpy(item_match.name0, name0);
	strcpy(item_match.name1, name1);

	hmput(item_system.item_matches_map, item_match, result);

	item_match = (Match){ 0 };
	strcpy(item_match.name0, name1);
	strcpy(item_match.name1, name0);

	hmput(item_system.item_matches_map, item_match, result);
}

static void AddScreenMatch(const char* name0, const char* name1, const char* result) {
	Match item_match = { 0 };
	strcpy(item_match.name0, name0);
	strcpy(item_match.name1, name1);

	hmput(item_system.screen_item_matches_map, item_match, result);

	item_match = (Match){ 0 };
	strcpy(item_match.name0, name1);
	strcpy(item_match.name1, name0);

	hmput(item_system.screen_item_matches_map, item_match, result);
}

static void AddScreenItem(const char* str, Rectangle rect) {
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

static void AddScreenItem2(const char* str, Rectangle rect) {
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

static void LoadScreenItems2() {
	//AddScreenItem2("ketchup", (Rectangle) { 400, 400, 50, 50 });
}

static void LoadInventory() {
	CreateItem("tool");
	CreateItem("tool2");
}

static void LoadMatches() {
	AddMatch("tool", "tool2", "dog");
}

static void LoadScreenMatches() {
	AddScreenMatch("ketchup", "arm", "wow much interaction");
}

void GrabItemToInventory() {
	//animate 
	//move to inventory
	//show some text
}

void DoInventory() {

}

/*
 * ===============================================================
 *
 *                          GAME STATE
 *
 * ===============================================================
 */

bool IsGrabbingItem() {
	return item_system.grabbed_item.index;
}

Rectangle GetEntityRect(Entity entity) {
	Rectangle rect = {
			entity.pixel_pos.pos.x - entity.size.x / 2,
			entity.pixel_pos.pos.y - entity.size.y / 2,
			entity.size.x,
			entity.size.y
	};

	return rect;
}

static void HandleEntityGrabbing() {
	//default released and hovered entities
	if (entity_system.grab_status == GrabStatus_Releasing) {
		entity_system.grabbed_entity = (EntityId){ 0 };
		entity_system.grab_status = GrabStatus_None;
	}
	entity_system.num_hovered_entities = 0;

	//get hovered entities
	EntityId* ordered_entities = GetOrderedEntities();
	for (int i = 0; i < arrlen(ordered_entities); ++i) {
		Entity current_entity = *GetEntity(ordered_entities[i]);

		Rectangle rect = GetEntityRect(current_entity);

		if (IsHovered(rect)) {
			AddHoveredEntity(current_entity.id);
		}
	}
	arrfree(ordered_entities);

	//get the first hovered entity
	if (entity_system.num_hovered_entities) {
		EntityId entity = entity_system.hovered_entities[entity_system.num_hovered_entities - 1];
		entity_system.hovered_entity = entity;
	}
	else {
		entity_system.hovered_entity = (EntityId){ 0 };
	}

	//asda
	if (input_system.input_event == InputEvent_LeftClickPressed && entity_system.hovered_entity.index) {
		if (!is_playing || GetEntity(entity_system.hovered_entity)->is_grabbable) {
			entity_system.grabbed_entity = entity_system.hovered_entity;
			entity_system.grab_status = GrabStatus_Grabbing;
		}
	}

	//get the released entity
	if (input_system.input_event == InputEvent_LeftClickReleased && entity_system.grabbed_entity.index) {
		entity_system.grab_status = GrabStatus_Releasing;
	}

}

void SetEntityPixelPos(EntityId id, Vector2 pos) {
	GetEntity(id)->pixel_pos = (PixelPos){ pos };
}

static void HandleItemGrabbing() {

	if (entity_system.grabbed_entity.index) {
		item_system.grabbed_item = entity_system.grabbed_entity;
		for (int i = 0; i < arrlen(item_system.inventory_items_buff); ++i) {
			if (IsIdEqual(item_system.grabbed_item, item_system.inventory_items_buff[i])) {
				item_system.grabbed_item_index = i;
			}
		}
	}
	else {
		item_system.grabbed_item = (EntityId){ 0 };
	}

	//check if released entity
	if (IsEntityNone(item_system.grabbed_item) || entity_system.grab_status != GrabStatus_Releasing) {
		return;
	}

	//get the most forefront entity that is not currently released item
	EntityId entity = { 0 };

	for (int i = 0; i < entity_system.num_hovered_entities; ++i) {
		EntityId current_entity = entity_system.hovered_entities[i];

		if (IsIdEqual(current_entity, entity_system.grabbed_entity)) {
			continue;
		}

		entity = current_entity;
	}

	bool interaction_success = false;

	switch (GetEntity(entity)->type) {
	case EntityType_None: {
		break;
	}
	case EntityType_InventoryItem: {
		interaction_success = DoInventoryItemInteractionWithInventoryItem(entity);
		break;
	}
	case EntityType_Screen: {
		interaction_success = DoInventoryItemInteractionWithScreenItem(entity);
		break;
	}
	}

	if (!interaction_success)
		SetEntityPixelPos(item_system.grabbed_item, GetItemPos(item_system.grabbed_item_index));
}






static void DrawEntities() {
	EntityId* ordered_entities = GetOrderedEntities();

	for (int i = 0; i < arrlen(ordered_entities); ++i) {
		Entity entity = *GetEntity(ordered_entities[i]);
		Texture2 texture = entity.texture;
		Rectangle rect = GetEntityRect(entity);

		if (entity.type == EntityType_None) continue;


		if (entity.current_animation) {
			Animation animation = shget(entity.animations_map, entity.current_animation);
			DrawAnimation(animation, rect, entity.flipped);
		}
		else {
			//TODO: draw center flipped
			if (!entity.flipped)
				DrawTextureRect(texture.texture, rect);
			else
				DrawTextureRectFlipped(texture.texture, rect);
		}

		if (!is_playing) {
			DrawRectangleLinesEx(rect, 1, (Color) { 150, 150, 150, 200 });
		}

	}

	arrfree(ordered_entities);
}

/*
 * ===============================================================
 *
 *                          PATHFINDING
 *
 * ===============================================================
 */

#define MAX_NUM_WALKING_AREA_VERTICES 128

typedef struct PathfindingSystem {
	//Vector2 num_nodes;

	NodePos* current_path_buff;
	int next_pos_index;
	//Vector2 end_pos;
	//Vector2 original_pos;
	bool is_moving;
	NodePos walking_area_vertices[MAX_NUM_WALKING_AREA_VERTICES];
	int num_vertices;

	struct { NodePos key; int value; } *obstacle_map;
} PathFindingSystem;

PathFindingSystem pathfinding_system;


void AddWalkingAreaVertex(NodePos node) {
	if (pathfinding_system.walking_area_vertices < MAX_NUM_WALKING_AREA_VERTICES - 1);
	pathfinding_system.walking_area_vertices[pathfinding_system.num_vertices++] = node;

}


NodePos GetNode(Vector2 pos) {
	assert(pos.x < NUM_NODES_X);
	assert(pos.y < NUM_NODES_Y);
	assert(pos.y < NUM_NODES_Y);
	assert(pos.y < NUM_NODES_Y);

	NodePos node = { pos };

	return node;
}

PixelPos GetPixelPosFromNodePos(NodePos node) {

	float width = GAME_SCREEN_WIDTH / NUM_NODES_X;
	float x = width * node.pos.x + width;

	float height = GAME_SCREEN_HEIGHT / NUM_NODES_Y;
	float y = height * node.pos.y + height;

	PixelPos result = (PixelPos){ x, y };

#if 0
	assert(result.pos.x < GAME_SCREEN_WIDTH);
	assert(result.pos.y < GAME_SCREEN_HEIGHT);
	assert(result.pos.x > 0);
	assert(result.pos.y > 0);
#endif

	return result;
}

void LoadPathFindingSystem() {
	//NOTE: must be before loading the vertices
	/*pathfinding_system = (PathFindingSystem){
		.num_nodes = (Vector2) {num_nodes_x, num_nodes_y}
	};*/
}

bool HasReachedPoint(PixelPos new_pos, PixelPos end_pos) {
	if (abs(end_pos.pos.x - new_pos.pos.x) < 0.01 && abs(end_pos.pos.y - new_pos.pos.y) < 0.01) return true;
	return false;
}

PixelPos Vector2ToPixelPos(Vector2 v) {
	return (PixelPos) { v };
}

NodePos GetNearestNodeNumFromPixelPos(PixelPos pos) {
	assert(pos.pos.x > 0 && pos.pos.y > 0 && pos.pos.x < GAME_SCREEN_WIDTH && pos.pos.y < GAME_SCREEN_HEIGHT);

	float width = GAME_SCREEN_WIDTH / NUM_NODES_X;
	int x = pos.pos.x / width;

	float height = GAME_SCREEN_HEIGHT / NUM_NODES_Y;
	int y = pos.pos.y / height;

	NodePos result = { x , y };

	return result;
}

bool IsMouseInsideGameScreen() {
	bool result = input_system.mouse_pos.x > GAME_ORIGIN_X && input_system.mouse_pos.x > GAME_ORIGIN_Y && input_system.mouse_pos.x < GAME_SCREEN_WIDTH && input_system.mouse_pos.y < GAME_SCREEN_HEIGHT;

	return result;
}

void DoWalkingAreaVertices() {

	if (!g_start_drawing && g_is_drawing) {
		g_is_drawing = false;
	}

	if (g_start_drawing) {
		if (!g_is_drawing && pathfinding_system.num_vertices != 0) {
			pathfinding_system.num_vertices = 0;
		}
		g_is_drawing = true;
	}

	if (g_is_drawing) {
		if (input_system.input_event == InputEvent_LeftClickPressed && IsMouseInsideGameScreen()) {
			AddWalkingAreaVertex(GetNearestNodeNumFromPixelPos(Vector2ToPixelPos(input_system.mouse_pos)));
		}
	}
}

bool IsObstacle(NodePos neighbor) {
	if (hmgeti(pathfinding_system.obstacle_map, neighbor) != -1)
		return true;

	NodePos right = (NodePos){ neighbor.pos.x + 1, neighbor.pos.y };
	NodePos left = (NodePos){ neighbor.pos.x - 1, neighbor.pos.y };
	NodePos down = (NodePos){ neighbor.pos.x, neighbor.pos.y - 1 };
	NodePos up = (NodePos){ neighbor.pos.x, neighbor.pos.y + 1 };

	if (hmgeti(pathfinding_system.obstacle_map, right) != -1 &&
		hmgeti(pathfinding_system.obstacle_map, down) != -1)
		return true;

	if (hmgeti(pathfinding_system.obstacle_map, up) != -1 &&
		hmgeti(pathfinding_system.obstacle_map, right) != -1)
		return true;

	if (hmgeti(pathfinding_system.obstacle_map, up) != -1 &&
		hmgeti(pathfinding_system.obstacle_map, left) != -1)
		return true;

	if (hmgeti(pathfinding_system.obstacle_map, left) != -1 &&
		hmgeti(pathfinding_system.obstacle_map, down) != -1)
		return true;

	if (neighbor.pos.x < 0 || neighbor.pos.y < 0 || neighbor.pos.x > GAME_SCREEN_WIDTH || neighbor.pos.y > GAME_SCREEN_HEIGHT)
		return true;

	return false;
}

float GetDistance(Vector2 pos0, Vector2 pos1) {

	float dx = fabs(pos0.x - pos1.x);
	float dy = fabs(pos0.y - pos1.y);
	//float result = dx + dy - (sqrt(2) - 2) * fmin(dx, dy);
	//float result = sqrt(pow(dx, 2) + pow(dy, 2));
	float result = sqrt(pow(dx, 2) + pow(dy, 2));

	return result;
}

float Heuristic(Vector2 pos0, Vector2 pos1) {
	float dx = fabs(pos0.x - pos1.x);
	float dy = fabs(pos0.y - pos1.y);
	float d2 = sqrt(pow(dx, 2) + pow(dy, 2));
	//float result = fabs(pos0.x - pos1.x) + fabs(pos1.y - pos1.y);
	float d1 = 1;
	float result = (dx + dy) * d1 + (d2 - 2 * d1) * fmin(dx, dy);

	return result;
}

NodePos* nodess;

Vector2* GetPath(NodePos origin_node, NodePos dest_node) {

	assert(!IsObstacle(origin_node));
	assert(!IsObstacle(dest_node));


	//initialize open list which contains nodes that may be expanded
	struct { Vector2 key; int value; } *open_map = 0;
	hmput(open_map, origin_node.pos, 0);

	//g map
	struct { Vector2 key; float value; } *g_map = 0;
	hmput(g_map, origin_node.pos, 0);

	//f map
	struct { Vector2 key; float value; } *f_map = 0;
	hmput(f_map, origin_node.pos, Heuristic(origin_node.pos, dest_node.pos));

	//initialize came from map
	struct { Vector2 key; Vector2 value; } *came_from_map = 0;

	while (hmlen(open_map)) {
		//current node
		Vector2 current_pos = origin_node.pos;

		//search for node with lowest f
		float min_f = FLT_MAX;

		for (int i = 0; i < hmlen(open_map); ++i) {
			float f;
			if (hmget(f_map, open_map[i].key) == -1) {
				f = FLT_MAX;
			}
			else {
				f = hmget(f_map, open_map[i].key);
			}
			if (f < min_f) {
				min_f = f;
				current_pos = open_map[i].key;
			}
		}


		assert(hmgeti(pathfinding_system.obstacle_map, current_pos) == -1);

		//remove node with lowest f from open list
		hmdel(open_map, current_pos);

		//check if we are at the destination
		if (Vector2Equals(current_pos, dest_node.pos)) {
			Vector2* path_reversed = 0;
			arrput(path_reversed, dest_node.pos);
			Vector2 current_pos = dest_node.pos;
			while (!Vector2Equals(current_pos, origin_node.pos)) {
				Vector2 previous_pos = hmget(came_from_map, current_pos);
				arrput(path_reversed, previous_pos);
				current_pos = previous_pos;
			}

			Vector2* path = 0;
			for (int i = arrlen(path_reversed) - 1; i >= 0; --i) {
				arrput(path, path_reversed[i]);
			}

			return path;
		}

		//get neighbors
		Vector2 neighbors[] = {
			{current_pos.x, current_pos.y + 1},
			{current_pos.x + 1 , current_pos.y + 1},
			{current_pos.x + 1, current_pos.y},
			{current_pos.x + 1, current_pos.y - 1},
			{current_pos.x, current_pos.y - 1},
			{current_pos.x - 1, current_pos.y - 1},
			{current_pos.x - 1, current_pos.y },
			{current_pos.x - 1, current_pos.y + 1},
		};

		for (int i = 0; i < 8; ++i) {

			if (IsObstacle((NodePos) { neighbors[i] })) {
				arrput(nodess, (NodePos) { neighbors[i] });
				continue;
			}

			//get distance to neighbor from current node
			float cost_to_neighbor = hmget(g_map, current_pos) + GetDistance(current_pos, neighbors[i]);

			//get g
			float g;

			if (hmgeti(g_map, neighbors[i]) == -1) {
				hmput(g_map, neighbors[i], g);
				g = FLT_MAX;
			}
			else {
				g = hmget(g_map, neighbors[i]);
			}

			//if new distance is shorter than the distance to  neighbor
			if (cost_to_neighbor < g) {
				/*printf("neighbor  %f %f: came from %f %f\n", neighbors[i].x, neighbors[i].y, current_pos.x, current_pos.y);
				printf("old and new g %f %f \n", g, cost_to_neighbor);*/

				//set prevous node
				if (hmgeti(came_from_map, neighbors[i]) == -1) {
					hmput(came_from_map, neighbors[i], current_pos);
				}
				else {
					hmgetp(came_from_map, neighbors[i])->value = current_pos;
				}

				//set new g
				hmgetp(g_map, neighbors[i])->value = cost_to_neighbor;

				//set new f
				if (hmgeti(f_map, neighbors[i]) == -1) {
					hmput(f_map, neighbors[i], cost_to_neighbor + Heuristic(neighbors[i], dest_node.pos));
				}
				else {
					hmgetp(f_map, neighbors[i])->value = cost_to_neighbor + Heuristic(neighbors[i], dest_node.pos);
				}


				if (hmgeti(open_map, neighbors[i]) == -1) {
					hmput(open_map, neighbors[i], 0);
				}
			}

		}
	}

	assert(0);
}

void DrawWalkingArea() {

	//draw lines and vertices
	int _x = arrlen(pathfinding_system.walking_area_vertices);
	for (int i = 0; i < arrlen(pathfinding_system.walking_area_vertices); ++i) {
		Vector2 v0 = GetPixelPosFromNodePos(pathfinding_system.walking_area_vertices[i]).pos;
		Vector2 v1 = GetPixelPosFromNodePos(pathfinding_system.walking_area_vertices[(i + 1) % arrlen(pathfinding_system.walking_area_vertices)]).pos;
		DrawLineEx(v0, v1, 1, RED);
		DrawRectangle(v0.x - 5, v0.y - 5, 10, 10, RED);
	}

	//draw nodes
	for (int i = 0; i < NUM_NODES_X - 1; ++i) {
		for (int j = 0; j < NUM_NODES_Y - 1; ++j) {
			PixelPos n = GetPixelPosFromNodePos((NodePos) { i, j });
			Color color = GRAY;
			color.a = 120;
			DrawCircle(n.pos.x, n.pos.y, 1, color);
		}
	}
}

PixelPos GetNearestPixelPosFromNodePos(NodePos pos) {
	float width = GAME_SCREEN_WIDTH / NUM_NODES_X;
	int x = pos.pos.x / width;

	float height = GAME_SCREEN_HEIGHT / NUM_NODES_Y;
	int y = pos.pos.y / height;

	PixelPos result = GetPixelPosFromNodePos((NodePos) { x, y });

	return result;
}

void StartMoveEntity(NodePos current_node, NodePos dest_node) {
	pathfinding_system.next_pos_index = 0;
	pathfinding_system.is_moving = true;

	pathfinding_system.current_path_buff = GetPath(current_node, dest_node);

	//StartEntityAnimation(game_state.player);

	//pathfinding_system.end_pos = input_system.mouse_pos;
	/*pathfinding_system.end_pos = GetNearestNodePos(input_system.mouse_pos);
	pathfinding_system.original_pos = GetPos(game_state.player);*/
}

void EndMoveEntity() {
	pathfinding_system.is_moving = false;
	pathfinding_system.next_pos_index = 0;
	//EndEntityAnimation(game_state.player);
}

void MoveEntity(EntityId id) {
	if (pathfinding_system.is_moving) {
		PixelPos pos = GetEntityPixelPos(id);
		NodePos next_pos = pathfinding_system.current_path_buff[pathfinding_system.next_pos_index];
		PixelPos new_pos = MovePos(pos, GetPixelPosFromNodePos(next_pos));
		//SetEntityPos(game_state.player, new_pos);
		GetEntity(id)->pixel_pos = new_pos;

		//printf("%f %f\n", new_pos.x, new_pos.y);

		if (HasReachedPoint(new_pos, GetPixelPosFromNodePos(next_pos))) {
			if (HasReachedPoint(new_pos, GetPixelPosFromNodePos(pathfinding_system.current_path_buff[arrlen(pathfinding_system.current_path_buff) - 1]))) {
				EndMoveEntity();
			}
			else {
				pathfinding_system.next_pos_index++;
				GetEntity(id)->node_pos = pathfinding_system.current_path_buff[pathfinding_system.next_pos_index];
			}
		}

		if (Vector2Subtract(next_pos.pos, GetEntityPixelPos(id).pos).x < 0) {
			GetEntity(id)->flipped = true;
		}
		else {
			GetEntity(id)->flipped = false;
		}
	}
}

void HandlePlayerMovement() {
	if (input_system.input_event == InputEvent_LeftClickReleased) {
		if (IsObstacle(GetNearestNodeNumFromPixelPos(Vector2ToPixelPos(input_system.mouse_pos)))) {
			return;
		}
		if (IsInsidePolygon(pathfinding_system.walking_area_vertices, GetNearestNodeNumFromPixelPos(Vector2ToPixelPos(input_system.mouse_pos)).pos)) {
			StartMoveEntity(GetEntityByName("player")->node_pos, GetNearestNodeNumFromPixelPos((PixelPos) { input_system.mouse_pos }));
		}
	}
}

NodePos* GetLine2(NodePos p0, NodePos p1) {
	NodePos* nodes = 0;

	float dx = p1.pos.x - p0.pos.x;
	float dy = p1.pos.y - p0.pos.y;

	float step = fmax(fabs(dx), fabs(dy));

	if (step) {
		float step_x = dx / step;
		float step_y = dy / step;

		for (int i = 0; i < step + 1; ++i) {
			float x = roundf(p0.pos.x + i * step_x);
			float y = roundf(p0.pos.y + i * step_y);
			NodePos n = (NodePos){ x, y };
			arrput(nodes, n);
		}
	}

	return nodes;
}

NodePos* GetLine(NodePos p0, NodePos p1) {

	NodePos* nodes = 0;


	int x0 = p0.pos.x;
	int y0 = p0.pos.y;
	int x1 = p1.pos.x;
	int y1 = p1.pos.y;

	if (x0 > x1) {
		int x = x1;
		x1 = x0;
		x0 = x;

		int y = y1;
		y1 = y0;
		y0 = y;
	}

	float m = (p1.pos.y - p0.pos.y) / (p1.pos.x - p0.pos.x);

	NodePos previous_pos = { 0 };


	for (int x = x0; x <= x1; x += 1) {
		float y = m * (x - x0) + y0;

		y = roundf(y);

		NodePos node = (NodePos){ x, y };

		if (!(node.pos.x == previous_pos.pos.x && node.pos.y == previous_pos.pos.y)) {
			arrput(nodes, node);
		}

		previous_pos = node;


	}

	int debug_num = arrlen(nodes);

	return nodes;
}

/*
  * ===============================================================
  *
  *                          SERIALIZATION
  *
  * ===============================================================
  */
#define get_file_code(a, b, c, d) (((int)(a) << 0 | (int)(b) << 8 | (int)(c) << 16 | (int)(d) << 24))
#define save_file_magic_value get_file_code('p', 'a', 's', 'f')

#pragma pack(push, 1)
typedef struct Save_file_header {
	int magic_value;
	int big_string_size;
	int num_vertices;
	int num_entities;
	int num_textures;
} Save_file_header;
#pragma pack(pop)

void SaveGame() {
	const char* save_file_name = "filey";
	FILE* file = fopen(save_file_name, "wb+");
	assert(file);

	int birg_string_size = string_system.size;
	int num_vertices = pathfinding_system.num_vertices;
	int num_entities = entity_system.num_entities;

	Save_file_header save_file_header = {
		.magic_value = save_file_magic_value,
		.big_string_size = birg_string_size,
		.num_vertices = num_vertices,
		.num_entities = num_entities,
	};

	fwrite(&save_file_header, 1, sizeof(Save_file_header), file);
	fwrite(string_system.big_string, string_system.size, 1, file);
	fwrite(pathfinding_system.walking_area_vertices, sizeof(Vector2), pathfinding_system.num_vertices, file);
	fwrite(entity_system.entities, sizeof(Entity), entity_system.num_entities, file);

	fclose(file);
}

void LoadGame(const char* save_file_name) {
	FILE* file = fopen(save_file_name, "rb");
	assert(file);

	fseek(file, 0, SEEK_END);
	int length = ftell(file);
	fseek(file, 0, SEEK_SET);

	assert(length);

	Save_file_header save_file_header = { 0 };
	fread(&save_file_header, 1, sizeof(Save_file_header), file);
	assert(save_file_header.magic_value == save_file_magic_value);

	fread(string_system.big_string, 1, save_file_header.big_string_size, file);

	for (int i = 0; i < save_file_header.num_vertices; ++i) {
		Vector2 v;
		fread(&v, sizeof(Vector2), 1, file);
		AddWalkingAreaVertex((NodePos) { v });
	}

	for (int i = 0; i < save_file_header.num_entities; ++i) {
		Entity e;
		fread(&e, sizeof(Entity), 1, file);
		AddEntity(e);
		AddToHashTable(&entity_system.entities_map, e.name, entity_system.entities + (entity_system.num_entities - 1));

	}
}



/*
 * ===============================================================
 *
 *                          S_PARSER
 *
 * ===============================================================
 */
#define NUM_ENTITIES 16
#define NUM_EVENTS 16

typedef struct Area {
	Entity entities[NUM_ENTITIES];
	int num_entities;

	Event events[NUM_EVENTS];
	int num_events;
} Area;

//typedef struct ConfigEntity {
//	char name[256];
//	char texture_name[256];
//	Vector2 pos;
//} ConfigEntity;

typedef struct Tokenizer {
	char* token;
	int len;
	int line_number;
	char* str;

	bool error;
	char error_name[256];

	Event event;
	Entity* entities_buff;
} Tokenizer;

Tokenizer tokenizer;

typedef struct Token {
	char* str;
	int size;
} Token;

bool IsTokenEqual(Token t0, const char* str) {
	if (str == 0 || *str == 0 || *str == EOF || t0.str == 0) return false;
	if (t0.size != (int)strlen(str)) return false;

	for (int i = 0; i < t0.size; ++i) {
		if (t0.str[i] != str[i]) return false;
	}

	return true;
}

bool Peek(const char* name) {
	if (tokenizer.error) return false;

	char* str = tokenizer.str;
	Token token = { 0 };

	while (isspace(*str)) {
		str++;
	}

	if (*str == '\0' && *name == '\0') return true;

	token.str = str;

	while (!isspace(*str) && *str != EOF && *str != 0) {
		str++;
		token.size++;
	}

	if (IsTokenEqual(token, name))
		return true;

	return false;
}

bool GetNextToken() {
	tokenizer.len = 0;

	if (*tokenizer.str == EOF || *tokenizer.str == 0) return false;

	while (isspace(*tokenizer.str) || *tokenizer.str == ',') {
		if (*tokenizer.str == '\n') {
			tokenizer.line_number++;
		}
		tokenizer.str++;
		//if (*tokenizer.current_string == EOF)
			//return false;
	}

	tokenizer.token = tokenizer.str;

	while (!isspace(*tokenizer.str) && *tokenizer.str != ',' && *tokenizer.str != EOF && *tokenizer.str != 0) {
		//if (*tokenizer.current_string == EOF || *tokenizer.current_string == 0) return false;

		tokenizer.str++;
		tokenizer.len++;
	}

	return true;
}



void Expect(const char* str) {
	if (tokenizer.error) return;

	GetNextToken();
	Token token = { tokenizer.token, tokenizer.len };
	bool result = IsTokenEqual(token, str);
	if (!result) {
		tokenizer.error = true;
		Log("[LOG] Expected \"%s\" at line %d\n", str, tokenizer.line_number);
	}
}

void StoreString(char* name) {
	if (tokenizer.error) return;

	GetNextToken();
	for (int i = 0; i < tokenizer.len; ++i) {
		char c = tokenizer.token[i];
		assert((c <= 'z' && c >= 'a') || (c >= '0' && c <= '9') || c == '_');
		*name = c;
		name++;
	}
	*name = 0;
}

void StoreInt(int* i) {
	if (tokenizer.error) return;

	GetNextToken();

	*i = atoi(tokenizer.token);
}


void StoreFloat(float* f) {
	if (tokenizer.error) return;

	GetNextToken();
	char s[256] = { 0 };
	for (int i = 0; i < tokenizer.len; ++i) {
		char c = tokenizer.token[i];
		assert((c == '.') || (c >= '0' && c <= '9'));
		s[i] = c;
	}

	*f = atof(s);
}

//bool OptionalExpect(const char *str) {
//	Token token = Peek(str);
//	bool result = IsTokenEqual(token, str);
//
//	return result;
//
//}

//void OptionalStoreString(char *name) {
//	for (int i = 0; i < tokenizer.len; ++i) {
//		char c = tokenizer.str[i];
//		assert((c <= 'z' && c >= 'a') || (c >= '0' && c <= '9'));
//		*name = c;
//	}
//}



typedef struct ConfigAction {
	char type_name[256];
	ActionType type;
	Vector2 pos;
	char depends_on[256];
} ConfigAction;

typedef struct ConfigEvent {
	char name[256];
	ConfigAction* config_action_buff;
} ConfigEvent;

SpriteSheet ParseSpriteData(const char* file_name) {
	char path[256] = { 0 };
	sprintf(path, "C:\\Users\\elior\\OneDrive\\Desktop\\ConsoleApplication1\\Resoruces\\%s", file_name);

	char* str = LoadFileText(path);

	if (!str) {
		tokenizer.error = true;
		return (SpriteSheet) { 0 };
	}

	tokenizer = (Tokenizer){ 0 };
	tokenizer.str = str;
	tokenizer.error = false;

	char texture_name[NAME_SIZE];
	int x, y, width, height, num_rows, num_columns;
	StoreString(texture_name);
	StoreInt(&x);
	StoreInt(&y);
	StoreInt(&width);
	StoreInt(&height);
	StoreInt(&num_rows);
	StoreInt(&num_columns);
	// TODO: store animation names

	SpriteSheet result = {
		.num_sprites = (Vector2i){num_rows, num_columns}
	};

	strcpy(result.texture_name, texture_name);

	return result;
}

void Parse(char* str) {
	if (!str) {
		tokenizer.error = true;
		return;
	}
	tokenizer = (Tokenizer){ 0 };
	tokenizer.str = str;
	tokenizer.error = false;

	while (isspace(*tokenizer.str)) tokenizer.str++;

	Expect("entities:");
	if (tokenizer.error) return;

	while (!tokenizer.error && !Peek("events:") && !Peek("\0")) {
		char name[NAME_SIZE];
		Vector2 pos = { 0 };
		Vector2 size = { 0 };

		Expect("name:");
		StoreString(name);

		Expect("pos:");
		StoreFloat(&pos.x);
		StoreFloat(&pos.y);

		Expect("size:");
		StoreFloat(&size.x);
		StoreFloat(&size.y);

		Entity e = CreateCharacter(name, pos, size);
		arrput(tokenizer.entities_buff, e);
	}

	if (Peek("\0"))
		return;

	//TODO: add more than one event
	Event event = { 0 };
	Expect("events:");
	Expect("name:");
	StoreString(event.name);

	Expect("actions:");
	while (!tokenizer.error && !Peek("\0")) {
		Action action = { 0 };
		Expect("name:");
		StoreString(action.name);

		Expect("type:");
		char type_name[NAME_SIZE];
		StoreString(type_name);

		if (strcmp(type_name, "animate_once") == 0) {
			action.type = ActionType_AnimateOnce;
		}
		else if (strcmp(type_name, "create_item") == 0) {
			action.type = ActionType_CreateItem;
		}
		else {
			assert(0);
		}

		Expect("pos:");
		StoreFloat(&action.pos.x);
		StoreFloat(&action.pos.y);

		if (Peek("depends_on:")) {
			GetNextToken();
			StoreString(&action.depends_on);
		}

		action.size = (Vector2){ 50, 50 };
		arrput(event.actions_buff, action);
	}

	tokenizer.event = event;
}

void InitGameFromText() {
	char* txt = LoadFileText("C:\\Users\\elior\\OneDrive\\Desktop\\ConsoleApplication1\\Resoruces\\config.txt");
	Parse(txt);
	if (!tokenizer.error) {
		Log("LOG: Successfuly loaded config file\n");
		//TODO: remove code from the parse function that creates entities, and move it here
		//      and also handle adding events
#if 0
		ResetEntitySystem();
		//add entities from parsed config file
		for (int i = 0; i < arrlen(tokenizer.entities_buff); ++i) {
			CreateEntity(tokenizer.entities_buff[i]);
		}
#endif
	}
	else {
		Log("LOG: Failed to load config file\n");
	}

}

/*
 * ===============================================================
 *
 *                          S_EDITOR
 *
 * ===============================================================
 */

typedef enum EditorWindow { EditorWindow_Default, EditorWindow_Lists, EditorWindow_Slice_Spritesheet } EditorWindow;

typedef struct Editor {
	char text0[64];
	char text1[64];
	Vector2 pos;
	Vector2 current_pos;
	Vector2 button_size;
	float gap;
	bool column_mode;
	char str[NAME_SIZE];
	bool drop_down_panel_active;
	char drop_down_panel_id[NAME_SIZE];
	Rectangle drop_down_panel_rec;
	int num_lines;
	Vector2 scroll;
	EditorWindow window;
} Editor;

Editor editor;

typedef enum EditorTab { EditorTab_Entities, EditorTab_Events, EditorTab_Areas, EditorTab_Inventory } EditorTab;

EditorWindow editor_window;
EditorTab editor_tab;
void EditorNext() {
	//TODO: stop using button size for all elements, consider storing the previous elements rect
	if (editor.column_mode) {
		editor.current_pos.x += editor.gap + editor.button_size.x;
	}
	else {
		editor.current_pos.y += editor.gap + editor.button_size.y;
	}
}
int EditorBeginRow() {
	editor.column_mode = true;

	return editor.current_pos.x;
}

void EditorEndRow(int original_x) {
	editor.column_mode = false;
	editor.current_pos.x = original_x;
	EditorNext();
}

bool EditorButton(const char* str) {
	Rectangle rect = { editor.current_pos.x, editor.current_pos.y, 110, 24 };

	bool result = EguiButton(rect, str);

	EditorNext();

	return result;
}

bool EditorButtonIcon(const char* str) {
	Rectangle rect = { editor.current_pos.x, editor.current_pos.y, editor.button_size.x, editor.button_size.y };

	bool result = EguiButton(rect, str);

	EditorNext();

	return result;
}


bool EditorToggleIcon(const char* str, bool* b) {
	Rectangle rect = { editor.current_pos.x, editor.current_pos.y, editor.button_size.x, editor.button_size.y };
	EguiToggle(rect, str, b);

	EditorNext();
}

bool EditorToggle(const char* str, bool* b) {
	Rectangle rect = { editor.current_pos.x, editor.current_pos.y, 70, 24 };
	EguiToggle(rect, str, b);

	EditorNext();
}

bool EditorLabel(const char* str) {
	Rectangle rect = { editor.current_pos.x, editor.current_pos.y, 256, editor.button_size.y };
	GuiLabel(rect, str);

	EditorNext();
}


bool EditorLabelButton(const char* str) {
	Rectangle rect = { editor.current_pos.x, editor.current_pos.y, 256, editor.button_size.y };
	bool result = EguiLabelButton(rect, str);

	EditorNext();

	return result;
}

bool EditorTextBox(char* str, bool active) {
	Rectangle rect = { editor.current_pos.x, editor.current_pos.y, 140, editor.button_size.y };
	bool res = GuiTextBox(rect, str, 11, active);

	EditorNext();

	return res;
}

bool EditorInputBox(const char* label, char* text_box, bool active) {
	int x = EditorBeginRow();
	EditorLabel(label);
	editor.current_pos.x += 60;
	bool res = EditorTextBox(text_box, active);
	EditorEndRow(x);


	return res;
}



void DeInitGame() {
	ResetEntitySystem();
}

//void EditorToggle(const char *str, bool *b) {
//	Rectangle r = { editor.pos.x, editor.pos.y, 50, 50 };
//	GuiToggle(r, str, b);
//
//	editor.pos.y += 10;
//}
//
//void EditorInput(const char* str) {
//	Rectangle r = { editor.pos.x, editor.pos.y, 50, 50 };
//	DrawText("Name:", rect.x, rect.y, 5, DARKGRAY);
//
//	rect.x += rect.width + 5;
//	GuiTextBox(rect, editor.text0, 100, false);
//}

void EditorBeginOffset(Vector2 offset) {
	editor.current_pos.x += offset.x;
	editor.current_pos.y += offset.y;
}

void EditorEndOffset(Vector2 offset) {
	editor.current_pos.x -= offset.x;
	editor.current_pos.y -= offset.y;
}

void EditorTabBar(char** str, int count, int* active) {
	EguiTabBar((Rectangle) { editor.current_pos.x, editor.current_pos.y, 10, 24 }, str, count, active);
	editor.current_pos.y += 24;

	EditorNext();
}

bool EditorBeginDropDownPanel(const char* id, Rectangle area_rect) {
	Vector2 panel_size = { 100, 300 };
	int z = 1;

	if (CheckCollisionPointRec(GetMousePosition(), area_rect) && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
		editor.drop_down_panel_active = true;
		strcpy(editor.drop_down_panel_id, id);
		editor.drop_down_panel_rec = (Rectangle){ GetMousePosition().x, GetMousePosition().y, panel_size.x, panel_size.y };
	}

	if (strcmp(editor.drop_down_panel_id, id) == 0 && editor.drop_down_panel_active) {
		if (!CheckCollisionPointRec(GetMousePosition(), editor.drop_down_panel_rec) &&
			(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))) {
			editor.drop_down_panel_active = false;
			DeActivatePanel(z);

			return false;
		}

		EguiBeginPanel(z, editor.drop_down_panel_rec, BLUE);

		return true;
	}

	return false;
}

void EditorEndDropDownPanel() {
	EguiEndPanel();
}

char multi_line_text[1024 * 10];

void EditorLog() {
	if (is_log_dirty) {
		const int max_chars_in_line = 20;
		int counter = 0;
		int line_len = 0;
		int line_num = 0;
		multi_line_text[counter++] = line_num + '0';
		multi_line_text[counter++] = ' ';
		char line_num_string[8] = { 0 };
		for (int i = 0; i < 1024 * 10; ++i) {
			if (log_buffer[i] == '\0') {
				multi_line_text[counter] = 0;
				break;
			}
			else if (log_buffer[i] == '\n') {
				line_len = 0;
				line_num++;
				multi_line_text[counter++] = '\n';
				sprintf(line_num_string, "%d", line_num);
				strcpy(multi_line_text + counter, line_num_string);
				counter += strlen(line_num_string);
				multi_line_text[counter++] = ' ';
			}
			else if (line_len == 50) {
				multi_line_text[counter++] = '\n';
				line_num++;
				multi_line_text[counter++] = line_num + '0';
				multi_line_text[counter++] = ' ';
				line_len = 0;
			}
			else {
				multi_line_text[counter++] = log_buffer[i];
			}
		}

		editor.num_lines = line_num;
		editor.scroll.y = -1 * 10 * (editor.num_lines - 10);
	}

	Rectangle scissorRec = { 0 };
	Rectangle log_rect = { LOG_ORIGIN_X, LOG_ORIGIN_Y, LOG_WIDTH, LOG_HEIGHT };
	if (is_log_dirty) {
		editor.scroll.y = -(10 * (editor.num_lines - 10));
	}
	GuiScrollPanel(log_rect, "asd", (Rectangle) { log_rect.x, log_rect.y, log_rect.width, 966 }, & editor.scroll, & scissorRec);
	BeginScissorMode(scissorRec.x, scissorRec.y, scissorRec.width, scissorRec.height);

	Rectangle label_rect1 = (Rectangle){ log_rect.x + 10, log_rect.y + editor.scroll.y + 30, log_rect.width, log_rect.height };
	GuiLabel(label_rect1, multi_line_text);

	EndScissorMode();
}
bool IsNumber(const char* str) {
	if (!str || str[0] == '\0')
		return false;
	int i = 0;
	if (str[0] == '-') i = 1;
	for (; i < strlen(str); ++i) {
		if (str[i] > '9' || str[i] < '0') return false;
	}

	return true;
}

void EditorSliceSpritesheet() {
	//TODO: function fails when x and y are in the center
	static char file_name[NAME_SIZE];
	static bool b1, b2, b3, b4, b5, b6, b7, b8;
	static bool do_it;
	static int num_rows = 1;
	static int num_columns = 1;
	static int x = 0;
	static int y = 0;
	static int width = GAME_SCREEN_WIDTH;
	static int height = GAME_SCREEN_HEIGHT;
	static Texture2D t;
	static char animation_names[NAME_SIZE * 10];
	static char rows_str[NAME_SIZE], columns_str[NAME_SIZE], x_str[NAME_SIZE], y_str[NAME_SIZE], height_str[NAME_SIZE], width_str[NAME_SIZE];
	static bool init;

	if (!init) {
		sprintf(rows_str, "%d", num_rows);
		sprintf(columns_str, "%d", num_columns);
		sprintf(x_str, "%d", x);
		sprintf(y_str, "%d", y);
		sprintf(width_str, "%d", width);
		sprintf(height_str, "%d", height);
		init = true;
	}

	EguiDrawPanel((Rectangle) { editor.current_pos.x, editor.current_pos.y - 5, EDITOR_WIDTH - 10, 600 }, WHITE, current_panel);

	Vector2 offset = (Vector2){ 5, 0 };
	EditorBeginOffset(offset);

	if (EditorInputBox("Texture:", file_name, b1)) {
		b1 = !b1;
	}

	if (EditorButton("Load texture")) {
		t = GetTexture(file_name).texture;
		sprintf(width_str, "%d", t.width);
		sprintf(height_str, "%d", t.height);
	}
	if (EditorInputBox("x:", x_str, b4)) {
		b4 = !b4;
	}
	if (EditorInputBox("y:", y_str, b5)) {
		b5 = !b5;
	}

	if (EditorInputBox("width:", width_str, b7)) {
		b7 = !b7;
	}
	if (EditorInputBox("height:", height_str, b8)) {
		b8 = !b8;
	}

	if (EditorInputBox("Rows:", rows_str, b2)) {
		b2 = !b2;
	}

	if (EditorInputBox("Columns:", columns_str, b3)) {
		b3 = !b3;
	}

	if (EditorInputBox("Animation names:", animation_names, b6)) {
		b6 = !b6;
	}


	if (IsNumber(rows_str)) {
		num_rows = TextToInteger(rows_str);
	}

	if (IsNumber(columns_str)) {
		num_columns = TextToInteger(columns_str);
	}

	if (IsNumber(x_str)) {
		x = TextToInteger(x_str);
	}

	if (IsNumber(y_str)) {
		y = TextToInteger(y_str);
	}

	if (IsNumber(width_str)) {
		width = TextToInteger(width_str);
	}

	if (IsNumber(height_str)) {
		height = TextToInteger(height_str);
	}

	if (EditorButton("Save!")) {
		if (file_name && file_name[0] != 0) {
			char text[1000] = { 0 };
			sprintf(text, "%s\n%d %d\n%d %d\n%d %d\n%s", file_name, x, y, width, height, num_rows, num_columns, animation_names);
			char path[256] = { 0 };
			sprintf(path, "C:\\Users\\elior\\OneDrive\\Desktop\\ConsoleApplication1\\Resoruces\\%s", file_name);
			SaveFileText(path, text);

			Log("Saved spritesheet data\n");
		}
	}

	float new_width, new_height;
	float w_ratio = 1, h_ratio = 1;

	if (t.id) {
		BeginTextureMode(target);
		float texture_ratio = t.width / t.height;
		float screen_ratio = GAME_SCREEN_WIDTH / GAME_SCREEN_HEIGHT;
		if (screen_ratio > texture_ratio) {
			new_width = t.width * GAME_SCREEN_HEIGHT / t.height;
			new_height = GAME_SCREEN_HEIGHT;
		}
		else {
			new_width = GAME_SCREEN_WIDTH;
			new_height = t.height * GAME_SCREEN_WIDTH / t.width;
			DrawTextureRect(t, (Rectangle) { 0, 0, GAME_SCREEN_WIDTH, t.height* GAME_SCREEN_WIDTH / t.width });
		}
		DrawTextureRect(t, (Rectangle) { 0, 0, new_width, new_height });

		w_ratio = new_width / t.width;
		h_ratio = new_height / t.height;

		EndTextureMode();
	}

	float delta_x = width * w_ratio / num_columns;
	float pos_x = x;
	for (int i = 0; i < num_columns + 1; ++i) {
		BeginTextureMode(target);
		DrawLineEx((Vector2) { pos_x, y }, (Vector2) { pos_x, height* h_ratio + y }, 2, BLUE);
		EndTextureMode();

		pos_x += delta_x;
	}

	float delta_y = height * h_ratio / num_rows;
	float pos_y = y;
	for (int i = 0; i < num_rows + 1; ++i) {
		BeginTextureMode(target);
		DrawLineEx((Vector2) { x, pos_y }, (Vector2) { width* w_ratio + x, pos_y }, 2, BLUE);
		EndTextureMode();
		pos_y += delta_y;
	}

	EditorEndOffset(offset);
}

void EditorChangeWindow(EditorWindow window) {
	if (window == editor.window) {
		editor.window = EditorWindow_Default;
		return;
	}

	editor.window = window;
}

Vector2 BeginDropwDownPanel() {
	BeginTextureMode(drop_down_panel_target);
	ClearBackground(WHITE);


	Rectangle rect = { 0, 0, 100, 300 };

	GuiPanel(rect, editor.drop_down_panel_id);
}

void EndDropwDownPanel() {
	EndTextureMode();
}

void DoEditor() {

	EguiBegin();

	strcpy(editor.text0, "editor0");
	strcpy(editor.text1, "editor1");

	editor.button_size = (Vector2){ 24, 24 };
	editor.pos = (Vector2){ EDITOR_ORIGIN_X + 5, EDITOR_ORIGIN_Y + 5 };
	editor.current_pos = editor.pos;
	editor.gap = 10;
	editor.column_mode = false;

	static bool show_add_entity_box;
	static bool show_add_item_box;

	EguiBeginPanel(0, (Rectangle) { EDITOR_ORIGIN_X, EDITOR_ORIGIN_Y, EDITOR_WIDTH, EDITOR_HEIGHT }, GRAY);

	int x = EditorBeginRow();
	{
		EditorToggleIcon("#152#", &is_playing);

		/*if (EditorButton("Load Config")) {
			DeInitGame();
			InitGameFromText();
		}*/
		static bool animate;
		if (EditorButtonIcon("#160#")) {
			animate = !animate;
			Entity* entity = GetEntityByName("player");
			Animation* animation = &shgetp(entity->animations_map, entity->current_animation)->value;
			if (animate)
				BeginAnimation(animation);
			else
				EndAnimation(animation);
		}

		if (EditorButtonIcon("#201#")) {
			LoadTextures();

			for (int i = 0; i < entity_system.num_entities; ++i) {
				Entity* entity = entity_system.entities + i;
				if (entity->type == EntityType_Character || entity->type == EntityType_Background)
					entity->texture = GetTexture(entity->texture.name);
				else {
					entity->texture = GetTextureFromMap(texture_system.inventory_map, entity->texture.name);
				}
			}
		}

		EditorToggleIcon("#177#", &g_start_drawing);

		if (EditorButtonIcon("#202#"))
			EditorChangeWindow(EditorWindow_Slice_Spritesheet);

		if (EditorButtonIcon("#203#")) {
			EditorChangeWindow(EditorWindow_Lists);
		}

		if (EditorButtonIcon("#208#")) {
			EntitySystem es = Redo();
			if (es.valid)
				entity_system = es;
		}

		if (EditorButtonIcon("#209#")) {
			EntitySystem es = Undo();
			if (es.valid)
				entity_system = es;
		}
	}
	EditorEndRow(x);

	Vector2 list_end = { 0 };

	if (editor.window == EditorWindow_Lists) {
		char* t0 = "Entities";
		char* t1 = "Events";
		char* t2 = "Areas";
		char* t3 = "Inventory";
		char* t[] = { t0, t1, t2, t3 };
		EditorTabBar(t, 4, &editor_tab);
		Rectangle panel_rect = { editor.current_pos.x, editor.current_pos.y, EDITOR_WIDTH - 10, 400 };
		EguiDrawPanel(panel_rect, WHITE, current_panel);

		if (entity_system.hovered_entity.index) {
			DrawText(GetEntity(entity_system.hovered_entity)->name, GetMousePosition().x, GetMousePosition().y, 10, BLUE);
		}

		/*
		static bool active;
		static Vector2 pos;

		if (CheckCollisionPointRec(GetMousePosition(), list_rect) && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){
			active = true;
			pos = GetMousePosition();
		}

		if (active) {
			Rectangle rec = { pos.x, pos.y, 50, 50 };
			if (GuiDropdownBox(rec, "hi", &active, true)) {
				active = false;
			}
		}*/
		//}

		//if (editor_tab == EditorTab_Entities) {
			//EditorBeginRow();
			/*if (EditorButton("Add Entity")) {
				editor_window = EditorWindow_AddEntity;
			}*/

			//EditorEndRow();
		if (editor_tab == EditorTab_Entities) {

			for (int i = 0; i < entity_system.num_entities; ++i) {
				if (entity_system.entities[i].type == EntityType_None) continue;
				int x = EditorBeginRow();
				Vector2 offset = { 5, 0 };
				EditorBeginOffset(offset);

				char entity_name[128];
				sprintf(entity_name, "#149#%s", entity_system.entities[i].name);

				Rectangle label_rect = { editor.current_pos.x, editor.current_pos.y, 256, editor.button_size.y };

				if (EditorBeginDropDownPanel(entity_system.entities[i].name, label_rect)) {

					Rectangle panel_rect = egui_panels[current_panel].rect;
					Rectangle label_rect = (Rectangle){ panel_rect.x + 5, panel_rect.y + 40, 256, editor.button_size.y };

					if (EguiLabelButton(label_rect, "Delete")) {
						DestroyEntity(GetEntityByName(editor.drop_down_panel_id)->id);
						editor.drop_down_panel_active = false;
						DeActivatePanel(current_panel);
					}



					EditorEndDropDownPanel();
				}


				EditorLabelButton(entity_name);
				//EditorLabelButton("Add entity");

				EditorEndOffset(offset);
				EditorEndRow(x);
			}

			if (EditorBeginDropDownPanel("add entity", (Rectangle) { editor.current_pos.x, editor.current_pos.y, 256, panel_rect.x + panel_rect.width })) {

				Rectangle panel_rect = egui_panels[current_panel].rect;
				Rectangle label_rect = (Rectangle){ panel_rect.x + 5, panel_rect.y + 40, 256, editor.button_size.y };
				if (EguiLabelButton(label_rect, "Add entity")) {
					show_add_entity_box = true;
				}

				EndDropwDownPanel();
#if 0
				GuiPanel(rect, editor.drop_down_panel_id);

				Rectangle label_rect = (Rectangle){ rect.x + 5, rect.y + 40, 256, editor.button_size.y };

				if (GuiLabelButton(label_rect, "Delete")) {
					DestroyEntity(GetEntityByName(editor.drop_down_panel_id)->id);
					editor.drop_down_panel_active = false;
				}

				label_rect.y += editor.button_size.y;
#endif
			}
		}
		else if (editor_tab == EditorTab_Inventory) {

			for (int i = 0; i < arrlen(entity_system.entities); ++i) {
				if (entity_system.entities[i].type != EntityType_InventoryItem) continue;

				int x = EditorBeginRow();
				Vector2 offset = { 5, 0 };
				EditorBeginOffset(offset);

				char entity_name[128];
				sprintf(entity_name, "#149#%s", entity_system.entities[i].name);

				Rectangle panel_rect = { 0 };
				Rectangle label_rect = { editor.current_pos.x, editor.current_pos.y, 256, editor.button_size.y };
				if (EditorBeginDropDownPanel(entity_system.entities[i].name, label_rect)) {
					BeginTextureMode(drop_down_panel_target);
					Rectangle rect = { 0, 0, 100, 300 };
					ClearBackground(WHITE);
					GuiPanel(rect, editor.drop_down_panel_id);

					Rectangle label_rect = (Rectangle){ rect.x + 5, rect.y + 40, 256, editor.button_size.y };

					if (GuiLabelButton(label_rect, "Delete")) {
						DestroyEntity(GetEntityByName(editor.drop_down_panel_id)->id);
						editor.drop_down_panel_active = false;
					}

					label_rect.y += editor.button_size.y;

					EndTextureMode();
				}

				EditorLabelButton(entity_name);
				//EditorLabelButton("Add entity");

				EditorEndOffset(offset);
				EditorEndRow(x);
			}

			EditorBeginDropDownPanel("add item", (Rectangle) { editor.current_pos.x, editor.current_pos.y, 256, panel_rect.x + panel_rect.width });
		}
	}
	else if (editor.window == EditorWindow_Slice_Spritesheet) {
		EditorSliceSpritesheet();
	}
	else if (editor.window == EditorWindow_Default) {

	}



	// Draw drop down panel
	if (editor.drop_down_panel_active) {
#if 0
		if (strcmp(editor.drop_down_panel_id, "add entity") == 0) {
			GuiPanel(editor.drop_down_panel_rec, editor.drop_down_panel_id);
			Rectangle label_rect = (Rectangle){ editor.drop_down_panel_rec.x + 5, editor.drop_down_panel_rec.y + 40, 256, editor.button_size.y };
			if (GuiLabelButton(label_rect, "Add entity")) {
				show_add_entity_box = true;
			}
		}
		else if (strcmp(editor.drop_down_panel_id, "add item") == 0) {
			GuiPanel(editor.drop_down_panel_rec, editor.drop_down_panel_id);
			Rectangle label_rect = (Rectangle){ editor.drop_down_panel_rec.x + 5, editor.drop_down_panel_rec.y + 40, 256, editor.button_size.y };
			if (GuiLabelButton(label_rect, "Add item")) {
				show_add_item_box = true;
			}

		}
#endif
		//else {

		//	GuiPanel(editor.drop_down_panel_rec, editor.drop_down_panel_id);

		//	Rectangle label_rect = (Rectangle){ editor.drop_down_panel_rec.x + 5, editor.drop_down_panel_rec.y + 40, 256, editor.button_size.y };

		//	if (GuiLabelButton(label_rect, "Delete")) {
		//		DestroyEntity(GetEntityByName(editor.drop_down_panel_id)->id);
		//		editor.drop_down_panel_active = false;
		//	}

		//	label_rect.y += editor.button_size.y;

		//}
	}

	if (show_add_entity_box) {
		Rectangle rect = (Rectangle){ 50, 50, 200, 200 };
		if (GuiWindowBox(rect, "hi!")) {
			show_add_entity_box = false;
		}

		Vector2 pos = { rect.x + 5, rect.y + 20 };

		editor.current_pos = pos;
		static bool bb;
		static char text_box[NAME_SIZE];
		if (EditorInputBox("name:", text_box, bb)) {
			bb = !bb;
		}

		if (EditorButton("create!")) {
			CreateCharacter(text_box, (Vector2) { 300, 300 }, (Vector2) { 50, 50 });
		}
	}
	else if (show_add_item_box) {
		Rectangle rect = (Rectangle){ 50, 50, 200, 200 };
		if (GuiWindowBox(rect, "hi!")) {
			show_add_entity_box = false;
		}

		Vector2 pos = { rect.x + 5, rect.y + 20 };

		editor.current_pos = pos;
		static bool bb;
		static char text_box[NAME_SIZE];
		if (EditorInputBox("name:", text_box, bb)) {
			bb = !bb;
		}

		if (EditorButton("create!")) {
			CreateItem(text_box);
		}
	}

	EguiEndPanel();
	EguiEnd();

	EditorLog();
}

/*
 * ===============================================================
 *
 *                          MAIN
 *
 * ===============================================================
 */

void AddArm() {
	EntityId arm = AddEntityToScreen("arm", (Rectangle) { 300, 300, 100, 100 }, EntityType_Screen);
}

void DoAnimations() {
	for (int i = 0; i < entity_system.num_entities; ++i) {
		if (entity_system.entities[i].type == EntityType_None) {
			continue;
		}

		if (entity_system.entities[i].current_animation) {
			int ii = shgeti(entity_system.entities[i].animations_map, entity_system.entities[i].current_animation);
			Animation* animation = &entity_system.entities[i].animations_map[ii].value;
			if (animation->status == Status_Running) {
				Animate(animation);
			}
		}
	}
}

void DrawCurrentPath() {

	for (int i = 0; i < arrlen(pathfinding_system.current_path_buff); ++i) {
		PixelPos pos = GetPixelPosFromNodePos(pathfinding_system.current_path_buff[i]);
		DrawRectangle(pos.pos.x - 5, pos.pos.y - 5, 10, 10, BLUE);
	}

	for (int j = 0; j < hmlen(pathfinding_system.obstacle_map); ++j) {
		PixelPos pos = GetPixelPosFromNodePos(pathfinding_system.obstacle_map[j].key);

		DrawRectangle(pos.pos.x - 5, pos.pos.y - 5, 10, 10, YELLOW);
	}
}



void RunGameLoop(RenderTexture2D target) {

	CountTime();

	HandleInput();

	HandleEntityGrabbing();

	//move grabbed entity, should this code be here?
	if (entity_system.grabbed_entity.index) {
		if (is_playing && GetEntity(entity_system.grabbed_entity)->type == EntityType_InventoryItem)
			GetEntity(entity_system.grabbed_entity)->pixel_pos.pos = Vector2Add(GetEntity(entity_system.grabbed_entity)->pixel_pos.pos,
				Vector2Subtract(input_system.mouse_pos, input_system.previous_mouse_pos));
		else if (!is_playing && GetEntity(entity_system.grabbed_entity)->type == EntityType_Character) {
			Vector2 mouse_pos = input_system.mouse_pos;

			//Vector2 pos = GetEntity(entity_system.grabbed_entity)->pixel_pos.pos;
			Vector2 size = GetEntity(entity_system.grabbed_entity)->size;
			Vector2 left_corner = { mouse_pos.x - size.x / 2, mouse_pos.y - size.y / 2 };
			Vector2 right_corner = { left_corner.x + size.x, left_corner.y + size.y };

			if (mouse_pos.x < GAME_SCREEN_WIDTH && mouse_pos.y < GAME_SCREEN_HEIGHT && mouse_pos.y > 0 && mouse_pos.x > 0 &&
				left_corner.x > 0 && left_corner.y > 0 && right_corner.x < GAME_SCREEN_WIDTH && right_corner.y < GAME_SCREEN_HEIGHT) {
				NodePos node_pos = GetNearestNodeNumFromPixelPos((PixelPos) { mouse_pos });
				GetEntity(entity_system.grabbed_entity)->pixel_pos = GetPixelPosFromNodePos(node_pos);
			}
		}
		//item_system.grabbed_item = entity_system.grabbed_entity;
	}

	DoInventory();
	BeginTextureMode(target);
	ClearBackground(WHITE);

	if (is_playing) {
		HandleItemGrabbing();
#if 0
		HandlePlayerMovement();
		MoveEntity();
		DoActions2();
#endif
	}
	DoAnimations();
	DrawEntities();
	EndTextureMode();
}



int main(void)
{
	is_playing = false;

	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
	InitWindow(WINDOW_SIZE_X, WINDOW_SIZE_Y, "my game");
	SetTargetFPS(60);

	GuiLoadIcons("C:\\Users\\elior\\OneDrive\\Desktop\\ConsoleApplication1\\Resoruces\\iconset.rgi", false);

	InitStringSystem();

	LoadTextures();

	InitEntitySystem();

	bool load = false;
	if (!load) {
		InitGameFromText();
		NodePos node_pos = (NodePos){ 20, 20 };
		PixelPos player_pixel_pos = GetPixelPosFromNodePos(node_pos);
		//game_state.player = AddEntityToScreen("player", CreateString("run"), (Rectangle) { player_pixel_pos.pos.x, player_pixel_pos.pos.y, 100, 100 }, EntityType_Character);
		GetEntityByName("player")->node_pos = node_pos;
	}
	else {
		LoadGame("filey");
		//game_state.player = GetEntityByName("player")->id;
	}

	LoadInventory();
	//LoadScreenItems2();

	LoadMatches();
	LoadScreenMatches();

	SpriteSheet sprite_sheet = ParseSpriteData("anim");
	Animation run_animation = CreateAnimation("run", sprite_sheet, 0);
	Animation idle_animation = CreateAnimation("idle", sprite_sheet, 1);

	Entity* player = GetEntityByName("player");
	shput(player->animations_map, "run", run_animation);
	shput(player->animations_map, "idle", idle_animation);
	player->current_animation = "idle";

	LoadPathFindingSystem();

#if 0
	for (int i = 0; i < arrlen(pathfinding_system.limit_nodes_buff); ++i) {
		NodePos* nodes = GetLine2(pathfinding_system.limit_nodes_buff[i], pathfinding_system.limit_nodes_buff[(i + 1) % arrlen(pathfinding_system.limit_nodes_buff)]);
		for (int j = 0; j < arrlen(nodes); ++j) {
			hmput(pathfinding_system.obstacle_map, nodes[j], 0);
		}
	}
#endif

	//StartEntityAnimation(GetEntity);


	target = LoadRenderTexture(GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT);
	BeginTextureMode(target);
	ClearBackground(WHITE);
	EndTextureMode();

	CreateBackground();

	drop_down_panel_target = LoadRenderTexture(100, 300);

	// main loop
	bool run_game_loop = true;
	bool close = false;

	//run_game_loop = false;
	//run_slice_spritesheet = true;

	while (!close)
	{
		if (WindowShouldClose() || IsKeyPressed(KEY_ESCAPE)) {
			SaveGame();
			close = true;
		}

		BeginTextureMode(target);
		ClearBackground(RAYWHITE);
		EndTextureMode();

		if (IsKeyReleased(KEY_P)) {
			run_game_loop = false;
		}

		if (run_game_loop)
			RunGameLoop(target);

		if (run_slice_spritesheet) {
			run_game_loop = false;

		}



		BeginDrawing();
		{
			ClearBackground(RAYWHITE);
#if 0
			BeginTextureMode(target);
			{
				if (!is_playing) {
					DoWalkingAreaVertices();
					DrawWalkingArea();
					DrawCurrentPath();
				}
			}
			EndTextureMode();
#endif

			DrawTargetTexture(target, (Vector2) { GAME_ORIGIN_X, GAME_ORIGIN_Y });
			DoEditor();
		}
		EndDrawing();

		is_log_dirty = false;
	}

	CloseWindow();

	return 0;
}