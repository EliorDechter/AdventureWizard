#include "Game.h"

#define MAX_NUM_DRAW_COMMANDS 1024
typedef enum DrawCommandType { DrawCommandType_Default, DrawCommandType_Rect, DrawCommandType_Text } DrawCommandType;

typedef struct DrawCommand {
	DrawCommandType type;
	EguiRect rect;
	Str32 str;
	EguiColor rect_color;
} DrawCommand;

typedef struct RenderingSystem {
	DrawCommand draw_commands[MAX_NUM_DRAW_COMMANDS];
	int num_commands;
} RenderingSystem;

RenderingSystem rendering_system;

void Draw(DrawCommand command) {
	assert(command.rect.x >= 0);
	assert(command.rect.y >= 0);
	assert(command.rect.w >= 0);
	assert(command.rect.h >= 0);
	assert(command.rect_color.a);

	assert(rendering_system.num_commands < MAX_NUM_DRAW_COMMANDS - 1);
	rendering_system.draw_commands[rendering_system.num_commands++] = command;
}

void DrawBatch() {
	for (int i = 0; i < rendering_system.num_commands; ++i) {
		DrawCommand command = rendering_system.draw_commands[i];
		PlatformRectDraw(*((PlatformRect*)&command.rect), *((PlatformColor*)&command.rect_color));
		if (*command.str.str)
			PlatformTextDraw(command.str.str, command.rect.x, command.rect.y);
	}

	rendering_system.num_commands = 0;
}

void DrawEditor(EguiDrawCommandsBuffer commands) {
	for (int i = 0; i < egui.num_draw_commands; ++i) {
		EguiDrawCommand command = commands.commands[i];
#if 1
		switch (command.type) {
		case EguiDrawCommandType_Text: {
			//GuiDrawText(command.text_data.text, command.rect, command.text_data.alignment, command.text_data.color);
			//DrawTextEx(egui.font, command.text_data.text, (Vector2) { command.rect.x, command.rect.y }, 14, 1, BLACK);
			assert(0);
			//DrawText(command.text_data.text, command.rect.x, command.rect.y, 13, BLACK);
			break;
		}
		case EguiDrawCommandType_Box: {
			Box box = egui.boxes[command.box_data.box_index];
			assert(box.color.a);
			//PlatformRect rect = (PlatformRect){ box.x_internal, box.y_internal, box.w_internal, box.h_internal };
			Draw((DrawCommand) {
				.rect = (EguiRect){ .x = box.absolute_rect.x, .y = box.absolute_rect.y,
.h = box.absolute_rect.h, .w = box.absolute_rect.w },
.rect_color = box.color
			});

			if (box.border_type == BorderType_Default) {
				// Draw top and left lines
				Draw((DrawCommand) { .rect = { box.absolute_rect.x, box.absolute_rect.y, box.absolute_rect.w , 1 }, .rect_color = EGUI_WHITE });
				Draw((DrawCommand) { .rect = { box.absolute_rect.x, box.absolute_rect.y, 1, box.absolute_rect.h }, .rect_color = EGUI_WHITE });
				Draw((DrawCommand) { .rect = { box.absolute_rect.x, box.absolute_rect.y + 1, box.absolute_rect.w, 1 }, .rect_color = EGUI_LIGHTGRAY });
				Draw((DrawCommand) { .rect = { box.absolute_rect.x + 1, box.absolute_rect.y, 1, box.absolute_rect.h }, .rect_color = EGUI_LIGHTGRAY });

				// Draw bottom and right lines
				Draw((DrawCommand) { .rect = { box.absolute_rect.x, box.absolute_rect.y + box.absolute_rect.h - 1, box.absolute_rect.w, 1 }, .rect_color = EGUI_BLACK });
				Draw((DrawCommand) { .rect = { box.absolute_rect.x + box.absolute_rect.w - 1, box.absolute_rect.y, 1, box.absolute_rect.h }, .rect_color = EGUI_BLACK });
				Draw((DrawCommand) { .rect = { box.absolute_rect.x, box.absolute_rect.y + box.absolute_rect.h - 1 - 1, box.absolute_rect.w, 1 }, .rect_color = EGUI_GRAY });
				Draw((DrawCommand) { .rect = { box.absolute_rect.x + box.absolute_rect.w - 1 - 1, box.absolute_rect.y, 1, box.absolute_rect.h }, .rect_color = EGUI_GRAY });
			}
			else if (box.border_type == BorderType_Black) {
				// Draw top and left lines
				Draw((DrawCommand) { .rect = { box.absolute_rect.x, box.absolute_rect.y, box.absolute_rect.w, 1 }, .rect_color = EGUI_BLACK });
				Draw((DrawCommand) { .rect = { box.absolute_rect.x, box.absolute_rect.y, 1, box.absolute_rect.h }, .rect_color = EGUI_BLACK });

				// Draw bottom and right lines
				Draw((DrawCommand) { .rect = { box.absolute_rect.x, box.absolute_rect.y + box.absolute_rect.h - 1, box.absolute_rect.w, 1 }, .rect_color = EGUI_BLACK });
				Draw((DrawCommand) { .rect = { box.absolute_rect.x + box.absolute_rect.w - 1, box.absolute_rect.y, 1, box.absolute_rect.h }, .rect_color = EGUI_BLACK });
			}

			if (*box.str.str) {
				//DrawText(box.str.str, box.absolute_rect.x, box.absolute_rect.y, 13, BLACK);
				/*Draw((DrawCommand) {
					.str = box.str,
						.rect = (EguiRect){ box.absolute_rect.x, box.absolute_rect.y,
						box.absolute_rect.w, box.absolute_rect.h },
				});*/
			}

			break;
		}
		case EguiDrawCommandType_Rect: {
			assert(0);
			//DrawPlatformRect(command.rect, command.rect_data.border_width, command.rect_data.border_color, command.rect_data.color);
			break;
		}

		case EguiDrawCommandType_Texture: {
			//PlatformTextureDraw(command.texture, command.rect.x, command.rect.y, command.rect.w, command.rect.h);

			break;

		}
		}
#endif
	}
}


