#pragma once

#include "Egui.h"
#include "Strings.h"

Crate* EguiGetCurrentWindow() {
	Crate* result = &wzrd_gui.crates_stack[wzrd_gui.current_crate_index];

	return result;
}

Crate* EguiGetPreviousWindow() {
	assert(wzrd_gui.current_crate_index > 0);
	Crate* result = &wzrd_gui.crates_stack[wzrd_gui.current_crate_index - 1];

	return result;
}

wzrd_box* EguiBoxGetPrevious() {
	if (wzrd_gui.crates_stack[wzrd_gui.current_crate_index].box_stack_count < 2) return 0;
	int current_box_index = wzrd_gui.crates_stack[wzrd_gui.current_crate_index].box_stack_count - 2;
	int final_index = wzrd_gui.crates_stack[wzrd_gui.current_crate_index].box_stack[current_box_index];
	wzrd_box* result = &wzrd_gui.boxes[final_index];

	return result;
}

wzrd_box* wzrd_box_get_current() {
	assert(wzrd_gui.current_crate_index >= 0);
	int crate_index = wzrd_gui.current_crate_index;

	int current_box_index = wzrd_gui.crates_stack[crate_index].box_stack_count - 1;
	if (current_box_index < 0) return 0;
	int final_index = wzrd_gui.crates_stack[crate_index].box_stack[current_box_index];
	assert(current_box_index >= 0);
	wzrd_box* result = &wzrd_gui.boxes[final_index];

	return result;
}

int wzrd_box_get_current_index() {
	assert(wzrd_gui.current_crate_index >= 0);
	int current_box_index = wzrd_gui.crates_stack[wzrd_gui.current_crate_index].box_stack_count - 1;
	if (current_box_index < 0) return 0;
	int final_index = wzrd_gui.crates_stack[wzrd_gui.current_crate_index].box_stack[current_box_index];
	int result = final_index;

	return result;
}

int CompareBoxes(const void* element1, const void* element2) {
	wzrd_box* c1 = (wzrd_box*)element1;
	wzrd_box* c2 = (wzrd_box*)element2;

	if (c1->depth > c2->depth) {
		return 1;
	}
	if (c1->depth < c2->depth) {
		return -1;
	}
	if (c1->depth == c2->depth) {
		if (c1->index > c2->index) return 1;
		if (c1->index < c2->index) return -1;
		return 0;
	}
}

void EguiBoxResize(wzrd_v2* size) {
	wzrd_box* box = wzrd_box_get_current();
	wzrd_box* previous_box = EguiBoxGetPrevious();

	box->resizable = true;

	float mouse_delta_x = wzrd_gui.mouse_pos.x - wzrd_gui.previous_mouse_pos.x;
	float mouse_delta_y = wzrd_gui.mouse_pos.y - wzrd_gui.previous_mouse_pos.y;

	if (previous_box->row_mode) {
		if (str128_equal(box->name, wzrd_gui.left_resized_item)) {
			if (size->x - mouse_delta_x >= 0)
				size->x -= mouse_delta_x;
		}

		else if (str128_equal(box->name, wzrd_gui.right_resized_item)) {
			if (size->x + mouse_delta_x >= 0)
				size->x += mouse_delta_x;
		}
	}
	else {
		if (str128_equal(box->name, wzrd_gui.top_resized_item)) {
			if (size->y - mouse_delta_y <= 0)
				size->y -= mouse_delta_y;
		}
		else if (str128_equal(box->name, wzrd_gui.bottom_resized_item)) {
			if (size->y + mouse_delta_y >= 0)
				size->y += mouse_delta_y;
		}
	}

	wzrd_box_get_current()->w += size->x;
	wzrd_box_get_current()->h += size->y;
}

bool wzrd_box_begin(wzrd_box box) {

	assert(wzrd_gui.boxes_count < MAX_NUM_BOXES - 1);

	if (box.w == 0 && !box.fit_w)
		box.grow_horizontal = true;
	if (box.h == 0 && !box.fit_h)
		box.grow_vertical = true;

	if (box.fit_w) {
		box.w += 4 * wzrd_gui.line_size;
	}
	if (box.fit_h) {
		box.h += 4 * wzrd_gui.line_size;
	}

	box.depth = wzrd_gui.current_crate_index;
	wzrd_box* current_box = wzrd_box_get_current();

	// Test for repeating id's
	if (box.name.len) {
		for (int i = 0; i < wzrd_gui.boxes_count; ++i) {
			assert(!str128_equal(box.name, wzrd_gui.boxes[i].name));
		}
	}
	else {
		if (!current_box) assert(0);
	}

	// Increment the panel index in the window's stack
	wzrd_gui.total_num_panels++;

	box.window_index = wzrd_gui.current_crate_index;

	if (current_box) {

		//Name
		if (!box.name.len) {
			assert(*current_box->name.val);
			int n = snprintf(box.name.val, 128, "%s-%d", current_box->name.val, current_box->children_count);
			box.name.len = strlen(box.name.val);
			assert(n <= 128);
		}

		// Children
		assert(current_box->children_count < MAX_NUM_CHILDREN);
		current_box->children[current_box->children_count++] = wzrd_gui.boxes_count;

	}

	// Default color 
	if (box.color.r == 0 && box.color.g == 0 && box.color.b == 0 && box.color.a == 0)
		//box.color = EguiBoxGetCurrent()->color;
		box.color = EGUI_LIGHTGRAY;

	// Add box
	assert(wzrd_gui.boxes_count < 256);
	box.index = wzrd_gui.boxes_count;
	assert(wzrd_gui.current_crate_index >= 0);
	wzrd_gui.crates_stack[wzrd_gui.current_crate_index].box_stack_count++;
	wzrd_gui.crates_stack[wzrd_gui.current_crate_index].box_stack[wzrd_gui.crates_stack[wzrd_gui.current_crate_index].box_stack_count - 1] = wzrd_gui.boxes_count;
	wzrd_gui.boxes[wzrd_gui.boxes_count++] = box;

	// Return if clicked
	bool result = false;
	if (str128_equal(box.name, wzrd_gui.clicked_item)) {
		result = true;
	}

	return result;
}

void wzrd_box_end() {
	wzrd_gui.total_num_panels--;
	assert(wzrd_gui.total_num_panels >= 0);
	assert(wzrd_gui.current_window_id != 0);

	wzrd_box* current_box = wzrd_box_get_current();
	Crate* current_crate = EguiGetCurrentWindow();

	//set to previous panel
	wzrd_box* previous_box = EguiBoxGetPrevious();
	if (previous_box) {

		// Handle fitting
		if (previous_box->fit_h) {
			assert(current_box->h);
			assert(current_box->w);

			if (!previous_box->row_mode) {
				previous_box->h += current_box->h;
			}
			else {
				if (current_box->h > previous_box->h) {
					previous_box->h += current_box->h;
				}

			}
		}
		if (previous_box->fit_w) {
			assert(current_box->h);
			assert(current_box->w);

			if (previous_box->row_mode) {
				previous_box->w += current_box->w;
			}
			else {
				if (current_box->w > previous_box->w) {
					previous_box->w += current_box->w;
				}
			}
		}
	}

	current_crate->box_stack_count--;
	assert(current_crate->box_stack_count >= 0);
}

