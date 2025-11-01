#include "Editor.h"
#include "Platform.h"
#include "Textures.h"
#include "Game.h"

wzrd_str wzrd_str_from_str128(str128* str)
{
	wzrd_str result = { .str = str->val, .len = str->len };

	return result;
}

void editor_seperator_horizontal(wzrd_handle parent)
{
	wzrd_widget((wzrd_style) {
		.space = wzrd_space_create((wzrd_space) { .h = 2 }),
			.skin = wzrd_skin_create((wzrd_skin) { .border_type = BorderType_BottomLine })
	},
		parent);
}

void editor_seperator_vertical(wzrd_handle parent)
{
	wzrd_widget((wzrd_style) {
		.space = wzrd_space_create((wzrd_space) { .w = 2, }),
		.skin = wzrd_skin_create((wzrd_skin) { .border_type = BorderType_LeftLine })
	}, parent);
}

wzrd_handle editor_vertical_panel(wzrd_v2 size, wzrd_handle parent)
{
	wzrd_handle p = wzrd_widget((wzrd_style)
	{
		.space = wzrd_space_create((wzrd_space) { .w = size.x, .h = size.y}),
			.layout = wzrd_canvas_get()->v_panel_layout,
			.skin = wzrd_canvas_get()->panel_skin,
			.structure = wzrd_canvas_get()->panel_structure
	}
	, parent);

	return p;
}

wzrd_handle editor_vertical_panel_bordered(wzrd_v2 size, wzrd_handle parent)
{
	wzrd_handle p = wzrd_widget((wzrd_style)
	{
		.space = wzrd_space_create((wzrd_space) { .w = size.x, .h = size.y }),
			.layout = wzrd_canvas_get()->v_panel_layout,
			.skin = wzrd_canvas_get()->panel_border_skin,
			.structure = wzrd_canvas_get()->panel_structure
	}
	, parent);

	return p;
}

wzrd_handle editor_horizontal_panel_bordered(wzrd_v2 size, wzrd_handle parent)
{
	wzrd_handle p = wzrd_widget((wzrd_style)
	{
		.space = wzrd_space_create((wzrd_space) { .w = size.x, .h = size.y }),
			.layout = wzrd_canvas_get()->h_panel_layout,
			.skin = wzrd_canvas_get()->panel_border_skin,
			.structure = wzrd_canvas_get()->panel_structure
	}
	, parent);

	return p;
}

wzrd_handle editor_horizontal_panel(wzrd_v2 size, wzrd_handle parent)
{
	wzrd_handle p = wzrd_widget((wzrd_style)
	{
		.space = wzrd_space_create((wzrd_space) { .w = size.x, .h = size.y }),
			.layout = wzrd_canvas_get()->h_panel_layout,
			.skin = wzrd_canvas_get()->panel_skin,
			.structure = wzrd_canvas_get()->panel_structure
	}
	, parent);

	return p;
}

void editor_left_panel(wzrd_handle parent)
{
	wzrd_handle outer_panel = editor_vertical_panel_bordered((wzrd_v2) { .x = 200 }, parent);

	//wzrd_str texts[] = { wzrd_str_create("Entities"), wzrd_str_create("Textures"), wzrd_str_create("Actions") };
	//static bool active;
	static int selected_category;
	//wzrd_dropdown(&selected_category, texts, 3, 100, &active);

	wzrd_handle inner_panel = editor_vertical_panel((wzrd_v2) { 0 }
	, outer_panel);

	if (selected_category == 0) {

		wzrd_str labels[MAX_NUM_ENTITIES] = { 0 };

		for (unsigned int i = 0; i < g_game.sorted_entities_count; ++i)
		{
			Entity_handle handle = g_game.sorted_entities[i];
			Entity* entity = game_entity_get(handle);
			labels[i] = (wzrd_str){ .str = entity->name.val, .len = entity->name.len };
		}

		if (g_game.selected_entity_index_to_sorted_entities >= 0)
		{
			// TODO: if I remove the if code crashes
			if (g_game.delete)
			{
				game_entity_remove(g_game.sorted_entities[g_game.selected_entity_index_to_sorted_entities]);
			}
		}

		wzrd_label_list_sorted(labels, g_game.sorted_entities_count, (int*)g_game.sorted_entities,
			(wzrd_v2) {
			0
		},
			& g_game.selected_entity_index_to_sorted_entities, & g_game.is_entity_selected, inner_panel);

	}
}

