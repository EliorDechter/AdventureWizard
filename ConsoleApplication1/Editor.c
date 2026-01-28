#include "Editor.h"
#include "Platform.h"
#include "Textures.h"
#include "Game.h"



WzStr wzrd_str_from_str128(str128* str)
{
	WzStr result = { .str = str->val, .len = str->len };

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

Editor g_editor;

float g_width = 20, g_target_width = 255;

void editor_init()
{

}

void editor_run(WzGui* gui, PlatformTargetTexture target_texture,
	wzrd_icons icons, WzStr* debug_str, WzWidget* target_panel_out)
{
	(void)icons;
	static bool finish;
	static int selected_category;
	static str128 name;
	static unsigned int selected;
	static bool is_selected = false;

	WzWidget draw_panel;

	WzWidget window0 = wz_gui_begin(gui,
		g_platform.window_width, g_platform.window_height,
		g_platform.mouse_x, g_platform.mouse_y,
		platform_string_get_size,
		(WzState)g_platform.mouse_left,
		*(WzKeyboardKeys*)&g_platform.keys_pressed, true);

	WzWidget window = wz_vpanel(window0);
	wz_widget_set_max_constraints(window, g_platform.window_width, g_platform.window_height);

	wz_widget_set_cross_axis_alignment(window, WZ_CROSS_AXIS_ALIGNMENT_STRETCH);
	{
		WzWidget files_panel = wz_hpanel(window);

		bool b1 = false, b2 = false;
		wz_label_button(wz_str_create("View"), &b1, files_panel);
		wz_label_button(wz_str_create("Help"), &b2, files_panel);

		editor_seperator_horizontal(window);
		{
			WzWidget panel = wz_hpanel(window);
			wz_widget_set_pad(panel, 10);
			wz_widget_set_child_gap(panel, 15);

			bool b = false;
			WzWidget button = wz_command_button(wz_str_create("Add Object"), &b, panel);

			if (b)
			{
				g_editor.create_object_dialog_active = true;
			}

			wz_command_toggle(panel, wz_str_create("Add Polygon"), &g_game.polygon_adding_active);

			wz_button_icon(panel, &g_editor.is_drawing_widget, editor_textures[EditorTextureButton]);

			//bool delete_toggle;
			//wzrd_button_icon(icons.delete, &delete_toggle, buttons_panel);
			//g_game.delete = delete_toggle;

			// seperator

			bool is_dragged = wzrd_box_is_dragged(&(WzWidgetData) { 0 });
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

		WzWidget bottom_panel = wz_hpanel(window);
		wz_widget_set_expanded(bottom_panel);
		wz_widget_set_cross_axis_alignment(bottom_panel, WZ_CROSS_AXIS_ALIGNMENT_STRETCH);

		//editor_left_panel(bottom_panel);
		{
			WzWidget outer_panel = wz_vpanel(bottom_panel);
			//wz_widget_set_expanded(outer_panel);

			//wzrd_str texts[] = { wzrd_str_create("Entities"), wzrd_str_create("Textures"), wzrd_str_create("Actions") };
			//static bool active;
			//wzrd_dropdown(&selected_category, texts, 3, 100, &active);

			WzWidget inner_panel = wz_vpanel(outer_panel);
			wz_widget_set_expanded(inner_panel);

			if (selected_category == 0) {

				static WzStr labels[MAX_NUM_ENTITIES] = { 0 };

				for (unsigned int i = 0; i < g_game.sorted_entities_count; ++i)
				{
					Entity_handle handle = g_game.sorted_entities[i];
					Entity* entity = game_entity_get(handle);
					labels[i] = wz_str_create(entity->name.val);
				}

				if (g_game.selected_entity_index_to_sorted_entities >= 0)
				{
					// TODO: if I remove the if code crashes
					if (g_game.delete)
					{
						game_entity_remove(g_game.sorted_entities[g_game.selected_entity_index_to_sorted_entities]);
					}
				}

				wzrd_label_list_sorted(labels, g_game.sorted_entities_count,
					(int*)g_game.sorted_entities,
					50, 70, 0XC8C8C8FF,
					&g_game.selected_entity_index_to_sorted_entities,
					&g_game.is_entity_selected, inner_panel);
			}
		}


		WzWidget panel = wz_hpanel(bottom_panel);
		wz_widget_set_expanded(panel);
		wz_widget_set_cross_axis_alignment(panel, WZ_CROSS_AXIS_ALIGNMENT_STRETCH);
		//wz_widget_get(panel)->disable_input = true;

		draw_panel = wz_panel(panel);
		wz_widget_set_expanded(draw_panel);
		wz_widget_set_cross_axis_alignment(draw_panel, WZ_CROSS_AXIS_ALIGNMENT_STRETCH);

#if 0
		// Game panel
		WzWidget target_panel = editor_hpanel(panel);
		//wz_widget_set_expanded(target_panel);
		wz_widget_set_tight_constraints(target_panel, GAME_WIDTH, GAME_HEIGHT);
		wz_widget_get(target_panel)->disable_input = true;
		*target_panel_out = target_panel;

		wz_widget_add_item(target_panel, (WzWidgetItem) {
			.type = ItemType_Texture,
				.val = { .texture = *(WzTexture*)&target_texture },
				.scissor = true
		});
#endif


		if (g_editor.create_object_dialog_active)
		{
			{
				wzrd_v2 size = { 500, 500 };
				bool ok = false, cancel = false;

				WzWidget dialog = wzrd_dialog_begin(&g_editor.create_object_dialog_pos,
					size, &g_editor.create_object_dialog_active, wz_str_create("add object"), 4, window);

				bool* active = &g_editor.create_object_dialog_active;
				if (1)
				{
					WzWidget panel = wz_hpanel(dialog);
					wz_widget_set_expanded(panel);

					WzStr labels[] = { wz_str_create("Entity"), wz_str_create("Texture") };
					wzrd_label_list(labels, 2,
						100, 100,
						0xFFFFFFFF,
						0, &selected, &is_selected, panel);
					WzWidget form = wz_vpanel(panel);
					wz_widget_set_expanded(form);
					wz_widget_set_cross_axis_alignment(form, WZ_CROSS_AXIS_ALIGNMENT_STRETCH);
					//wz_widget_set_cross_axis_alignment(form, CROSS_AXIS_ALIGNMENT_STRETCH);
					//WzWidget b = wz_widget(form);
					//wz_widget_set_tight_constraints(b, 50, 50);
					//WzWidget wdg = { 0 };
					//WzWidget row = editor_add_row(form, wzrd_str_create("Name:"), wdg);
					WzWidget row = wz_hpanel(form);
					wz_label(row, wz_str_create("Name:"));
					WzWidget wdg = wzrd_input_box(name.val, &name.len, 10, row);

					wz_widget_set_border(row, WZ_BORDER_TYPE_DEFAULT);
					{
						WzWidget buttons_panel = wz_hpanel(dialog);
						{
							bool b1;

							wz_command_button(wz_str_create("OK"), &b1, buttons_panel);
							if (b1) {
								ok = true;
							}

							bool b2;
							wz_command_button(wz_str_create("Cancel"), &b2, buttons_panel);
							if (b2) {
								cancel = true;
							}
						}
					}
				}

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

	wz_do_layout_refactor_me();

	if (wz_widget_is_activating(draw_panel))
	{
		WzWidgetData* draw_panel_data = wz_widget_get(draw_panel);
		g_editor.drawing_widget_x = gui->mouse_pos.x;
		g_editor.drawing_widget_y = gui->mouse_pos.y;
	}

	unsigned w = gui->mouse_pos.x - g_editor.drawing_widget_x;
	unsigned h = gui->mouse_pos.y - g_editor.drawing_widget_y;

	if (persistent_widgets_count)
	{
		for (unsigned i = 0; i < persistent_widgets_count; ++i)
		{
			wz_widget_persistent(draw_panel, persistent_widgets[i]);
		}
	}

	if (wz_widget_is_active(draw_panel))
	{
		WzWidgetData* draw_panel_data = wz_widget_get(draw_panel);

		WzWidget widg = wz_widget(draw_panel);
		wz_widget_set_tight_constraints(widg, w, h);
		wz_widget_set_border(widg, WZ_BORDER_TYPE_DEFAULT);
		wz_widget_set_x(widg, g_editor.drawing_widget_x - draw_panel_data->actual_x);
		wz_widget_set_y(widg, g_editor.drawing_widget_y - draw_panel_data->actual_y);

		widg = wz_widget(draw_panel);
		wz_widget_set_tight_constraints(widg, w, h);
		wz_widget_set_border(widg, WZ_BORDER_TYPE_DEFAULT);
		wz_widget_set_x(widg, g_editor.drawing_widget_x - draw_panel_data->actual_x);
		wz_widget_set_y(widg, g_editor.drawing_widget_y - draw_panel_data->actual_y);
	}

	if (wz_widget_is_deactivating(draw_panel))
	{
		WzWidgetData* draw_panel_data = wz_widget_get(draw_panel);

		WzWidgetData widg = wz_widget_create(draw_panel);
		wz_widget_data_set_tight_constraints(&widg, w, h);
		wz_widget_data_set_border(&widg, WZ_BORDER_TYPE_DEFAULT);
		wz_widget_data_set_x(&widg, g_editor.drawing_widget_x - draw_panel_data->actual_x);
		wz_widget_data_set_y(&widg, g_editor.drawing_widget_y - draw_panel_data->actual_y);
		persistent_widgets[persistent_widgets_count++] = widg;
	}

	wz_do_layout_refactor_me();

	if (0)
	{
		// Draw panel dots
		WzWidgetData* draw_panel_data = wz_widget_get(draw_panel);
		unsigned stride_x = draw_panel_data->actual_w / 5;
		unsigned stride_y = draw_panel_data->actual_h / 5;

		for (unsigned i = 0; i < draw_panel_data->actual_w / 2; ++i)
		{
			for (unsigned j = 0; j < draw_panel_data->actual_h / 2; ++j)
			{
				wz_widget_add_rect_absolute(draw_panel_data->handle,
					draw_panel_data->actual_x + i * stride_x,
					draw_panel_data->actual_y + j * stride_y,
					10, 10, (WzColor) { 0, 0, 0, 255 });
			}
		}
	}

	wz_gui_end(debug_str);
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