bool EguiBox(wzrd_box box) {
	bool result = wzrd_box_begin(box);
	wzrd_box_end();

	return result;
}

int wzrd_v2_is_inside_rect(wzrd_v2 v, wzrd_rect rect) {
	bool result = false;
	if (v.x >= rect.x &&
		v.y >= rect.y &&
		v.x <= rect.x + rect.w &&
		v.y <= rect.y + rect.h) {

		result = true;
	}

	return result;
}

bool* EguiToggle(wzrd_box box) {
	bool* result = EguiToggleBegin(box);
	EguiToggleEnd();

	return result;
}

bool IsClicked() {
	if (str128_equal(wzrd_box_get_current()->name, wzrd_gui.clicked_item)) {
		return true;
	}

	return false;
}

bool IsHovered() {
	if (str128_equal(wzrd_box_get_current()->name, wzrd_gui.hot_item)) {
		return true;
	}

	return false;
}

bool* EguiToggleBegin(wzrd_box box) {
	wzrd_box_begin(box);

	bool found = false;
	bool* toggle = 0;
	for (int i = 0; i < wzrd_gui.toggles_count; ++i) {
		if (str128_equal(wzrd_gui.toggles[i].name, wzrd_box_get_current()->name)) {
			toggle = &wzrd_gui.toggles[i].val;
			found = true;
		}
	}

	if (!found) {
		wzrd_gui.toggles[wzrd_gui.toggles_count].name = wzrd_box_get_current()->name;
		wzrd_gui.toggles[wzrd_gui.toggles_count].val = false;
		toggle = &wzrd_gui.toggles[wzrd_gui.toggles_count].val;
		wzrd_gui.toggles_count++;
	}

	if (str128_equal(wzrd_box_get_current()->name, wzrd_gui.clicked_item)) {
		*toggle = !*toggle;
	}

	if (IsHovered()) {
		wzrd_gui.cursor = EguiCursorHand;
	}

	return toggle;
}

void EguiToggleEnd() {
	wzrd_box_end();
}

void EguiStringAdd(str128 str) {
	wzrd_item_add((Item) { .type = wzrd_item_type_str, .str = str });
}

void wzrd_texture_add(wzrd_texture texture, wzrd_v2 size) {
	wzrd_item_add((Item) {
		.type = ItemType_Texture,
			.size = size,
			.texture = texture
	});
}

bool* EguiCheckMark() {
	bool* result = EguiToggleBegin((wzrd_box) { .w = 35, .h = 35, .color = EGUI_GREEN });
	{
		if (*result)
			wzrd_texture_add(wzrd_gui.checkmark, (wzrd_v2) { 35, 35 });
	}
	EguiToggleEnd();

	return result;
}

wzrd_box_text(str128 txt) {
	wzrd_box box = (wzrd_box){ .w = FONT_WIDTH * txt.len, .h = WZRD_FONT_HEIGHT,
		.border_type = BorderType_None, .color = EGUI_WHITE };
	wzrd_box_begin(box);
	{
		EguiStringAdd(txt);
	}
	wzrd_box_end();
}

wzrd_box* EguiHotItemGet() {
	for (int i = 0; i < wzrd_gui.boxes_count; ++i) {
		if (str128_equal(wzrd_gui.boxes[i].name, wzrd_gui.hot_item)) {
			return wzrd_gui.boxes + i;
		}
	}

	return 0;
}


void EguiCrateBegin(int window_id, wzrd_box box) {

	wzrd_gui.total_num_windows++;
	wzrd_gui.current_crate_index++;

	Crate* current_window = EguiGetCurrentWindow();

	// Set current hot window
	if (wzrd_v2_is_inside_rect((wzrd_v2) { wzrd_gui.mouse_pos.x, wzrd_gui.mouse_pos.y },
		(wzrd_rect) {
		box.x, box.y, box.w, box.h
	})) {
		if (window_id > wzrd_gui.hot_window)
			wzrd_gui.hot_window = window_id;
	}
	else {
		if (window_id == wzrd_gui.hot_window) wzrd_gui.hot_window = 0;
	}

	// Set new window
	*current_window = (Crate){ .id = window_id, .index = wzrd_gui.current_crate_index, };

	// Set window id
	assert(window_id != 0);
	wzrd_gui.current_window_id = window_id;

	// Set color
	if (box.color.r == 0 && box.color.g == 0 && box.color.b == 0 && box.color.a == 0)
		box.color = EGUI_LIGHTGRAY;

	// Begin drawing panel

	box.is_crate = true;
	wzrd_box_begin(box);
}

EguiCrateEnd() {

	// Count number of windows for debugging
	wzrd_gui.total_num_windows--;

	wzrd_box_end();

	if (wzrd_gui.current_crate_index > 0) {
		Crate* previous_window = EguiGetPreviousWindow();

		assert(previous_window->id != 0);
		wzrd_gui.current_window_id = previous_window->id;

	}

	wzrd_gui.current_crate_index--;
	assert(wzrd_gui.current_crate_index >= -1);
}

void EguiCrate(int window_id, wzrd_box box) {
	EguiCrateBegin(window_id, box);
	EguiCrateEnd();
}

void wzrd_begin(double time_in_ms, wzrd_v2 mouse_pos, wzrd_state mouse_left, wzrd_keyboard_keys keys, wzrd_texture checkmark) {
	wzrd_gui.cursor = wzrd_cursor_default;
	wzrd_gui.keyboard_keys = keys;
	wzrd_gui.checkmark = checkmark;
	// Set parameters
	wzrd_gui.time = time_in_ms;
	wzrd_gui.mouse_left = mouse_left;
	wzrd_gui.mouse_pos = mouse_pos;
	wzrd_gui.commands_count = 0;
	wzrd_gui.current_crate_index = -1;
	wzrd_gui.boxes_count = 0;

	// Move out of the function
	wzrd_gui.line_size = 1;

	// Zero out stuff
	memset(wzrd_gui.boxes, 0, sizeof(wzrd_box) * wzrd_gui.boxes_count);

	static double last_time_clicked;
	static double time = 0;
	time += 16.7;

	if (mouse_left == EguiDeactivating) {
		if (time - last_time_clicked <= 500) {
			wzrd_gui.double_click = true;
		}
		else {
			wzrd_gui.double_click = false;
		}
		last_time_clicked = time;
	}

	// Begin drawing first window
	static EguiV2i pos;
	EguiCrateBegin(
		CrateId_Screen,
		(wzrd_box) {
		.w = wzrd_gui.window_width,
			.h = wzrd_gui.window_height,
			.name = str128_create("Main window"),
			.pad_left = 5, .pad_top = 5,
			.pad_right = 5,
			.pad_bottom = 5,
			.child_gap = 5,
	});

	wzrd_gui.input_box_timer += 16.7;
}

