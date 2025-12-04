#include "Editor.h"
#include "Platform.h"
#include "Textures.h"
#include "Game.h"


#define editor_vpanel(parent) wz_vpanel_raw(parent, __FILE__, __LINE__)
#define editor_hpanel(parent) wz_hpanel_raw(parent, __FILE__, __LINE__)

WzWidget wz_vpanel_raw(WzWidget parent, const char* file, unsigned int line)
{
	WzWidget widget = wz_vbox_raw(parent, file, line);
	wz_widget_set_pad(widget, 5);
	wz_widget_set_child_gap(widget, 5);
	wz_widget_set_border(widget, WZ_BORDER_TYPE_DEFAULT);
	wz_widget_set_cross_axis_alignment(widget, CROSS_AXIS_ALIGNMENT_CENTER);

	return widget;
}

WzWidget wz_hpanel_raw(WzWidget parent, const char* file, unsigned int line)
{
	WzWidget widget = wz_hbox_raw(parent, file, line);
	wz_widget_set_pad(widget, 5);
	wz_widget_set_child_gap(widget, 5);
	wz_widget_set_border(widget, WZ_BORDER_TYPE_DEFAULT);
	wz_widget_set_cross_axis_alignment(widget, CROSS_AXIS_ALIGNMENT_CENTER);

	return widget;
}

wzrd_str wzrd_str_from_str128(str128* str)
{
	wzrd_str result = { .str = str->val, .len = str->len };

	return result;
}

void editor_seperator_horizontal(WzWidget parent)
{
	WzWidget w = wz_widget(parent);
	wz_widget_set_max_constraint_h(w, 2);
	wz_widget_set_border(w, BorderType_BottomLine);
}

void editor_seperator_vertical(WzWidget parent)
{
	WzWidget w = wz_widget(parent);
	wz_widget_set_max_constraint_w(w, 2);
}

WzWidget editor_horizontal_panel_bordered(wzrd_v2 size, WzWidget parent)
{
	WzWidget p = wz_hbox(parent);

	//wz_widget_set_tight_constraints(p, size.x, size.y);

	return p;
}

typedef struct Editor
{
	bool create_object_dialog_active;
	wzrd_v2 create_object_dialog_pos;
} Editor;

Editor g_editor;

float g_width = 20, g_target_width = 255;

WzWidget editor_add_row(WzWidget form, wzrd_str label, WzWidget widget)
{
	const int h = 30;

	WzWidget row = wz_hbox(form);
	wz_widget_set_max_constraint_h(row, h);
	wz_label(label, row);
	wzrd_box_add_child(row, widget);

	return row;
}


