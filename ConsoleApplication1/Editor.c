#include "Editor.h"
#include "Platform.h"
#include "Textures.h"
#include "Game.h"

void editor_do(Egui* gui, wzrd_draw_commands_buffer* buffer, wzrd_cursor* cursor, bool enable_input, PlatformTargetTexture target_texture, wzrd_icons icons) {
	static int selected_item = -1;
	static int selected_category;
	Texture_handle handle = { 0 };

	// Drawing data
	wzrd_v2 drop_down_panel_size = { BUTTON_WIDTH, BUTTON_HEIGHT * 2 };

	wzrd_begin(gui, 0,
		(wzrd_v2) {
		g_platform.mouse_x, g_platform.mouse_y
	},
		g_platform.mouse_left,
		* (wzrd_keyboard_keys*)&g_platform.keys_pressed,
		(wzrd_v2) {
		g_platform.window_width, g_platform.window_height
	},
		icons, EGUI_LIGHTGRAY, enable_input,
		1);
	{
		static bool create_object_active;
		static int dialog_parent;
		static wzrd_v2 create_object_pos = { 100, 100 };

#if 1
		// Files Panel
		wzrd_box_begin(((wzrd_box) {
			.name = str128_create("Files"),
				.row_mode = true,
				.border_type = BorderType_None,
				.h = 30,
				.size_type = SizeType_Fixed,
				.child_gap = 10,
				.pad_left = 5,
				.center_y = true,
		}));
		{
			static wzrd_v2 s;
			EguiBoxResize(&s);
			static bool f;
			if (EguiLabelButtonBegin(str128_create("File"))) f = !f;
			{
				if (f) {
					wzrd_v2 origin = (wzrd_v2){ 40, wzrd_box_get_current()->h };
					static wzrd_v2 offset;
					wzrd_crate(
						6,
						(wzrd_box) {
						.name = str128_create("Main window2 bla bla"),
							.x = origin.x,
							.y = origin.y + offset.y,
							.w = 50,
							.h = 50
					});

					if (wzrd_box_is_hot(wzrd_box_get_last()))
					{
						offset = wzrd_lerp(offset, (wzrd_v2) { 0,30 });
					}
					else 
					{
						offset = wzrd_lerp(offset, (wzrd_v2) { 0, 0});
					}
				}
			}
			EguiLabelButtonEnd();

			EguiLabelButton(str128_create("View"));
			EguiLabelButton(str128_create("Help"));
		}
		wzrd_box_end();

		// Seperator
		EguiBox((wzrd_box) {
			.border_type = BorderType_BottomLine,
				.h = 2
		});

		// Buttons panel
		wzrd_box_begin(((wzrd_box) {
			.name = str128_create("sss"),
				.pad_left = 5,
				.center_y = true,
				.child_gap = 5,
				.row_mode = true,
				.h = 36,
				.border_type = BorderType_None,
		}));
		{
			static wzrd_v2 s;
			EguiBoxResize(&s);
			dialog_parent = wzrd_box_get_current_index();

			if (EguiButton(str128_create("Add Object"))) {
				create_object_pos = (wzrd_v2){ 50, 50 };
				create_object_active = true;
			}

			EguiToggle3((wzrd_box) { .center = true, .w = 200, .h = 32 },
				str128_create("Add Polygon"), & g_game.polygon_adding_active);

			// Seperator
			EguiBox((wzrd_box) {
				.border_type = BorderType_LeftLine,
					.w = 2
			});

			if (wzrd_button_icon(icons.delete)) {
				if (selected_category == 0) {
					if (selected_item >= 0) {
						game_texture_remove_by_index(selected_item);
					}
				}

				g_game.delete = true;
			}
			else
			{
				g_game.delete = false;
			}

			// Seperator
			EguiBox((wzrd_box) {
				.border_type = BorderType_LeftLine,
					.w = 2
			});

			static bool b = false;
			if (wzrd_button_icon(icons.entity))
			{
			}

			static bool finish;
			bool is_dragged = wzrd_box_is_dragged(&(wzrd_box){.name = "hi" });
			if (!is_dragged && !finish)
			{
				EguiBox((wzrd_box) { .w = 25, .h = 25, .is_draggable = true, .name = str128_create("hi") });
			}

			static wzrd_color color = { 100, 100, 100, 255 };
			EguiBox((wzrd_box) { .w = 25, .h = 25, .is_slot = true, .color = color, .name = str128_create("bye") });

			if (wzrd_box_is_hot(wzrd_box_get_last()) && wzrd_is_releasing())
			{
				color = EGUI_LIME;
				finish = true;
			}

			// Seperator
			EguiBox((wzrd_box) {
				.border_type = BorderType_LeftLine,
					.w = 2
			});

			if (wzrd_button_icon(icons.play)) {

			}

			if (wzrd_button_icon(icons.stop)) {

			}

			if (wzrd_button_icon(icons.pause)) {
				
			}

			// Seperator
			EguiBox((wzrd_box) {
				.border_type = BorderType_LeftLine,
					.w = 2
			});

			static wzrd_v2 p;
			EguiBox((wzrd_box) { .w = 25, .h = 25, .x = p.x, .y = p.y, .name = str128_create("die") });

			if (wzrd_box_is_hot(wzrd_box_get_last()))
			{
				p = wzrd_lerp(p, (wzrd_v2) { 0, 5 });
			}
			else {
				p = wzrd_lerp(p, (wzrd_v2) { 0, 0 });
			}
		}
		wzrd_box_end();

		wzrd_box_begin(((wzrd_box) {
			.border_type = BorderType_None, .name = str128_create("main space"),
				.row_mode = true, .child_gap = 5
		}));
		{
			wzrd_box_begin((wzrd_box) {
				.name = str128_create("left menu"),
					.pad_top = 5,
					.pad_left = 5,
					.pad_right = 5,
					.pad_bottom = 5,
					.w = 200,
					.child_gap = 5,
					.border_type = BorderType_Clicked,
			});
			{
				static wzrd_v2 size_offset;

				EguiBoxResize(&size_offset);

				str128 texts[] = { str128_create("Entities"), str128_create("Textures"), str128_create("Actions") };
				static bool active;
				wzrd_dropdown(&selected_category, texts, 3, 100, &active);

				wzrd_box_begin((wzrd_box) {
					.name = str128_create("white"), .border_type = BorderType_Clicked, .color = EGUI_WHITE,
				});
				{

					if (selected_category == 0) {
						Label_list label_list = (Label_list){
							.count = g_game.entities_handle_map.count - 1
						};

						int it = 1;
						int i = 0;
						Entity* entity = 0;
						while (entity = entity_get_next(&it)) {
							label_list.val[i++] = entity->name;
						}

						wzrd_label_list2(label_list,
							(wzrd_box) {
							.color = EGUI_WHITE, .border_type = BorderType_None,
						},
							& selected_item);

					}

					if (selected_category == 0) {
#if 0
						Label_list label_list = (Label_list){
							.count = textures_handle_map.count
						};

						int c = 0;
						for (int i = 0; i < textures_handle_map.count; ++i) {
							Handle_internal handle = textures_handle_map.handles[i];
							if (handle.used) {
								int index = i;
								label_list.val[c++] = textures[index].name;
							}
						}

						wzrd_label_list2(label_list,
							(wzrd_box) {
							.color = EGUI_WHITE, .border_type = BorderType_None,
						},
							& selected_item);
#endif

					}
					SDL_SetRenderDrawColor(g_sdl.renderer, 255, 255, 255, 255);
					SDL_RenderClear(g_sdl.renderer);

					int size = 16;
					int rects_in_row_count = target_texture.data->w / size;
					int rects_in_column_count = target_texture.data->h / size;
					int x = 0, y = 0;
					bool flip = false;
					for (int i = 1; i < rects_in_column_count; ++i) {
						for (int j = 1; j < rects_in_row_count; ++j) {

							PlatformRectDraw((PlatformRect) { x, y, 2, 2 }, PLATFORM_GRAY);

							x += size;
						}

						x = 0;
						y += size;
						flip = !flip;
					}


					if (g_game.polygon_adding_active)
					{
						//float mouse_x = g_platform.mouse_x / scale;
					}
				}
				wzrd_box_end();
			}
			wzrd_box_end();

			//Right Panel
			{
				// Target
				wzrd_box_begin(((wzrd_box) {
					.center = true,
						.color = EGUI_PINK,
						.disable_input = true,
						.border_type = BorderType_Clicked
				}));
				{
					wzrd_box_begin(((wzrd_box) { .w = 1920 / 6, .h = 1080 / 6, .best_fit = true, .name = str128_create("Target"), .disable_input = true }));
					{
						wzrd_item_add((Item) {
							.type = ItemType_Texture,
								.texture = *(wzrd_texture*)&target_texture,
								.scissor = true
						});
					}
					wzrd_box_end();
				}
				wzrd_box_end();

			}
			//wzrd_box_end();
		}
		wzrd_box_end();

#endif
		wzrd_box_begin(((wzrd_box) {
			.name = str128_create("bottom "),
				.row_mode = true,
				.h = 40,
				.border_type = BorderType_None
		}));
		{
			EguiBox((wzrd_box) { .w = 90 });
			EguiBox((wzrd_box) { 0 });
		}
		wzrd_box_end();

		// Add object dialog
		{
			wzrd_v2 size = { 500, 500 };
			bool ok = false, cancel = false;
			static int selected_text;
			static str128 name;

			wzrd_dialog_begin(&create_object_pos, size, &create_object_active, str128_create("add object"), dialog_parent);
			if (create_object_active) {
				wzrd_box_begin((wzrd_box) {
					.row_mode = true, .border_type = BorderType_None, .child_gap = 5,

				});
				{
					static int selected;
					wzrd_label_list((Label_list) {
						.count = 2,
							.val = { str128_create("Texture"), str128_create("Entity"), str128_create("Event") }
					}, & selected);

					wzrd_box_begin((wzrd_box) {
						.border_type = BorderType_Clicked,
							.pad_left = 5,
							.pad_top = 5
					});
					{
						if (selected_text == 0) {
							wzrd_box_begin((wzrd_box) { .border_type = BorderType_None, .row_mode = true, .h = 50, });
							{
								EguiLabel(str128_create("Name:"));
								wzrd_input_box(&name, 10);
							}
							wzrd_box_end();
						}
						else if (selected_text == 1) {

						}
					}
					wzrd_box_end();
				}
				wzrd_box_end();

				wzrd_box_begin((wzrd_box) { .row_mode = true, .h = 50, .border_type = BorderType_None });
				{
					EguiBox((wzrd_box) { .border_type = BorderType_None });
					wzrd_box_begin((wzrd_box) {
						.center = true,
							.child_gap = 10,
							.pad_left = 5,
							.pad_top = 5,
							.pad_right = 5,
							.pad_bottom = 5,
							.row_mode = true, .w = 140, .child_gap = 10,
							.border_type = BorderType_None
					});
					{
						if (EguiButton(str128_create("OK"))) {
							ok = true;
						}
						if (EguiButton(str128_create("Cancel"))) {
							cancel = true;
						}
					}
					wzrd_box_end();
				}
				wzrd_box_end();

			}
			EguiDialogEnd(create_object_active);

			if (ok) {
				if (selected_text == 0) {
					Texture texture = texture_get_by_name(name);
					game_texture_add(texture);
				}

				create_object_active = false;
			}

			if (cancel) {
				create_object_active = false;
			}
		}
	}

	wzrd_end(cursor, buffer);
}

