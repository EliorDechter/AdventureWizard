#include "Editor.h"
#include "Platform.h"
#include "Textures.h"
#include "Game.h"

void editor_do(float total_time_in_seconds, wzrd_v2 mouse_pos, int window_width, int window_height) {

	assert(window_width);
	assert(window_height);
	wzrd_gui.window_width = window_width;
	wzrd_gui.window_height = window_height;

	static int selected_item = -1;
	static int selected_category;
	Texture_handle handle = { 0 };

	// Drawing data
	wzrd_v2 drop_down_panel_size = { BUTTON_WIDTH, BUTTON_HEIGHT * 2 };

	wzrd_keyboard_keys keys = (wzrd_keyboard_keys){
		.count = platform.input_keys_count
	};

	for (int i = 0; i < keys.count; ++i) {
		keys.keys[i] = *(wzrd_keyboard_key *)&platform.input_keys[i];
		printf("%c\n", platform.keyboard_states[i]);
	}

	wzrd_begin(total_time_in_seconds,
		mouse_pos, platform.mouse_left, keys, *(wzrd_texture*)&g_checkmark_texture);
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
					static EguiV2i pos;
					EguiCrate(
						6,
						(wzrd_box) {
						.name = str128_create("Main window2 bla bla"),
							.y = wzrd_box_get_current()->h,
							.w = 50,
							.h = 50
					});
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

			// Seperator
			EguiBox((wzrd_box) {
				.border_type = BorderType_LeftLine,
					.w = 2
			});

			if (wzrd_button_icon(wzrd_gui.icons.delete)) {
				if (selected_category == 0) {
					if (selected_item >= 0) {
						game_texture_remove_by_index(selected_item);
					}
				}
			}

			// Seperator
			EguiBox((wzrd_box) {
				.border_type = BorderType_LeftLine,
					.w = 2
			});

			if (wzrd_button_icon(wzrd_gui.icons.entity)) {

			}

			// Seperator
			EguiBox((wzrd_box) {
				.border_type = BorderType_LeftLine,
					.w = 2
			});

			if (wzrd_button_icon(wzrd_gui.icons.play)) {

			}

			if (wzrd_button_icon(wzrd_gui.icons.stop)) {

			}

			if (wzrd_button_icon(wzrd_gui.icons.pause)) {

			}

			// Seperator
			EguiBox((wzrd_box) {
				.border_type = BorderType_LeftLine,
					.w = 2
			});

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

				str128 texts[] = { str128_create("Textures"), str128_create("Entities"), str128_create("Actions") };
				static bool active;
				wzrd_dropdown(&selected_category, texts, 3, 100, &active);

				wzrd_box_begin((wzrd_box) {
					.name = str128_create("white"), .border_type = BorderType_Clicked, .color = EGUI_WHITE,
				});
				{

					if (selected_category == 0) {
						Label_list label_list = (Label_list){
							.count = game.textures_handle_map.count
						};
						
#if 1
						int c = 0;
						for (int i = 0; i < game.textures_handle_map.count; ++i) {
							Handle_internal handle = game.textures_handle_map.handles[i];
							if (handle.used) {
								int index = i;
								label_list.val[c++] = game.textures[index].name;
							}
						}
#else
						Handle_iterator
#endif

						wzrd_label_list2(label_list,
							(wzrd_box){
							.color = EGUI_WHITE, .border_type = BorderType_None,
						},
							&selected_item);
					}
#if 0
					EguiMenuNodeAdd(str128_create("MyGame"), 0, false);
					static bool c = false;
					if (EguiMenuNodeAdd(str128_create("Entities"), 1, false)) {
						c = true;
					}
					static EguiV2i pos;
					if (c) {
						EguiCrate(0, (Box) { .w = 100, .h = 100 });
					}
#endif
				}
				wzrd_box_end();
			}
			wzrd_box_end();

			//Right Panel
			//wzrd_box_begin(((Box) { .border_type = BorderType_None, }));
			{
				// Target
				wzrd_box_begin(((wzrd_box) {
					//.h = 500,
					0,
						.border_type = BorderType_Clicked
				}));
				{
					wzrd_box_begin(((wzrd_box) { .name = str128_create("Target") }));
					{
						wzrd_item_add((Item) {
							.type = ItemType_Texture,
								.texture = *(wzrd_texture*)&g_target,
								.scissor = true
						});
					}
					wzrd_box_end();
				}
				wzrd_box_end();

				PlatformTextureBeginTarget(g_target);
				{
					SDL_SetRenderDrawColor(sdl.renderer, 255, 255, 255, 255);
					SDL_RenderClear(sdl.renderer);

					int size = 16;
					int rects_in_row_count = g_target.data->w / size;
					int rects_in_column_count = g_target.data->h / size;
					int x = 0, y = 0;
					bool flip = false;
					platform_color color1 = (platform_color){ 0xDA, 0xB1, 0xDA, 255 };
					platform_color color2 = (platform_color){ 150, 150, 150, 255 };
					//(PlatformColor){ 200, 200, 200, 255 };
					for (int i = 0; i < rects_in_column_count; ++i) {
						for (int j = 0; j < rects_in_row_count; ++j) {
							platform_color color = { 0 };
							if ((j + i * rects_in_row_count) % 2 == 0) {
								if (!flip)
									color = color1;
								else
									color = color2;
							}
							else {
								if (!flip)
									color = color2;
								else
									color = color1;
							}
							PlatformRectDraw((PlatformRect) { x, y, size, size }, color);

							x += size;
						}

						x = 0;
						y += size;
						flip = !flip;
					}

				}
				PlatformTextureEndTarget();
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

	bool change_click_icon = false;
	wzrd_end(&change_click_icon);
	PlatformSetCursor(wzrd_gui.cursor);
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

void EguiLog(wzrd_rect rect) {

	static int num_lines;
	static wzrd_v2 scroll;
	static s_is_log_dirty;
	static char log_buffer[128];
	static char multi_line_text[128];

	if (s_is_log_dirty) {
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

		num_lines = line_num;
		scroll.y = -1 * 10 * (num_lines - 10);
	}

	wzrd_rect scissorRec = { 0 };
	if (s_is_log_dirty) {
		scroll.y = -(10 * (num_lines - 10));
	}
	//GuiScrollPanel(rect, "Log", (PlatformRect) { rect.x, rect.y, rect.width, 966 }, & scroll, & scissorRec);
	BeginScissorMode(scissorRec.x, scissorRec.y, scissorRec.w, scissorRec.h);

	wzrd_rect label_rect1 = (wzrd_rect){ rect.x + 10, rect.y + scroll.y + 30, rect.w, rect.h };
	//GuiLabel(label_rect1, multi_line_text);

	EndScissorMode();
}

void editor_entity_grab(Entity_handle handle) {

}