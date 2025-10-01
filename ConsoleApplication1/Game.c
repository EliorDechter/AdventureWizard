#include "Game.h"
#include "Editor.h"

void game_draw_gui_commands(wzrd_draw_commands_buffer* buffer) {
	for (int i = 0; i < buffer->count; ++i) {
		EguiDrawCommand command = buffer->commands[i];
		if (command.type == DrawCommandType_Texture) {
			PlatformTextureDrawFromSource(*(PlatformTexture*)&command.texture,
				*(PlatformRect*)&command.dest_rect, *(PlatformRect*)&command.src_rect, (platform_color) { 255, 255, 255, 255 });
		}
		else if (command.type == DrawCommandType_String) {
			PlatformTextDraw(command.str.val, command.dest_rect.x, command.dest_rect.y);
		}
		else if (command.type == DrawCommandType_Rect) {
			PlatformRectDraw(*((PlatformRect*)&command.dest_rect), *((platform_color*)&command.color));
		}
		else if (command.type == DrawCommandType_Line) {
			PlatformLineDraw(command.dest_rect.x, command.dest_rect.y, command.dest_rect.w, command.dest_rect.h,
				command.color.r, command.color.g, command.color.b);
		}
		else if (command.type == DrawCommandType_VerticalLine) {
			PlatformLineDrawVertical(command.dest_rect.x, command.dest_rect.y, command.dest_rect.w, command.dest_rect.h);
		}
		else if (command.type == DrawCommandType_HorizontalLine) {
			PlatformLineDrawHorizontal(command.dest_rect.x, command.dest_rect.y, command.dest_rect.w, command.dest_rect.h);
		}
		else if (command.type == DrawCommandType_IconClose) {
			PlatformTextureDraw(*(PlatformTexture*)&g_close_texture,
				*(PlatformRect*)&command.dest_rect);
		}
		else {
			//assert(0);
		}
	}
}

EguiV2i TextGetSize(const char* str) {
	PlatformV2i result = PlatformTextGetSize(str);
	return *(EguiV2i*)&result;
}

EguiV2i TextGetSizeFromLength(int len) {
	assert(len < 128);
	str128 str = { 0 };
	for (int i = 0; i < len; ++i)
		str.val[i] = 'a';
	PlatformV2i result = PlatformTextGetSize(str.val);
	return *(EguiV2i*)&result;

}

void EguiBoxDo(wzrd_box box) {
	wzrd_box_begin(box);
	wzrd_box_end();
}

wzrd_rect EguiRectScale(wzrd_rect rect, float scale) {
	float new_w = rect.w * scale;
	float new_h = rect.h * scale;

	wzrd_rect result = {
		rect.x + (rect.w / 2 - new_w / 2),
		rect.y + (rect.h / 2 - new_h / 2),
		new_w,
		new_h
	};

	return result;
}

bool f;

typedef struct EguiMenuNode {
	str128 str;
	int depth;
	bool expandable;
} EguiMenuNode;