void EguiRectDraw(wzrd_rect rect, wzrd_color color) {

	EguiDrawCommand command = (EguiDrawCommand){
		.type = DrawCommandType_Rect,
		.dest_rect = rect,
		.color = color
	};

	assert(command.dest_rect.x >= 0);
	assert(command.dest_rect.y >= 0);
	assert(command.dest_rect.w >= 0);
	assert(command.dest_rect.h >= 0);
	assert(command.dest_rect.x + command.dest_rect.w <= wzrd_gui.window_width);
	assert(command.dest_rect.y + command.dest_rect.h <= wzrd_gui.window_height);

	assert(wzrd_gui.commands_count < MAX_NUM_DRAW_COMMANDS - 1);
	wzrd_gui.draw_commands[wzrd_gui.commands_count++] = command;
}

wzrd_box* egui_box_get_by_name(str128 name) {
	if (!name.len) return 0;
	for (int i = 0; i < wzrd_gui.boxes_count; ++i) {
		if (str128_equal(name, wzrd_gui.boxes[i].name)) {
			return wzrd_gui.boxes + i;
		}
	}

	return 0;
}

bool IsRectInsideRect(wzrd_rect a, wzrd_rect b) {
	bool res = a.x >= b.x;
	res &= a.x >= b.x;
	res &= a.x + a.w <= b.x + b.w;
	res &= a.y + a.h <= b.y + b.h;

	return res;
}

