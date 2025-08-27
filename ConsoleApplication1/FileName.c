
#if 0

// TODO:
// -log doesn't show first line
// -serialize entity animations map (currently segfaults)



#if 0
typedef struct PlatformRenderTexture {
	RenderTexture2D texture;
} PlatformRenderTexture;

typedef struct PlatformImage {
	Image image;
} PlatformImage;

typedef struct PlatformTexture {
	Texture2D texture;
} PlatformTexture;

typedef struct PlatformFont {
	Font font;
} PlatformFont;

PlatformImage PlatformImageLoad(const char* str) {
	PlatformImage result = (PlatformImage){ LoadImage(str) };

	return result;
}

void PlatformFontLoad() {

}

typedef struct Rect {
	float x, y, w, h;
} Rect;

RenderTexture2D PlatformLoadRenderTexture(int w, int h) {
	RenderTexture2D result = LoadRenderTexture(w, h);

	BeginTextureMode(result);
	ClearBackground(WHITE);
	EndTextureMode();

	return result;
}

void PlatformBeginDrawing() {
	BeginDrawing();
	ClearBackground(LIGHTGRAY);
}

void PlatformEndDrawing() {
	EndDrawing();
}

PlatformDrawText(const char* str, int x, int y) {
	DrawText(str, x, y, 13, BLACK);
}

PlatformTextureDraw(PlatformTexture texture, float x, float y, float w, float h) {
	DrawTexturePro(texture.texture,
		(Rectangle) {
		0, 0, (float)texture.texture.width, (float)-texture.texture.height
	},
		(Rectangle) {
		x, y, w, h
	},
		(Vector2) {
		0, 0
	},
		0,
		WHITE);
}

void PlatformRectDraw(Rect rect, PlatformColor color) {
	DrawRectangle(rect.x, rect.y, rect.w, rect.h, *((Color*)&color));
}
#else
#endif

#if 1
#define FIX_ME 0

#define NAME_SIZE 30
#include <assert.h>
#include <stdio.h>
#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"
#include <float.h>
#include "json.h"
#include <stdbool.h>

#include "raymath.h"
#include <stdarg.h>

//#include "raygui.h"
#include "editor.h"


// Gui definitions



const int NUM_NODES_X = 90;
const int NUM_NODES_Y = 50;

bool run_slice_spritesheet;

bool g_start_drawing;
bool g_is_drawing;
bool g_init;

#if 0
// Vector2 type
typedef struct Vector2 {
	float x;
	float y;
} Vector2;

// Vector3 type                 // -- ConvertHSVtoRGB(), ConvertRGBtoHSV()
typedef struct Vector3 {
	float x;
	float y;
	float z;
} Vector3;

// Color type, RGBA (32bit)
typedef struct Color {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
} Color;

// PlatformRect type
typedef struct PlatformRect {
	float x;
	float y;
	float width;
	float height;
} PlatformRect;

// TODO: PlatformTexture type is very coupled to raylib, required by Font type
// It should be redesigned to be provided by user
typedef struct PlatformTexture {
	unsigned int id;        // OpenGL texture id
	int width;              // Texture base width
	int height;             // Texture base height
	int mipmaps;            // Mipmap levels, 1 by default
	int format;             // Data format (PixelFormat type)
} PlatformTexture;

// Image, pixel data stored in CPU memory (RAM)
typedef struct Image {
	void* data;             // Image raw data
	int width;              // Image base width
	int height;             // Image base height
	int mipmaps;            // Mipmap levels, 1 by default
	int format;             // Data format (PixelFormat type)
} Image;

// Texture, tex data stored in GPU memory (VRAM)
typedef struct Texture {
	unsigned int id;        // OpenGL texture id
	int width;              // Texture base width
	int height;             // Texture base height
	int mipmaps;            // Mipmap levels, 1 by default
	int format;             // Data format (PixelFormat type)
} Texture;

typedef struct RenderTexture {
	unsigned int id;        // OpenGL framebuffer object id
	Texture texture;        // Color buffer attachment texture
	Texture depth;          // Depth buffer attachment texture
} RenderTexture;

typedef RenderTexture PlatformTexture;
#endif

//PlatformTexture target;
//PlatformTexture drop_down_panel_target;

/*
 * ===============================================================
 *
 *                          TIMER
 *
 * ===============================================================
 */

typedef struct TimeSystem {
	double total_time_in_seconds;
} TimeSystem;

TimeSystem time_system;

typedef struct Timer {
	double time;
	double time_elapsed;
} Timer;

struct { char* key; Timer value; } *timer_map;

typedef struct TimeInSeconds {
	double time;
} TimeInSeconds;

double ConvertSecondsToMilliseconds(TimeInSeconds time) {
	double result = time.time * 1000;

	return result;
}