void DoEditor(float total_time_in_seconds, EguiV2 mouse_pos, int window_width, int window_height) {

	assert(window_width);
	assert(window_height);
	egui.window_width = window_width;
	egui.window_height = window_height;

	// Drawing data
	EguiV2 drop_down_panel_size = { BUTTON_WIDTH, BUTTON_HEIGHT * 2 };

	EguiBegin(total_time_in_seconds, (EguiV2) { 5, 5 }, mouse_pos, platform_system.mouse_left);
	{
		// Files Panel
#if 1
		EguiBoxBegin(((Box) {
			.per = 0.05,
				.name = Str32Create("Files"),
				.row_mode = true,
				.n = 1,
				.grow_horizontal = true,
				.h = 50,
				.size_type = SizeType_Fixed,
		}));
		{

		}
		EguiBoxEnd();
#endif

		// Top panel
		EguiBoxBegin(((Box) {
			.name = Str32Create("Top button panel"), .inner_padding = (EguiRect){ 8, 8, 0, 0 },
				.n = 1, .per = 0.05, .row_mode = true, .push = 8,
				.grow_horizontal = true,
				.h = 50,
		}));
		{
			// Play
			//EditorToggleIcon("#152#", &is_playing);

#if 0
			// Undo
			if (EditorButtonIcon("#209#", "Undo")) {
				EntitySystem es = UndoSystemUndo();
				if (es.valid)
					entity_system = es;
			}

			// Redo
			if (EditorButtonIcon("#208#", "Redo")) {
				EntitySystem es = UndoSystemRedo();
				if (es.valid)
					entity_system = es;
			}
#endif

			// toggle animations
			static bool animate;
#if 0
			if (EditorButtonIcon("#160#", "Toggle animation")) {
				animate = !animate;
				Entity* entity = GetEntityByName("player");
				Animation* animation = &shgetp(entity->animations_map, entity->current_animation)->value;
				if (animate)
					BeginAnimation(animation);
				else
					EndAnimation(animation);
			}
#endif
#if 0
			// load textures
			if (EditorButtonIcon("#201#", "Load Textures")) {
				LoadTextures();

				for (int i = 1; i < MAX_NUM_ENTITIES; ++i) {
					Entity* entity = entity_system.entities + i;
					if (entity->type == EntityType_Character || entity->type == EntityType_Background)
						entity->sprite = GetSprite(entity->sprite.name);
					else {
						entity->sprite = GetTextureFromMap(texture_system.inventory_map, entity->sprite.name);
					}
				}
			}
#endif
			// toggle drawing traversal area
			//EditorToggleIcon("#177#", &g_start_drawing);

			// open spritesheet slicer
			if (EditorButtonIcon("#202#", "Open Spritesheet Slicer"))
				EditorChangeWindow(EditorWindow_Slice_Spritesheet);

			// reinit game
			if (EditorButtonIcon("#154#", "Reinit Game")) {
				//g_init = true;
			}

		}
		EguiBoxEnd();

		// Bottom Panel
		EguiBoxBegin(((Box) {
			.name = Str32Create("Bottom part of main screen"), .row_mode = true, .grow_horizontal = true,
				.grow_vertical = true
		}));
		{
#if 1
			//Left Panel
			EguiBoxBegin(((Box) { .name = Str32Create("Left part of main screen"),  .grow_horizontal = true, .grow_vertical = true }));
			{
#if 0
				// Game screen
				EguiBoxBegin(((Box) { .name = Str32Create("Game screen"), .per = 0.5, .color = LIGHTGRAY }));
				{
					EguiTexturePanel(egui.current_pos, 1, target);
				}
				EguiBoxEnd();

				// Log
				EguiBoxBegin(((Box) { .name = Str32Create("Game screen"), .color = LIGHTGRAY }));
				{
					EguiLog((PlatformRect) { LOG_ORIGIN_X, LOG_ORIGIN_Y, LOG_WIDTH, LOG_HEIGHT });
				};
				EguiBoxEnd();
#endif
			}
			EguiBoxEnd();

			// Right panel
			EguiBoxBegin(((Box) {
				.name = Str32Create("Right part of main screen"),
					.grow_vertical = true, .w = 100
			}));
			{
#if 0

				// lists window
				if (editor.window == EditorWindow_Lists) {
					// Top
					EguiBoxBegin(((Box) { .name = Str32Create("tabs"), .h = 100, .row_mode = true, .grow_horizontal = true, .color = LIGHTGRAY }));
					{
						//	EditorTabBar((char* []) { "Entities", "Events", "Areas", "Inventory" }, 4, & editor.editor_tab);
						EditorButton("hi", "hi");
						EditorButton("bye", "hi");
					}
					EguiBoxEnd();
#if 0
					// List panel
					EguiBoxBegin(((Box) { .grow_horizontal = true, .h = 400, .color = LIGHTGRAY }));
					{
						//EguiPanel(EguiBoxGetCurrent()->absolute_rect, 0);

						if (editor.editor_tab == EditorTab_Entities) {
							for (int entity_index = 1; entity_index < MAX_NUM_ENTITIES; ++entity_index) {
								if (entity_system.entities[entity_index].type == EntityType_None) continue;

								// draw entity name and position
								Entity entity = entity_system.entities[entity_index];
								char entity_name[128] = { 0 };
								sprintf(entity_name, "#149# %s", entity.name);
								EditorBeginOffset((Vector2) { 5, 0 });
								PlatformRect entity_text_rect = (PlatformRect){ egui.current_pos.x, egui.current_pos.y, 256, EDITOR_BUTTON_SIZE_Y };
								EditorLabelButton(entity_name);
								EditorEndOffset();

								// Drop down panel
								if (CheckCollisionPointRec(input_system.mouse_pos, entity_text_rect) && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
									editor.modify_entity_drop_down_panel = true;
									editor.drop_down_panel_pos = input_system.mouse_pos;
									editor.entity = entity_system.entities[entity_index].id;
									sprintf(editor.entity_width, "%d", editor.entity_size.x);
									sprintf(editor.entity_height, "%d", editor.entity_size.y);
								}
							}
						}
					}
					EguiBoxEnd();
#endif
				}

#endif

			}
			EguiBoxEnd();
#endif
		}
		EguiBoxEnd();

#if FIX_ME
		if (editor.show_add_entity_box) {
			AddWindowBox();
		}
		else if (editor.show_add_item_box) {
			PlatformRect rect = (PlatformRect){ 50, 50, 200, 200 };
			if (GuiWindowBox(rect, "hi!")) {
				editor.show_add_entity_box = false;
			}

			Vector2 pos = { rect.x + 5, rect.y + 20 };

			egui.current_pos = pos;
			static bool bb;
			static char text_box[NAME_SIZE];
			if (EditorInputBox2("name:", text_box, bb)) {
				bb = !bb;
			}

			if (EditorButton("create!")) {
				CreateItem(text_box);
			}
		}
#endif

		// TODO: Something's broken about the exit button, and we have two 'show_modify_entity_box' bools nested
		// Modify entity window box
		//editor.show_modify_entity_box = true;
		if (editor.show_modify_entity_box)
		{
			EguiDialogBegin(Str32Create("modify entity box"), EditorWindowId_ModifyEntity,
				&editor.modify_entity_window_pos.x,
				&editor.modify_entity_window_pos.y,
				editor.modify_entity_window_size.x,
				editor.modify_entity_window_size.y,
				(EguiV2) {
				0, 0
			},
				& editor.show_modify_entity_box);
			{
				// Lock entity position
				//EditorButton("Lock", &GetEntity(editor.entity)->is_locked);
				EditorButton("Lock", "Lock");

				// Inputs
#if 0
				EguiBoxBegin(((Box) { .w_internal = 100, .h_internal = 100 }));
				{
					EditorLabel("name:");
					EditorTextBox("asd");
				}
				EguiBoxEnd();
#endif
				// Modify button
#if 0
				if (EditorButton("modify!", 0)) {
					Entity* entity = GetEntity(editor.entity);

					strcpy(entity->name, editor.entity_name);
					entity->size = (Vector2){ TextToInteger(editor.entity_width), TextToInteger(editor.entity_height) };
				}
#endif
			}
			EguiEndDialog();
		}

		// Drop down panel for entity 
		if (editor.modify_entity_drop_down_panel) {
			if (EguiDropDownBoxBegin(editor.drop_down_panel_pos, drop_down_panel_size)) {

				if (EditorLabel2("Delete", 0)) {
					//DestroyEntity(editor.entity);
					editor.modify_entity_drop_down_panel = false;
				};

				if (EditorLabel2("Modify", 0)) {
					editor.show_modify_entity_box = true;
					editor.modify_entity_window_pos = (EguiV2){ 100, 100 };
					editor.modify_entity_window_size = (EguiV2){ 200, 200 };
				}

				if (EditorLabel2("Modify2", 0)) {
					editor.show_modify_entity_box = true;
					editor.modify_entity_window_pos = (EguiV2){ 100, 100 };
					editor.modify_entity_window_size = (EguiV2){ 200, 200 };
				}
			}
			else {
				editor.modify_entity_drop_down_panel = false;
			}
			EguiDropDownBoxEnd();
		}

		// Draw tool
#if 1
		static bool init;
		if (!init) {
			init = true;
			editor.draw_tool_pos = (EguiV2){ 0, 0 };
			editor.draw_tool_size = (EguiV2){ 500, 500 };
			editor.draw_tool_active = true;
		}

		editor.draw_tool_active = false;
		if (editor.draw_tool_active) {
			EguiV2 new_pos = editor.draw_tool_pos;

			//editor.draw_tool_pos.x = egui.mouse_pos.x;
			//editor.draw_tool_pos.y = egui.mouse_pos.y;

			EguiDialogBegin(Str32Create("Draw tool"), EditorWindowId_DrawTool,
				&editor.draw_tool_pos.x,
				&editor.draw_tool_pos.y,
				editor.draw_tool_size.x,
				editor.draw_tool_size.y,
				(EguiV2) {
				0, 0
			},
				& editor.draw_tool_active);
			{
#if 0
				EguiBeginPanel(((Panel) { .w = 0.2 * editor.draw_tool_size.x, .h = 0 }));
				{
					Panel* current_panel = EguiGetCurrentPanel();
					Panel* previous_panel = EguiGetPreviousPanel();


					// TODO: Delete
					static bool f;
					static RenderPlatformTexture t;
					if (!f) {
						f = true;
						t = LoadRenderTexture(current_panel->absolute_rect.width, current_panel->absolute_rect.height);
						BeginTextureMode(t);
						ClearBackground(BLUE);
						EndTextureMode();
					}

					EguiTexturePanel(((Vector2) { current_panel->absolute_rect.x, current_panel->absolute_rect.y }),
						((Vector2) {
						current_panel->absolute_rect.width, current_panel->absolute_rect.height
					}), t);

					if (CheckCollisionPointRec(input_system.mouse_pos, current_panel->absolute_rect) && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
						BeginTextureMode(t);
						{
							float x = input_system.mouse_pos.x - current_panel->absolute_rect.x;
							float y = input_system.mouse_pos.y - current_panel->absolute_rect.y;
							DrawPlatformRect(x, y, 10, 10, GREEN);
						}
						EndTextureMode();
					}
				}
				EguiEndPanel();
#endif
			}
			EguiEndDialog();
			//if (new_pos.x != 0 && new_pos.y != 0)
				//editor.draw_tool_pos = new_pos;
		}

#endif

		// Game panel (for mouse collision prevention)
		//EguiBoxBegin(((Box) { .x_internal =  GAME_SCREEN_X, .y_internal = GAME_SCREEN_Y , .w_internal =  GAME_SCREEN_WIDTH, .h_internal = GAME_SCREEN_HEIGHT  }));
		//EguiBoxEnd();

	}

	EguiDrawCommandsBuffer draw_commands = EguiEnd();
	DrawEditor(draw_commands);

	// Draw mouse position
	EguiRect mouse_rect = (EguiRect){ egui.window_width - 200, 0, 200, 100 };
	char mouse_pos_text[128] = { 0 };
	sprintf(mouse_pos_text, "mouse pos: %f %f", mouse_pos.x, mouse_pos.y);
	PlatformTextDraw(mouse_pos_text, mouse_rect.x, mouse_rect.y);
}

void GameInit(int window_width, int window_height) {
}

void GameRun(int window_width, int window_height) {

	DoEditor(0, (EguiV2) {
		platform_system.mouse_x, platform_system.mouse_y
	}, window_width, window_height);
	DrawBatch();
}