EguiDrawCommandsBuffer wzrd_end(bool* change_click_icon) {

	EguiCrateEnd();

	// Test each opened panel has been closed
	if (wzrd_gui.total_num_panels != 0) {
		assert(0);
	}


	assert(wzrd_gui.total_num_windows == 0);

	// Calculate size
	for (int i = 0; i < wzrd_gui.boxes_count; ++i) {
		wzrd_box* owner = &wzrd_gui.boxes[i];

		owner->absolute_rect.w = owner->w;
		owner->absolute_rect.h = owner->h;

		float max_w = 0, max_h = 0;
		float children_h = 0, children_w = 0;
		for (int j = 0; j < owner->children_count; ++j) {
			wzrd_box* child = &wzrd_gui.boxes[owner->children[j]];

			if (child->is_crate) continue;

			if (!child->grow_horizontal)
				children_w += child->w;

			if (!child->grow_vertical)
				children_h += child->h;
		}

		// Default resizing
		float available_w = owner->w - owner->pad_left - owner->pad_right - 4 * wzrd_gui.line_size;
		float available_h = owner->h - owner->pad_top - owner->pad_bottom - 4 * wzrd_gui.line_size;

		if (owner->children_count) {
			if (owner->row_mode)
				available_w -= owner->child_gap * (owner->children_count - 1);
			else
				available_h -= owner->child_gap * (owner->children_count - 1);
		}

		// Handle growing
		for (int j = 0; j < owner->children_count; ++j) {
			wzrd_box* child = &wzrd_gui.boxes[owner->children[j]];

			if (child->grow_horizontal) {
				if (owner->row_mode) {
					child->w = available_w - children_w;
				}
				else {
					child->w = available_w;
				}

				assert(child->w > 0);
			}

			if (child->grow_vertical) {
				if (!owner->row_mode) {
					child->h = available_h - children_h;
				}
				else {
					child->h = available_h;
				}
			}

			child->absolute_rect.w = child->w;
			child->absolute_rect.h = child->h;

			assert(child->absolute_rect.w > 0);
			assert(child->absolute_rect.h > 0);
		}

		// assert
		{
			float w = 0, h = 0;
			for (int j = 0; j < owner->children_count; ++j) {
				wzrd_box* child = &wzrd_gui.boxes[owner->children[j]];
				if (owner->row_mode)
					w += child->absolute_rect.w;
				else
					h += child->absolute_rect.h;
			}
			//assert(w <= owner->absolute_rect.w);
			//assert(h <= owner->absolute_rect.h);
		}
	}



	// Calculate positions
	for (int i = 0; i < wzrd_gui.boxes_count; ++i) {
		wzrd_box* owner = &wzrd_gui.boxes[i];

		if (owner->is_crate) {
			if (owner->parent) {
				owner->x += wzrd_gui.boxes[owner->parent].x;
				owner->y += wzrd_gui.boxes[owner->parent].y;

			}
			else {
				owner->x += owner->x;
				owner->y += owner->y;
			}

			owner->absolute_rect.x = owner->x;
			owner->absolute_rect.y = owner->y;
		}

		float x = owner->x + owner->pad_left, y = owner->y + owner->pad_top;

		x += 2 * wzrd_gui.line_size;
		y += 2 * wzrd_gui.line_size;

		// Center
		float w = 0, h = 0, max_w = 0, max_h = 0;
		for (int i = 0; i < owner->children_count; ++i) {
			wzrd_box* child = &wzrd_gui.boxes[owner->children[i]];

			if (child->w > max_w)
				max_w = child->w;

			if (child->h > max_h)
				max_h = child->h;

			if (owner->row_mode)
				w += child->w;
			else
				h += child->h;
		}

		/*
				if (owner->row_mode)
					h = max_h;
				else
					w = max_w;
					*/

		if (owner->row_mode)
			w += owner->child_gap * (owner->children_count - 1);
		else
			h += owner->child_gap * (owner->children_count - 1);

		if (owner->center) {
			owner->center_x = true;
			owner->center_y = true;
		}

		if (owner->center_x && owner->row_mode) {
			x += (owner->w - 4 * wzrd_gui.line_size - owner->pad_left - owner->pad_right) / 2 - w / 2;
		}
		if (owner->center_y && !owner->row_mode) {
			y += (owner->h - 4 * wzrd_gui.line_size - owner->pad_top - owner->pad_bottom) / 2 - h / 2;
		}

		// Calc positions
		for (int j = 0; j < owner->children_count; ++j) {
			wzrd_box* child = &wzrd_gui.boxes[owner->children[j]];

			child->x += x;
			child->y += y;

			if (owner->center_y && owner->row_mode) {
				child->y += (owner->h - 4 * wzrd_gui.line_size - owner->pad_top - owner->pad_bottom) / 2 - child->h / 2;
			}
			if (owner->center_x && !owner->row_mode) {
				child->x += (owner->w - 4 * wzrd_gui.line_size - owner->pad_top - owner->pad_bottom) / 2 - child->w / 2;
			}

			if (owner->row_mode) {
				x += child->w;
				x += owner->child_gap;
			}
			else {
				y += child->h;
				y += owner->child_gap;
			}

			child->absolute_rect.x = child->x;
			child->absolute_rect.y = child->y;

			//assert(child->absolute_rect.x >= 0);
			//assert(child->absolute_rect.y >= 0);
		}
	}

	for (int i = 0; i < wzrd_gui.boxes_count; ++i) {
		for (int j = 0; j < wzrd_gui.boxes[i].children_count; ++j) {
			wzrd_box* owner = wzrd_gui.boxes + i;
			wzrd_box* child = wzrd_gui.boxes + wzrd_gui.boxes[i].children[j];
			if (!IsRectInsideRect(child->absolute_rect, owner->absolute_rect)) {
				owner->color = EGUI_ORANGE;
				child->color = EGUI_RED;
			}
		}
	}

	// Mouse interaction
	wzrd_box* hovered_box = 0;
	int depth = 0;

	for (int i = 0; i < wzrd_gui.boxes_count; ++i) {
		wzrd_box* box = wzrd_gui.boxes + i;
		bool is_hover = wzrd_v2_is_inside_rect((wzrd_v2) { wzrd_gui.mouse_pos.x, wzrd_gui.mouse_pos.y },
			box->absolute_rect);
		if (is_hover
			&& box->depth >= depth
			) {
			hovered_box = box;
			depth = box->depth;
		}
	}

	wzrd_box* hot_box = egui_box_get_by_name(wzrd_gui.hot_item);
	wzrd_box* active_box = egui_box_get_by_name(wzrd_gui.active_item);

	if (active_box) {
		if (wzrd_gui.mouse_left == EguiDeactivating) {
			if (hot_box == active_box) {
				wzrd_gui.clicked_item = active_box->name;

			}
			wzrd_gui.active_item = (str128){ 0 };
		}
	}

	if (hot_box) {
		if (hot_box->flat_button)
			hot_box->color = EGUI_DARKBLUE;
		if (wzrd_gui.mouse_left == EguiActivating) {
			wzrd_gui.active_item = hot_box->name;
		}
	}

	if (hovered_box) {
		wzrd_gui.hot_item = hovered_box->name;
	}
	else {
		wzrd_gui.hot_item = (str128){ 0 };
	}

	// stuff
	if (wzrd_gui.clicked_item.len) {
		wzrd_box* clicked_box = egui_box_get_by_name(wzrd_gui.clicked_item);
		if (clicked_box->is_input_box) {
			wzrd_gui.active_input_box = clicked_box->name;
		}
		else {
			wzrd_gui.active_input_box = (str128){ 0 };
		}
	}

	if (wzrd_gui.mouse_left == EguiInactive) {
		wzrd_gui.clicked_item = (str128){ 0 };
	}

	// Border resize
	{
		wzrd_gui.left_resized_item = (str128){ 0 };
		wzrd_gui.right_resized_item = (str128){ 0 };
		wzrd_gui.top_resized_item = (str128){ 0 };
		wzrd_gui.bottom_resized_item = (str128){ 0 };

		for (int i = 0; i < wzrd_gui.boxes_count; ++i) {
			wzrd_box* owner = wzrd_gui.boxes + i;
			for (int j = 0; j < owner->children_count; ++j) {
				wzrd_box* child = &wzrd_gui.boxes[owner->children[j]];

				float border_size = 2;

				if (!child->resizable) continue;

				bool is_inside_left_border =
					wzrd_gui.mouse_pos.x >= child->absolute_rect.x &&
					wzrd_gui.mouse_pos.y >= child->absolute_rect.y &&
					wzrd_gui.mouse_pos.x < child->absolute_rect.x + border_size &&
					wzrd_gui.mouse_pos.y < child->absolute_rect.y + child->absolute_rect.h;
				bool is_inside_right_border =
					wzrd_gui.mouse_pos.x >= child->absolute_rect.x + child->absolute_rect.w - border_size &&
					wzrd_gui.mouse_pos.y >= child->absolute_rect.y &&
					wzrd_gui.mouse_pos.x < child->absolute_rect.x + child->absolute_rect.w &&
					wzrd_gui.mouse_pos.y < child->absolute_rect.y + child->absolute_rect.h;
				bool is_inside_top_border =
					wzrd_gui.mouse_pos.x >= child->absolute_rect.x &&
					wzrd_gui.mouse_pos.y >= child->absolute_rect.y &&
					wzrd_gui.mouse_pos.x < child->absolute_rect.x + child->absolute_rect.w &&
					wzrd_gui.mouse_pos.y < child->absolute_rect.y + border_size;
				bool is_inside_bottom_border =
					wzrd_gui.mouse_pos.x >= child->absolute_rect.x &&
					wzrd_gui.mouse_pos.y >= child->absolute_rect.y + child->absolute_rect.h - border_size &&
					wzrd_gui.mouse_pos.x < child->absolute_rect.x + child->absolute_rect.w &&
					wzrd_gui.mouse_pos.y < child->absolute_rect.y + child->absolute_rect.h;

				if (is_inside_top_border || is_inside_bottom_border) {
					wzrd_gui.cursor = EguiCursorVerticalArrow;
				}
				else if (is_inside_left_border || is_inside_right_border) {
					wzrd_gui.cursor = EguiCursorHorizontalArrow;
				}

				if (*wzrd_gui.active_item.val) {
					if (is_inside_bottom_border) {
						child->color = EGUI_PURPLE;
						wzrd_gui.bottom_resized_item = child->name;
					}
					else if (is_inside_top_border) {
						child->color = EGUI_PURPLE;
						wzrd_gui.top_resized_item = child->name;
					}
					else if (is_inside_left_border) {
						child->color = EGUI_PURPLE;
						wzrd_gui.left_resized_item = child->name;
					}
					else if (is_inside_right_border) {
						child->color = EGUI_PURPLE;
						wzrd_gui.right_resized_item = child->name;
					}
				}
			}
		}
	}

	// Drawing
	wzrd_box boxes[MAX_NUM_BOXES] = { 0 };
	memcpy(boxes, wzrd_gui.boxes, sizeof(wzrd_box) * wzrd_gui.boxes_count);
	qsort(boxes, wzrd_gui.boxes_count, sizeof(wzrd_box), CompareBoxes);

	// Mouse position
	wzrd_gui.previous_mouse_pos = wzrd_gui.mouse_pos;

	// Draw boxes
	for (int i = 0; i < wzrd_gui.boxes_count; ++i) {

		wzrd_box box = boxes[i];

		EguiRectDraw((wzrd_rect) {
			.x = box.absolute_rect.x,
				.y = box.absolute_rect.y,
				.h = box.absolute_rect.h,
				.w = box.absolute_rect.w
		},
			box.color);

		// Borders (1215 x 810)
		int line_size = wzrd_gui.line_size;

		wzrd_rect top0 = (wzrd_rect){ box.absolute_rect.x, box.absolute_rect.y, box.absolute_rect.w - line_size, line_size };
		wzrd_rect left0 = (wzrd_rect){ box.absolute_rect.x, box.absolute_rect.y, line_size, box.absolute_rect.h };

		wzrd_rect top1 = (wzrd_rect){ box.absolute_rect.x + line_size, box.absolute_rect.y + line_size, box.absolute_rect.w - 3 * line_size, line_size };
		wzrd_rect left1 = (wzrd_rect){ box.absolute_rect.x + line_size, box.absolute_rect.y + line_size, line_size, box.absolute_rect.h - line_size };

		wzrd_rect bottom0 = (wzrd_rect){ box.absolute_rect.x, box.absolute_rect.y + box.absolute_rect.h - line_size, box.absolute_rect.w, line_size };
		wzrd_rect right0 = (wzrd_rect){ box.absolute_rect.x + box.absolute_rect.w - line_size, box.absolute_rect.y, line_size, box.absolute_rect.h };

		wzrd_rect bottom1 = (wzrd_rect){ box.absolute_rect.x + 1 * line_size, box.absolute_rect.y + box.absolute_rect.h - 2 * line_size, box.absolute_rect.w - 3 * line_size, line_size };
		wzrd_rect right1 = (wzrd_rect){ box.absolute_rect.x + box.absolute_rect.w - 2 * line_size, box.absolute_rect.y + 1 * line_size, line_size, box.absolute_rect.h - 3 * line_size };

		IsRectInsideRect(top0, box.absolute_rect);
		IsRectInsideRect(left0, box.absolute_rect);
		IsRectInsideRect(top1, box.absolute_rect);
		IsRectInsideRect(left1, box.absolute_rect);
		IsRectInsideRect(bottom0, box.absolute_rect);
		IsRectInsideRect(right0, box.absolute_rect);
		IsRectInsideRect(bottom1, box.absolute_rect);
		IsRectInsideRect(right1, box.absolute_rect);

		bool debug = false;
		if (debug) {
			// Draw top and left lines
			EguiRectDraw(top0, EGUI_BLUE);
			EguiRectDraw(left0, EGUI_BLUE);
			EguiRectDraw(top1, EGUI_ORANGE);
			EguiRectDraw(left1, EGUI_ORANGE);

			// Draw bottom and right lines
			EguiRectDraw(bottom0, EGUI_GREEN);
			EguiRectDraw(right0, EGUI_GREEN);
			EguiRectDraw(bottom1, EGUI_PURPLE);
			EguiRectDraw(right1, EGUI_PURPLE);

		}
		else if (box.border_type == BorderType_Default) {
			// Draw top and left lines
			EguiRectDraw(top0, EGUI_WHITE2);
			EguiRectDraw(left0, EGUI_WHITE2);
			EguiRectDraw(top1, EGUI_LIGHTGRAY);
			EguiRectDraw(left1, EGUI_LIGHTGRAY);

			// Draw bottom and right lines
			EguiRectDraw(bottom0, EGUI_BLACK);
			EguiRectDraw(right0, EGUI_BLACK);
			EguiRectDraw(bottom1, EGUI_GRAY);
			EguiRectDraw(right1, EGUI_GRAY);
		}
		else if (box.border_type == BorderType_Clicked) {
			// Draw top and left lines
			EguiRectDraw(top0, EGUI_BLACK);
			EguiRectDraw(left0, EGUI_BLACK);
			EguiRectDraw(top1, EGUI_GRAY);
			EguiRectDraw(left1, EGUI_GRAY);

			// Draw bottom and right lines
			EguiRectDraw(bottom0, EGUI_WHITE2);
			EguiRectDraw(right0, EGUI_WHITE2);
			EguiRectDraw(bottom1, EGUI_LIGHTESTGRAY);
			EguiRectDraw(right1, EGUI_LIGHTESTGRAY);
		}
		else if (box.border_type == BorderType_InputBox) {
			// Draw top and left lines
			EguiRectDraw(top0, EGUI_GRAY);
			EguiRectDraw(left0, EGUI_GRAY);
			EguiRectDraw(top1, EGUI_BLACK);
			EguiRectDraw(left1, EGUI_BLACK);

			// Draw bottom and right lines
			EguiRectDraw(bottom0, EGUI_WHITE2);
			EguiRectDraw(right0, EGUI_WHITE2);
			EguiRectDraw(bottom1, EGUI_LIGHTESTGRAY);
			EguiRectDraw(right1, EGUI_LIGHTESTGRAY);
		}
		else if (box.border_type == BorderType_Black) {
			// Draw top and left lines
			EguiRectDraw(top0, EGUI_BLACK);
			EguiRectDraw(left0, EGUI_BLACK);
			EguiRectDraw(top1, EGUI_WHITE2);
			EguiRectDraw(left1, EGUI_WHITE2);

			// Draw bottom and right lines
			EguiRectDraw(bottom0, EGUI_BLACK);
			EguiRectDraw(right0, EGUI_BLACK);
			EguiRectDraw(bottom1, EGUI_WHITE2);
			EguiRectDraw(right1, EGUI_WHITE2);
		}
		else if (box.border_type == BorderType_BottomLine) {
			EguiRectDraw(bottom0, EGUI_WHITE2);
			EguiRectDraw(bottom1, EGUI_GRAY);
		}
		else if (box.border_type == BorderType_LeftLine) {
			EguiRectDraw(left0, EGUI_GRAY);
			EguiRectDraw(left1, EGUI_WHITE2);
		}

		for (int j = 0; j < box.items_count; ++j) {
			Item item = box.items[j];
			EguiDrawCommand command = { 0 };

			if (item.size.x == 0)
				item.size.x = box.absolute_rect.w;
			if (item.size.y == 0)
				item.size.y = box.absolute_rect.h;

			if (item.type == wzrd_item_type_str) {
				command = (EguiDrawCommand){
					.type = DrawCommandType_String,
						.str = item.str,
						.dest_rect = (wzrd_rect){ box.absolute_rect.x, box.absolute_rect.y,
						box.absolute_rect.w, box.absolute_rect.h },
				};
			}
			else if (item.type == ItemType_Texture) {
				command = (EguiDrawCommand){
					.type = DrawCommandType_Texture,
					.dest_rect = box.absolute_rect,
					.src_rect = (wzrd_rect) {0, 0, item.texture.w, item.texture.h},
					.texture = item.texture
				};

				if (item.scissor) {
					command.src_rect = (wzrd_rect){ 0, 0, command.dest_rect.w, command.dest_rect.h };
				}
			}
			else if (item.type == ItemType_Line) {
				command = (EguiDrawCommand){
					.type = DrawCommandType_Line,
						.dest_rect = (wzrd_rect){
							box.absolute_rect.x + item.rect.x,
							box.absolute_rect.y + item.rect.y,
							box.absolute_rect.x + item.rect.w,
							box.absolute_rect.y + item.rect.h
					},
						.color = item.color
				};
			}
			else if (item.type == ItemType_HorizontalDottedLine) {
				command = (EguiDrawCommand){
					.type = DrawCommandType_HorizontalLine,
						.dest_rect = (wzrd_rect){
							box.absolute_rect.x,
							box.absolute_rect.y + box.absolute_rect.h / 2,
							box.absolute_rect.x + box.absolute_rect.w,
							box.absolute_rect.y + box.absolute_rect.h / 2
					}
				};
			}
			else if (item.type == ItemType_LeftHorizontalDottedLine) {
				command = (EguiDrawCommand){
					.type = DrawCommandType_HorizontalLine,
						.dest_rect = (wzrd_rect){
							box.absolute_rect.x,
							box.absolute_rect.y + box.absolute_rect.h / 2,
							box.absolute_rect.x + box.absolute_rect.w / 2,
							box.absolute_rect.y + box.absolute_rect.h / 2
					}
				};
			}
			else if (item.type == ItemType_RightHorizontalDottedLine) {
				command = (EguiDrawCommand){
					.type = DrawCommandType_HorizontalLine,
						.dest_rect = (wzrd_rect){
							box.absolute_rect.x + box.absolute_rect.w / 2,
							box.absolute_rect.y + box.absolute_rect.h / 2,
							box.absolute_rect.x + box.absolute_rect.w,
							box.absolute_rect.y + box.absolute_rect.h / 2
					}
				};
			}
			else if (item.type == ItemType_VerticalDottedLine) {
				command = (EguiDrawCommand){
					.type = DrawCommandType_VerticalLine,
						.dest_rect = (wzrd_rect){
							box.absolute_rect.x + box.absolute_rect.w / 2,
							box.absolute_rect.y,
							box.absolute_rect.x + box.absolute_rect.w / 2 + box.absolute_rect.w,
							box.absolute_rect.y + box.absolute_rect.h
					}
				};
			}
			else if (item.type == ItemType_TopVerticalDottedLine) {
				command = (EguiDrawCommand){
					.type = DrawCommandType_VerticalLine,
						.dest_rect = (wzrd_rect){
							box.absolute_rect.x + box.absolute_rect.w / 2,
							box.absolute_rect.y,
							box.absolute_rect.x + box.absolute_rect.w / 2 + box.absolute_rect.w,
							box.absolute_rect.y + box.absolute_rect.h / 2
					}
				};
			}
			else if (item.type == ItemType_BottomVerticalDottedLine) {
				command = (EguiDrawCommand){
					.type = DrawCommandType_VerticalLine,
						.dest_rect = (wzrd_rect){
							box.absolute_rect.x + box.absolute_rect.w / 2,
							box.absolute_rect.y + box.absolute_rect.h / 2,
							box.absolute_rect.x + box.absolute_rect.w / 2,
							box.absolute_rect.y + box.absolute_rect.h
					}
				};
			}
			else if (item.type == ItemType_IconClose) {
				command = (EguiDrawCommand){
					.type = DrawCommandType_IconClose,
					.dest_rect = (wzrd_rect){box.absolute_rect.x, box.absolute_rect.y, 12, 12 }
				};
			}
			else if (item.type == ItemType_Rect) {
				command = (EguiDrawCommand){
					.type = DrawCommandType_Rect,
						.dest_rect = (wzrd_rect){
							box.absolute_rect.x + item.rect.x,
							box.absolute_rect.y + item.rect.y,
							item.rect.w,
							item.rect.h
					},
						.color = item.color
				};
			}

			command.dest_rect.x += item.pad_left;
			command.dest_rect.y += item.pad_top;

			Draw(command);
		}
	}

	// Return
	EguiDrawCommandsBuffer result = { 0 };
	memcpy(result.commands, wzrd_gui.draw_commands, wzrd_gui.commands_count * sizeof(EguiDrawCommand));
	result.num = wzrd_gui.commands_count;

	return result;
}

