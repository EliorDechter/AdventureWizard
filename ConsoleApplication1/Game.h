#pragma once
#include "Platform.h"
#include "Egui.h"
#include "Textures.h"
#include "Handle.h"
#include "ArrayList.h"

#define GAME_TEXTURES_COUNT 256
#define GAME_ENTITIES_COUNT 256

typedef struct Entity_handle {
	Handle val;
} Entity_handle;

typedef struct Texture_handle {
	Handle val;
} Texture_handle;

typedef enum EntityType { EntityType_None, EntityType_Background, EntityType_Screen, EntityType_ScreenItem, EntityType_Character, EntityType_InventoryItem, EntityType_Animation } EntityType;

typedef struct EntityId {
	//char name[NAME_SIZE];
	int index;
	int generation;
} EntityId;

typedef struct Rect {
	float x, y, w, h;
} Rect;

typedef struct v2i {
	int x, y;
} v2i;

typedef struct v2 {
	float x, y;
} v2;

typedef struct PixelPos {
	v2i pos;
} PixelPos;

typedef struct NodePos {
	v2i pos;
} NodePos;


typedef struct Entity {
	NodePos node_pos;
	//char texture_name[NAME_SIZE];
	//Texture sprite;

	//EntityId id;


	//bool flipped;
	/*struct {
		char* key; Animation value;
	}  *animations_map;*/
	//char* event;
	//char* current_animation;

	//bool is_grabbable;

	//bool is_locked;

	// NEW
	Rect rect;
	str128 name;
	Texture_handle texture;
	wzrd_color color;
	EntityType type;
	PixelPos pixel_pos;
	v2 size;
	int rendering_order;
	Entity_handle handle;
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

static EntitySystem g_entity_system;

typedef struct GrabSystem {

	EntityId hovered_entities[MAX_NUM_ENTITIES];
	int num_hovered_entities;

	EntityId hovered_entity;
	EntityId grabbed_entity;

	GrabStatus grab_status;
} GrabSystem;

static GrabSystem g_grab_system;

typedef struct Game {
	Handle_map textures_handle_map;
	Texture textures[GAME_TEXTURES_COUNT];

	Handle_map entities_handle_map;
	Entity entities[GAME_ENTITIES_COUNT];
	Entity_handle sorted_entities[GAME_ENTITIES_COUNT];
	unsigned int sorted_entities_count;
	unsigned int selected_entity_index_to_sorted_entities;
	bool is_entity_selected;

	PlatformTargetTexture target_texture;

	v2 mouse_pos, mouse_delta;

	Entity_handle hot_entity, active_entity;

	wzrd_icons icons;

	bool polygon_adding_active;

	bool delete;

	bool run;

} Game;

extern Game g_game;

void game_draw_entities();
Entity_handle game_entity_create(Entity entity);
PlatformTargetTexture game_target_texture_get();
Texture_handle game_texture_add(Texture texture);
void game_texture_remove(Texture_handle handle);
Texture* game_texture_get(Texture_handle handle);
Texture_handle game_texture_add(Texture texture);
void game_texture_remove_by_index(int index);
void game_init();
void game_gui_do(wzrd_draw_commands_buffer* buffer, wzrd_canvas* gui, wzrd_rect_struct window, wzrd_cursor* cursor, bool enable_input, int scale, unsigned int layer, wzrd_str* debug_str);
void game_run(v2 window_size, bool enable, unsigned int scale);
wzrd_icons game_icons_get();
Entity* entity_get_next(int* iterator_index);
Entity* game_entity_get(Entity_handle handle);
void game_entity_remove(Entity_handle handle);
Texture* game_texture_get_by_name(str128 str);
Texture_handle game_texture_get_handle_by_name(str128 str);
void game_draw_screen_dots();
bool game_entity_is_equal(Entity_handle a, Entity_handle b);