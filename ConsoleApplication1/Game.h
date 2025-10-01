#pragma once
#include "Platform.h"
#include "Egui.h"
#include "Textures.h"
#include "Handle.h"

#define GAME_TEXTURES_COUNT 256

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

//typedef struct wzrd_color {
//	char c, m, y, k;
//} wzrd_color;
//


typedef struct Entity {
	NodePos node_pos;
	//char texture_name[NAME_SIZE];
	Texture sprite;

	EntityId id;

	EntityType type;
	PixelPos pixel_pos;

	v2 size;
	bool flipped;
	/*struct {
		char* key; Animation value;
	}  *animations_map;*/
	char* event;
	char* current_animation;

	bool is_grabbable;

	bool is_locked;

	// NEW
	Rect rect;
	str128 name;
	Texture_handle texture;
	wzrd_color color;

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

typedef struct Game {
	Handle_map textures_handle_map;
	Texture textures[GAME_TEXTURES_COUNT];

	Handle_map entities_handle_map;
	Entity entities[GAME_TEXTURES_COUNT];

	PlatformTargetTexture target_texture;

	v2 mouse_pos, mouse_delta;

	Entity_handle hot_entity, active_entity, selected_entity;

	wzrd_icons icons;

} Game;

Game game;

Texture_handle game_texture_add(Texture texture);
void game_texture_remove(Texture_handle handle);
Texture* game_texture_get(Texture_handle handle);

Texture_handle game_texture_add(Texture texture);
void game_texture_remove_by_index(int index);
void game_init();
void game_run(wzrd_v2 window_size, wzrd_v2 *cursor, bool *game_gui_interaction, bool *is_hovering);
void game_draw_gui_commands(wzrd_draw_commands_buffer *buffer);