bool EguiButtonIcon2(wzrd_texture texture) {
	bool result = false;
	result |= EguiButtonRawBegin((wzrd_box) { .w = 24, .h = 22, .center = true });
	{
		result |= EguiButtonRawBegin((wzrd_box) { .border_type = BorderType_None, .w = 10, .h = 10 });
		{
			wzrd_item_add((Item) {
				.type = ItemType_Texture,
					.texture = texture
			});
		}
		EguiButtonRawEnd();
	}
	EguiButtonRawEnd();

	return result;
}

bool wzrd_button_icon2(wzrd_box box, wzrd_texture texture) {
	bool result = false;
	result |= EguiButtonRawBegin(box);
	{
		result |= EguiButtonRawBegin((wzrd_box) { .border_type = BorderType_None, .w = 16, .h = 16 });
		{
			wzrd_item_add((Item) {
				.type = ItemType_Texture,
					.texture = texture
			});
		}
		EguiButtonRawEnd();
	}
	EguiButtonRawEnd();

	return result;
}

bool IsActive() {
	if (str128_equal(wzrd_box_get_current()->name, wzrd_gui.active_item)) {
		return true;
	}

	return false;
}

bool wzrd_button_icon(wzrd_texture texture) {
	bool result = false;
	bool active = false;
	result |= EguiButtonRawBegin((wzrd_box) { .w = 24, .h = 22, .center = true });
	{
		active = IsActive();
		
		result |= EguiButtonRawBegin((wzrd_box) { .border_type = BorderType_None, .w = 16, .h = 16 });
		{
			active |= IsActive();

			wzrd_item_add((Item) {
				.type = ItemType_Texture,
					.texture = texture
			});
		}
		EguiButtonRawEnd();

		if (active) {
			wzrd_box_get_current()->border_type = BorderType_Clicked;
		}

	}
	EguiButtonRawEnd();

	return result;
}