typedef struct Editor
{
	bool create_object_dialog_active;
	wzrd_v2 create_object_dialog_pos;
} Editor;

Editor g_editor;

void editor_file_panel(wzrd_handle window)
{
	wzrd_handle files_panel = editor_horizontal_panel((wzrd_v2) { .y = 30 }, window);

	bool b1 = false, b2 = false;
	wzrd_label_button(wzrd_str_create("View"), &b1, files_panel);
	wzrd_label_button(wzrd_str_create("Help"), &b2, files_panel);
}

void editor_buttons_panel(wzrd_handle window)
{
	wzrd_handle panel = editor_horizontal_panel((wzrd_v2) { 0, 36 }, window);

	bool b = false;
	wzrd_command_button(wzrd_str_create("Add Object"), &b, panel);

	if (b)
	{
		g_editor.create_object_dialog_active = true;
	}

	wzrd_command_toggle(wzrd_str_create("Add Polygon"), &g_game.polygon_adding_active, panel);

	editor_seperator_vertical(panel);

	//bool delete_toggle;
	//wzrd_button_icon(icons.delete, &delete_toggle, buttons_panel);
	//g_game.delete = delete_toggle;

	// seperator

	static bool finish;
	bool is_dragged = wzrd_box_is_dragged(&(wzrd_box) {
		0
	});
	if (!is_dragged && !finish)
	{
		/*	wzrd_widget((wzrd_style) {
				.style = wzrd_style_create((wzrd_style_template) {
					.w = 25, .h = 25
				}), .is_draggable = true,
			}, panel);*/
	}

	static wzrd_color color = { 100, 100, 100, 255 };
	//wzrd_widget((wzrd_style) {
	//	.is_slot = true,
	//		.style = wzrd_style_create((wzrd_style_template) {
	//		.color = color, .w = 25, .h = 25,
	//	})
	//}, panel);

	if (wzrd_box_is_hot(wzrd_box_get_last()) && wzrd_is_releasing())
	{
		color = EGUI_LIME;
		finish = true;
	}

	editor_seperator_vertical(panel);

	//wzrd_toggle_icon(icons.play, &g_game.run, &panel);

	editor_seperator_vertical(panel);
}

void editor_right_panel(wzrd_handle parent, wzrd_texture texture)
{
	wzrd_handle panel = wzrd_widget((wzrd_style)
	{
		.skin = wzrd_skin_create((wzrd_skin) { .color = EGUI_GREEN, .border_type = BorderType_Clicked }),
		.layout = wzrd_canvas_get()->v_panel_layout
	}, parent);

	wzrd_box_get_by_handle(panel)->disable_input = true;

	wzrd_handle target_panel = wzrd_widget((wzrd_style)
	{
		.layout = wzrd_layout_create((wzrd_layout) { .best_fit = true }),
		.space = wzrd_space_create((wzrd_space) { .w = 1920 / 6, .h = 1080 / 6, }),
		.skin = wzrd_skin_create((wzrd_skin) { .color = EGUI_RED })
	}
	, panel);
	target_panel = wzrd_box_set_unique_handle(target_panel, wzrd_str_create("Target"));

	wzrd_box_get_by_handle(target_panel)->disable_input = true;

	wzrd_item_add((Item) {
		.type = ItemType_Texture,
			.val = { .texture = texture },
			.scissor = true
	}, target_panel);
}

wzrd_handle editor_add_row(wzrd_handle form, wzrd_str label, wzrd_handle widget)
{
	const int h = 30;

	wzrd_handle row = editor_horizontal_panel((wzrd_v2) { 0, h }, form);
	wzrd_label(label, row);
	wzrd_box_add_child(row, widget);

	return row;
}

void editor_status_bar(wzrd_handle parent)
{
	wzrd_handle bar = editor_horizontal_panel((wzrd_v2) { .y = 40 }, parent);
	editor_horizontal_panel((wzrd_v2) { .x = 90 }, bar);
	wzrd_widget((wzrd_style) { 0 }, bar);

}

