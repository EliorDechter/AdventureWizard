#include "Game.h"

#define MAX_NUM_DRAW_COMMANDS 1024

void Draw(EguiDrawCommand command) {
	assert(command.dest_rect.x >= 0);
	assert(command.dest_rect.y >= 0);
	assert(command.dest_rect.w >= 0);
	assert(command.dest_rect.h >= 0);
	//assert(command.rect_color.a);

	assert(egui.commands_count < MAX_NUM_DRAW_COMMANDS - 1);
	egui.draw_commands[egui.commands_count++] = command;
}

void DrawBatch() {
	for (int i = 0; i < egui.commands_count; ++i) {
		EguiDrawCommand command = egui.draw_commands[i];
		if (command.type == DrawCommandType_Texture)
			PlatformTextureDraw(*(PlatformTexture*)&command.texture,
				*(PlatformRect*)&command.dest_rect);
		else if (command.type == DrawCommandType_String) {
			//assert(!*command.str.str);
			PlatformTextDraw(command.str.str, command.dest_rect.x, command.dest_rect.y);
		}
		else if (command.type == DrawCommandType_Rect) {
			PlatformRectDraw(*((PlatformRect*)&command.dest_rect), *((PlatformColor*)&command.color));
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
		else {
			//assert(0);
		}



	}

}

void DrawEditor(EguiDrawCommandsBuffer commands) {

}

EguiV2i TextGetSize(const char* str) {
	PlatformV2i result = PlatformTextGetSize(str);
	return *(EguiV2i*)&result;
}
void EguiBoxDo(Box box) {
	EguiBoxBegin(box);
	EguiBoxEnd();
}

EguiRect EguiRectScale(EguiRect rect, float scale) {
	float new_w = rect.w * scale;
	float new_h = rect.h * scale;

	EguiRect result = {
		rect.x + (rect.w / 2 - new_w / 2),
		rect.y + (rect.h / 2 - new_h / 2),
		new_w,
		new_h
	};

	return result;
}

void EguiMenuNode(const char* list[], int count) {

	// Head
	EguiBoxBegin((Box) {
		.border_type = BorderType_None,
			.color = EGUI_WHITE,
			.w = 25,
			.h = 25
	});
	{
		EguiItemAdd((Item) { .type = ItemType_BottomVerticalDottedLine });
		//EguiItemAdd((Item) { .type = ItemType_Rect });
		EguiItemAdd((Item) {
			.type = ItemType_Texture,
				.texture = *(EguiTexture*)&g_texture,
				.size = (EguiV2){ 35, 35 }
		});
	}
	EguiBoxEnd();

	// List
	for (int i = 0; i < count; ++i) {
		EguiBoxBegin((Box) {
			.row_mode = true,
				.h = 20,
				.grow_horizontal = true,
				.color = EGUI_WHITE,
				.border_type = BorderType_None,
		});
		{
			// Button
			EguiBoxBegin((Box) { .border_type = BorderType_None,  .w = EDITOR_ICON_SIZE, .h = EDITOR_ICON_SIZE });
			{
				if (i != count - 1)
					EguiItemAdd((Item) { .type = ItemType_VerticalDottedLine });
				else
					EguiItemAdd((Item) { .type = ItemType_TopVerticalDottedLine });

				EguiItemAdd((Item) { .type = ItemType_RightHorizontalDottedLine });
				//EguiItemAdd((Item) { .type = ItemType_Rect, .rect = (EguiRect){ 0, 0, 100, 100 }, .color = EGUI_RED });

				EguiRect new_rect = EguiRectScale((EguiRect) { 0, 0, EguiBoxGetCurrent2()->w, EguiBoxGetCurrent2()->h }, 0.5);
				EguiButtonBegin((Box) { .border_type = BorderType_None, .color = EGUI_GREEN,
					.x = new_rect.x, .y = new_rect.y, .w = new_rect.w, .h = new_rect.h });
				{
#if 0
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
#endif
				}
				EguiButtonEnd();
			}
			EguiBoxEnd();

			// Icon
			EguiBoxBegin((Box) {
				.border_type = BorderType_None,
					.color = EGUI_WHITE,
					.w = 25,
					.h = 25
			});
			{
				EguiItemAdd((Item) { .type = ItemType_LeftHorizontalDottedLine });
				EguiItemAdd((Item) {
					.type = ItemType_Texture,
						.size = (EguiV2){ 30, 30 },
						.texture = *(EguiTexture*)&g_texture
				});
			}
			EguiBoxEnd();

			// Name
			EguiLabel(Str32Create("Entities"), (EguiV2i) { 10, 10 });
		}
		EguiBoxEnd();
	}

#if 1

#else
	// Top Line
	EguiBoxBegin((Box) {
		.row_mode = true,
			.h = 20,
			.grow_horizontal = true,
			.color = EGUI_WHITE,
			.border_type = BorderType_None
	});
	{
		EguiBoxBegin((Box) {
			.name = Str32Create("F"),
				.border_type = BorderType_None,
				.color = EGUI_WHITE,
				.w = 25,
				.h = 25
		});
		{
			EguiItemAdd((Item) { .type = ItemType_BottomVerticalDottedLine });
			EguiItemAdd((Item) { .type = ItemType_Rect });
		}
		EguiBoxEnd();
		EguiLabel(Str32Create("Entities"), (EguiV2i) { 10, 10 });
	}
	EguiBoxEnd();

	// Bottom Line
	EguiBoxBegin((Box) {
		.row_mode = true,
			.h = 20,
			.grow_horizontal = true,
			.color = EGUI_WHITE,
			.border_type = BorderType_None
	});
	{
		EguiBoxBegin((Box) {
			.name = Str32Create("F"),
				.border_type = BorderType_None,
				.color = EGUI_WHITE,
				.w = 25,
				.h = 25
		});
		{
			EguiItemAdd((Item) { .type = ItemType_TopVerticalDottedLine });
			EguiItemAdd((Item) { .type = ItemType_RightHorizontalDottedLine });
			EguiItemAdd((Item) { .type = ItemType_Rect });
		}
		EguiBoxEnd();

		EguiBoxBegin((Box) {
			.name = Str32Create("F"),
				.border_type = BorderType_None,
				.color = EGUI_WHITE,
				.w = 25,
				.h = 25
		});
		{
			EguiItemAdd((Item) { .type = ItemType_LeftHorizontalDottedLine });
			EguiItemAdd((Item) { .type = ItemType_Rect });
		}
		EguiBoxEnd();

		EguiLabel(Str32Create("Entity"), (EguiV2i) { 10, 10 });
	}
	EguiBoxEnd();
#endif
}

void EguiMenu() {
	EguiMenuNode(0, 3);
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
			.name = Str32Create("Files"),
				.row_mode = true,
				.n = 1,
				.grow_horizontal = true,
				.h = 50,
				.size_type = SizeType_Fixed,
				.child_gap = 5,
				.center = true
		}));
		{
			EguiLabel(Str32Create("File"), TextGetSize("File"));
			EguiLabel(Str32Create("View"), TextGetSize("View"));
			EguiLabel(Str32Create("Help"), TextGetSize("Help"));
		}
		EguiBoxEnd();
