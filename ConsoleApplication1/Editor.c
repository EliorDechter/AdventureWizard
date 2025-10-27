#include "Editor.h"
#include "Platform.h"
#include "Textures.h"
#include "Game.h"

wzrd_str wzrd_str_from_str128(str128* str)
{
	wzrd_str result = { .str = str->val, .len = str->len };

	return result;
}


void editor_do(wzrd_canvas* gui, wzrd_draw_commands_buffer* buffer, wzrd_cursor* cursor, PlatformTargetTexture target_texture, wzrd_icons icons, unsigned int layer, wzrd_str* debug_str) {
	//static int selected_category;

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
		wzrd_box_begin(((wzrd_box) {
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

			if (wzrd_button(wzrd_str_create("Add Object"))) {
				create_object_pos = (wzrd_v2){ 50, 50 };
				create_object_active = true;
			}

			wzrd_toggle(
				wzrd_str_create("Add Polygon"), & g_game.polygon_adding_active);

			// Seperator
			wzrd_box_do((wzrd_box) {
				.style = wzrd_style_create((wzrd_style) {
					.w = 2,
						.border_type = BorderType_LeftLine,
				}),
			});

			bool delete_toggle = wzrd_button_icon(icons.delete);
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

			if (wzrd_button_icon(icons.stop)) {

			}

			if (wzrd_button_icon(icons.pause)) {

			}

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
						if (wzrd_button(wzrd_str_create("OK"))) {
							ok = true;
						}
						if (wzrd_button(wzrd_str_create("Cancel"))) {
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