#if 0
void nodes() {
	if (nodes[i].depth < minimized_depth)
		disable_next_depth = false;

	if (disable_next_depth)
		continue;

	EguiBoxBegin((Box) {
		.row_mode = true,
			.h = 20,

			.color = EGUI_WHITE,
			.border_type = BorderType_None,
	});
	{
		if (nodes[i].depth > 0) {
			// Space
			if (nodes[i].depth > 1)
				EguiBox((Box) { .border_type = BorderType_None, .w = EDITOR_ICON_SIZE * (nodes[i].depth - 1), .h = EDITOR_ICON_SIZE });

			// Lines and button
			EguiBoxBegin((Box) {
				.border_type = BorderType_None, .w = EDITOR_ICON_SIZE, .h = EDITOR_ICON_SIZE + 2
			});
			{
				EguiItemAdd((Item) { .type = ItemType_TopVerticalDottedLine });

				EguiItemAdd((Item) { .type = ItemType_RightHorizontalDottedLine });

				// Button
				if (nodes[i].expandable) {
					EguiRect new_rect = EguiRectScale((EguiRect) { 0, 0, EguiBoxGetCurrent()->w, EguiBoxGetCurrent()->h }, 0.5);
					bool* is_toggled = EguiToggleBegin((Box) {
						.border_type = BorderType_None, .color = EGUI_WHITE,
							.x = new_rect.x, .y = new_rect.y, .w = new_rect.w, .h = new_rect.h
					});
					{
						EguiItemAdd((Item) {
							.type = ItemType_Rect,
								.rect = (EguiRect){ 0, 0, new_rect.w, 1 },
								.color = EGUI_GRAY
						});
						EguiItemAdd((Item) {
							.type = ItemType_Rect,
								.rect = (EguiRect){ 0 + new_rect.w - 1, 0, 1, new_rect.h },
								.color = EGUI_GRAY
						});
						EguiItemAdd((Item) {
							.type = ItemType_Rect,
								.rect = (EguiRect){ 0, 0 + new_rect.h - 1, new_rect.w, 1
							},
								.color = EGUI_GRAY
						});
						EguiItemAdd((Item) {
							.type = ItemType_Rect,
								.rect = (EguiRect){ 0, 0, 1, new_rect.h },
								.color = EGUI_GRAY
						});
						EguiItemAdd((Item) {
							.type = ItemType_Rect,
								.rect = (EguiRect){ 0 + new_rect.w / 2, 0 + 3, 1, new_rect.h - 6 },
								.color = EGUI_BLACK
						});
						EguiItemAdd((Item) {
							.type = ItemType_Rect,
								.rect = (EguiRect){ 0 + 3, 0 + new_rect.w / 2, new_rect.w - 6, 1 },
								.color = EGUI_BLACK
						});
					}
					EguiToggleEnd();

					if (*is_toggled) {
						disable_next_depth = true;
					}
				}
			}
			EguiBoxEnd();
		}

		// Icon
		char buff[64] = { 0 };
		sprintf(buff, "icon %d", i);
		EguiBoxBegin((Box) {
			.border_type = BorderType_None,
				.color = EGUI_WHITE,
				.w = 15,
				.h = 15,
				.name = Str128Create(buff)
		});
		{
			if (nodes[i].depth)
				EguiItemAdd((Item) { .type = ItemType_LeftHorizontalDottedLine });

			EguiItemAdd((Item) {
				.type = ItemType_Texture,
					.size = (EguiV2){ 30, 30 },
					.texture = *(EguiTexture*)&g_texture
			});
		}
		EguiBoxEnd();

		// Name
		if (EguiLabelButton(nodes[i].str, (EguiV2i) { 10, 10 })) {
			result = nodes[i].str;
		}
	}
	EguiBoxEnd();

	int f = 5;
	f++;
}
#endif

EguiV2i EguiV2iAdd(EguiV2i a, EguiV2i b) {
	EguiV2i result = (EguiV2i){ a.x + b.x, a.y + b.y };

	return result;
}

Texture_handle game_texture_add(Texture texture) {
	Handle handle = handle_create(&game.textures_handle_map);
	int index = handle_get(&game.textures_handle_map, handle);
	game.textures[index] = texture;

	Texture_handle result = (Texture_handle){ .val = handle };

	return result;
}

void game_texture_remove_by_index(int index) {
	handle_remove_by_index(&game.textures_handle_map, index);
}

void game_texture_remove(Texture_handle handle) {
	handle_remove(&game.textures_handle_map, handle.val);
}

Texture* game_texture_get(Texture_handle handle) {
	int index = handle_get(&game.textures_handle_map, handle.val);

	Texture* result = game.textures + index;

	return result;
}

Entity_handle game_entity_add(Entity entity) {
	Handle handle = handle_create(&game.entities_handle_map);
	int index = handle_get(&game.entities_handle_map, handle);
	game.entities[index] = entity;

	Entity_handle result = (Entity_handle){ .val = handle };

	return result;
}

void game_entity_remove_by_index(int index) {
	handle_remove_by_index(&game.entities_handle_map, index);
}

void game_entity_remove(Entity_handle handle) {
	handle_remove(&game.entities_handle_map, handle.val);
}

Entity* game_entity_get(Entity_handle handle) {
	int index = handle_get(&game.entities_handle_map, handle.val);

	if (!index) return 0;

	Entity* result = game.entities + index;

	return result;
}

/*
 * ===============================================================
 *
 *                          S_MISC
 *
 * ===============================================================
 */



v2i v2i_sub(v2i v0, v2i v1) {
	v2i result = { v0.x - v1.x, v0.y - v1.y };

	return result;
}

v2i v2i_add(v2i v0, v2i v1) {
	v2i result = { v0.x + v1.x, v0.y + v1.y };

	return result;
}

v2 v2_sub(v2 v0, v2 v1) {
	v2 result = { v0.x - v1.x, v0.y - v1.y };

	return result;
}

v2 v2_add(v2 v0, v2 v1) {
	v2 result = { v0.x + v1.x, v0.y + v1.y };

	return result;
}

