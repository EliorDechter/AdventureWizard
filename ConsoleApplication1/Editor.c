#include "Editor.h"
#include "Platform.h"
#include "Textures.h"
#include "Game.h"

wzrd_str wzrd_str_from_str128(str128* str)
{
	wzrd_str result = { .str = str->val, .len = str->len };

	return result;
}

void editor_seperator_horizontal(WzWidget parent)
{
	WzWidget w = wz_hbox(parent);
	wz_widget_set_h(w, 2);
	wz_widget_set_border(w, BorderType_BottomLine);
}

void editor_seperator_vertical(WzWidget parent)
{
	WzWidget w = wz_hbox(parent);
	wz_widget_set_w(w, 2);
}

WzWidget editor_vertical_panel(wzrd_v2 size, WzWidget parent)
{
	WzWidget p = wz_hbox(parent);

	return p;
}

WzWidget editor_vertical_panel_bordered(wzrd_v2 size, WzWidget parent)
{
	WzWidget p = wz_hbox(parent);

	wz_widget_set_size(p, size.x, size.y);

	return p;
}

WzWidget editor_horizontal_panel_bordered(wzrd_v2 size, WzWidget parent)
{
	WzWidget p = wz_hbox(parent);

	wz_widget_set_size(p, size.x, size.y);

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
	wz_widget_set_h(row, h);
	wzrd_label(label, row);
	wzrd_box_add_child(row, widget);

	return row;
}

WzWidget wzrd_scroll_panel(wzrd_v2 size, int* scroll, WzWidget parent, const char* tag)
{
	const int scroll_bar_width = 20;

	WzWidget panel = wz_hbox(parent);
	wz_widget_set_size(panel, size.x, size.y);
	wz_widget_set_color(panel, EGUI_BEIGE);

	WzWidget clip_panel = editor_vertical_panel((wzrd_v2) { 0 }, panel);
	wz_widget_get(clip_panel)->color = WZ_YELLOW;
	wzrd_widget_clip(clip_panel);
	wzrd_widget_tag(clip_panel, tag);

	WzWidget user_panel = wz_hbox(clip_panel);
	wz_widget_set_y(panel, -*scroll);
	wz_widget_set_h(panel, 1500);

	WzWidget scrollbar_panel = wz_hbox(panel);
	wz_widget_set_w(scrollbar_panel, scroll_bar_width + 4);

	WzWidget top_button = wz_hbox(scrollbar_panel);
	wz_widget_set_w(top_button, scroll_bar_width);
	wz_widget_set_h(top_button, scroll_bar_width);

	WzWidget middle_panel = wz_hbox(scrollbar_panel);
	wz_widget_set_w(middle_panel, scroll_bar_width);
	wz_widget_get(middle_panel)->color = EGUI_GRAY;
	wz_widget_get(middle_panel)->tag = tag;
	wz_widget_get(middle_panel)->secondary_tag = "scrollbar_middle";

	WzWidget bottom_button = wz_hbox(scrollbar_panel);
	wz_widget_set_size(bottom_button, scroll_bar_width, scroll_bar_width);

	WzWidgetData b = wzrd_widget_get_cached_box(tag);
	WzWidget scrollbar = (WzWidget){ 0 };
	if (wz_handle_is_valid(b.handle))
	{
		WzWidgetData* box = &b;
		float ratio = (float)b.actual_h / (float)b.content_h;
		if (ratio > 1)
			ratio = 1;

		scrollbar = wz_hbox(middle_panel);
		wz_widget_get(scrollbar)->percentage_h = ratio;
		wz_widget_get(scrollbar)->y = *scroll;

		wz_widget_get(scrollbar)->tag = tag;
		wz_widget_get(scrollbar)->secondary_tag = "scrollbar";
	}

	WzWidgetData scrollbar_middle_box = wzrd_widget_get_cached_box_with_secondary_tag(tag, "scrollbar_middle");
	WzWidgetData scrollbar_box = wzrd_widget_get_cached_box_with_secondary_tag(tag, "scrollbar");

	if (wzrd_widget_is_active(top_button))
	{
		if (*scroll > 0)
			*scroll -= 1;
	}

	if (wzrd_widget_is_active(bottom_button))
	{
		if (*scroll + scrollbar_box.actual_h < scrollbar_middle_box.actual_h - 2)
			*scroll += 1;
	}

	if (wzrd_widget_is_active(scrollbar))
	{
		int scroll_temp = *scroll + wzrd_canvas_get()->mouse_delta.y;

		if (scroll_temp + scrollbar_box.actual_h < scrollbar_middle_box.actual_h - 2 && scroll_temp > 0)
		{
			*scroll = scroll_temp;
		}
	}

	printf("%d %d\n", wzrd_canvas_get()->mouse_pos.x, wzrd_canvas_get()->mouse_pos.y);

	return user_panel;
}

