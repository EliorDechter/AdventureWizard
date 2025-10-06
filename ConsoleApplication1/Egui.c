#include "Egui.h"
#include "Strings.h"

Crate* EguiGetCurrentWindow() {
	Crate* result = &g_gui->crates_stack[g_gui->current_crate_index];

	return result;
}

Crate* EguiGetPreviousWindow() {
	assert(g_gui->current_crate_index > 0);
	Crate* result = &g_gui->crates_stack[g_gui->current_crate_index - 1];

	return result;
}

wzrd_box* wzrd_box_get_parent() {
	if (g_gui->crates_stack[g_gui->current_crate_index].box_stack_count < 2) return 0;
	int current_box_index = g_gui->crates_stack[g_gui->current_crate_index].box_stack_count - 2;
	int final_index = g_gui->crates_stack[g_gui->current_crate_index].box_stack[current_box_index];
	wzrd_box* result = &g_gui->boxes[final_index];

	return result;
}

wzrd_box* wzrd_box_get_current() {
	assert(g_gui->current_crate_index >= 0);
	int crate_index = g_gui->current_crate_index;

	int current_box_index = g_gui->crates_stack[crate_index].box_stack_count - 1;
	if (current_box_index < 0) return 0;
	int final_index = g_gui->crates_stack[crate_index].box_stack[current_box_index];
	assert(current_box_index >= 0);
	wzrd_box* result = &g_gui->boxes[final_index];

	return result;
}

int wzrd_box_get_current_index() {
	assert(g_gui->current_crate_index >= 0);
	int current_box_index = g_gui->crates_stack[g_gui->current_crate_index].box_stack_count - 1;
	if (current_box_index < 0) return 0;
	int final_index = g_gui->crates_stack[g_gui->current_crate_index].box_stack[current_box_index];
	int result = final_index;

	return result;
}

wzrd_box* wzrd_box_get_last() {
	wzrd_box* result = g_gui->boxes + (g_gui->boxes_count - 1);

	return result;
}

int CompareBoxes(const void* element1, const void* element2) {
	wzrd_box* c1 = (wzrd_box*)element1;
	wzrd_box* c2 = (wzrd_box*)element2;

	if (c1->z > c2->z) {
		return 1;
	}
	if (c1->z < c2->z) {
		return -1;
	}
	if (c1->z == c2->z) {
		if (c1->index > c2->index) return 1;
		if (c1->index < c2->index) return -1;
		return 0;
	}
}

void EguiBoxResize(wzrd_v2* size) {
	wzrd_box* box = wzrd_box_get_current();
	wzrd_box* previous_box = wzrd_box_get_parent();

	box->resizable = true;

	float mouse_delta_x = g_gui->mouse_pos.x - g_gui->previous_mouse_pos.x;
	float mouse_delta_y = g_gui->mouse_pos.y - g_gui->previous_mouse_pos.y;

	if (previous_box->row_mode) {
		if (str128_equal(box->name, g_gui->left_resized_item)) {
			if (size->x - mouse_delta_x >= 0)
				size->x -= mouse_delta_x;
		}

		else if (str128_equal(box->name, g_gui->right_resized_item)) {
			if (size->x + mouse_delta_x >= 0)
				size->x += mouse_delta_x;
		}
	}
	else {
		if (str128_equal(box->name, g_gui->top_resized_item)) {
			if (size->y - mouse_delta_y <= 0)
				size->y -= mouse_delta_y;
		}
		else if (str128_equal(box->name, g_gui->bottom_resized_item)) {
			if (size->y + mouse_delta_y >= 0)
				size->y += mouse_delta_y;
		}
	}

	wzrd_box_get_current()->w += size->x;
	wzrd_box_get_current()->h += size->y;
}

bool wzrd_box_begin(wzrd_box box) {

	assert(g_gui->boxes_count < MAX_NUM_BOXES - 1);

	box.z = EguiGetCurrentWindow()->z;

	if (box.w == 0 && !box.fit_w)
		box.grow_horizontal = true;
	if (box.h == 0 && !box.fit_h)
		box.grow_vertical = true;

	if (box.fit_w) {
		box.w += 4 * g_gui->line_size;
	}
	if (box.fit_h) {
		box.h += 4 * g_gui->line_size;
	}

	box.z = g_gui->current_crate_index;
	wzrd_box* current_box = wzrd_box_get_current();

	// Test for repeating id's
	if (box.name.len) {
		for (int i = 0; i < g_gui->boxes_count; ++i) {
			assert(!str128_equal(box.name, g_gui->boxes[i].name));
		}
	}
	else {
		if (!current_box) assert(0);
	}

	// Increment the panel index in the window's stack
	g_gui->total_num_panels++;

	box.window_index = g_gui->current_crate_index;

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
		current_box->children[current_box->children_count++] = g_gui->boxes_count;
	}

	// Default color 
	if (box.color.r == 0 && box.color.g == 0 && box.color.b == 0 && box.color.a == 0) {
		box.color = g_gui->default_color;
	}

	// Add box
	assert(g_gui->boxes_count < 256);
	box.index = g_gui->boxes_count;
	assert(g_gui->current_crate_index >= 0);
	g_gui->crates_stack[g_gui->current_crate_index].box_stack_count++;
	g_gui->crates_stack[g_gui->current_crate_index].box_stack[g_gui->crates_stack[g_gui->current_crate_index].box_stack_count - 1] = g_gui->boxes_count;
	g_gui->boxes[g_gui->boxes_count++] = box;

	// Return if clicked
	bool result = false;
	if (str128_equal(box.name, g_gui->clicked_item)) {
		result = true;
	}

	return result;
}