// Draw tooltip using control bounds
static void EguiTooltip(wzrd_rect controlRec, Str32 str)
{
	if (str.val != 0)
	{
		wzrd_v2 textSize = { 0 };//MeasureTextEx(GuiGetFont(), str.str, (float)GuiGetStyle(DEFAULT, TEXT_SIZE), (float)GuiGetStyle(DEFAULT, TEXT_SPACING));

		if ((controlRec.x + textSize.x + 16) > GetScreenWidth()) controlRec.x -= (textSize.x + 16 - controlRec.w);

		wzrd_rect rect = (wzrd_rect){ controlRec.x, controlRec.y + controlRec.h + 4, textSize.x + 16, 0 + 8.0f };

		//EguiCrateBegin(Str32Create("Tooltip"), (EguiV2) { rect.x, rect.y }, (Vector2) { rect.width, rect.height }, (Vector2) { 0 }, BorderType_Black, (PlatformColor) { 255, 255, 204, 255 }, CrateId_Tooltip, EguiCrateBegin);
#if 0
		EguiCrateBegin(false, (EguiV2) { rect.x, rect.y }, (EguiV2) { rect.w, rect.h },
			CrateId_Tooltip,
			(Box) {
			.name = Str128Create("Tooltip"),
				.border_type = BorderType_Black,
				.color = (EguiColor){ 255, 255, 204, 255 }
		});
		{
			//EguiLabel((EguiRect) { controlRec.x, controlRec.y + controlRec.h + 4, textSize.x + 16, 8.0f }, str.str);
		}
		EguiCrateEnd();
#endif
	}
}

typedef enum {
	STATE_NORMAL = 0,
	STATE_FOCUSED,
	STATE_PRESSED,
	STATE_DISABLED
} GuiState;

