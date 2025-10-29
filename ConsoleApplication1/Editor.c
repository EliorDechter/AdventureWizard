#include "Editor.h"
#include "Platform.h"
#include "Textures.h"
#include "Game.h"

wzrd_str wzrd_str_from_str128(str128* str)
{
	wzrd_str result = { .str = str->val, .len = str->len };

	return result;
}

void editor_file_panel(wzrd_handle window)
{
	wzrd_handle files_panel = wzrd_widget(((wzrd_box) {
		.style = wzrd_style_create((wzrd_style)
		{
			.h = 30,
				.row_mode = true,
				.border_type = BorderType_None,
				.child_gap = 10,
				.pad_left = 5,
				.center_y = true,
				.color = EGUI_LIGHTGRAY
		})
	}), window);

	bool b1 = false, b2 = false;
	wzrd_label_button(wzrd_str_create("View"), &b1, files_panel);
	wzrd_label_button(wzrd_str_create("Help"), &b2, files_panel);
}

void editor_seperator_horizontal(wzrd_handle parent)
{
	wzrd_widget((wzrd_box) {
		.style = wzrd_style_create((wzrd_style) {
			.h = 2,
				.border_type = BorderType_BottomLine,
		}),
	}, parent);
}

void editor_seperator_vertical(wzrd_handle parent)
{
	wzrd_widget((wzrd_box) {
		.style = wzrd_style_create((wzrd_style) {
			.w = 2,
				.border_type = BorderType_LeftLine,
		}),
	}, parent);
}

void editor_left_panel(wzrd_handle parent)
{
	wzrd_handle outer_panel = wzrd_widget((wzrd_box) {
		.style = wzrd_style_create((wzrd_style) {
			.w = 200,
				.pad_top = 5,
				.pad_left = 5,
				.pad_right = 5,
				.pad_bottom = 5,
				.child_gap = 5,
				.color = EGUI_LIGHTGRAY,
				.border_type = BorderType_Clicked,
		})
	}, parent);

	//wzrd_str texts[] = { wzrd_str_create("Entities"), wzrd_str_create("Textures"), wzrd_str_create("Actions") };
	//static bool active;
	static int selected_category;
	//wzrd_dropdown(&selected_category, texts, 3, 100, &active);

	wzrd_handle inner_panel = wzrd_widget((wzrd_box) {
		.style = wzrd_style_create((wzrd_style) {
			.border_type = BorderType_Clicked, .color = EGUI_WHITE,
		})
	}, outer_panel);

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

void editor_buttons_panel(wzrd_handle window)
{
	wzrd_handle panel = wzrd_vbox((wzrd_v2) { 0, 36 }, window);

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
		wzrd_widget((wzrd_box) {
			.style = wzrd_style_create((wzrd_style) {
				.w = 25, .h = 25
			}), .is_draggable = true,
		}, panel);
	}

	static wzrd_color color = { 100, 100, 100, 255 };
	wzrd_widget((wzrd_box) {
		.is_slot = true,
			.style = wzrd_style_create((wzrd_style) {
			.color = color, .w = 25, .h = 25,
		})
	}, panel);

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
	wzrd_handle panel = wzrd_widget(((wzrd_box) {
		.disable_input = true,
			.style = wzrd_style_create((wzrd_style) {
			.center_x = true, .center_y = true,
				.color = (wzrd_color){ 50, 50, 50, 255 },
				.border_type = BorderType_Clicked
		})
	}), parent);

	wzrd_handle target_panel = wzrd_widget(((wzrd_box) {

		.disable_input = true,
			.handle = wzrd_unique_handle_create(wzrd_str_create("Target")),
			.style = wzrd_style_create((wzrd_style) {
			.best_fit = true, .w = 1920 / 6, .h = 1080 / 6,
		})
	}), panel);

	wzrd_item_add((Item) {
		.type = ItemType_Texture,
			.val = { .texture = texture },
			.scissor = true
	}, target_panel);

}

void editor_status_bar(wzrd_handle parent)
{
	wzrd_handle bar = wzrd_widget(((wzrd_box) {
		.style = wzrd_style_create((wzrd_style) {
			.h = 40,
				.row_mode = true,
				.border_type = BorderType_None
		})
	}), parent);
	wzrd_widget((wzrd_box) { .style = wzrd_style_create((wzrd_style) { .w = 90 }) }, bar);
	wzrd_widget((wzrd_box) { 0 }, bar);

}