v2 v2_normalize(v2 v) {
	float size = sqrt(pow((v.x), 2) + pow((v.y), 2));
	v2 result = { v.x / size, v.y / size };

	return result;
}

v2 v2_lerp(v2 pos, v2 end_pos) {
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


#define MAX_NUM_VERTICES_IN_POLYGON 32

typedef struct Polygon {
	v2 vertices[MAX_NUM_VERTICES_IN_POLYGON];
	int count;
} Polygon;

bool v2_is_inside_polygon(v2 point, Polygon polygon) {

	bool inside = false;
	int i, j;

	for (i = 0, j = polygon.count - 1; i < polygon.count; j = i++) {
		if (((polygon.vertices[i].y > point.y) != (polygon.vertices[j].y > point.y)) &&
			(point.x < (polygon.vertices[j].x - polygon.vertices[i].x) * (point.y - polygon.vertices[i].y) / (polygon.vertices[j].y - polygon.vertices[i].y) + polygon.vertices[i].x))
			inside = !inside;
	}

	return inside;
}


bool v2_is_inside_rect(v2 point, Rect rect) {
	bool result = false;

	if (point.x >= rect.x && point.y >= rect.y && point.x < rect.x + rect.w && point.y < rect.y + rect.h) {
		result = true;
	}

	return result;
}

/*
 * ===============================================================
 *
 *                          Section_Entities
 *
 * ===============================================================
 */


EntityId GetAvailableId() {
#if 0
	if (!entity_system.num_available_handles) {
		Log(__LINE__, "No available entity handles");

		return (EntityId) { 0 };
	}

	EntityId result = entity_system.available_handles[entity_system.num_available_handles - 1];
	entity_system.num_available_handles--;

	return result;
#else
	return (EntityId) { 0 };
#endif
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
#if 0
	if (GetEntity(id)->type == EntityType_None) {
		Log(__LINE__, "Failed to destroy entity with id %d,%d\n", id.index, id.generation);
		return;
	}
	Log(__LINE__, "Entity \"%s\" destroyed\n", GetEntity(id)->name);

	UndoSystemDo();

	//RemoveFromHashTable(&entity_system.entities_map, GetEntity(id)->name);
	entity_system.entities[id.index] = (Entity){ 0 };
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
#if 0
	for (int i = 1; i < MAX_NUM_ENTITIES; ++i) {
		if (entity_system.entities[i].type != EntityType_None)
			arrput(ordered_entities, entity_system.entities[i].id);
	}

	int _x = arrlen(ordered_entities);
	qsort(ordered_entities, arrlen(ordered_entities), sizeof(EntityId), CompareEntities);
#endif
	return ordered_entities;
}

void DestroyEntityByName(const char* str) {
	//TODO: delete from entity map too!
	for (int i = 0; i < MAX_NUM_ENTITIES; ++i) {
		if (strcmp(str, entity_system.entities[i].name.val) == 0) {
			DestroyEntity(entity_system.entities[i].id);
		}
	}
}

Entity* GetEntityByName(char* name) {
	//int index = GetHashTableElement(entity_system.entities_map, name);

	//if (!index) {
		//OldLog("LOG: Could not find entity \"%s\"\n", name);
	//}

	//Entity* result = &entity_system.entities[index];

	//return result;

	return 0;
}

void ResetEntitySystem() {
	//entity_system.num_entities = 0;
	entity_system.entities_map.num = 0;
	Entity nop = { 0 };
	grab_system.num_hovered_entities = 0;
}

bool IsRectHovered(Rect r) {
	v2 mouse_pos = (v2){ platform.mouse_x, platform.mouse_y };

	if (mouse_pos.x < r.x + r.w &&
		mouse_pos.x > r.x &&
		mouse_pos.y < r.y + r.h &&
		mouse_pos.y > r.y) {
		return true;
	}

	return false;
}

bool IsRectClicked(Rect r) {
	return IsRectHovered(r) && platform.mouse_left == Deactivating;
}

PixelPos GetEntityPixelPos(EntityId id) {

	return  GetEntity(id)->pixel_pos;
}

v2 v2Tov2i(v2i v) {
	v2 result = { v.x, v.y };

	return result;
}

PixelPos MovePos(PixelPos current_pos, PixelPos dest) {
	//TODO: warning! broken code!
	float speed = 1;


	v2 distance = v2_sub((v2) { dest.pos.x, dest.pos.y }, (v2) { current_pos.pos.x, current_pos.pos.y });
	v2 direction = v2_normalize(distance);

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
	if (!entity.name.len) return (EntityId) { 0 };
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

Entity CreateEntityRaw(char* name, EntityType type, v2i pixel_pos, v2 size, Texture sprite) {
	//TODO: input santiizing
	Entity entity = {
		.type = type,
		.pixel_pos = pixel_pos,
		.size = size,
		.sprite = sprite,
		.name = str128_create(name)
	};

	return entity;
}

EntityId CreateEntity(char* name, EntityType type, v2i pixel_pos, v2 size, Texture sprite) {
#if 0
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
#else
	return (EntityId) { 0 };
#endif
}

v2 ScreenPosToCenterPos(v2 pos, v2 size) {
	v2 result = { pos.x + size.x / 2, pos.y + size.y / 2 };

	return result;
}

bool IsIdEqual(EntityId id0, EntityId id1) {
	return id0.index == id1.index;
}

EntityId CreateCharacter(const char* name, v2 pos, v2 size) {

	EntityId id = CreateEntity(name, EntityType_Character, (v2i) { pos.x, pos.y }, (v2) { size.x, size.y }, texture_get_by_name(str128_create(name)));

	return id;
}

Entity* entity_get_next(int* iterator_index) {
	Handle handle = { 0 };
	Entity* result = 0;
	if (handle_get_next(&game.entities_handle_map, iterator_index, &handle)) {
		result = &game.entities[handle.index];
	}

	return result;
}

bool entity_get_next_handle(int* iterator_index, Entity_handle* handle) {
	bool result = handle_get_next(&game.entities_handle_map, iterator_index, handle);

	return result;
}

platform_color wzrd_color_to_platform_color(wzrd_color color) {

	platform_color result = *(platform_color*)&color;

	return result;

	/*platform_color result = {
		.r = 255 * (1 - color.c) * (1 - color.k),
		.b = 255 * (1 - color.m) * (1 - color.k),
		.g = 255 * (1 - color.y) * (1 - color.k),
		.a = 255
	};

	return result;*/
}

void game_run(wzrd_v2 window_size, wzrd_v2* cursor, bool *game_gui_interaction, bool *is_hovering) {

	// game screen gui
	static Egui gui;
	static wzrd_draw_commands_buffer gui_draw_commands_buffer;
	bool blue_button = false;
	wzrd_begin(&gui, 0,
		(wzrd_v2) {
		game.mouse_pos.x, game.mouse_pos.y
	},
		platform.mouse_left,
		* (wzrd_keyboard_keys*)&platform.keys_pressed,
		window_size
		, game.icons,
		(wzrd_color) {
		0, 0, 0, 0
	});
	{
	
		if (game.selected_entity.val.index) {
			Entity* selected_entity = game_entity_get(game.selected_entity);
			blue_button = wzrd_game_buttonesque((wzrd_v2) { selected_entity->rect.x - 5, selected_entity->rect.y - 5 }, (wzrd_v2) { 10, 10 }, (wzrd_color) { 0, 0, 255, 255 });
		}
	}
	wzrd_end(cursor, &gui_draw_commands_buffer, game_gui_interaction, is_hovering);

	// entity mouse interaction 
	if (*game_gui_interaction == false)
	{
		if (game.mouse_pos.x >= 0 && game.mouse_pos.y >= 0 && game.mouse_pos.x <= window_size.x && game.mouse_pos.y <= window_size.y) {
			Entity_handle hovered_entity = { 0 };
			int iterator_index = 1;
			Entity_handle entity_handle = { 0 };
			while (entity_get_next_handle(&iterator_index, &entity_handle)) {
				Entity* entity = game_entity_get(entity_handle);
				bool is_hover = v2_is_inside_rect((v2) { game.mouse_pos.x, game.mouse_pos.y }, entity->rect);
				if (is_hover) {
					hovered_entity = entity_handle;
				}
			}

			if (hovered_entity.val.index) {
				if (game.active_entity.val.index) {
					if (platform.mouse_left == EguiDeactivating) {
						game.selected_entity = game.active_entity;
						game.active_entity = (Entity_handle){ 0 };
					}
				}

				if (game.hot_entity.val.index) {
					if (platform.mouse_left == EguiActivating) {
						game.active_entity = game.hot_entity;
					}
				}

				if (hovered_entity.val.index) {
					game.hot_entity = hovered_entity;
				}
				else {
					game.hot_entity = (Entity_handle){ 0 };
				}
			}
			else {
				if (platform.mouse_left == EguiDeactivating) {
					game.selected_entity = (Entity_handle){ 0 };
				}
			}
		}
	}

	// Draw
	PlatformTextureBeginTarget(game.target_texture);
	{

		// Draw selected entity border
		Entity* selected_entity = game_entity_get(game.selected_entity);
		if (selected_entity) {
			PlatformRect rect = { selected_entity->rect.x - 1, selected_entity->rect.y - 1, selected_entity->rect.w + 2, selected_entity->rect.h + 2 };
			PlatformRectDraw(rect, (platform_color) { 0, 0, 255, 255 });
		}


		// Draw entities
		int iterator_index = 0;
		Entity* entity = 0;
		while (entity = entity_get_next(&iterator_index)) {
			Texture* texture = game_texture_get(entity->texture);
			PlatformTextureDrawFromSource(game_texture_get(entity->texture)->val, *(PlatformRect*)&entity->rect,
				(PlatformRect) {
				0, 0, texture->val.w, texture->val.h
			}, wzrd_color_to_platform_color(entity->color));
		}

		// Draw gui
		game_draw_gui_commands(&gui_draw_commands_buffer);
	}
	PlatformTextureEndTarget();

	// Late logic
	if (blue_button) 
	{
		Entity* selected_entity = game_entity_get(game.selected_entity);

		if (selected_entity) {
			selected_entity->rect.x += game.mouse_delta.x;
			selected_entity->rect.y += game.mouse_delta.y;
		}
	}

}

void game_init() {

	textures_load();

	g_texture = PlatformTextureLoad("C:\\Users\\elior\\OneDrive\\Desktop\\ConsoleApplication1\\Resoruces\\Textures\\ass.png");
	g_checkmark_texture = PlatformTextureLoad("C:\\Users\\elior\\OneDrive\\Desktop\\ConsoleApplication1\\Resoruces\\Textures\\checkmark.png");
	g_target = PlatformTargetTextureCreate();
	g_close_texture = PlatformTextureLoad("C:\\Users\\elior\\OneDrive\\Desktop\\ConsoleApplication1\\Resoruces\\Textures\\x.png");
	PlatformTextureBeginTarget(g_target);
	{
		SDL_SetRenderDrawColor(sdl.renderer, 255, 255, 255, 255);
	}
	PlatformTextureEndTarget();

	PlatformTexture icon_delete = PlatformTextureLoad("C:\\Users\\elior\\OneDrive\\Desktop\\ConsoleApplication1\\Resoruces\\Textures\\icon_delete.png");
	PlatformTexture icon_entity = PlatformTextureLoad("C:\\Users\\elior\\OneDrive\\Desktop\\ConsoleApplication1\\Resoruces\\Textures\\entity.png");
	PlatformTexture icon_play = PlatformTextureLoad("C:\\Users\\elior\\OneDrive\\Desktop\\ConsoleApplication1\\Resoruces\\Textures\\play.png");
	PlatformTexture icon_pause = PlatformTextureLoad("C:\\Users\\elior\\OneDrive\\Desktop\\ConsoleApplication1\\Resoruces\\Textures\\pause.png");
	PlatformTexture icon_stop = PlatformTextureLoad("C:\\Users\\elior\\OneDrive\\Desktop\\ConsoleApplication1\\Resoruces\\Textures\\stop.png");
	PlatformTexture icon_dropdown = PlatformTextureLoad("C:\\Users\\elior\\OneDrive\\Desktop\\ConsoleApplication1\\Resoruces\\Textures\\icon_dropdown.png");

	game.icons = (wzrd_icons){
		.close = *(wzrd_texture*)&g_close_texture,
		.delete = *(wzrd_texture*)&icon_delete,
		.entity = *(wzrd_texture*)&icon_entity,
		.play = *(wzrd_texture*)&icon_play,
		.pause = *(wzrd_texture*)&icon_pause,
		.stop = *(wzrd_texture*)&icon_stop,
		.dropdown = *(wzrd_texture*)&icon_dropdown,
	};

	game.target_texture = g_target;

	// Empty entity
	game_entity_add((Entity) { 0 });

	Entity_handle e = game_entity_add((Entity) {
		.rect = { .x = 50, .y = 50, .w = 500, .h = 500 }, .name = str128_create("e1"),
			.texture = game_texture_add(texture_get_by_name(str128_create("clouds"))),
			.color = (wzrd_color){ 255, 255, 255, 255 }
	});

}

#if 0
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

		PlatformRectDraw((Rect) { pos.pos.x - 5, pos.pos.y - 5, 10, 10 }, PLATFORM_YELLOW);
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

	drop_down_panel_target = (PlatformRenderTexture){ PlatformLoadRenderTexture(100, 300) };

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

#endif