void EditorSliceSpritesheet() {
#if 0
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
	static PlatformTexture t;
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

	EguiDrawPanel((PlatformRect) { egui.current_pos.x, egui.current_pos.y - 5, EDITOR_RIGHT_PANEL_WIDTH - 10, 600 }, WHITE);

	Vector2 offset = (Vector2){ 5, 0 };
	EditorBeginOffset(offset);

	if (EditorInputBox2("Texture:", file_name, b1)) {
		b1 = !b1;
	}

	if (EditorButton("Load texture")) {
		t = GetSprite(file_name).texture;
		sprintf(width_str, "%d", t.width);
		sprintf(height_str, "%d", t.height);
	}
	if (EditorInputBox2("x:", x_str, b4)) {
		b4 = !b4;
	}
	if (EditorInputBox2("y:", y_str, b5)) {
		b5 = !b5;
	}

	if (EditorInputBox2("width:", width_str, b7)) {
		b7 = !b7;
	}
	if (EditorInputBox2("height:", height_str, b8)) {
		b8 = !b8;
	}

	if (EditorInputBox2("Rows:", rows_str, b2)) {
		b2 = !b2;
	}

	if (EditorInputBox2("Columns:", columns_str, b3)) {
		b3 = !b3;
	}

	if (EditorInputBox2("Animation names:", animation_names, b6)) {
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

			OldLog("Saved spritesheet data\n");
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
			DrawTextureRect(t, (PlatformRect) { 0, 0, GAME_SCREEN_WIDTH, t.height* GAME_SCREEN_WIDTH / t.width });
		}
		DrawTextureRect(t, (PlatformRect) { 0, 0, new_width, new_height });

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

	EditorEndOffset();
#endif
}

char multi_line_text[1024 * 10];

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