void editor_create_object_dialog(wzrd_v2* pos, bool* active, wzrd_handle parent)
{
	wzrd_v2 size = { 500, 500 };
	bool ok = false, cancel = false;
	static str128 name;
	static unsigned int selected;
	static bool is_selected = false;

	wzrd_handle dialog = wzrd_dialog_begin(pos, size, active, wzrd_str_create("add object"), 4, parent);
	
	if (active) {
		wzrd_handle panel = editor_horizontal_panel((wzrd_v2) { 0 }, dialog);
		{
			wzrd_str labels[] = { wzrd_str_create("Entity"), wzrd_str_create("Texture") };
			wzrd_label_list(labels, 2,
				(wzrd_v2) {
				100, 0
			},
				0, &selected, &is_selected, panel);

			wzrd_handle form = editor_vertical_panel_bordered((wzrd_v2){0}, panel);
			editor_add_row(form, wzrd_str_create("Name:"), wzrd_input_box(name.val, &name.len, 10, (wzrd_handle) { 0 }));
		}

		wzrd_handle bottom_panel = editor_horizontal_panel((wzrd_v2) { .y = 50 }, dialog);
		{
			wzrd_handle buttons_panel = editor_horizontal_panel((wzrd_v2) { 0 }, bottom_panel);
			{
				bool b1;

				wzrd_command_button(wzrd_str_create("OK"), &b1, buttons_panel);
				if (b1) {
					ok = true;
				}

				bool b2;
				wzrd_command_button(wzrd_str_create("Cancel"), &b2, buttons_panel);
				if (b2) {
					cancel = true;
				}
			}
		}
	}
	wzrd_dialog_end(active);

	if (ok) {
		if (selected == 1) {
			Texture texture = texture_get_by_name(name);
			game_texture_add(texture);
		}
		else if (selected == 0)
		{
			game_entity_create((Entity) {
				.name = str128_create(name.val),
					.texture = game_texture_get_handle_by_name(str128_create("player")),
					.rect = { .x = 0, .y = 0, .w = 100, .h = 100 },
					.color = (wzrd_color){ 255, 255, 255, 255 }
			});
		}

		*active = false;
	}

	if (cancel) {
		*active = false;
	}
}

void editor_debug_panel(wzrd_handle parent, wzrd_str str)
{
	wzrd_handle panel = wzrd_widget_free((wzrd_style) 
	{
		.space = wzrd_space_create((wzrd_space)
		{
			.x = (int)g_platform.window_width - 300,
				.y = 10,
				.w = 295,
				.h = 500,
		}),
		.skin = wzrd_skin_create((wzrd_skin)
		{
				.color = (wzrd_color){150, 200, 60, 200}
		})
	}
	, parent);

	//static float scrollbar_x, scrollbar_y;
	wzrd_box_get_last()->clip = true;
	//wzrd_box_get_last()->scrollbar_x = &scrollbar_x;
	//wzrd_box_get_last()->scrollbar_y = &scrollbar_y;
	wzrd_label(str, panel);
	//*debug_str->str = 0;
	//debug_str->len = 0;
	//wzrd_box_do((wzrd_box) { .h = 1000, .w = 50, .color = EGUI_BLUE });

}


void editor_do(wzrd_canvas* gui, PlatformTargetTexture target_texture, wzrd_icons icons, wzrd_str* debug_str) {
	(void)icons;

	wzrd_handle window = wzrd_begin(gui,
		(wzrd_rect_struct) {
		0, 0, (int)g_platform.window_width, (int)g_platform.window_height
	},
		platform_string_get_size,
		(wzrd_v2) {
		(int)g_platform.mouse_x, (int)g_platform.mouse_y
	},
		(wzrd_state)g_platform.mouse_left,
		* (wzrd_keyboard_keys*)&g_platform.keys_pressed, true);
	{

		editor_file_panel(window);

		editor_seperator_horizontal(window);

		editor_buttons_panel(window);

		wzrd_handle bottom_panel = editor_horizontal_panel_bordered((wzrd_v2) { 0 }, window);
		editor_left_panel(bottom_panel);
		editor_right_panel(bottom_panel, *(wzrd_texture*)&target_texture);

		editor_status_bar(window);

		editor_debug_panel(window, *debug_str);

		if (g_editor.create_object_dialog_active)
		{
			editor_create_object_dialog(&g_editor.create_object_dialog_pos, &g_editor.create_object_dialog_active, window);
		}

	}
	wzrd_end(debug_str);
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