void wzrd_box_end() {
	g_gui->total_num_panels--;
	assert(g_gui->total_num_panels >= 0);

	wzrd_box* current_box = wzrd_box_get_current();
	Crate* current_crate = EguiGetCurrentWindow();

	//set to previous panel
	wzrd_box* previous_box = wzrd_box_get_parent();
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
	if (str128_equal(wzrd_box_get_current()->name, g_gui->clicked_item)) {
		return true;
	}

	return false;
}

bool IsHovered() {
	if (str128_equal(wzrd_box_get_current()->name, g_gui->hot_item)) {
		return true;
	}

	return false;
}

bool* EguiToggleBegin(wzrd_box box) {

	box.is_button = true;

	wzrd_box_begin(box);

	bool found = false;
	bool* toggle = 0;
	for (int i = 0; i < g_gui->toggles_count; ++i) {
		if (str128_equal(g_gui->toggles[i].name, wzrd_box_get_current()->name)) {
			toggle = &g_gui->toggles[i].val;
			found = true;
		}
	}

	if (!found) {
		g_gui->toggles[g_gui->toggles_count].name = wzrd_box_get_current()->name;
		g_gui->toggles[g_gui->toggles_count].val = false;
		toggle = &g_gui->toggles[g_gui->toggles_count].val;
		g_gui->toggles_count++;
	}

	if (str128_equal(wzrd_box_get_current()->name, g_gui->clicked_item)) {
		*toggle = !*toggle;
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

wzrd_box* EguiHotItemGet() {
	for (int i = 0; i < g_gui->boxes_count; ++i) {
		if (str128_equal(g_gui->boxes[i].name, g_gui->hot_item)) {
			return g_gui->boxes + i;
		}
	}

	return 0;
}

void wzrd_crate_begin(int window_id, wzrd_box box) {

	g_gui->total_num_windows++;
	g_gui->current_crate_index++;

	Crate* current_window = EguiGetCurrentWindow();

	// Set new window
	*current_window = (Crate){ .z = window_id, .index = g_gui->current_crate_index, };

	// Begin drawing panel

	box.is_crate = true;
	wzrd_box_begin(box);
}

void wzrd_crate_end() {

	// Count number of windows for debugging
	g_gui->total_num_windows--;

	wzrd_box_end();

	if (g_gui->current_crate_index > 0) {
		Crate* previous_window = EguiGetPreviousWindow();

		assert(previous_window->z != 0);
	}

	g_gui->current_crate_index--;
	assert(g_gui->current_crate_index >= -1);
}

void wzrd_crate(int window_id, wzrd_box box) {
	wzrd_crate_begin(window_id, box);
	wzrd_crate_end();
}

void wzrd_begin(Egui* gui, double time_in_ms, wzrd_v2 mouse_pos, wzrd_state mouse_left, wzrd_keyboard_keys keys, wzrd_v2 window_size, wzrd_icons icons, wzrd_color default_color, bool enable_input, unsigned int scale) {

	g_gui = gui;
	g_gui->enable_input = enable_input;
	g_gui->default_color = default_color;
	g_gui->icons = icons;
	g_gui->keyboard_keys = keys;
	g_gui->time = time_in_ms;
	g_gui->mouse_left = mouse_left;
	g_gui->mouse_pos = mouse_pos;
	g_gui->current_crate_index = -1;
	g_gui->boxes_count = 0;
	g_gui->window_width = window_size.x;
	g_gui->window_height = window_size.y;

	assert(scale);
	g_gui->scale = scale;

	g_gui->mouse_delta.x = g_gui->mouse_pos.x - g_gui->previous_mouse_pos.x;
	g_gui->mouse_delta.y = g_gui->mouse_pos.y - g_gui->previous_mouse_pos.y;


	// Move out of the function
	g_gui->line_size = 1;

	// Zero out stuff
	memset(g_gui->boxes, 0, sizeof(wzrd_box) * g_gui->boxes_count);

	static double time = 0;
	time += 16.7;

	// Begin drawing first window
	static wzrd_v2i pos;
	wzrd_crate_begin(
		CrateId_Screen,
		(wzrd_box) {
		.w = g_gui->window_width,
			.h = g_gui->window_height,
			.name = str128_create("Main window"),
			.pad_left = 5, .pad_top = 5,
			.pad_right = 5,
			.pad_bottom = 5,
			.child_gap = 5,
			.border_type = BorderType_None,
			.disable_input = true
	});

	g_gui->input_box_timer += 16.7;
}

void EguiRectDraw(wzrd_draw_commands_buffer* buffer, wzrd_rect rect, wzrd_color color) {

	EguiDrawCommand command = (EguiDrawCommand){
		.type = DrawCommandType_Rect,
		.dest_rect = rect,
		.color = color
	};

	/*assert(command.dest_rect.x >= 0);
	assert(command.dest_rect.y >= 0);
	assert(command.dest_rect.w >= 0);
	assert(command.dest_rect.h >= 0);*/
	/*assert(command.dest_rect.x + command.dest_rect.w <= g_gui->window_width);
	assert(command.dest_rect.y + command.dest_rect.h <= g_gui->window_height);*/

	assert(buffer->count < MAX_NUM_DRAW_COMMANDS - 1);
	buffer->commands[buffer->count++] = command;
}

wzrd_box* wzrd_box_get_by_name(str128 name) {
	if (!name.len) return 0;
	for (int i = 0; i < g_gui->boxes_count; ++i) {
		if (str128_equal(name, g_gui->boxes[i].name)) {
			return g_gui->boxes + i;
		}
	}

	return 0;
}

wzrd_box* wzrd_box_get_by_name_from_gui(Egui* gui, str128 name) {
	g_gui = gui;
	wzrd_box* result = wzrd_box_get_by_name(name);
	g_gui = 0;

	return result;
}

bool IsRectInsideRect(wzrd_rect a, wzrd_rect b) {
	bool res = a.x >= b.x;
	res &= a.x >= b.x;
	res &= a.x + a.w <= b.x + b.w;
	res &= a.y + a.h <= b.y + b.h;

	return res;
}

wzrd_box* wzrd_box_get_previous() {
	wzrd_box* result = &g_gui->boxes[g_gui->boxes_count - 1];

	return result;
}

void wzrd_end(wzrd_cursor* cursor, wzrd_draw_commands_buffer* buffer) {


	// Dragging
	if (g_gui->dragged_box.name.len)
	{
		g_gui->dragged_box.x += g_gui->mouse_delta.x;
		g_gui->dragged_box.y += g_gui->mouse_delta.y;

		wzrd_crate(1, g_gui->dragged_box);
	}

	wzrd_crate_end();

	// Test each opened panel has been closed
	if (g_gui->total_num_panels != 0) {
		assert(0);
	}

	assert(g_gui->total_num_windows == 0);

	// Calculate size
	for (int i = 0; i < g_gui->boxes_count; ++i) {
		wzrd_box* parent = &g_gui->boxes[i];

		assert(parent->w);
		assert(parent->h);

		parent->absolute_rect.w = parent->w;
		parent->absolute_rect.h = parent->h;

		float max_w = 0, max_h = 0;
		float children_h = 0, children_w = 0;
		for (int j = 0; j < parent->children_count; ++j) {
			wzrd_box* child = &g_gui->boxes[parent->children[j]];

			if (child->is_crate) continue;

			if (!child->grow_horizontal)
				children_w += child->w;

			if (!child->grow_vertical)
				children_h += child->h;
		}

		// Default resizing
		float available_w = parent->w - parent->pad_left - parent->pad_right - 4 * g_gui->line_size;
		float available_h = parent->h - parent->pad_top - parent->pad_bottom - 4 * g_gui->line_size;

		if (parent->children_count) {
			if (parent->row_mode)
				available_w -= parent->child_gap * (parent->children_count - 1);
			else
				available_h -= parent->child_gap * (parent->children_count - 1);
		}

		// Handle growing
		for (int j = 0; j < parent->children_count; ++j) {
			wzrd_box* child = &g_gui->boxes[parent->children[j]];

			if (child->grow_horizontal) {
				if (parent->row_mode) {
					child->w = available_w - children_w;
				}
				else {
					child->w = available_w;
				}

				assert(child->w > 0);
			}

			if (child->grow_vertical) {
				if (!parent->row_mode) {
					child->h = available_h - children_h;
				}
				else {
					child->h = available_h;
				}
			}

			if (child->best_fit)
			{
				int ratio_a = parent->w / parent->h;
				int ratio_b = child->w / child->h;
				int ratio = 0;

				if (ratio_a >= ratio_b)
				{
					ratio = parent->w / child->w;
				}
				else
				{
					ratio = parent->h / child->h;
				}

				child->w = child->w * ratio;
				child->h = child->h * ratio;
			}

			child->absolute_rect.w = child->w;
			child->absolute_rect.h = child->h;

			assert(child->absolute_rect.w > 0);
			assert(child->absolute_rect.h > 0);
		}

		// assert
		{
			float w = 0, h = 0;
			for (int j = 0; j < parent->children_count; ++j) {
				wzrd_box* child = &g_gui->boxes[parent->children[j]];
				if (parent->row_mode)
					w += child->absolute_rect.w;
				else
					h += child->absolute_rect.h;
			}
			//assert(w <= owner->absolute_rect.w);
			//assert(h <= owner->absolute_rect.h);
		}

	}

	// Calculate positions
	for (int i = 0; i < g_gui->boxes_count; ++i) {
		wzrd_box* owner = &g_gui->boxes[i];

		if (owner->is_crate) {
			if (owner->parent) {
				owner->x += g_gui->boxes[owner->parent].x;
				owner->y += g_gui->boxes[owner->parent].y;

			}
			else {
				//owner->x += owner->x;
				//owner->y += owner->y;
			}

			owner->absolute_rect.x = owner->x;
			owner->absolute_rect.y = owner->y;
		}

		float x = owner->x + owner->pad_left, y = owner->y + owner->pad_top;

		x += 2 * g_gui->line_size;
		y += 2 * g_gui->line_size;

		// Center
		float w = 0, h = 0, max_w = 0, max_h = 0;
		for (int i = 0; i < owner->children_count; ++i) {
			wzrd_box* child = &g_gui->boxes[owner->children[i]];

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
			x += (owner->w - 4 * g_gui->line_size - owner->pad_left - owner->pad_right) / 2 - w / 2;
		}
		if (owner->center_y && !owner->row_mode) {
			y += (owner->h - 4 * g_gui->line_size - owner->pad_top - owner->pad_bottom) / 2 - h / 2;
		}

		// Calc positions
		for (int j = 0; j < owner->children_count; ++j) {
			wzrd_box* child = &g_gui->boxes[owner->children[j]];

			child->x += x;
			child->y += y;

			if (owner->center_y && owner->row_mode) {
				child->y += (owner->h - 4 * g_gui->line_size - owner->pad_top - owner->pad_bottom) / 2 - child->h / 2;
			}
			if (owner->center_x && !owner->row_mode) {
				child->x += (owner->w - 4 * g_gui->line_size - owner->pad_top - owner->pad_bottom) / 2 - child->w / 2;
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

		}
	}

	for (int i = 0; i < g_gui->boxes_count; ++i) {
		for (int j = 0; j < g_gui->boxes[i].children_count; ++j) {
			wzrd_box* owner = g_gui->boxes + i;
			wzrd_box* child = g_gui->boxes + g_gui->boxes[i].children[j];
			if (!IsRectInsideRect(child->absolute_rect, owner->absolute_rect)) {
				owner->color = EGUI_ORANGE;
				child->color = EGUI_RED;
			}
		}
	}

	// Mouse interaction
	if (g_gui->enable_input)
	{
		*cursor = wzrd_cursor_default;
		wzrd_box* hovered_box = 0;
		int max_z = 0;
		for (int i = 0; i < g_gui->boxes_count; ++i) {
			wzrd_box* box = g_gui->boxes + i;

			wzrd_rect scaled_rect = { box->absolute_rect.x * g_gui->scale, box->absolute_rect.y * g_gui->scale, box->absolute_rect.w * g_gui->scale, box->absolute_rect.h * g_gui->scale };

			/*if (g_gui->scale > 1)
				printf("%f %f | %f %f %f %f\n", g_gui->mouse_pos.x , g_gui->mouse_pos.y , scaled_rect.x, scaled_rect.y, scaled_rect.w, scaled_rect.h);*/

			bool is_hover = wzrd_v2_is_inside_rect((wzrd_v2) { g_gui->mouse_pos.x , g_gui->mouse_pos.y },
				scaled_rect);

			if (is_hover && !box->disable_hover)
			{
				if (box->z >= max_z)
				{
					hovered_box = box;
					max_z = box->z;
				}
			}
		}

		// ...
		wzrd_box* half_clicked_box = wzrd_box_get_by_name(g_gui->half_clicked_item);
		if (half_clicked_box && g_gui->mouse_left == EguiActive)
		{
			g_gui->half_clicked_item = (str128){ 0 };
		}

		wzrd_box* hot_box = wzrd_box_get_by_name(g_gui->hot_item);
		wzrd_box* active_box = wzrd_box_get_by_name(g_gui->active_item);

		if (g_gui->mouse_left == EguiDeactivating)
		{
			g_gui->released_item = g_gui->dragged_item;
			g_gui->dragged_box = (wzrd_box){ 0 };
			g_gui->dragged_item = (str128){ 0 };
		}

		if (g_gui->mouse_left == EguiInactive)
		{
			g_gui->released_item = (str128){ 0 };
		}

		if (active_box) {
			if (g_gui->mouse_left == EguiDeactivating) {
				if (hot_box == active_box) {
					g_gui->clicked_item = active_box->name;
				}
				g_gui->active_item = (str128){ 0 };
			}

			//if (active_box->is_button || active_box->flat_button) {
				//*is_interacting = true;
			//}
		}
		else {
			//*is_interacting = false;
		}

		g_gui->is_interacting = false;
		g_gui->is_hovering = false;
		if (hot_box) {
			if (!hot_box->disable_input) {
				g_gui->is_hovering = true;
			}

		}
		if (active_box) {
			if (!active_box->disable_input) {
				g_gui->is_interacting = true;
			}
		}

		if (hot_box) {
			if (hot_box->flat_button)
				hot_box->color = (wzrd_color){ 0, 255, 255, 255 };
			if (g_gui->mouse_left == EguiActivating) {
				g_gui->active_item = hot_box->name;
				
				g_gui->half_clicked_item = hot_box->name;


				// Dragging
				wzrd_box *half_clicked_box = wzrd_box_get_by_name(g_gui->half_clicked_item);
				if (half_clicked_box->is_draggable) {
					g_gui->dragged_item = half_clicked_box->name;

					g_gui->dragged_box = *half_clicked_box;
					g_gui->dragged_box.x = g_gui->dragged_box.absolute_rect.x;
					g_gui->dragged_box.y = g_gui->dragged_box.absolute_rect.y;
					g_gui->dragged_box.w = g_gui->dragged_box.absolute_rect.w;
					g_gui->dragged_box.h = g_gui->dragged_box.absolute_rect.h;
					g_gui->dragged_box.name = str128_create("drag");
					g_gui->dragged_box.absolute_rect = (wzrd_rect){ 0 };
					g_gui->dragged_box.disable_hover = true;
				}
			}
		}

		if (hovered_box) {
			g_gui->hot_item = hovered_box->name;
		}
		else {
			g_gui->hot_item = (str128){ 0 };
		}

		// Cursor 
		if (g_gui->is_hovering) {
			*cursor = DrawCommandType_Default;

			wzrd_box* hot_box = wzrd_box_get_by_name(g_gui->hot_item);
			wzrd_box* active_box = wzrd_box_get_by_name(g_gui->active_item);

			if (hot_box) {
				if (hot_box->is_button) {
					*cursor = wzrd_cursor_hand;
				}
			}
			if (active_box) {
				if (active_box->is_button) {
					*cursor = wzrd_cursor_hand;
				}
			}
		}

		// stuff
		if (g_gui->clicked_item.len) {
			wzrd_box* clicked_box = wzrd_box_get_by_name(g_gui->clicked_item);
			assert(clicked_box);
			if (clicked_box->is_input_box) {
				g_gui->active_input_box = clicked_box->name;
			}
			else {
				g_gui->active_input_box = (str128){ 0 };
			}
		}

		if (g_gui->mouse_left == EguiInactive) {
			g_gui->clicked_item = (str128){ 0 };
		}

		// Border resize
		{
			g_gui->left_resized_item = (str128){ 0 };
			g_gui->right_resized_item = (str128){ 0 };
			g_gui->top_resized_item = (str128){ 0 };
			g_gui->bottom_resized_item = (str128){ 0 };

			for (int i = 0; i < g_gui->boxes_count; ++i) {
				wzrd_box* owner = g_gui->boxes + i;
				for (int j = 0; j < owner->children_count; ++j) {
					wzrd_box* child = &g_gui->boxes[owner->children[j]];

					float border_size = 2;

					if (!child->resizable) continue;

					bool is_inside_left_border =
						g_gui->mouse_pos.x >= child->absolute_rect.x &&
						g_gui->mouse_pos.y >= child->absolute_rect.y &&
						g_gui->mouse_pos.x < child->absolute_rect.x + border_size &&
						g_gui->mouse_pos.y < child->absolute_rect.y + child->absolute_rect.h;
					bool is_inside_right_border =
						g_gui->mouse_pos.x >= child->absolute_rect.x + child->absolute_rect.w - border_size &&
						g_gui->mouse_pos.y >= child->absolute_rect.y &&
						g_gui->mouse_pos.x < child->absolute_rect.x + child->absolute_rect.w &&
						g_gui->mouse_pos.y < child->absolute_rect.y + child->absolute_rect.h;
					bool is_inside_top_border =
						g_gui->mouse_pos.x >= child->absolute_rect.x &&
						g_gui->mouse_pos.y >= child->absolute_rect.y &&
						g_gui->mouse_pos.x < child->absolute_rect.x + child->absolute_rect.w &&
						g_gui->mouse_pos.y < child->absolute_rect.y + border_size;
					bool is_inside_bottom_border =
						g_gui->mouse_pos.x >= child->absolute_rect.x &&
						g_gui->mouse_pos.y >= child->absolute_rect.y + child->absolute_rect.h - border_size &&
						g_gui->mouse_pos.x < child->absolute_rect.x + child->absolute_rect.w &&
						g_gui->mouse_pos.y < child->absolute_rect.y + child->absolute_rect.h;

					if (str128_equal(g_gui->hot_item, child->name) || str128_equal(g_gui->active_item, child->name)) {

						if (is_inside_top_border || is_inside_bottom_border) {
							*cursor = wzrd_cursor_vertical_arrow;
						}
						else if (is_inside_left_border || is_inside_right_border) {
							*cursor = wzrd_cursor_horizontal_arrow;
						}
					}

					if (str128_equal(g_gui->active_item, child->name)) {
						if (is_inside_bottom_border) {
							child->color = EGUI_PURPLE;
							g_gui->bottom_resized_item = child->name;
						}
						else if (is_inside_top_border) {
							child->color = EGUI_PURPLE;
							g_gui->top_resized_item = child->name;
						}
						else if (is_inside_left_border) {
							child->color = EGUI_PURPLE;
							g_gui->left_resized_item = child->name;
						}
						else if (is_inside_right_border) {
							child->color = EGUI_PURPLE;
							g_gui->right_resized_item = child->name;
						}
					}
				}
			}
		}
	}

	// Mouse position
	g_gui->previous_mouse_pos = g_gui->mouse_pos;

	// Exit early if window is unfocused
	/*if (!wzrd_v2_is_inside_rect((wzrd_v2) { g_gui->mouse_pos.x, g_gui->mouse_pos.y },
		(wzrd_rect) {
		0, 0, g_gui->window_width, g_gui->window_height
	})) {
		*is_hovered = false;
		g_gui = 0;
		return;
	}
	else {
		*is_hovered = true;
	}*/

	// Drawing
	wzrd_box boxes[MAX_NUM_BOXES] = { 0 };
	memcpy(boxes, g_gui->boxes, sizeof(wzrd_box) * g_gui->boxes_count);
	qsort(boxes, g_gui->boxes_count, sizeof(wzrd_box), CompareBoxes);

	buffer->count = 0;
	for (int i = 0; i < g_gui->boxes_count; ++i) {

		wzrd_box box = boxes[i];

		EguiRectDraw(buffer, (wzrd_rect) {
			.x = box.absolute_rect.x,
				.y = box.absolute_rect.y,
				.h = box.absolute_rect.h,
				.w = box.absolute_rect.w
		},
			box.color);

		// Borders (1215 x 810)
		int line_size = g_gui->line_size;

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
			EguiRectDraw(buffer, top0, EGUI_BLUE);
			EguiRectDraw(buffer, left0, EGUI_BLUE);
			EguiRectDraw(buffer, top1, EGUI_ORANGE);
			EguiRectDraw(buffer, left1, EGUI_ORANGE);

			// Draw bottom and right lines
			EguiRectDraw(buffer, bottom0, EGUI_GREEN);
			EguiRectDraw(buffer, right0, EGUI_GREEN);
			EguiRectDraw(buffer, bottom1, EGUI_PURPLE);
			EguiRectDraw(buffer, right1, EGUI_PURPLE);
		}
		else if (box.border_type == BorderType_Default) {
			// Draw top and left lines
			EguiRectDraw(buffer, top0, EGUI_WHITE2);
			EguiRectDraw(buffer, left0, EGUI_WHITE2);
			EguiRectDraw(buffer, top1, EGUI_LIGHTGRAY);
			EguiRectDraw(buffer, left1, EGUI_LIGHTGRAY);

			// Draw bottom and right lines
			EguiRectDraw(buffer, bottom0, EGUI_BLACK);
			EguiRectDraw(buffer, right0, EGUI_BLACK);
			EguiRectDraw(buffer, bottom1, EGUI_GRAY);
			EguiRectDraw(buffer, right1, EGUI_GRAY);
		}
		else if (box.border_type == BorderType_Clicked) {
			// Draw top and left lines
			EguiRectDraw(buffer, top0, EGUI_BLACK);
			EguiRectDraw(buffer, left0, EGUI_BLACK);
			EguiRectDraw(buffer, top1, EGUI_GRAY);
			EguiRectDraw(buffer, left1, EGUI_GRAY);

			// Draw bottom and right lines
			EguiRectDraw(buffer, bottom0, EGUI_WHITE2);
			EguiRectDraw(buffer, right0, EGUI_WHITE2);
			EguiRectDraw(buffer, bottom1, EGUI_LIGHTESTGRAY);
			EguiRectDraw(buffer, right1, EGUI_LIGHTESTGRAY);
		}
		else if (box.border_type == BorderType_InputBox) {
			// Draw top and left lines
			EguiRectDraw(buffer, top0, EGUI_GRAY);
			EguiRectDraw(buffer, left0, EGUI_GRAY);
			EguiRectDraw(buffer, top1, EGUI_BLACK);
			EguiRectDraw(buffer, left1, EGUI_BLACK);

			// Draw bottom and right lines
			EguiRectDraw(buffer, bottom0, EGUI_WHITE2);
			EguiRectDraw(buffer, right0, EGUI_WHITE2);
			EguiRectDraw(buffer, bottom1, EGUI_LIGHTESTGRAY);
			EguiRectDraw(buffer, right1, EGUI_LIGHTESTGRAY);
		}
		else if (box.border_type == BorderType_Black) {
			// Draw top and left lines
			EguiRectDraw(buffer, top0, EGUI_BLACK);
			EguiRectDraw(buffer, left0, EGUI_BLACK);
			EguiRectDraw(buffer, top1, EGUI_WHITE2);
			EguiRectDraw(buffer, left1, EGUI_WHITE2);

			// Draw bottom and right lines
			EguiRectDraw(buffer, bottom0, EGUI_BLACK);
			EguiRectDraw(buffer, right0, EGUI_BLACK);
			EguiRectDraw(buffer, bottom1, EGUI_WHITE2);
			EguiRectDraw(buffer, right1, EGUI_WHITE2);
		}
		else if (box.border_type == BorderType_BottomLine) {
			EguiRectDraw(buffer, bottom0, EGUI_WHITE2);
			EguiRectDraw(buffer, bottom1, EGUI_GRAY);
		}
		else if (box.border_type == BorderType_LeftLine) {
			EguiRectDraw(buffer, left0, EGUI_GRAY);
			EguiRectDraw(buffer, left1, EGUI_WHITE2);
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

			//assert(command.dest_rect.x >= 0);
			//assert(command.dest_rect.y >= 0);
			assert(command.dest_rect.w >= 0);
			assert(command.dest_rect.h >= 0);

			assert(buffer->count < MAX_NUM_DRAW_COMMANDS - 1);
			buffer->commands[buffer->count++] = command;
		}
	}

	g_gui = 0;
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

wzrd_rect wzrd_box_get_rect(wzrd_box* box) {
	assert(box);

	return box->absolute_rect;
}

bool IsActive() {
	if (str128_equal(wzrd_box_get_current()->name, g_gui->active_item)) {
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
		g_gui->boxes[g_gui->boxes_count - 1].color = EGUI_DARKBLUE;
		g_gui->boxes[g_gui->boxes_count - 2].color = EGUI_DARKBLUE;

	}

	return result;
}

bool EguiButtonRawBegin(wzrd_box box) {
	bool result = false;
	box.is_button = true;
	wzrd_box_begin(box);

	if (str128_equal(wzrd_box_get_current()->name, g_gui->clicked_item)) {
		result = true;
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
	wzrd_v2i size = { FONT_WIDTH * str.len, WZRD_FONT_HEIGHT };

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
	if (str128_equal(wzrd_box_get_current()->name, g_gui->active_item)) {
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

			if (str128_equal(wzrd_box_get_current()->name, g_gui->active_input_box)) {

				for (int i = 0; i < g_gui->keyboard_keys.count; ++i) {
					wzrd_keyboard_key key = g_gui->keyboard_keys.keys[i];

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

				if (g_gui->input_box_timer - g_gui->time > 500) {
					g_gui->input_box_timer = 0;
					show_caret = !show_caret;
				}

				if (show_caret) {
					str2 = *str;
					str128_concat(&str2, "|");
				}

			}

			/*if (str128_equal(g_gui->active_input_box, wzrd_box_get_current()->name)) {
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
	wzrd_v2i v = { FONT_WIDTH * str.len, WZRD_FONT_HEIGHT };
	bool result = wzrd_box_begin((wzrd_box) {
		.border_type = BorderType_None,
			.color = wzrd_box_get_current()->color,
			.w = v.x,
			.h = v.y,
			.flat_button = true,
			.is_button = true
			//.center = true,
	});
	{
		wzrd_item_add((Item) { .type = wzrd_item_type_str, .str = str128_create(str.val) });
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

	wzrd_crate_begin(1, (wzrd_box) { .parent = parent, .x = pos->x, .y = pos->y, .w = size.x, .h = size.y, .name = name });

	bool close = false;
	wzrd_box_begin((wzrd_box) { .border_type = BorderType_None, .h = 28, .row_mode = true });
	{
		wzrd_box_begin((wzrd_box) {
			.border_type = BorderType_None,
				.color = (wzrd_color){ 57, 77, 205 }
		});
		{
			if (str128_equal(wzrd_box_get_current()->name, g_gui->active_item)) {
				pos->x += g_gui->mouse_pos.x - g_gui->previous_mouse_pos.x;
				pos->y += g_gui->mouse_pos.y - g_gui->previous_mouse_pos.y;
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
					wzrd_item_add((Item) { .texture = g_gui->icons.close, .type = ItemType_Texture });
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
		wzrd_crate_end();
	}
}

void EguiDialogEnd(bool active) {
	if (active)
		wzrd_crate_end();
}

int wzrd_dropdown(int* selected_text, const str128* texts, int texts_count, int w, bool* active) {
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
				.h = WZRD_FONT_HEIGHT + 4 * g_gui->line_size
		};

		wzrd_box b = box;

		int parent = 0;
		EguiButton2(box, texts[*selected_text], EGUI_DARKBLUE);

		parent = g_gui->boxes_count - 1;

		box.border_type = BorderType_None;

		//bool* toggle2;

		bool button = wzrd_button_icon2((wzrd_box) { .w = 20, .h = WZRD_FONT_HEIGHT + 4 * g_gui->line_size, .center = true }, g_gui->icons.dropdown);
		if (button) {
			*active = !*active;
		}

		if (*active) {
			char str[128];
			sprintf(str, "%s-dropdown", wzrd_box_get_current()->name.val);
			static wzrd_v2i pos;

			wzrd_crate_begin(2, (wzrd_box) {
				.parent = parent,
					.y = box.h,
					.w = box.w + 4 * g_gui->line_size,
					.h = box.h * texts_count + 4 * g_gui->line_size,
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
			wzrd_crate_end();
		}
	}
	wzrd_box_end();
}

bool EguiButton(str128 str) {
	wzrd_v2i v = { FONT_WIDTH * str.len + 12, WZRD_FONT_HEIGHT + 12 };
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

bool wzrd_game_buttonesque(wzrd_v2 pos, wzrd_v2 size, wzrd_color color) {
	bool result = false;
	bool active = false;

	wzrd_crate_begin(1, (wzrd_box) { .is_button = true, .color = color, .border_type = BorderType_None, .x = pos.x, .y = pos.y, .w = size.x, .h = size.y, .name = str128_create("wowy") });
	{
		result = IsActive();
	}
	wzrd_crate_end();

	return result;
}

void wzrd_slot() {

}

void wzrd_drag(wzrd_box box2, wzrd_v2* pos, bool* drag) {

	if (g_gui->mouse_left == EguiInactive)
	{
		*drag = false;
	}

	if (!(*drag)) return;


	g_gui->dragged_box.x += g_gui->mouse_delta.x;
	g_gui->dragged_box.y += g_gui->mouse_delta.y;

	wzrd_crate(1, g_gui->dragged_box);
}

bool wzrd_box_is_active(wzrd_box *box) {
	if (str128_equal(box->name, g_gui->active_item)) {
		return true;
	}

	return false;
}

bool wzrd_box_is_half_clicked(wzrd_box* box) {
	if (str128_equal(box->name, g_gui->half_clicked_item)) {
		return true;
	}

	return false;
}

bool wzrd_box_is_dragged(wzrd_box* box) {
	if (str128_equal(box->name, g_gui->dragged_item)) {
		return true;
	}

	return false;
}

bool wzrd_box_is_hot(wzrd_box* box) {
	if (str128_equal(box->name, g_gui->hot_item)) {
		return true;
	}

	return false;
}

wzrd_box* wzrd_box_get_released()
{
	wzrd_box* result = 0;

	if (g_gui->released_item.len)
	{
		result = wzrd_box_get_by_name(g_gui->released_item);
	}

	return result;
}

bool wzrd_is_releasing() {
	bool result = false;
	if (g_gui->released_item.len)
	{
		result = true;
	}

	return result;
}

wzrd_v2 wzrd_lerp(wzrd_v2 pos, wzrd_v2 end_pos) {
	float lerp_amount = 0.2;
	float delta = 0.1;
	if (abs(end_pos.x - pos.x) > delta) {
		pos.x = pos.x + lerp_amount * (end_pos.x - pos.x);
	}

	if (abs(end_pos.y - pos.y) > delta) {
		pos.y = pos.y + lerp_amount * (end_pos.y - pos.y);
	}

	return pos;
}