#endif

		// Top panel
		EguiBoxBegin(((Box) {
			.name = Str32Create("Top button panel"),
				.inner_padding = (EguiRect){ 8, 8, 0, 0 },
				.row_mode = true, .push = 8,
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
			//Left Panel
			EguiBoxBegin(((Box) {
				.name = Str32Create("Left part of main screen"),
					.grow_horizontal = true,
					.grow_vertical = true,
					.border_type = BorderType_None,
			}));
			{

				// Game screen
				EguiBoxBegin(((Box) {
					.name = Str32Create("Game screen"),
						.color = EGUI_LIGHTGRAY,
						.texture = *(EguiTexture*)&g_texture,
						.grow_horizontal = true,
						.grow_vertical = true,
						.border_type = BorderType_None
				}));
				{

					EguiItemAdd((Item) {
						.type = ItemType_HorizontalDottedLine,
							.line = (Line){ 5, 5, 100, 100 },
					});
				}
				EguiBoxEnd();
			}
			EguiBoxEnd();

			// Right panel
			EguiBoxBegin((Box) {
				.name = Str32Create("Right part of main screen"),
					.grow_vertical = true,
					.w = 200,
					.color = EGUI_WHITE
			});
			{
				EguiMenu();
			}
			EguiBoxEnd();
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

	EguiEnd();

	// Draw mouse position
	EguiRect mouse_rect = (EguiRect){ egui.window_width - 200, 0, 200, 100 };
	char mouse_pos_text[128] = { 0 };
	sprintf(mouse_pos_text, "mouse pos: %f %f", mouse_pos.x, mouse_pos.y);
	PlatformTextDraw(mouse_pos_text, mouse_rect.x, mouse_rect.y);
}

void GameRun(int window_width, int window_height) {

	DoEditor(0, (EguiV2) {
		platform_system.mouse_x, platform_system.mouse_y
	}, window_width, window_height);
	DrawBatch();
}

void GameInit() {
	g_texture = PlatformTextureLoad("C:\\Users\\elior\\OneDrive\\Desktop\\ConsoleApplication1\\Resoruces\\Textures\\ass.png");
}