GuiState guiState;
// Panel control
#if 0
int EguiPanel(PlatformRect bounds, const char* text)
{
#if !defined(RAYGUI_PANEL_BORDER_WIDTH)
#define RAYGUI_PANEL_BORDER_WIDTH   1
#endif

	int result = 0;
	GuiState state = guiState;

	// Text will be drawn as a header bar (if provided)
	PlatformRect statusBar = { bounds.x, bounds.y, bounds.width, bounds.height };
	//if ((text != NULL) && (bounds.height < RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT * 2.0f)) bounds.height = RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT * 2.0f;

	if (text != NULL)
	{
		// Move panel bounds after the header bar
		bounds.y += (float)RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT - 1;
		bounds.height -= (float)RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT - 1;
	}

	// Draw control
	//--------------------------------------------------------------------
	if (text != NULL) GuiStatusBar(statusBar, text);  // Draw panel header as status bar

#if 0
	EguiDrawRect(bounds, RAYGUI_PANEL_BORDER_WIDTH, GetColor(GuiGetStyle(DEFAULT, (state == STATE_DISABLED) ? (int)BORDER_COLOR_DISABLED : (int)LINE_COLOR)),
		GetColor(GuiGetStyle(DEFAULT, (state == STATE_DISABLED) ? (int)BASE_COLOR_DISABLED : (int)BACKGROUND_COLOR)));
#else 
	EguiDrawRect((PlatformRect) { bounds.x, bounds.y, bounds.width, 1 }, 0, BLACK, BLACK);
	EguiDrawRect((PlatformRect) { bounds.x, bounds.y, 1, bounds.height }, 0, BLACK, BLACK);
	EguiDrawRect((PlatformRect) { bounds.x, bounds.y + bounds.height, bounds.width, 1 }, 0, LIGHTGRAY, LIGHTGRAY);
	EguiDrawRect((PlatformRect) { bounds.x + bounds.width, bounds.y, 1, bounds.height }, 0, LIGHTGRAY, LIGHTGRAY);

#endif

	//--------------------------------------------------------------------

	return result;
}
#endif

bool EguiToggle2(wzrd_box box, str128 str, wzrd_color color, bool active) {
	bool b1 = false, b2 = false;
	b1 = EguiButtonRawBegin(box);
	{
		if (active) {
			wzrd_box_get_current()->color = color;
		}

		b2 = EguiButtonRawBegin((wzrd_box) {
			.border_type = BorderType_None,
				.color = wzrd_box_get_current()->color,
				.w = str.len * FONT_WIDTH, .h = WZRD_FONT_HEIGHT
		});
		{
			if (active) wzrd_box_get_current()->color = color;

			wzrd_item_add((Item) { .type = wzrd_item_type_str, .str = str128_create(str.val) });
		}
		EguiButtonRawEnd();
	}
	EguiButtonRawEnd();

	if (b1 || b2)
		return true;
	return false;
}

bool EguiButton2(wzrd_box box, str128 str, wzrd_color color) {
	bool flag = false;
	bool result = EguiButtonRawBegin(box);
	{
		if (IsHovered()) {
			flag = true;
		}

		result |= EguiButtonRawBegin((wzrd_box) {
			.border_type = BorderType_None,
				.color = EGUI_WHITE,
		});
		{
			if (IsHovered()) {
				flag = true;
			}

			wzrd_item_add((Item) { .type = wzrd_item_type_str, .str = str128_create(str.val) });
		}
		EguiButtonRawEnd();
	}
	EguiButtonRawEnd();

	if (flag) {
		wzrd_gui.boxes[wzrd_gui.boxes_count - 1].color = EGUI_DARKBLUE;
		wzrd_gui.boxes[wzrd_gui.boxes_count - 2].color = EGUI_DARKBLUE;

	}

	return result;
}

bool EguiButtonRawBegin(wzrd_box box) {
	bool result = false;
	wzrd_box_begin(box);

	if (str128_equal(wzrd_box_get_current()->name, wzrd_gui.clicked_item)) {
		result = true;
	}

	if (IsHovered()) {
		wzrd_gui.cursor = EguiCursorHand;
	}

	return result;
}

void  EguiButtonRawEnd() {
	wzrd_box_end();
}

bool EguiButtonRaw(wzrd_box box)
{
	bool result = EguiButtonRawBegin(box);
	EguiButtonRawEnd();

	return result;
}

void wzrd_item_add(Item item) {
	wzrd_box* box = wzrd_box_get_current();
	assert(box->items_count < MAX_NUM_ITEMS - 1);
	box->items[box->items_count++] = item;
}

void EguiLabel(str128 str) {
	EguiV2i size = { FONT_WIDTH * str.len, WZRD_FONT_HEIGHT };

	wzrd_box_begin((wzrd_box) {
		.border_type = BorderType_None,
			.color = wzrd_box_get_current()->color,
			.w = size.x,
			.h = size.y
	});
	{
		wzrd_item_add((Item) { .type = wzrd_item_type_str, .str = str128_create(str.val) });
	}
	wzrd_box_end();
}

bool wzrd_is_active() {
	bool result = false;
	if (str128_equal(wzrd_box_get_current()->name, wzrd_gui.active_item)) {
		result = true;
	}

	return result;
}

void wzrd_input_box(str128* str, int max_num_keys) {

	wzrd_box_begin((wzrd_box) {
		.color = EGUI_WHITE,
			.w = FONT_WIDTH * max_num_keys + 8,
			.h = WZRD_FONT_HEIGHT + 8,
			.pad_left = 2,
			.pad_top = 2,
			.pad_bottom = 2,
			.pad_right = 2,
			.center = true,
			.border_type = BorderType_InputBox,
			.is_input_box = true
	});
	{
		wzrd_box_begin((wzrd_box) { .is_input_box = true, .w = FONT_WIDTH * max_num_keys, .h = WZRD_FONT_HEIGHT, .border_type = BorderType_None, .color = EGUI_WHITE });
		{
			str128 str2 = *str;

			if (str128_equal(wzrd_box_get_current()->name, wzrd_gui.active_input_box)) {

				for (int i = 0; i < wzrd_gui.keyboard_keys.count; ++i) {
					wzrd_keyboard_key key = wzrd_gui.keyboard_keys.keys[i];

					if (key.val == '\b' &&
						str->len > 0 &&
						(key.state == EguiActive || key.state == EguiActivating)) {
						str->val[str->len - 1] = 0;
						str->len--;
					}

					if ((key.state == EguiActivating || key.state == EguiActive) &&
						//((key.val <= 'z' && key.val >= 'a') || (key.val <= '9' && key.val >= '0')) &&
						(isgraph(key.val) || key.val == ' ') &&
						str->len < max_num_keys - 1 &&
						str->len < 127) {
						char s[2] = { [0] = key.val };
						str128 ss = str128_create(s);
						str128_concat(str, &ss);
					}
				}

				static bool show_caret;

				if (wzrd_gui.input_box_timer - wzrd_gui.time > 500) {
					wzrd_gui.input_box_timer = 0;
					show_caret = !show_caret;
				}

				if (show_caret) {
					str2 = *str;
					str128_concat(&str2, "|");
				}

			}

			/*if (str128_equal(wzrd_gui.active_input_box, wzrd_box_get_current()->name)) {
				wzrd_box_get_current()->color = EGUI_PINK;
			}*/

			wzrd_v2 size = { FONT_WIDTH * max_num_keys + 4, WZRD_FONT_HEIGHT + 4 };


			wzrd_item_add((Item) { .type = wzrd_item_type_str, .str = str2 });
		}
		wzrd_box_end();
	}
	wzrd_box_end();
}