#include <stdbool.h>


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

bool DoTimerSeconds(char* name, TimeInSeconds seconds) {
	DoTimer(name, ConvertSecondsToMilliseconds(seconds));
}

void CountTime() {
	time_system.total_time_in_seconds += 0.016;
	assert(time_system.total_time_in_seconds <= DBL_MAX);
}

/*
 * ===============================================================
 *
 *                          S_MISC
 *
 * ===============================================================
 */
typedef struct Vector2i {
	int x, y;
} Vector2i;

Vector2i Vector2iSubtract(Vector2i v0, Vector2i v1) {
	Vector2i result = { v0.x - v1.x, v0.y - v1.y };

	return result;
}

Vector2i Vector2iAdd(Vector2i v0, Vector2i v1) {
	Vector2i result = { v0.x + v1.x, v0.y + v1.y };

	return result;
}

#define MAX_NUM_HASHTABLE_ELEMENTS 32

typedef struct HashTableElement {
	char key[NAME_SIZE];
	int value;
} HashTableElement;

typedef struct HashTable {
	HashTableElement elements[MAX_NUM_HASHTABLE_ELEMENTS];
	int num;
} HashTable;

void RemoveFromHashTable(HashTable* hashtable, char* key) {
	for (int i = 0; i < MAX_NUM_HASHTABLE_ELEMENTS; ++i) {
		if (strcmp(hashtable->elements[i].key, key) == 0) hashtable->elements[i] = (HashTableElement){ 0 };
	}
}


#if 0
// Color, 4 components, R8G8B8A8 (32bit)
typedef struct Color {
	unsigned char r;        // Color red value
	unsigned char g;        // Color green value
	unsigned char b;        // Color blue value
	unsigned char a;        // Color alpha value
} Color;

#endif

void PlatformDrawTargetTexture(PlatformTexture target, Vector2 position) {
	DrawTextureRec(target.texture, (Rectangle) {
		0, 0, (float)target.texture.width,
			(float)-target.texture.height
	}, position, WHITE);
}

char log_buffer[10000];
int log_buffer_size;
bool is_log_dirty;

void OldLog(const char* fmt, ...) {
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

void Log(int line, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);

	char buffer[256] = { 0 };
	sprintf(buffer, "[LOG %d] ", line);

	char buffer2[256] = { 0 };
	vsprintf(buffer2, fmt, args);

	strcat(buffer, buffer2);
	strcat(buffer, "\n");

	assert(log_buffer_size + strlen(buffer) < 10000);
	strcpy(log_buffer + log_buffer_size, buffer);
	log_buffer_size += strlen(buffer);

	//log_buffer[log_buffer_size - 1] = '\n';
	printf(buffer);

	va_end(args);

	is_log_dirty = true;
}

typedef enum Status { Status_Not_Running, Status_Running, Status_Ended } Status;

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
	Vector2i pos;
} PixelPos;

typedef struct NodePos {
	Vector2i pos;
} NodePos;

/*
 * ===============================================================
 *
 *                          S_STRINGS
 *
 * ===============================================================
 */

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