void editor_debug_panel(WzWidget parent, wzrd_str str)
{
	WzWidget main_panel = wzrd_widget_free(parent);

	wz_widget_set_size(main_panel, 295, 500);
	wz_widget_set_pos(main_panel, (int)g_platform.window_width - 300, 10);

	static int scroll;
	WzWidget panel = wzrd_scroll_panel((wzrd_v2) { 0 }, & scroll, main_panel, "debug_panel");

	//static float scrollbar_x, scrollbar_y;
	//wzrd_box_get_last()->clip = true;
	//wzrd_box_get_last()->scrollbar_x = &scrollbar_x;
	//wzrd_box_get_last()->scrollbar_y = &scrollbar_y;
	wzrd_label(str, panel);
	WzWidget some_panel = editor_vertical_panel((wzrd_v2) { .x = 20, .y = 2000 }, panel);
	wz_widget_get(some_panel)->color = WZ_RED;

	//*debug_str->str = 0;
	//debug_str->len = 0;
	//wzrd_box_do((wzrd_box) { .h = 1000, .w = 50, .color = EGUI_BLUE });
}

void editor_do(wzrd_canvas* gui, PlatformTargetTexture target_texture, wzrd_icons icons, wzrd_str* debug_str) {
	(void)icons;

	WzWidget window = wz_begin(gui,
		(WzRect) {
		0, 0, (int)g_platform.window_width, (int)g_platform.window_height
	},
		platform_string_get_size,
		(wzrd_v2) {
		(int)g_platform.mouse_x, (int)g_platform.mouse_y
	},
		(wzrd_state)g_platform.mouse_left,
		* (wzrd_keyboard_keys*)&g_platform.keys_pressed, true);

	WzWidget panel = wz_widget(window);
	wz_widget_set_color(panel, WZ_RED);
	wz_widget_set_layout(panel, WzLayoutHorizontal);

	/*WzWidget a = wz_widget(panel);
	wz_widget_set_color(a, WZ_GREEN);

	WzWidget b = wz_widget(panel);
	wz_widget_set_color(b, WZ_BLUE);
	wz_widget_set_w(b, 50);*/

	wz_widget_add_rect(panel, 50, 50, WZ_GREEN);

#if 0
	{
		WzWidget panel = wz_hbox(window);

		WzWidget a = wz_widget(panel);
		wz_widget_set_stretch_factor(a, 1);
		wz_widget_set_color(a, WZ_RED);
		static int a_x, a_y;
		wz_widget_resize(a, &a_x, &a_y);
		wz_widget_set_size_policy(a, WzSizePolicyPreferred);

		WzWidget b = wz_vbox(panel);
		wz_widget_set_stretch_factor(b, 1);
		wz_widget_set_color(b, WZ_BLUE);
		static  int b_x, b_y;
		wz_widget_resize(b, &b_x, &b_y);
		wz_widget_set_size_policy(b, WzSizePolicyPreferred);

		WzWidget c = wz_widget(b);
		wz_widget_set_stretch_factor(c, 1);
		wz_widget_set_color(c, WZ_YELLOW);
		static  int c_x, c_y;
		wz_widget_resize(c, &c_x, &c_y);
		wz_widget_set_size_policy(c, WzSizePolicyPreferred);

		WzWidget d = wz_widget(b);
		wz_widget_set_stretch_factor(d, 1);
		wz_widget_set_color(d, EGUI_GREEN);
		static  int d_x, d_y;
		wz_widget_resize(d, &d_x, &d_y);
		wz_widget_set_size_policy(d, WzSizePolicyPreferred);
	}
#endif
	if (0) {

		//editor_file_panel(window);
		{
			WzWidget files_panel = wz_hbox(window);
			wz_widget_set_h(files_panel, 30);

			bool b1 = false, b2 = false;
			wzrd_label_button(wzrd_str_create("View"), &b1, files_panel);
			wzrd_label_button(wzrd_str_create("Help"), &b2, files_panel);
		}

		editor_seperator_horizontal(window);

		//editor_buttons_panel(window);
		{
			WzWidget panel = wz_hbox(window);
			wz_widget_set_h(panel, 36);

			bool b = false;
			WzWidget button = wzrd_command_button(wzrd_str_create("Add Object"), &b, panel);

			g_width = g_width + (g_target_width - g_width) * 0.1;

			if (g_width > 250.0f)
			{
				g_target_width = 20;
			}
			else if (g_width < 25.0f)
			{
				g_target_width = 255;
			}

			wz_widget_get(button)->color.r = 255;
			wz_widget_get(button)->w = (int)g_width;

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
			bool is_dragged = wzrd_box_is_dragged(&(WzWidgetData) {
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

			static WzColor color = { 100, 100, 100, 255 };
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

			editor_seperator_vertical(panel);

			//wzrd_toggle_icon(icons.play, &g_game.run, &panel);

			editor_seperator_vertical(panel);
		}


		WzWidget bottom_panel = editor_horizontal_panel_bordered((wzrd_v2) { 0 }, window);

		//editor_left_panel(bottom_panel);
		{
			WzWidget outer_panel = editor_vertical_panel_bordered((wzrd_v2) { .x = 200 }, bottom_panel);

			//wzrd_str texts[] = { wzrd_str_create("Entities"), wzrd_str_create("Textures"), wzrd_str_create("Actions") };
			//static bool active;
			static int selected_category;
			//wzrd_dropdown(&selected_category, texts, 3, 100, &active);

			WzWidget inner_panel = editor_vertical_panel((wzrd_v2) { 0 }, outer_panel);

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


		//editor_right_panel(bottom_panel, *(wzrd_texture*)&target_texture);
		{
			WzWidget panel = wz_hbox(bottom_panel);

			wz_widget_get(panel)->disable_input = true;

			WzWidget target_panel = wz_hbox(panel);

			wz_widget_set_size(target_panel, 1920 / 6, 1080 / 6);

			wz_widget_get(target_panel)->disable_input = true;

			wzrd_item_add((Item) {
				.type = ItemType_Texture,
					.val = { .texture = *(wzrd_texture*)&target_texture },
					.scissor = true
			}, target_panel);
		}

		//editor_debug_panel(window, *debug_str);
		{
			WzWidget bar = wz_hbox(window);
			wz_widget_set_h(bar, 40);

			WzWidget b = wz_hbox(bar);
			wz_widget_set_h(b, 90);
		}


		if (g_editor.create_object_dialog_active)
		{
			//editor_create_object_dialog(&g_editor.create_object_dialog_pos, &g_editor.create_object_dialog_active, window);
			{
				wzrd_v2 size = { 500, 500 };
				bool ok = false, cancel = false;
				static str128 name;
				static unsigned int selected;
				static bool is_selected = false;

				WzWidget dialog = wzrd_dialog_begin(&g_editor.create_object_dialog_active, size, &g_editor.create_object_dialog_active, wzrd_str_create("add object"), 4, window);
				bool* active = &g_editor.create_object_dialog_active;
				if (*active) {
					WzWidget panel = wz_hbox(dialog);
					wzrd_v2 v = (wzrd_v2){
							100, 0
					};

					{
						wzrd_str labels[] = { wzrd_str_create("Entity"), wzrd_str_create("Texture") };
						wzrd_label_list(labels, 2,
							v,
							0, &selected, &is_selected, panel);

						WzWidget form = editor_vertical_panel_bordered((wzrd_v2) { 0 }, panel);
						editor_add_row(form, wzrd_str_create("Name:"), wzrd_input_box(name.val, &name.len, 10, (WzWidget) { 0 }));
					}

					WzWidget bottom_panel = wz_hbox(dialog);
					wz_widget_set_h(bottom_panel, 50);
					{
						WzWidget buttons_panel = wz_hbox(bottom_panel);
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