void editor_debug_panel(WzWidget parent, wzrd_str str)
{
#if 0
	static int scroll;

	WzWidget main_panel = wz_widget(parent);
	wz_widget_set_free_from_parent_horizontally(main_panel);
	wz_widget_set_free_from_parent_vertically(main_panel);

	wz_widget_set_tight_constraints(main_panel, 295, 500);
	wz_widget_set_pos(main_panel, (int)g_platform.window_width - 300, 10);

	WzWidget panel = wz_scroll_box((wzrd_v2) { 0 }, & scroll, main_panel, "debug_panel");

	//static float scrollbar_x, scrollbar_y;
	//wzrd_box_get_last()->clip = true;
	//wzrd_box_get_last()->scrollbar_x = &scrollbar_x;
	//wzrd_box_get_last()->scrollbar_y = &scrollbar_y;
	wz_label(str, panel);
	WzWidget some_panel = editor_vpanel(panel);
	wz_widget_set_constraint_w(some_panel, 20);
	wz_widget_set_constraint_w(some_panel, 2000);
	wz_widget_get(some_panel)->color = WZ_RED;
#endif
}
#
void do_editor(wzrd_canvas* gui, PlatformTargetTexture target_texture, wzrd_icons icons, wzrd_str* debug_str) {
	(void)icons;
	static bool finish;
	static int selected_category;
	static str128 name;
	static unsigned int selected;
	static bool is_selected = false;

	WzWidget window0 = wz_begin(gui,
		(WzRect) {
		0, 0, (int)g_platform.window_width, (int)g_platform.window_height
	},
		platform_string_get_size,
		(wzrd_v2) {
		(int)g_platform.mouse_x, (int)g_platform.mouse_y
	},
		(wzrd_state)g_platform.mouse_left,
		* (wzrd_keyboard_keys*)&g_platform.keys_pressed, true);
	
	
#if 1

#if 0
	WzWidget window = wz_vbox(window0);
	wz_widget_set_constraint_size(window, g_platform.window_width, g_platform.window_height);
	wz_widget_set_color_old(window, WZ_RED);
	//wz_widget_set_pad(window, 10);
	wz_widget_set_child_gap(window, 10);
	wz_widget_set_border(window, BorderType_Default);
#else
	WzWidget window = editor_vpanel(window0);
	wz_widget_set_constraint_size(window, g_platform.window_width, g_platform.window_height);
	wz_widget_set_color(window, 0xFFFF00FF);
	wz_widget_clip(window);
	wz_widget_set_cross_axis_alignment(window, CROSS_AXIS_ALIGNMENT_STRETCH);
#endif

#if 0
	WzWidget l_a = wz_label(wzrd_str_create("Label2"), window);
	WzWidget l_b = wz_label(wzrd_str_create("Label"), window);
	//WzWidget w = wz_widget(window);
	//wz_widget_set_color_old(w, WZ_GREEN);
	//wz_widget_set_flex(w);
	//wz_widget_set_constraint_h(w, 100);
	//WzWidget files_panel = wz_widget(window);
	//wz_widget_set_color_old(files_panel, WZ_BLUE);
	//wz_widget_set_flex(files_panel);
#endif
#endif

	{
#if 1

		WzWidget files_panel = editor_hpanel(window);
		wz_widget_set_color_old(files_panel, WZ_BLUE);

		bool b1 = false, b2 = false;
		wz_label_button(wzrd_str_create("View"), &b1, files_panel);
		wz_label_button(wzrd_str_create("Help"), &b2, files_panel);

		editor_seperator_horizontal(window);
		{
			WzWidget panel = editor_hpanel(window);
			//wz_widget_set_max_constraint_h(panel, 40);
			wz_widget_set_pad(panel, 10);
			wz_widget_set_child_gap(panel, 15);
			wz_widget_set_color(panel, 0xffff00ff);
			wz_widget_clip(panel);

			bool b = false;
			WzWidget button = wz_command_button(wzrd_str_create("Add Object"), &b, panel);

			if (b)
			{
				g_editor.create_object_dialog_active = true;
			}

			wz_command_toggle(wzrd_str_create("Add Polygon"), &g_game.polygon_adding_active, panel);
			//wzrd_command_toggle(wzrd_str_create("Add Polygon"), &g_game.polygon_adding_active, panel);

			//editor_seperator_vertical(panel);

			//bool delete_toggle;
			//wzrd_button_icon(icons.delete, &delete_toggle, buttons_panel);
			//g_game.delete = delete_toggle;

			// seperator

			bool is_dragged = wzrd_box_is_dragged(&(WzWidgetData) {0});
			if (!is_dragged && !finish)
			{
				/*	wzrd_widget((wzrd_style) {
						.style = wzrd_style_create((wzrd_style_template) {
							.w = 25, .h = 25
						}), .is_draggable = true,
					}, panel);*/
			}

			//static WzColor color = { 100, 100, 100, 255 };
			//wzrd_widget((wzrd_style) {
			//	.is_slot = true,
			//		.style = wzrd_style_create((wzrd_style_template) {
			//		.color = color, .w = 25, .h = 25,
			//	})
			//}, panel);

			//if (wzrd_box_is_hot(wzrd_box_get_last()) && wzrd_is_releasing())
			//{
			//	color = EGUI_LIME;
			//	finish = true;
			//}

			//editor_seperator_vertical(panel);

			//wzrd_toggle_icon(icons.play, &g_game.run, &panel);

			//editor_seperator_vertical(panel);
		}

		WzWidget bottom_panel = editor_hpanel(window);
		wz_widget_set_expanded(bottom_panel);
		wz_widget_clip(bottom_panel);
		wz_widget_set_cross_axis_alignment(bottom_panel, CROSS_AXIS_ALIGNMENT_STRETCH);

#if 1
		//editor_left_panel(bottom_panel);
		{
			WzWidget outer_panel = editor_vpanel(bottom_panel);
			wz_widget_set_expanded(outer_panel);
			wz_widget_set_max_constraint_w(outer_panel, 200);

			//wzrd_str texts[] = { wzrd_str_create("Entities"), wzrd_str_create("Textures"), wzrd_str_create("Actions") };
			//static bool active;
			//wzrd_dropdown(&selected_category, texts, 3, 100, &active);

			WzWidget inner_panel = editor_vpanel(outer_panel);
			wz_widget_set_expanded(inner_panel);

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

#if 1
				wzrd_label_list_sorted(labels, g_game.sorted_entities_count,
					(int*)g_game.sorted_entities,
					50, 70, 0xff00ffff,
					&g_game.selected_entity_index_to_sorted_entities,
					&g_game.is_entity_selected, inner_panel);
#endif
			}
		}
#endif

		{
			static unsigned int scroll;
			wz_scroll_box((wzrd_v2){100, 100}, &scroll, bottom_panel, "asd");
		}


#if 1

		//editor_right_panel(bottom_panel, *(wzrd_texture*)&target_texture);
		{
#if 0
			WzWidget panel = editor_hpanel(bottom_panel);
			wz_widget_set_constraint_h(panel, 100);
			wz_widget_set_expanded(panel);

			wz_widget_get(panel)->disable_input = true;

			WzWidget target_panel = editor_hpanel(panel);
			wz_widget_set_expanded(target_panel);

			wz_widget_set_tight_constraints(target_panel, 1920 / 6, 1080 / 6);

			wz_widget_get(target_panel)->disable_input = true;

			wzrd_item_add((Item) {
				.type = ItemType_Texture,
					.val = { .texture = *(wzrd_texture*)&target_texture },
					.scissor = true
			}, target_panel);
#endif
		}

		//editor_debug_panel(window, *debug_str);
		{
			/*WzWidget bar = editor_hpanel(window);
			wz_widget_set_constraint_h(bar, 40);

			WzWidget b = editor_hpanel(bar);
			wz_widget_set_expanded(b);
			wz_widget_set_constraint_h(b, 30);*/
		}

		if (g_editor.create_object_dialog_active)
		{
			//editor_create_object_dialog(&g_editor.create_object_dialog_pos, &g_editor.create_object_dialog_active, window);
			{
				wzrd_v2 size = { 500, 500 };
				bool ok = false, cancel = false;

				WzWidget dialog = wzrd_dialog_begin(&g_editor.create_object_dialog_active,
					size, &g_editor.create_object_dialog_active, wzrd_str_create("add object"), 4, window);
				bool* active = &g_editor.create_object_dialog_active;
				//if (*active) {
				if (1)
				{
					WzWidget panel = editor_hpanel(dialog);

					if (1)
					{
						wzrd_str labels[] = { wzrd_str_create("Entity"), wzrd_str_create("Texture") };
						wzrd_label_list(labels, 2,
							100, 100,
							0xFFFFFFFF,
							0, &selected, &is_selected, panel);

#if 0
						WzWidget form = editor_vpanel(panel);
						wz_widget_set_constraint_w(form, 50);
						wz_widget_set_constraint_h(form, 50);
						wz_widget_set_main_axis_size_min(form);
						//editor_add_row(form, wzrd_str_create("Name:"), wzrd_input_box(name.val, &name.len, 10, (WzWidget) { 0 }));
#endif
					}
#if 0

					WzWidget bottom_panel = editor_hpanel(dialog);
					wz_widget_set_constraint_h(bottom_panel, 50);
					{
						WzWidget buttons_panel = editor_hpanel(bottom_panel);
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
#endif
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
								.color = (WzColor){ 255, 255, 255, 255 }
						});
					}

					*active = false;
				}

				if (cancel) {
					*active = false;
				}
			}

		}
#endif
#endif
	}
	wz_end(debug_str);
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