#if 0
Animation LoadAnimation() {
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

void DrawAnimation(Animation animation, Rect dest, bool flip_flag) {

	// handle bad input
	if (!animation.sprite_sheet.num_sprites.x || !animation.sprite_sheet.num_sprites.y) {
		PlatformDrawTextureRect((PlatformTexture) { 0 }, dest);
		return;
	}

	PlatformTexture texture = GetSprite(animation.sprite_sheet.texture_name).texture;
	Vector2i num_sprites = animation.sprite_sheet.num_sprites;
	int row = animation.row;

	float width = texture.texture.width / num_sprites.y;
	float height = texture.texture.height / num_sprites.x;
	float x = width * animation.current_frame;
	float y = height * row;

	Rect source = { x, y, width, height };

	int flip = 1;
	if (flip_flag) flip = -1;


	PlatformTextureDraw(texture, dest.x, dest.y, dest.w, dest.h);

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
 *                          Section_Entities
 *
 * ===============================================================
 */

typedef enum EntityType { EntityType_None, EntityType_Background, EntityType_Screen, EntityType_ScreenItem, EntityType_Character, EntityType_InventoryItem, EntityType_Animation } EntityType;

typedef struct EntityId {
	//char name[NAME_SIZE];
	int index;
	int generation;
} EntityId;


typedef struct Entity {
	char name[NAME_SIZE];
	NodePos node_pos;
	//char texture_name[NAME_SIZE];
	Sprite sprite;

	EntityId id;

	EntityType type;
	PixelPos pixel_pos;

	Vector2 size;
	bool flipped;
	struct {
		char* key; Animation value;
	}  *animations_map;
	char* event;
	char* current_animation;

	bool is_grabbable;

	bool is_locked;
} Entity;

typedef enum GrabStatus { GrabStatus_None, GrabStatus_Grabbing, GrabStatus_Releasing } GrabStatus;

#define MAX_NUM_ENTITIES 32

typedef struct EntitySystem {
	bool valid;

	// Serializable

	Entity entities[MAX_NUM_ENTITIES];

	EntityId available_handles[MAX_NUM_ENTITIES];
	int num_available_handles;

	HashTable entities_map;

} EntitySystem;
EntitySystem entity_system;

typedef struct GrabSystem {

	EntityId hovered_entities[MAX_NUM_ENTITIES];
	int num_hovered_entities;

	EntityId hovered_entity;
	EntityId grabbed_entity;

	GrabStatus grab_status;
} GrabSystem;

GrabSystem grab_system;

EntityId GetAvailableId() {
	if (!entity_system.num_available_handles) {
		Log(__LINE__, "No available entity handles");

		return (EntityId) { 0 };
	}

	EntityId result = entity_system.available_handles[entity_system.num_available_handles - 1];
	entity_system.num_available_handles--;

	return result;
}

void DestroyId(EntityId id) {
	assert(entity_system.num_available_handles < MAX_NUM_ENTITIES);
	entity_system.available_handles[entity_system.num_available_handles++] = (EntityId){ .index = id.index, .generation = id.generation + 1 };
}

#define MAX_NUM_STACK 64

typedef struct UndoSystem {
	EntitySystem entity_system_stack[MAX_NUM_STACK];
	int num_stack;
	size_t index;
} UndoSystem;

UndoSystem undo_system;

void UndoSystemDo() {
	undo_system.entity_system_stack[undo_system.num_stack] = entity_system;
	undo_system.index = undo_system.num_stack;
	undo_system.num_stack = (undo_system.num_stack + 1) % MAX_NUM_STACK;
}

EntitySystem UndoSystemRedo() {
	if (undo_system.index == undo_system.num_stack) return undo_system.entity_system_stack[undo_system.num_stack];
	undo_system.index = (undo_system.index) % MAX_NUM_STACK;
	return undo_system.entity_system_stack[undo_system.index];
}

EntitySystem UndoSystemUndo() {
	//if (undo_system.index == undo_system.num_stack) return undo_system.entity_system_stack[undo_system.num_stack];
	EntitySystem result = undo_system.entity_system_stack[undo_system.index];

	if (undo_system.index) undo_system.index--;

	return result;
}

void AddHoveredEntity(EntityId entity) {

}

Entity* GetEntity(EntityId id) {

	Entity* entity = &entity_system.entities[id.index];

	return entity;
}

void DestroyEntity(EntityId id) {
	if (GetEntity(id)->type == EntityType_None) {
		Log(__LINE__, "Failed to destroy entity with id %d,%d\n", id.index, id.generation);
		return;
	}
	Log(__LINE__, "Entity \"%s\" destroyed\n", GetEntity(id)->name);

	UndoSystemDo();

	RemoveFromHashTable(&entity_system.entities_map, GetEntity(id)->name);
	entity_system.entities[id.index] = (Entity){ 0 };

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
	V2 mouse_pos;
	V2 previous_mouse_pos;
	State left_mouse, right_mouse;
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
	if (input_system.left_mouse == Active) {
		input_system.input_event = InputEvent_LeftClickPressed;
	}
	else if (input_system.left_mouse == Deactivating) {
		input_system.input_event = InputEvent_LeftClickReleased;
	}
	else {
		input_system.input_event = InputEvent_None;
	}

	input_system.previous_mouse_pos = input_system.mouse_pos;
	input_system.mouse_pos = (V2){ platform_system.mouse_x, platform_system.mouse_y };

	input_system.mouse_pos.x -= GAME_SCREEN_X;
	input_system.mouse_pos.y -= GAME_SCREEN_Y;
}


/*
 * ===============================================================
 *
 *                          S_TEXTURES
 *
 * ===============================================================
 */

typedef enum { SpriteType_None, SpriteType_Texture, SpriteType_Rect, SpriteType_Text } SpriteType;

typedef struct Sprite {
	char name[NAME_SIZE];
	SpriteType type;
	PlatformColor color;
	union {
		PlatformTexture texture;
		char text[NAME_SIZE];
	};
} Sprite;

typedef struct SpriteSheet {
	Vector2i num_sprites;
	char texture_name[NAME_SIZE];
} SpriteSheet;

typedef struct TextureMap { char* key; Sprite value; } TextureMap;

typedef struct TextureSystem {
	TextureMap* textures_map;

	TextureMap* spritesheet_map;
	TextureMap* inventory_map;

	char* texture_cache;
} TextureSystem;

TextureSystem texture_system;

void PlatformDrawTextureRect(PlatformTexture texture, Rect dest) {
	//draw red PlatformRect on error
	if (!texture.texture.id) {
		PlatformRectDraw(dest, PLATFORM_RED);
		return;
	}

	// draw
	DrawTexturePro(texture.texture,
		(Rectangle) {
		0.0f, 0.0f, (float)texture.texture.width, (float)texture.texture.height
	},
		* ((Rectangle*)&dest), (Vector2) { 0 }, (float)0, WHITE);
}

void DrawTextureRectFlipped(PlatformTexture texture, Rect dest) {

	//TODO: Flip!
	PlatformTextureDraw(texture, dest.x, dest.y, dest.w, dest.h);
}

void AddTextureToMap(TextureMap** map, const char* name, Sprite texture) {

	if (shgeti(*map, name) != -1) {
		UnloadTexture(shget(*map, name).texture.texture);
		shdel(*map, name);
	}

	//Log("Hello: %s %d %d\n", texture.name, texture.texture.width, texture.texture.width);

	shput(*map, name, texture);
}

void AddTexture(const char* name, Sprite texture) {
	AddTextureToMap(&texture_system.textures_map, name, texture);
}

Sprite GetTextureFromMap(TextureMap* map, const char* str) {
	if (!str || strcmp(str, "") == 0) return (Sprite) { 0 };
	int i = shgeti(map, str);
	if (i == -1) {
		Log(__LINE__, "Failed to find the texture \"%s\"", str);
		return (Sprite) { 0 };
	}

	Sprite t = map[i].value;

	return t;
}

Sprite GetSprite(const char* str) {
	Sprite res = GetTextureFromMap(texture_system.textures_map, str);

	return res;
}

Sprite CreateTextSprite(const char* str) {
	Sprite res = (Sprite){
		.color = PLATFORM_BLACK,
		.type = SpriteType_Text
	};
	strcpy(res.text, str);

	return res;
}

Sprite CreateRectSprite(PlatformColor color) {
	Sprite res = (Sprite){
		.color = color,
		.type = SpriteType_Rect
	};


	return res;
}

void LoadTextures() {
#if 0
	FilePathList files = LoadDirectoryFiles("C:\\Users\\elior\\OneDrive\\Desktop\\ConsoleApplication1\\Resoruces\\Textures");
	for (int i = 0; i < files.count; ++i) {
		char* name = GetFileNameWithoutExt(files.paths[i]);
		Sprite texture = {
			.type = SpriteType_Texture,
			.texture = LoadTexture(files.paths[i])
		};
		strcpy(texture.name, name);
		String s = CreateString(name);
		OldLog("LOG: Loaded texture \"%s\" size:%dx%d\n", s.name, texture.texture.texture.width, texture.texture.texture.height);
		AddTexture(s.name, texture);
	}
#if 1
	files = LoadDirectoryFiles("C:\\Users\\elior\\OneDrive\\Desktop\\ConsoleApplication1\\Resoruces\\Inventory");
	for (int i = 0; i < files.count; ++i) {
		char* name = GetFileNameWithoutExt(files.paths[i]);
		Sprite texture = {
			.type = SpriteType_Texture,
			.texture = LoadTexture(files.paths[i])
		};
		strcpy(texture.name, name);
		String s = CreateString(name);
		OldLog("LOG: Loaded texture \"%s\"\n", s.name);
		AddTextureToMap(&texture_system.inventory_map, s.name, texture);
	}
#endif
#endif

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
	for (int i = 1; i < MAX_NUM_ENTITIES; ++i) {
		if (entity_system.entities[i].type != EntityType_None)
			arrput(ordered_entities, entity_system.entities[i].id);
	}

	int _x = arrlen(ordered_entities);
	qsort(ordered_entities, arrlen(ordered_entities), sizeof(EntityId), CompareEntities);

	return ordered_entities;
}

void DestroyEntityByName(const char* str) {
	//TODO: delete from entity map too!
	for (int i = 0; i < MAX_NUM_ENTITIES; ++i) {
		if (strcmp(str, entity_system.entities[i].name) == 0) {
			DestroyEntity(entity_system.entities[i].id);
		}
	}
}

void InitEntitySystem() {
	entity_system = (EntitySystem){ 0 };
	entity_system.valid = true;
	entity_system.num_available_handles = MAX_NUM_ENTITIES;

	int index = 1;
	for (int i = MAX_NUM_ENTITIES - 1; i > 0; --i) {
		entity_system.available_handles[i] = (EntityId){ .index = index };
		index++;
	}
}

Entity* GetEntityByName(char* name) {
	int index = GetHashTableElement(entity_system.entities_map, name);

	if (!index) {
		OldLog("LOG: Could not find entity \"%s\"\n", name);
	}

	Entity* result = &entity_system.entities[index];

	return result;
}

void ResetEntitySystem() {
	//entity_system.num_entities = 0;
	entity_system.entities_map.num = 0;
	Entity nop = { 0 };
	grab_system.num_hovered_entities = 0;
}

bool IsHovered(Rect r) {
	V2 mouse_pos = input_system.mouse_pos;

	if (mouse_pos.x < r.x + r.w &&
		mouse_pos.x > r.x &&
		mouse_pos.y < r.y + r.h &&
		mouse_pos.y > r.y) {
		return true;
	}

	return false;
}

bool IsClicked(Rect r) {
	return IsHovered(r) && platform_system.mouse_left == Active;
}

PixelPos GetEntityPixelPos(EntityId id) {

	return  GetEntity(id)->pixel_pos;
}

Vector2 Vector2ToVector2i(Vector2i v) {
	Vector2 result = { v.x, v.y };

	return result;
}

PixelPos MovePos(PixelPos current_pos, PixelPos dest) {
	//TODO: warning! broken code!
	float speed = 1;


	Vector2 distance = Vector2Subtract((Vector2) { dest.pos.x, dest.pos.y }, (Vector2) { current_pos.pos.x, current_pos.pos.y });
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


EntityId CreateEntityOriginal(Entity entity) {
	if (!*entity.name) return (EntityId) { 0 };
#if 0
	entity.id = (EntityId){ .index = entity_system.num_entities };
	strcpy(entity.id.name, entity.name);

	assert(entity_system.num_entities < MAX_NUM_ENTITIES - 1);
	entity_system.entities[entity_system.num_entities++] = entity;

	Entity* e = entity_system.entities + (entity_system.num_entities - 1);
	AddToHashTable(&entity_system.entities_map, e->name, entity_system.num_entities - 1);

	Log("LOG: Added the entity \"%s\"\n", entity.name);
#else

#endif
	return entity.id;
}

Entity CreateEntityRaw(char* name, EntityType type, Vector2i pixel_pos, Vector2 size, Sprite sprite) {
	//TODO: input santiizing
	Entity entity = {
		.type = type,
		.pixel_pos = pixel_pos,
		.size = size,
		.sprite = sprite
	};

	strcpy(entity.name, name);

	return entity;
}

EntityId CreateEntity(char* name, EntityType type, Vector2i pixel_pos, Vector2 size, Sprite sprite) {

	Entity entity = CreateEntityRaw(name, type, pixel_pos, size, sprite);

	entity.id = GetAvailableId();

	if (!entity.id.index) {
		Log(__LINE__, "Failed to add the entity \"%s\"", entity.name);
		return entity.id;
	}

	//strcpy(entity.id.name, name);

	entity_system.entities[entity.id.index] = entity;

	AddToHashTable(&entity_system.entities_map, name, entity.id.index);

	Log(__LINE__, "Added the entity \"%s\"", entity.name);

	return entity.id;
}

Vector2 ScreenPosToCenterPos(Vector2 pos, Vector2 size) {
	Vector2 result = { pos.x + size.x / 2, pos.y + size.y / 2 };

	return result;
}

bool IsIdEqual(EntityId id0, EntityId id1) {
	return id0.index == id1.index;
}

EntityId CreateCharacter(const char* name, Vector2 pos, Vector2 size) {

	EntityId id = CreateEntity(name, EntityType_Character, (Vector2i) { pos.x, pos.y }, (Vector2) { size.x, size.y }, GetSprite(name));

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

Rect GetEntityRect(Entity entity) {
	Rect rect = {
			entity.pixel_pos.pos.x - entity.size.x / 2,
			entity.pixel_pos.pos.y - entity.size.y / 2,
			entity.size.x,
			entity.size.y
	};

	return rect;
}

void HandleEntityGrabbing(bool is_playing) {
	// Reset released and hovered entities
	if (grab_system.grab_status == GrabStatus_Releasing) {
		grab_system.grabbed_entity = (EntityId){ 0 };
		grab_system.grab_status = GrabStatus_None;
	}

	grab_system.num_hovered_entities = 0;

	// Get hovered entities
	EntityId* ordered_entities = GetOrderedEntities();
	for (int i = 0; i < arrlen(ordered_entities); ++i) {
		Entity* current_entity = GetEntity(ordered_entities[i]);

		Rect rect = GetEntityRect(*current_entity);

		if (IsHovered(rect)) {
			assert(grab_system.num_hovered_entities < MAX_NUM_ENTITIES - 1);
			grab_system.hovered_entities[grab_system.num_hovered_entities++] = current_entity->id;
		}
	}
	arrfree(ordered_entities);

	// Get the first hovered entity
	if (grab_system.num_hovered_entities) {
		EntityId entity = grab_system.hovered_entities[grab_system.num_hovered_entities - 1];
		grab_system.hovered_entity = entity;
	}
	else {
		grab_system.hovered_entity = (EntityId){ 0 };
	}

	// Set grabbed entity
	if (input_system.input_event == InputEvent_LeftClickPressed && grab_system.hovered_entity.index) {
		if ((!is_playing && !GetEntity(grab_system.hovered_entity)->is_locked) || GetEntity(grab_system.hovered_entity)->is_grabbable) {
			grab_system.grabbed_entity = grab_system.hovered_entity;
			grab_system.grab_status = GrabStatus_Grabbing;

			UndoSystemDo();
		}
	}

	// Get the released entity
	if (input_system.input_event == InputEvent_LeftClickReleased && grab_system.grabbed_entity.index) {
		grab_system.grab_status = GrabStatus_Releasing;
	}

}

void SetEntityPixelPos(EntityId id, Vector2i pos) {
	GetEntity(id)->pixel_pos = (PixelPos){ pos };
}

void HandleInventoryItemGrabbing() {

	if (grab_system.grabbed_entity.index) {
		item_system.grabbed_item = grab_system.grabbed_entity;
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
	if (IsEntityNone(item_system.grabbed_item) || grab_system.grab_status != GrabStatus_Releasing) {
		return;
	}

	//get the most forefront entity that is not currently released item
	EntityId entity = { 0 };

	for (int i = 0; i < grab_system.num_hovered_entities; ++i) {
		EntityId current_entity = grab_system.hovered_entities[i];

		if (IsIdEqual(current_entity, grab_system.grabbed_entity)) {
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

void DrawEntities(bool is_playing) {
	EntityId* ordered_entities = GetOrderedEntities();

	for (int i = 0; i < arrlen(ordered_entities); ++i) {
		Entity entity = *GetEntity(ordered_entities[i]);
		Rect rect = GetEntityRect(entity);

		if (entity.type == EntityType_None) continue;

		if (entity.current_animation) {
			Animation animation = shget(entity.animations_map, entity.current_animation);
			DrawAnimation(animation, rect, entity.flipped);
		}
		else {
			if (entity.sprite.type == SpriteType_Texture) {
				//TODO: draw center flipped
				if (!entity.flipped)
					PlatformDrawTextureRect(entity.sprite.texture, rect);
				else
					DrawTextureRectFlipped(entity.sprite.texture, rect);
			}
			else if (entity.sprite.type == SpriteType_Rect) {
				PlatformRectDraw(rect, entity.sprite.color);
			}
			else if (entity.sprite.type == SpriteType_Text) {
				PlatformDrawText(entity.sprite.text, entity.pixel_pos.pos.x, entity.pixel_pos.pos.y);
			}

		}

		if (!is_playing) {
			PlatformRectDraw(rect, (PlatformColor) { 150, 150, 150, 200 });
			//DrawPlatformRectLinesEx(rect, 1, (PlatformColor) { 150, 150, 150, 200 });
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


NodePos GetNode(Vector2i pos) {
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

PixelPos Vector2iToPixelPos(Vector2i v) {
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
	bool result = input_system.mouse_pos.x > GAME_SCREEN_X && input_system.mouse_pos.x > GAME_SCREEN_Y && input_system.mouse_pos.x < GAME_SCREEN_WIDTH && input_system.mouse_pos.y < GAME_SCREEN_HEIGHT;

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
			AddWalkingAreaVertex(GetNearestNodeNumFromPixelPos(Vector2iToPixelPos((Vector2i) { input_system.mouse_pos.x, input_system.mouse_pos.y })));
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
#if FIXE_ME

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
#endif
	assert(0);
}

void DrawWalkingArea() {
#if FIX_ME
	//draw lines and vertices
	int _x = arrlen(pathfinding_system.walking_area_vertices);
	for (int i = 0; i < arrlen(pathfinding_system.walking_area_vertices); ++i) {
		PixelPos v0 = GetPixelPosFromNodePos(pathfinding_system.walking_area_vertices[i]).pos;
		PixelPos v1 = GetPixelPosFromNodePos(pathfinding_system.walking_area_vertices[(i + 1) %
			arrlen(pathfinding_system.walking_area_vertices)]).pos;
		DrawLineEx((Vector2) { v0.pos.x, v0.pos.y }, (Vector2) { v1.pos.x, v1.pos.y }, 1, RED);
		DrawPlatformRect(v0.x - 5, v0.y - 5, 10, 10, RED);
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
#endif
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

		if (Vector2iSubtract(next_pos.pos, GetEntityPixelPos(id).pos).x < 0) {
			GetEntity(id)->flipped = true;
		}
		else {
			GetEntity(id)->flipped = false;
		}
	}
}

void HandlePlayerMovement() {
#if FIX_ME
	if (input_system.input_event == InputEvent_LeftClickReleased) {
		if (IsObstacle(GetNearestNodeNumFromPixelPos(Vector2iToPixelPos(input_system.mouse_pos)))) {
			return;
		}
		if (IsInsidePolygon(pathfinding_system.walking_area_vertices, GetNearestNodeNumFromPixelPos(Vector2iToPixelPos(input_system.mouse_pos)).pos)) {
			StartMoveEntity(GetEntityByName("player")->node_pos, GetNearestNodeNumFromPixelPos((PixelPos) { input_system.mouse_pos }));
		}
	}
#endif
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
	int num_textures;
	int num_vertices;

	EntitySystem entity_system;
} Save_file_header;
#pragma pack(pop)

void SaveGame() {
	const char* save_file_name = "filey";
	FILE* file = fopen(save_file_name, "wb+");
	assert(file);

	int birg_string_size = string_system.size;
	int num_vertices = pathfinding_system.num_vertices;

	Save_file_header save_file_header = {
		.magic_value = save_file_magic_value,
		.big_string_size = birg_string_size,
		.num_vertices = num_vertices,
		.entity_system = entity_system
	};

	fwrite(&save_file_header, 1, sizeof(Save_file_header), file);
	fwrite(string_system.big_string, string_system.size, 1, file);
	fwrite(pathfinding_system.walking_area_vertices, sizeof(Vector2), pathfinding_system.num_vertices, file);
	//fwrite(entity_system.entities, sizeof(Entity), MAX_NUM_ENTITIES, file);

	fclose(file);

	Log(__LINE__, "Succesfully saved game");

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
	entity_system = save_file_header.entity_system;

	fread(string_system.big_string, 1, save_file_header.big_string_size, file);

	for (int i = 0; i < save_file_header.num_vertices; ++i) {
		Vector2i v;
		fread(&v, sizeof(Vector2i), 1, file);
		AddWalkingAreaVertex((NodePos) { v });
	}

	//fread(&entity_system, sizeof(EntitySystem), 1, file);
#if 0
	for (int i = 0; i < MAX_NUM_ENTITIES; ++i) {
		Entity e;
		fread(&e, sizeof(Entity), 1, file);
		AddEntity(e);
		AddToHashTable(&entity_system.entities_map, e.name, entity_system.entities + (entity_system.num_entities - 1));

	}
#endif

	Log(__LINE__, "Succesfully loaded save file");
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
		OldLog("[LOG] Expected \"%s\" at line %d\n", str, tokenizer.line_number);
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

#if 1
		Entity e = CreateEntityRaw(name, EntityType_Character, (Vector2i) { pos.x, pos.y }, size, GetSprite(name));
		arrput(tokenizer.entities_buff, e);
#endif

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
		OldLog("LOG: Successfuly loaded config file\n");
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
		OldLog("LOG: Failed to load config file\n");
	}

}



/*
 * ===============================================================
 *
 *                          MAIN
 *
 * ===============================================================
 */

void DoAnimations() {
	for (int i = 1; i < MAX_NUM_ENTITIES; ++i) {
		if (entity_system.entities[i].type == EntityType_None) {
			continue;
		}

		if (entity_system.entities[i].current_animation) {
			assert(entity_system.entities[i].animations_map);

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
		PlatformRectDraw((Rect) { pos.pos.x - 5, pos.pos.y - 5, 10, 10 }, PLATFORM_BLUE);
	}

	for (int j = 0; j < hmlen(pathfinding_system.obstacle_map); ++j) {
		PixelPos pos = GetPixelPosFromNodePos(pathfinding_system.obstacle_map[j].key);

		PlatformRectDraw((Rect){ pos.pos.x - 5, pos.pos.y - 5, 10, 10 }, PLATFORM_YELLOW);
	}
}

void RunGameLoop(PlatformRenderTexture target, bool enable_input, bool is_playing) {

	CountTime();

	if (enable_input) {

		HandleInput();

		HandleEntityGrabbing(is_playing);

		// Handle entity movement during edit mode
		if (!is_playing) {
			if (grab_system.grabbed_entity.index) {
				V2 mouse_pos = input_system.mouse_pos;

				Vector2 size = GetEntity(grab_system.grabbed_entity)->size;
				Vector2 left_corner = { mouse_pos.x - size.x / 2, mouse_pos.y - size.y / 2 };
				Vector2 right_corner = { left_corner.x + size.x, left_corner.y + size.y };

				if (mouse_pos.x < GAME_SCREEN_WIDTH && mouse_pos.y < GAME_SCREEN_HEIGHT && mouse_pos.y > 0 && mouse_pos.x > 0) {
					GetEntity(grab_system.grabbed_entity)->pixel_pos.pos.x += mouse_pos.x - input_system.previous_mouse_pos.x;
					GetEntity(grab_system.grabbed_entity)->pixel_pos.pos.y += mouse_pos.y - input_system.previous_mouse_pos.y;
				}
			}
		}

		// Handle entity grabbing during play mode
		// NOTE: Same code as HandleInventoryGrabbing?
#if 0
		if (is_playing) {
			if (entity_system.grabbed_entity.index) {
				if (GetEntity(entity_system.grabbed_entity)->type == EntityType_InventoryItem) {
					GetEntity(entity_system.grabbed_entity)->pixel_pos.pos = Vector2iAdd(GetEntity(entity_system.grabbed_entity)->pixel_pos.pos,
						Vector2iSubtract(input_system.mouse_pos, input_system.previous_mouse_pos));
				}
			}
		}
#endif

	}

	// Game rendering?
#if 0
	BeginTextureMode(target);
	{
		ClearBackground(GRAY);

		if (is_playing) {
			HandleInventoryItemGrabbing();
#if 0
			HandlePlayerMovement();
			MoveEntity();
			DoActions2();
#endif
		}
		DoAnimations();
		DrawEntities();

	}
	EndTextureMode();
#endif
}

void PlatformSetRenderTarget(PlatformRenderTexture texture) {

}

void InitGame() {
	Log(__LINE__, "Init started");

	//GuiLoadIcons("C:\\Users\\elior\\OneDrive\\Desktop\\ConsoleApplication1\\Resoruces\\iconset.rgi", false);

	InitStringSystem();

	LoadTextures();

	bool load = false;
	bool load_parse_file = false;
	bool load_cold = true;

	if (load_cold) {
		InitEntitySystem();
	}
	else if (load_parse_file) {
		InitGameFromText();
		NodePos node_pos = (NodePos){ 20, 20 };
		PixelPos player_pixel_pos = GetPixelPosFromNodePos(node_pos);
		//game_state.player = AddEntityToScreen("player", CreateString("run"), (PlatformRect) { player_pixel_pos.pos.x, player_pixel_pos.pos.y, 100, 100 }, EntityType_Character);
		GetEntityByName("player")->node_pos = node_pos;
	}
	else if (load) {
		LoadGame("filey");
		//game_state.player = GetEntityByName("player")->id;
	}

	//LoadInventory();
	//LoadScreenItems2();

	LoadMatches();
	LoadScreenMatches();


	SpriteSheet sprite_sheet = ParseSpriteData("anim");
	Animation run_animation = CreateAnimation("run", sprite_sheet, 0);
	Animation idle_animation = CreateAnimation("idle", sprite_sheet, 1);

#if FIX_ME
	Entity* player = GetEntityByName("player");
	if (player->type) {
		shput(player->animations_map, "run", run_animation);
		shput(player->animations_map, "idle", idle_animation);
		player->current_animation = "idle";
	}
	LoadPathFindingSystem();
#endif

#if 0
	for (int i = 0; i < arrlen(pathfinding_system.limit_nodes_buff); ++i) {
		NodePos* nodes = GetLine2(pathfinding_system.limit_nodes_buff[i], pathfinding_system.limit_nodes_buff[(i + 1) % arrlen(pathfinding_system.limit_nodes_buff)]);
		for (int j = 0; j < arrlen(nodes); ++j) {
			hmput(pathfinding_system.obstacle_map, nodes[j], 0);
		}
	}
#endif

	//StartEntityAnimation(GetEntity);

	target = (PlatformRenderTexture){ PlatformLoadRenderTexture(GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT) };

	//CreateBackground();

	drop_down_panel_target = (PlatformRenderTexture){PlatformLoadRenderTexture(100, 300)};

	// main loop


	//run_game_loop = false;
	//run_slice_spritesheet = true;

	CreateEntity("background", EntityType_Background,
		(Vector2i) {
		GAME_SCREEN_WIDTH / 2, GAME_SCREEN_HEIGHT / 2
	},
		(Vector2) {
		GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT
	}, GetSprite("background"));

	Log(__LINE__, "Init ended");
}

Vector2 GetScreenCenter() {
	Vector2 result = (Vector2){ GAME_SCREEN_WIDTH / 2, GAME_SCREEN_HEIGHT / 2 };

	return result;
}

#else

/*
 * ===============================================================
 *
 *                          SECTION_PLATFORM
 *
 * ===============================================================
 */


#endif
#endif