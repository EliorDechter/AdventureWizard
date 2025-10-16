#include "Editor.h"
#include "Platform.h"
#include "Textures.h"
#include "Game.h"

void editor_do(wzrd_gui* gui, wzrd_draw_commands_buffer* buffer, wzrd_cursor* cursor, PlatformTargetTexture target_texture, wzrd_icons icons, unsigned int layer) {
	static int selected_item = -1;
	static int selected_category;


	wzrd_update_input((wzrd_v2) {
		g_platform.mouse_x, g_platform.mouse_y
	},
		(wzrd_state)g_platform.mouse_left,
		*(wzrd_keyboard_keys*)&g_platform.keys_pressed);
	platform_time_begin();

	wzrd_begin(gui, 
	(wzrd_rect) {
		0, 0, (float)g_platform.window_width, (float)g_platform.window_height
	}, wzrd_style_get_default(),
			platform_string_get_size,
			layer);
	if (1) {
		static bool create_object_active;
		static int dialog_parent;
		static wzrd_v2 create_object_pos = { 100, 100 };
		
		// Files Panel
		wzrd_box_begin(((wzrd_box) {
			//.name = str128_create("Files"),
				.row_mode = true,
				.border_type = BorderType_None,
				.h = 30,
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
					wzrd_v2 origin = (wzrd_v2){ 40, wzrd_box_get_from_top_of_stack()->h };
					static wzrd_v2 offset;
					wzrd_crate(
						6,
						(wzrd_box) {
						//.name = str128_create("Main window2 bla bla"),
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
		wzrd_box_do((wzrd_box) {
			.border_type = BorderType_BottomLine,
				.h = 2
		});

		// Buttons panel
		wzrd_box_begin(((wzrd_box) {
			//.name = str128_create("sss"),
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

			if (wzrd_button(str128_create("Add Object"))) {
				create_object_pos = (wzrd_v2){ 50, 50 };
				create_object_active = true;
			}
			
			EguiToggle3((wzrd_box) { .center = true, .w = 200, .h = 32 },
				str128_create("Add Polygon"), & g_game.polygon_adding_active);

			// Seperator
			wzrd_box_do((wzrd_box) {
				.border_type = BorderType_LeftLine,
					.w = 2
			});

			static bool delete_toggle;
			wzrd_toggle_icon(icons.delete, &delete_toggle);

			if (delete_toggle) {
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
			wzrd_box_do((wzrd_box) {
				.border_type = BorderType_LeftLine,
					.w = 2
			});

			/*static bool b = false;
			if (wzrd_button_icon(icons.entity))
			{
			}*/

			static bool finish;
			bool is_dragged = wzrd_box_is_dragged(&(wzrd_box){
				//.name = str128_create("hi") 
				0
			});
			if (!is_dragged && !finish)
			{
				wzrd_box_do((wzrd_box) { .w = 25, .h = 25, .is_draggable = true, 
					//.name = str128_create("hi")
				});
			}

			static wzrd_color color = { 100, 100, 100, 255 };
			wzrd_box_do((wzrd_box) { .w = 25, .h = 25, .is_slot = true, .color = color, 
				//.name = str128_create("bye")
			});

			if (wzrd_box_is_hot(wzrd_box_get_last()) && wzrd_is_releasing())
			{
				color = EGUI_LIME;
				finish = true;
			}

			// Seperator
			wzrd_box_do((wzrd_box) {
				.border_type = BorderType_LeftLine,
					.w = 2
			});

			wzrd_toggle_icon(icons.play, &g_game.run);

			if (wzrd_button_icon(icons.stop)) {

			}

			if (wzrd_button_icon(icons.pause)) {
				
			}

			// Seperator
			wzrd_box_do((wzrd_box) {
				.border_type = BorderType_LeftLine,
					.w = 2
			});

			static wzrd_v2 p;
			wzrd_box_do((wzrd_box) { .w = 25, .h = 25, .x = p.x, .y = p.y, 
				//.name = str128_create("die")
			});

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
			.border_type = BorderType_None,
				//.name = str128_create("main space"),
				.row_mode = true, .child_gap = 5
		}));
		{
			wzrd_box_begin((wzrd_box) {
				//.name = str128_create("left menu"),
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
					//.name = str128_create("white"),
						.border_type = BorderType_Clicked, .color = EGUI_WHITE,
				});
				{

					if (selected_category == 0) {
						Label_list label_list = (Label_list){
							.count = g_game.entities_handle_map.count - 1
						};

						int it = 1;
						int i = 0;
						Entity* entity = 0;
						while ((entity = entity_get_next(&it))) {
							label_list.val[i++] = entity->name;
						}

						wzrd_label_list2(label_list,
							(wzrd_box) {
							.color = EGUI_WHITE, .border_type = BorderType_None,
						},
							& selected_item);

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
						.color = (wzrd_color){50, 50, 50, 255},
						.disable_input = true,
						.border_type = BorderType_Clicked
				}));
				{
					wzrd_box_begin(((wzrd_box) { .w = 1920 / 6, .h = 1080 / 6, .best_fit = true,
						//.name = str128_create("Target"),
						.disable_input = true }));
					{
						wzrd_item_add((Item) {
							.type = ItemType_Texture,
								.val = { .texture = *(wzrd_texture*)&target_texture },
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

		wzrd_box_begin(((wzrd_box) {
			//.name = str128_create("bottom "),
				.row_mode = true,
				.h = 40,
				.border_type = BorderType_None
		}));
		{
			wzrd_box_do((wzrd_box) { .w = 90 });
			wzrd_box_do((wzrd_box) { 0 });
		}
		wzrd_box_end();

		// Add object dialog
		{
			wzrd_v2 size = { 500, 500 };
			bool ok = false, cancel = false;
			static int selected_text;
			static str128 name;

			wzrd_dialog_begin(&create_object_pos, size, &create_object_active, str128_create("add object"), dialog_parent, 4);
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
								wzrd_label(str128_create("Name:"));
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
					wzrd_box_do((wzrd_box) { .border_type = BorderType_None });
					wzrd_box_begin((wzrd_box) {
						.center = true,
							.child_gap = 10,
							.pad_left = 5,
							.pad_top = 5,
							.pad_right = 5,
							.pad_bottom = 5,
							.row_mode = true, .w = 140, 
							.border_type = BorderType_None
					});
					{
						if (wzrd_button(str128_create("OK"))) {
							ok = true;
						}
						if (wzrd_button(str128_create("Cancel"))) {
							cancel = true;
						}
					}
					wzrd_box_end();
				}
				wzrd_box_end();

			}
			wzrd_dialog_end(create_object_active);

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
	platform_time_end();

}

char multi_line_text[1024 * 10];

bool IsNumber(const char* str) {
	if (!str || str[0] == '\0')
		return false;
	size_t i = 0;
	if (str[0] == '-') i = 1;
	for (; i < strlen(str); ++i) {
		if (str[i] > '9' || str[i] < '0') return false;
	}

	return true;
}