bool EguiLabelButtonBegin(str128 str) {
	EguiV2i v = { FONT_WIDTH * str.len, WZRD_FONT_HEIGHT };
	bool result = wzrd_box_begin((wzrd_box) {
		.border_type = BorderType_None,
			.color = wzrd_box_get_current()->color,
			.w = v.x,
			.h = v.y,
			.flat_button = true,
			//.center = true,
	});
	{
		wzrd_item_add((Item) { .type = wzrd_item_type_str, .str = str128_create(str.val) });
	}

	if (IsHovered()) {
		wzrd_gui.cursor = EguiCursorHand;
	}

	return result;
}

bool EguiLabelButtonEnd() {
	wzrd_box_end();

}

bool EguiLabelButton(str128 str) {
	bool result = EguiLabelButtonBegin(str);
	EguiLabelButtonEnd();
	return result;
}


void wzrd_dialog_begin(wzrd_v2* pos, wzrd_v2 size, bool* active, str128 name, int parent) {

	if (!*active) return;

	EguiCrateBegin(1, (wzrd_box) { .parent = parent, .x = pos->x, .y = pos->y, .w = size.x, .h = size.y, .name = name });

	bool close = false;
	wzrd_box_begin((wzrd_box) { .border_type = BorderType_None, .h = 28, .row_mode = true });
	{
		wzrd_box_begin((wzrd_box) {
			.border_type = BorderType_None,
				.color = (wzrd_color){ 57, 77, 205 }
		});
		{
			if (str128_equal(wzrd_box_get_current()->name, wzrd_gui.active_item)) {
				pos->x += wzrd_gui.mouse_pos.x - wzrd_gui.previous_mouse_pos.x;
				pos->y += wzrd_gui.mouse_pos.y - wzrd_gui.previous_mouse_pos.y;
			}
		}
		wzrd_box_end();

		wzrd_box_begin((wzrd_box) {
			.border_type = BorderType_None, .w = 28, .row_mode = true,
				.center = true,
				.color = (wzrd_color){ 57, 77, 205 },
		});
		{
			bool b = EguiButtonRawBegin((wzrd_box) { .w = 20, .h = 20, .center = true });
			{
				b |= EguiButtonRawBegin((wzrd_box) { .w = 12, .h = 12, .border_type = BorderType_None });
				{
					wzrd_item_add((Item) { .texture = wzrd_gui.icons.close, .type = ItemType_Texture });
				}
				EguiButtonRawEnd();
			}
			EguiButtonRawEnd();

			if (b) {
				*active = false;
				close = true;
			}

		}
		wzrd_box_end();
	}
	wzrd_box_end();

	if (close) {
		EguiCrateEnd();
	}
}

void EguiDialogEnd(bool active) {
	if (active)
		EguiCrateEnd();
}

int wzrd_dropdown(int* selected_text, const str128* texts, int texts_count, int w, bool *active) {
	assert(texts);
	assert(texts_count);
	assert(selected_text);
	assert(*selected_text >= 0);

	w = 150;

	wzrd_box_begin((wzrd_box) {
		.fit_w = true,
			.fit_h = true,
			.color = EGUI_WHITE, .border_type = BorderType_InputBox, .row_mode = true
	});
	{
		wzrd_box box = (wzrd_box){
			.center = true,
				.pad_left = 2,
				.pad_top = 2,
				.pad_bottom = 2, .pad_right = 2,
				.border_type = BorderType_None,
				.color = EGUI_WHITE,
				.w = w,
				.h = WZRD_FONT_HEIGHT + 4 * wzrd_gui.line_size
		};

		wzrd_box b = box;

		int parent = 0;
		EguiButton2(box, texts[*selected_text], EGUI_DARKBLUE);

		parent = wzrd_gui.boxes_count - 1;

		box.border_type = BorderType_None;

		//bool* toggle2;

		bool button = wzrd_button_icon2((wzrd_box) { .w = 20, .h = WZRD_FONT_HEIGHT + 4 * wzrd_gui.line_size, .center = true }, wzrd_gui.icons.dropdown);
		if (button) {
			*active = !*active;
		}
		
		if (*active) {
			char str[128];
			sprintf(str, "%s-dropdown", wzrd_box_get_current()->name.val);
			static EguiV2i pos;

			EguiCrateBegin(6, (wzrd_box) {
				.parent = parent,
					.y = box.h,
					.w = box.w + 4 * wzrd_gui.line_size,
					.h = box.h * texts_count + 4 * wzrd_gui.line_size,
					.name = str128_create(str),
					.border_type = BorderType_Black,
			});
			{
				for (int i = 0; i < texts_count; ++i) {

					bool pressed = EguiButton2(box, texts[i], EGUI_DARKBLUE);

					if (pressed) {
						*selected_text = i;
						*active = false;
					}
				}
			}
			EguiCrateEnd();
		}
	}
	wzrd_box_end();
}

bool EguiButton(str128 str) {
	EguiV2i v = { FONT_WIDTH * str.len + 12, WZRD_FONT_HEIGHT + 12 };
	bool result = EguiButtonRawBegin((wzrd_box) {
		.w = v.x,
			.h = v.y,
			.pad_left = 2,
			.pad_right = 2,
			.pad_bottom = 2,
			.pad_top = 2,
	});
	{
		result |= EguiButtonRawBegin((wzrd_box) {
			.border_type = BorderType_None
		});
		{
			wzrd_item_add((Item) { .type = wzrd_item_type_str, .str = str128_create(str.val) });
		}
		EguiButtonRawEnd();
	}
	EguiButtonRawEnd();

	return result;
}

void wzrd_label_list2(Label_list label_list, wzrd_box box, int* selected) {
	wzrd_box_begin(box);
	{
		if (IsClicked()) {
			*selected = -1;
		}

		for (int i = 0; i < label_list.count; ++i) {
			bool active = false;
			if (*selected == i)
				active = true;
			bool res = EguiToggle2((wzrd_box) { .center = true, .grow_horizontal = true, .h = 32, .border_type = BorderType_None, .color = EGUI_WHITE },
				label_list.val[i], EGUI_DARKBLUE, active);

			if (res) {
				*selected = i;
			}
		}

	}
	wzrd_box_end();
}

void wzrd_label_list(Label_list label_list, int* selected) {
	wzrd_box_begin((wzrd_box) {
		.color = EGUI_WHITE, .border_type = BorderType_InputBox,
			.border_type = BorderType_Clicked, .fit_h = true, .fit_w = true
	});
	{
		for (int i = 0; i < label_list.count; ++i) {
			bool active = false;
			if (*selected == i)
				active = true;
			bool res = EguiToggle2((wzrd_box) { .w = 150, .h = 30, .border_type = BorderType_None, .color = EGUI_WHITE, .center = true },
				label_list.val[i], EGUI_DARKBLUE, active);

			if (res) {
				*selected = i;
			}
		}
	}
	wzrd_box_end();
}