void editor_create_object_dialog(wzrd_v2* pos, bool* active, wzrd_handle parent)
{
	wzrd_v2 size = { 500, 500 };
	bool ok = false, cancel = false;
	static str128 name;
	static unsigned int selected;

	wzrd_handle dialog = wzrd_dialog_begin(pos, size, active, wzrd_str_create("add object"), 4, parent);

	if (active) {
		wzrd_handle panel = wzrd_widget((wzrd_box) {
			.style = wzrd_style_create((wzrd_style) {
				.row_mode = true, .border_type = BorderType_None, .child_gap = 5,
			})
		}, dialog);
		{
			wzrd_str labels[] = { wzrd_str_create("Entity"), wzrd_str_create("Texture") };
			bool is_selected = false;
			wzrd_label_list(labels, 2,
				(wzrd_v2) {
				100, 0
			},
				0, & selected, & is_selected, panel);

			wzrd_handle right_panel = wzrd_widget((wzrd_box) {
				.style = wzrd_style_create((wzrd_style) {
					.border_type = BorderType_Clicked,
						.pad_left = 5,
						.pad_top = 5
				})
			}, panel);
			{
				if (selected == 1) {

					wzrd_handle name_panel = wzrd_widget((wzrd_box) {
						.style = wzrd_style_create((wzrd_style) {
							.h = 50,
								.border_type = BorderType_None, .row_mode = true,
						})
					}, right_panel);
					wzrd_label(wzrd_str_create("Name:"), name_panel);
					wzrd_input_box(name.val, &name.len, 10, name_panel);

				}
				else if (selected == 0) {
					wzrd_handle w = wzrd_widget((wzrd_box) {
						.style = wzrd_style_create((wzrd_style) {
							.border_type = BorderType_None, .row_mode = true, .h = 50,
						}),
					}, right_panel);
					wzrd_label(wzrd_str_create("Name:"), w);
					wzrd_input_box(name.val, &name.len, 10, w);
				}
			}
		}

		wzrd_handle bottom_panel = wzrd_widget((wzrd_box) {
			.style = wzrd_style_create((wzrd_style) {
				.h = 50,
					.row_mode = true, .border_type = BorderType_None,
			})
		}, dialog);
		{
			wzrd_handle buttons_panel = wzrd_widget((wzrd_box) {
				.style = wzrd_style_create((wzrd_style) {
					.center_x = true, .center_y = true,
						.child_gap = 10,
						.pad_left = 5,
						.pad_top = 5,
						.pad_right = 5,
						.pad_bottom = 5,
						.row_mode = true,
						.border_type = BorderType_None,
				})
			}, bottom_panel);
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
	wzrd_handle panel = wzrd_widget_free(
		(wzrd_box) {
		.style = wzrd_style_create((wzrd_style) {
			.x = (int)g_platform.window_width - 300,
				.y = 10,
				.w = 295,
				.h = 500,
				.color = (wzrd_color){255, 255, 150, 170}
		})
	}, parent);
	{
		//static float scrollbar_x, scrollbar_y;
		wzrd_box_get_last()->clip = true;
		//wzrd_box_get_last()->scrollbar_x = &scrollbar_x;
		//wzrd_box_get_last()->scrollbar_y = &scrollbar_y;
		wzrd_label(str, panel);
		//*debug_str->str = 0;
		//debug_str->len = 0;
		//wzrd_box_do((wzrd_box) { .h = 1000, .w = 50, .color = EGUI_BLUE });
	}
}

void editor_do(wzrd_canvas* gui, wzrd_draw_commands_buffer* buffer, wzrd_cursor* cursor, PlatformTargetTexture target_texture, wzrd_icons icons, unsigned int layer, wzrd_str* debug_str) {
	(void)icons;

	wzrd_handle window = wzrd_begin(gui,
		(wzrd_rect_struct) {
		0, 0, (int)g_platform.window_width, (int)g_platform.window_height
	},
		platform_string_get_size,
		layer, (wzrd_v2) {
		(int)g_platform.mouse_x, (int)g_platform.mouse_y
	},
		(wzrd_state)g_platform.mouse_left,
		* (wzrd_keyboard_keys*)&g_platform.keys_pressed, true);
	{

		editor_file_panel(window);

		editor_seperator_horizontal(window);

		editor_buttons_panel(window);

		wzrd_handle bottom_panel = wzrd_vbox((wzrd_v2) { 0 }, window);
		editor_left_panel(bottom_panel);
		editor_right_panel(bottom_panel, *(wzrd_texture*)&target_texture);

		editor_status_bar(window);

		editor_debug_panel(window, *debug_str);

		if (g_editor.create_object_dialog_active)
		{
			editor_create_object_dialog(&g_editor.create_object_dialog_pos, &g_editor.create_object_dialog_active, window);
		}

	}
	wzrd_end(cursor, buffer, debug_str);
}

void editor_do2() {
	// wzrd_canvas* gui, wzrd_draw_commands_buffer* buffer, wzrd_cursor* cursor, PlatformTargetTexture target_texture, wzrd_icons icons, unsigned int layer, wzrd_str* debug_str
	//static int selected_category;
#if 0
	platform_time_begin();

	wzrd_begin(gui,
		(wzrd_rect_struct) {
		0, 0, (int)g_platform.window_width, (int)g_platform.window_height
	},
		platform_string_get_size,
		layer, (wzrd_v2) {
		(int)g_platform.mouse_x, (int)g_platform.mouse_y
	},
		(wzrd_state)g_platform.mouse_left,
		* (wzrd_keyboard_keys*)&g_platform.keys_pressed, true);
	if (1) {
		static bool create_object_active;
		static int dialog_parent;
		static wzrd_v2 create_object_pos = { 100, 100 };

		// Files Panel
		wzrd_widget(((wzrd_box) {
			.style = wzrd_style_create((wzrd_style)
			{
				.h = 30,
					.row_mode = true,
					.border_type = BorderType_None,
					.child_gap = 10,
					.pad_left = 5,
					.center_y = true,
			})
		}));
		{
			static wzrd_v2 s;
			wzrd_box_resize(&s);
			/*static bool f;
			if (wzrd_label_button_begin(wzrd_str_create("File"))) f = !f;
			{
				if (f) {
					wzrd_style style = wzrd_style_get(wzrd_box_get_from_top_of_stack()->style);
					wzrd_v2 origin = (wzrd_v2){ 40, style.h };
					static wzrd_v2 offset;
					wzrd_crate(
						6,
						(wzrd_box) {
						.style = wzrd_style_create((wzrd_style) {
							.x = origin.x,
								.y = origin.y + offset.y,
								.w = 50,
								.h = 50
						})
					});

					if (wzrd_box_is_hot(gui, wzrd_box_get_last()))
					{
						offset = wzrd_lerp(offset, (wzrd_v2) { 0, 30 });
					}
					else
					{
						offset = wzrd_lerp(offset, (wzrd_v2) { 0, 0 });
					}
				}
			}
			wzrd_label_button_end();*/

			wzrd_label_button(wzrd_str_create("View"));
			wzrd_label_button(wzrd_str_create("Help"));
		}
		wzrd_box_end();

		// Seperator
		wzrd_box_do((wzrd_box) {
			.style = wzrd_style_create((wzrd_style) {
				.h = 2,
					.border_type = BorderType_BottomLine,
			}),
		});

		// Buttons panel
		wzrd_box_begin(((wzrd_box) {
			.style = wzrd_style_create((wzrd_style) {
				.h = 36,
					.pad_left = 5,
					.center_y = true,
					.child_gap = 5,
					.row_mode = true,
					.border_type = BorderType_None,
			})
		}));
		{
			static wzrd_v2 s;
			wzrd_box_resize(&s);
			dialog_parent = wzrd_box_get_current_index();

			bool b;
			wzrd_button(wzrd_str_create("Add Object"), &b);
			if (b) {
				create_object_pos = (wzrd_v2){ 50, 50 };
				create_object_active = true;
			}

			wzrd_toggle(
				wzrd_str_create("Add Polygon"), &g_game.polygon_adding_active);

			// Seperator
			wzrd_box_do((wzrd_box) {
				.style = wzrd_style_create((wzrd_style) {
					.w = 2,
						.border_type = BorderType_LeftLine,
				}),
			});
			bool delete_toggle;
			wzrd_button_icon(icons.delete, &delete_toggle);
			g_game.delete = delete_toggle;

			// Seperator
			wzrd_box_do((wzrd_box) {
				.style = wzrd_style_create((wzrd_style) {
					.w = 2,
						.border_type = BorderType_LeftLine,
				}),
			});

			static bool finish;
			bool is_dragged = wzrd_box_is_dragged(&(wzrd_box) {
				0
			});
			if (!is_dragged && !finish)
			{
				wzrd_box_do((wzrd_box) {
					.style = wzrd_style_create((wzrd_style) {
						.w = 25, .h = 25
					}), .is_draggable = true,
				});
			}

			static wzrd_color color = { 100, 100, 100, 255 };
			wzrd_box_do((wzrd_box) {
				.is_slot = true,
					.style = wzrd_style_create((wzrd_style) {
					.color = color, .w = 25, .h = 25,
				})
			});

			if (wzrd_box_is_hot(gui, wzrd_box_get_last()) && wzrd_is_releasing())
			{
				color = EGUI_LIME;
				finish = true;
			}

			// Seperator
			wzrd_box_do((wzrd_box) {
				.style = wzrd_style_create((wzrd_style) {
					.w = 2,
						.border_type = BorderType_LeftLine,
				}),
			});

			wzrd_toggle_icon(icons.play, &g_game.run);

			/*	if (wzrd_button_icon(icons.stop)) {

				}

				if (wzrd_button_icon(icons.pause)) {

				}*/

				// Seperator
			wzrd_box_do((wzrd_box) {
				.style = wzrd_style_create((wzrd_style) {
					.border_type = BorderType_LeftLine,
						.w = 2
				}),
			});

			static wzrd_v2 p;
			wzrd_box_do((wzrd_box) {
				.style = wzrd_style_create((wzrd_style) { .w = 25, .h = 25, .x = p.x, .y = p.y })
			});

			if (wzrd_box_is_hot(gui, wzrd_box_get_last()))
			{
				//p = wzrd_lerp(p, (wzrd_v2) { 0, 5 });
			}
			else {
				//p = wzrd_lerp(p, (wzrd_v2) { 0, 0 });
			}
		}
		wzrd_box_end();

		wzrd_box_begin(((wzrd_box) {
			.style = wzrd_style_create((wzrd_style) {
				.border_type = BorderType_None,
					.row_mode = true, .child_gap = 5
			})
		}));
		{
			wzrd_box_begin((wzrd_box) {
				.style = wzrd_style_create((wzrd_style) {
					.w = 200,
						.pad_top = 5,
						.pad_left = 5,
						.pad_right = 5,
						.pad_bottom = 5,
						.child_gap = 5,
						.border_type = BorderType_Clicked,
				})
			});
			{
				static wzrd_v2 size_offset;

				wzrd_box_resize(&size_offset);

				wzrd_str texts[] = { wzrd_str_create("Entities"), wzrd_str_create("Textures"), wzrd_str_create("Actions") };
				static bool active;
				static int selected_category;
				wzrd_dropdown(&selected_category, texts, 3, 100, &active);

				wzrd_box_begin((wzrd_box) {
					.style = wzrd_style_create((wzrd_style) {
						.border_type = BorderType_Clicked, .color = EGUI_WHITE,
					})
				});
				{
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
							/*(wzrd_box) {
							.style = wzrd_style_create((wzrd_style) {
								.color = EGUI_WHITE, .border_type = BorderType_None,
							})*/
							(wzrd_v2) {
							0
						},
							& g_game.selected_entity_index_to_sorted_entities, & g_game.is_entity_selected);

					}
				}
				wzrd_box_end();
			}
			wzrd_box_end();

			//Right Panel
			{
				// Target
				wzrd_box_begin(((wzrd_box) {
					.disable_input = true,
						.style = wzrd_style_create((wzrd_style) {
						.center_x = true, .center_y = true,
							.color = (wzrd_color){ 50, 50, 50, 255 },
							.border_type = BorderType_Clicked
					})
				}));
				{
					wzrd_box_begin(((wzrd_box) {

						.disable_input = true,
							.handle = wzrd_unique_handle_create(wzrd_str_create("Target")),
							.style = wzrd_style_create((wzrd_style) {
							.best_fit = true, .w = 1920 / 6, .h = 1080 / 6,
						})
					}));
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
		}
		wzrd_box_end();

		wzrd_box_begin(((wzrd_box) {
			.style = wzrd_style_create((wzrd_style) {
				.h = 40,
					.row_mode = true,
					.border_type = BorderType_None
			})
		}));
		{
			wzrd_box_do((wzrd_box) { .style = wzrd_style_create((wzrd_style) { .w = 90 }) });
			wzrd_box_do((wzrd_box) { 0 });
		}
		wzrd_box_end();

		// Add object dialog
		{
			wzrd_v2 size = { 500, 500 };
			bool ok = false, cancel = false;
			static str128 name;
			static unsigned int selected;

			wzrd_dialog_begin(&create_object_pos, size, &create_object_active, wzrd_str_create("add object"), 4);
			if (create_object_active) {
				wzrd_box_begin((wzrd_box) {
					.style = wzrd_style_create((wzrd_style) {
						.row_mode = true, .border_type = BorderType_None, .child_gap = 5,
					})
				});
				{
					wzrd_str labels[] = { wzrd_str_create("Entity"), wzrd_str_create("Texture") };
					bool is_selected = false;
					wzrd_label_list(labels, 2,
						(wzrd_v2) {
						100, 0
					},
						0, & selected, & is_selected);

					wzrd_box_begin((wzrd_box) {
						.style = wzrd_style_create((wzrd_style) {

							.border_type = BorderType_Clicked,
								.pad_left = 5,
								.pad_top = 5
						})
					});
					{
						if (selected == 1) {
							wzrd_box_begin((wzrd_box) {
								.style = wzrd_style_create((wzrd_style) {
									.h = 50,
										.border_type = BorderType_None, .row_mode = true,
								})
							});
							{
								wzrd_label(wzrd_str_create("Name:"));
								wzrd_input_box(name.val, &name.len, 10);
							}
							wzrd_box_end();
						}
						else if (selected == 0) {
							wzrd_box_begin((wzrd_box) {
								.style = wzrd_style_create((wzrd_style) {
									.border_type = BorderType_None, .row_mode = true, .h = 50,
								}),
							});
							{
								wzrd_label(wzrd_str_create("Name:"));
								wzrd_input_box(name.val, &name.len, 10);
							}
							wzrd_box_end();
						}
					}
					wzrd_box_end();
				}
				wzrd_box_end();

				wzrd_box_begin((wzrd_box) {
					.style = wzrd_style_create((wzrd_style) {
						.h = 50,
							.row_mode = true, .border_type = BorderType_None,
					})
				});
				{
					wzrd_box_do((wzrd_box) {
						.style = wzrd_style_create((wzrd_style) { .border_type = BorderType_None })
					});
					wzrd_box_begin((wzrd_box) {
						.style = wzrd_style_create((wzrd_style) {
							.center_x = true, .center_y = true,
								.child_gap = 10,
								.pad_left = 5,
								.pad_top = 5,
								.pad_right = 5,
								.pad_bottom = 5,
								.row_mode = true,
								.w = 140,
								.border_type = BorderType_None
						})
					});
					{
						bool b1;
						wzrd_button(wzrd_str_create("OK"), &b1);
						if (b1) {
							ok = true;
						}

						bool b2;
						wzrd_button(wzrd_str_create("Cancel"), &b2);
						if (b2) {
							cancel = true;
						}
					}
					wzrd_box_end();
				}
				wzrd_box_end();

			}
			wzrd_dialog_end(create_object_active);

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

				create_object_active = false;
			}

			if (cancel) {
				create_object_active = false;
			}
		}

		// Scrollbar
#if 0
		wzrd_crate_begin(2,
			(wzrd_box) {
			.style = wzrd_style_create((wzrd_style) {
				.x = (int)g_platform.window_width - 300,
					.y = 10,
					.w = 295,
					.h = 500
			})
		});
		{
			//static float scrollbar_x, scrollbar_y;
			wzrd_box_get_last()->clip = true;
			//wzrd_box_get_last()->scrollbar_x = &scrollbar_x;
			//wzrd_box_get_last()->scrollbar_y = &scrollbar_y;
			wzrd_label(*debug_str);
			*debug_str->str = 0;
			debug_str->len = 0;
			//wzrd_box_do((wzrd_box) { .h = 1000, .w = 50, .color = EGUI_BLUE });
		}
		wzrd_crate_end();
#endif
	}
	platform_time_begin();
	wzrd_end(cursor, buffer, debug_str);
	platform_time_end();
#endif
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
