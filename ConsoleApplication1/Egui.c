// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#define WZRD_ASSERT(x) assert(x)
//#define WZRD_ASSERT(x) (void)(x)
#define WZRD_UNUSED(x) (void)x

#include "Egui.h"
#include "Strings.h"

static wzrd_gui* g_gui;
static wzrd_input g_wzrd_input;

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

void wzrd_update_layers(wzrd_v2 screen_mouse_pos, wzrd_state mouse_state)
{

	g_wzrd_input.hovered_layer = -1;
	for (int layer = 0; layer < 32; ++layer)
	{
		if (wzrd_v2_is_inside_rect(screen_mouse_pos, g_wzrd_input.layers[layer]) && layer > g_wzrd_input.hovered_layer)
		{
			g_wzrd_input.hovered_layer = layer;
		}
	}

	if (mouse_state == WZRD_ACTIVE && g_wzrd_input.active_layer == -1)
	{
		g_wzrd_input.active_layer = g_wzrd_input.hovered_layer;
	}

	if (mouse_state == WZRD_INACTIVE)
	{
		g_wzrd_input.active_layer = -1;
	}

	printf("%d %d\n", g_wzrd_input.hovered_layer, g_wzrd_input.active_layer);

}

Crate* EguiGetCurrentWindow() {
	Crate* result = &g_gui->crates_stack[g_gui->current_crate_index];

	return result;
}

Crate* EguiGetPreviousWindow() {
	WZRD_ASSERT(g_gui->current_crate_index > 0);
	Crate* result = &g_gui->crates_stack[g_gui->current_crate_index - 1];

	return result;
}

wzrd_box* wzrd_box_get_parent() {
	if (g_gui->crates_stack[g_gui->current_crate_index].box_stack_count < 2)
	{
		return &g_gui->boxes[0];
	}
	int current_box_index = g_gui->crates_stack[g_gui->current_crate_index].box_stack_count - 2;
	int final_index = g_gui->crates_stack[g_gui->current_crate_index].box_stack[current_box_index];
	wzrd_box* result = &g_gui->boxes[final_index];

	return result;
}

wzrd_box* wzrd_box_get_from_top_of_stack() {
	WZRD_ASSERT(g_gui->current_crate_index >= 0);
	int crate_index = g_gui->current_crate_index;

	int current_box_index = g_gui->crates_stack[crate_index].box_stack_count - 1;
	if (current_box_index < 0)
	{
		return &g_gui->boxes[0];
	}
	int final_index = g_gui->crates_stack[crate_index].box_stack[current_box_index];
	wzrd_box* result = &g_gui->boxes[final_index];

	WZRD_ASSERT(result);

	return result;
}

int wzrd_box_get_current_index() {
	WZRD_ASSERT(g_gui->current_crate_index >= 0);
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


void wzrd_text_add(str128 str)
{
	wzrd_item_add((Item) { .type = wzrd_item_type_str, .val = { .str = str128_create(str.val) }, .color = g_gui->style.font_color });
}

int CompareBoxes2(const void* element1, const void* element2) {
	wzrd_box* c1 = g_gui->boxes + *(int*)element1;
	wzrd_box* c2 = g_gui->boxes + *(int*)element2;

	if (c1->z > c2->z) {
		return 1;
	}
	if (c1->z < c2->z) {
		return -1;
	}
	//if (c1->z == c2->z) 
	{
		if (c1->index > c2->index) return 1;
		if (c1->index < c2->index) return -1;
		return 0;
	}
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
	//if (c1->z == c2->z)
	{
		if (c1->index > c2->index) return 1;
		if (c1->index < c2->index) return -1;
		return 0;
	}
}

void EguiBoxResize(wzrd_v2* size) {
	wzrd_box* box = wzrd_box_get_from_top_of_stack();
	wzrd_box* previous_box = wzrd_box_get_parent();

	box->resizable = true;

	float mouse_delta_x = g_wzrd_input.mouse_pos.x - g_wzrd_input.previous_mouse_pos.x;
	float mouse_delta_y = g_wzrd_input.mouse_pos.y - g_wzrd_input.previous_mouse_pos.y;

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

	wzrd_box_get_from_top_of_stack()->w += size->x;
	wzrd_box_get_from_top_of_stack()->h += size->y;
}

int wzrd_float_compare(float a, float b)
{
	if (a - b < FLT_EPSILON)
	{
		return 0;
	}
	if (a - b > 0)
	{
		return 1;
	}
	else if (a - b < 0)
	{
		return -1;
	}

	return 0;
}

bool wzrd_box_begin(wzrd_box box) {


	WZRD_ASSERT(g_gui->boxes_count < MAX_NUM_BOXES - 1);

	box.z = EguiGetCurrentWindow()->layer;

	if (wzrd_float_compare(box.w, 0) == 0 && !box.fit_w)
		box.grow_horizontal = true;
	if (wzrd_float_compare(box.h, 0) == 0 && !box.fit_h)
		box.grow_vertical = true;

	if (box.fit_w) {
		box.w += 4 * WZRD_BORDER_SIZE;
	}
	if (box.fit_h) {
		box.h += 4 * WZRD_BORDER_SIZE;
	}

	wzrd_box* current_box = wzrd_box_get_from_top_of_stack();

	// Test for repeating id's
	if (box.name.len) {
		for (int i = 0; i < g_gui->boxes_count; ++i) {
			WZRD_ASSERT(!str128_equal(box.name, g_gui->boxes[i].name));
		}
	}

	// Increment the panel index in the window's stack
	g_gui->total_num_panels++;

	box.window_index = g_gui->current_crate_index;

	if (current_box->name.len) {

		//Name
		if (!box.name.len) {
			WZRD_ASSERT(*current_box->name.val);
			/*int n = snprintf(box.name.val, 128, "%s-%d", current_box->name.val, current_box->children_count);
			box.name.len = strlen(box.name.val);*/
			box.name = str128_create("%s-%d", current_box->name.val, current_box->children_count);
			//WZRD_ASSERT(n <= 128);
		}

		// Children
		WZRD_ASSERT(current_box->children_count < MAX_NUM_CHILDREN);
		current_box->children[current_box->children_count++] = g_gui->boxes_count;
	}

	// Default color 
	if (box.color.r == 0 && box.color.g == 0 && box.color.b == 0 && box.color.a == 0) {
		box.color = g_gui->style.background_color;
	}

	// Add box
	if (!memcmp(&g_gui->boxes[g_gui->boxes_count], &box, sizeof(wzrd_box)))
	{
		g_gui->clean = false;
	}

	WZRD_ASSERT(g_gui->boxes_count < 256);
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
	WZRD_ASSERT(g_gui->total_num_panels >= 0);

	wzrd_box* current_box = wzrd_box_get_from_top_of_stack();
	Crate* current_crate = EguiGetCurrentWindow();

	//set to previous panel
	wzrd_box* previous_box = wzrd_box_get_parent();
	if (previous_box) {

		// Handle fitting
		if (previous_box->fit_h) {
			WZRD_ASSERT(current_box->h);
			WZRD_ASSERT(current_box->w);

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
			WZRD_ASSERT(current_box->h);
			WZRD_ASSERT(current_box->w);

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
	WZRD_ASSERT(current_crate->box_stack_count >= 0);

}

bool wzrd_box_do(wzrd_box box) {
	bool result = wzrd_box_begin(box);
	wzrd_box_end();

	return result;
}

bool IsClicked() {
	if (str128_equal(wzrd_box_get_from_top_of_stack()->name, g_gui->clicked_item)) {
		return true;
	}

	return false;
}

bool IsHovered() {
	if (str128_equal(wzrd_box_get_from_top_of_stack()->name, g_gui->hot_item)) {
		return true;
	}

	return false;
}

void wzrd_texture_add(wzrd_texture texture, wzrd_v2 size) {
	wzrd_item_add((Item) {
		.type = ItemType_Texture,
			.size = size,
			.val = { .texture = texture }
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

void wzrd_crate_begin(int layer, wzrd_box box) {

	box.is_crate = true;
	g_gui->current_crate_name = box.name;
	g_gui->total_num_windows++;
	g_gui->current_crate_index++;

	Crate* current_window = EguiGetCurrentWindow();

	// Set new window
	*current_window = (Crate){ .layer = layer, .index = g_gui->current_crate_index, };

	// Begin drawing panel
	box.layer = layer;
	wzrd_layer_add(layer, (wzrd_rect) { box.x, box.y, box.w, box.h });
	wzrd_box_begin(box);
}

void wzrd_crate_end() {

	// Count number of windows for debugging
	g_gui->total_num_windows--;

	wzrd_box_end();

	if (g_gui->current_crate_index > 0) {
		//Crate* previous_window = EguiGetPreviousWindow();

		//WZRD_ASSERT(previous_window->layer != 0);
	}

	g_gui->current_crate_index--;
	WZRD_ASSERT(g_gui->current_crate_index >= -1);
}

void wzrd_crate(int window_id, wzrd_box box) {
	wzrd_crate_begin(window_id, box);
	wzrd_crate_end();
}

void wzrd_begin_ex(wzrd_gui* gui, wzrd_box box, wzrd_style style, int layer)
{
	if (layer < g_wzrd_input.hovered_layer && layer < g_wzrd_input.active_layer)
	{
		g_wzrd_input.mouse_pos = (wzrd_v2){ -1, -1 };
	}

	WZRD_UNUSED(layer);

	g_gui = gui;

	g_gui->current_crate_index = -1;
	g_gui->boxes_count = 0;
	g_gui->style = style;
	g_gui->layer = layer;
	g_gui->input_box_timer += 16.7f;

	g_gui->boxes[g_gui->boxes_count++] = (wzrd_box){ 0 };

	// Begin drawing first window
	//static wzrd_v2i pos;
	wzrd_crate_begin(
		layer,
		box);
}

void wzrd_update_input(wzrd_v2 mouse_pos, wzrd_state mouse_left, wzrd_keyboard_keys keys)
{
	g_wzrd_input.keyboard_keys = keys;
	g_wzrd_input.mouse_left = mouse_left;
	g_wzrd_input.mouse_pos = mouse_pos;

	g_wzrd_input.mouse_delta.x = g_wzrd_input.mouse_pos.x - g_wzrd_input.previous_mouse_pos.x;
	g_wzrd_input.mouse_delta.y = g_wzrd_input.mouse_pos.y - g_wzrd_input.previous_mouse_pos.y;

	if (mouse_left == WZRD_INACTIVE)
	{
		//g_wzrd_input.disable_input = true;
	}
	else
	{
		//g_wzrd_input.disable_input = false;
	}

}

wzrd_style wzrd_style_get_default()
{
	wzrd_style result =
	{
		.background_color = EGUI_LIGHTGRAY,
		.font_color = EGUI_BLACK,
		.window_border_type = BorderType_None
	};

	return result;
}

void wzrd_layer_add(unsigned int layer, wzrd_rect size)
{
	g_wzrd_input.layers[layer] = size;
}

void wzrd_begin(wzrd_gui* gui, wzrd_rect window, wzrd_style style, void (*get_string_size)(char*, float*, float*), int layer)
{

	//wzrd_layer_add(layer, window);


	gui->get_string_size = get_string_size;

	wzrd_box box =
		(wzrd_box){
		.x = window.x,
		.y = window.y,
		.w = window.w,
		.h = window.h,
		.name = str128_create("Main window"),
		.pad_left = 5, .pad_top = 5,
		.pad_right = 5,
		.pad_bottom = 5,
		.child_gap = 5,
		.border_type = style.window_border_type,
		.disable_input = true,
		.color = style.background_color
	};

	wzrd_begin_ex(gui, box, style, layer);
}

void EguiRectDraw(wzrd_draw_commands_buffer* buffer, wzrd_rect rect, wzrd_color color, int z) {

	wzrd_draw_command command = (wzrd_draw_command){
		.type = DrawCommandType_Rect,
		.dest_rect = rect,
		.color = color,
		.z = z
	};

	WZRD_ASSERT(buffer->count < MAX_NUM_DRAW_COMMANDS - 1);
	buffer->commands[buffer->count++] = command;



}

wzrd_box* wzrd_box_get_by_name(str128 name) {
	if (!name.len)
	{
		return g_gui->boxes;
	}

	for (int i = 0; i < g_gui->boxes_count; ++i) {
		if (str128_equal(name, g_gui->boxes[i].name)) {
			return g_gui->boxes + i;
		}
	}

	return g_gui->boxes;

}

wzrd_box* wzrd_box_get_by_name_from_gui(wzrd_gui* gui, str128 name) {
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

void wzrd_handle_cursor(wzrd_cursor* cursor)
{
	*cursor = wzrd_cursor_default;

	//if (g_gui->is_hovering)
	{

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
}

void wzrd_handle_border_resize(wzrd_cursor* cursor)
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
				g_wzrd_input.mouse_pos.x >= child->x &&
				g_wzrd_input.mouse_pos.y >= child->y &&
				g_wzrd_input.mouse_pos.x < child->x + border_size &&
				g_wzrd_input.mouse_pos.y < child->y + child->h;
			bool is_inside_right_border =
				g_wzrd_input.mouse_pos.x >= child->x + child->w - border_size &&
				g_wzrd_input.mouse_pos.y >= child->y &&
				g_wzrd_input.mouse_pos.x < child->x + child->w &&
				g_wzrd_input.mouse_pos.y < child->y + child->h;
			bool is_inside_top_border =
				g_wzrd_input.mouse_pos.x >= child->x &&
				g_wzrd_input.mouse_pos.y >= child->y &&
				g_wzrd_input.mouse_pos.x < child->x + child->w &&
				g_wzrd_input.mouse_pos.y < child->y + border_size;
			bool is_inside_bottom_border =
				g_wzrd_input.mouse_pos.x >= child->x &&
				g_wzrd_input.mouse_pos.y >= child->y + child->h - border_size &&
				g_wzrd_input.mouse_pos.x < child->x + child->w &&
				g_wzrd_input.mouse_pos.y < child->y + child->h;

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

void wzrd_handle_input()
{

	wzrd_box* hovered_box = 0;
	int max_z = 0;
	for (int i = 0; i < g_gui->boxes_count; ++i) {
		wzrd_box* box = g_gui->boxes + i;

		wzrd_rect scaled_rect = { box->x, box->y, box->w, box->h };

		bool is_hover = wzrd_v2_is_inside_rect((wzrd_v2) { g_wzrd_input.mouse_pos.x, g_wzrd_input.mouse_pos.y },
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
	if (half_clicked_box && g_wzrd_input.mouse_left == WZRD_ACTIVE)
	{
		g_gui->half_clicked_item = (str128){ 0 };
	}

	wzrd_box* hot_box = wzrd_box_get_by_name(g_gui->hot_item);
	wzrd_box* active_box = wzrd_box_get_by_name(g_gui->active_item);

	if (g_wzrd_input.mouse_left == EguiDeactivating)
	{
		g_gui->released_item = g_gui->dragged_item;
		g_gui->dragged_box = (wzrd_box){ 0 };
		g_gui->dragged_item = (str128){ 0 };

		g_gui->clean = false;
	}

	if (g_wzrd_input.mouse_left == WZRD_INACTIVE)
	{
		g_gui->released_item = (str128){ 0 };
	}

	if (active_box->name.len) {

		if (active_box->is_crate)
		{
			g_wzrd_input.active_crate = active_box->name;
		}

		if (g_wzrd_input.mouse_left == EguiDeactivating) {
			if (hot_box == active_box) {
				g_gui->clicked_item = active_box->name;
			}
			g_gui->active_item = (str128){ 0 };
		}

		g_gui->clean = false;
	}

	//g_gui->is_hovering = false;
	if (hot_box->name.len) {
		if (!hot_box->disable_input) {
			//g_gui->is_hovering = true;
		}

	}
	if (active_box->name.len) {
		if (!active_box->disable_input) {
			//g_wzrd_input.active_layer = g_gui->layer;
		}
	}

	if (hot_box->name.len) {
		if (active_box->is_crate)
		{
			g_wzrd_input.active_crate = active_box->name;
		}
		if (hot_box->flat_button)
			hot_box->color = (wzrd_color){ 0, 255, 255, 255 };
		if (g_wzrd_input.mouse_left == EguiActivating) {
			g_gui->active_item = hot_box->name;

			g_gui->half_clicked_item = hot_box->name;
			g_gui->selected_item = g_gui->hot_item;

			// Dragging
			//wzrd_box* half_clicked_box = wzrd_box_get_by_name(g_gui->half_clicked_item);
			WZRD_ASSERT(half_clicked_box);
			if (half_clicked_box->is_draggable) {
				g_gui->dragged_item = half_clicked_box->name;

				g_gui->dragged_box = *half_clicked_box;
				//g_gui->dragged_box.x = g_gui->dragged_box.x;
				//g_gui->dragged_box.y = g_gui->dragged_box.y;
				//g_gui->dragged_box.w = g_gui->dragged_box.w;
				//g_gui->dragged_box.h = g_gui->dragged_box.h;
				g_gui->dragged_box.name = str128_create("drag");
				//g_gui->dragged_box.absolute_rect = (wzrd_rect){ 0 };
				g_gui->dragged_box.disable_hover = true;
			}
		}
	}

	if (hovered_box->name.val) {
		if (!str128_equal(hovered_box->name, g_gui->hot_item))
		{
			g_gui->clean = false;
		}

		g_gui->hot_item = hovered_box->name;
	}
	else {
		g_gui->hot_item = (str128){ 0 };
	}

	// stuff
	if (g_gui->clicked_item.len) {
		wzrd_box* clicked_box = wzrd_box_get_by_name(g_gui->clicked_item);
		WZRD_ASSERT(clicked_box);
		if (clicked_box->is_input_box) {
			g_gui->active_input_box = clicked_box->name;
		}
		else {
			g_gui->active_input_box = (str128){ 0 };
		}
	}

	if (g_gui->clicked_item.len && g_wzrd_input.mouse_left == WZRD_INACTIVE) {
		g_gui->clicked_item = (str128){ 0 };

		g_gui->clean = false;
	}

}

void wzrd_end(wzrd_cursor* cursor, wzrd_draw_commands_buffer* buffer) {

	// Dragging
	if (g_gui->dragged_box.name.len)
	{
		g_gui->dragged_box.x += g_wzrd_input.mouse_delta.x;
		g_gui->dragged_box.y += g_wzrd_input.mouse_delta.y;

		wzrd_crate(1, g_gui->dragged_box);
	}

	wzrd_crate_end();

	// Test each opened panel has been closed
	if (g_gui->total_num_panels != 0) {
		WZRD_ASSERT(0);
	}
	
	WZRD_ASSERT(g_gui->total_num_windows == 0);

	// Calculate size
	for (int i = 0; i < g_gui->boxes_count; ++i) {
		wzrd_box* parent = &g_gui->boxes[i];

		//float max_w = 0, max_h = 0;

		float children_h = 0, children_w = 0;
		for (int j = 0; j < parent->children_count; ++j) {
			wzrd_box* child = &g_gui->boxes[parent->children[j]];
			children_w += child->w;
			children_h += child->h;
		}

		// Default resizing
		float available_w = parent->w - parent->pad_left - parent->pad_right - 4 * WZRD_BORDER_SIZE;
		float available_h = parent->h - parent->pad_top - parent->pad_bottom - 4 * WZRD_BORDER_SIZE;

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

				WZRD_ASSERT(child->w > 0);
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
				float ratio_a = parent->w / parent->h;
				float ratio_b = child->w / child->h;
				int ratio = 0;

				//if (ratio_b >= ratio_a)
				if (wzrd_float_compare(ratio_b, ratio_a) >= 0)
				{
					ratio = (int)(parent->w / child->w);
				}
				else
				{
					ratio = (int)(parent->h / child->h);
				}

				child->w = child->w * ratio;
				child->h = child->h * ratio;


				WZRD_ASSERT(child->w <= parent->w);
				WZRD_ASSERT(child->h <= parent->h);

			}

			WZRD_ASSERT(child->w > 0);
			WZRD_ASSERT(child->h > 0);
		}
	}

	// Calculate content size
	for (int i = 0; i < g_gui->boxes_count; ++i)
	{
		wzrd_box* parent = g_gui->boxes + i;

		if (!parent->clip)
			continue;

		parent->content_size.x = 2 * WZRD_BORDER_SIZE;
		parent->content_size.y = 2 * WZRD_BORDER_SIZE;

		if (parent->row_mode)
		{
			parent->content_size.x += (parent->children_count + 1) * parent->child_gap;
			parent->content_size.y += parent->child_gap;
		}
		else
		{
			parent->content_size.y += (parent->children_count + 1) * parent->child_gap;
			parent->content_size.x += parent->child_gap;
		}

		float max_child_w = 0, max_child_h = 0;
		for (int j = 0; j < parent->children_count; ++j)
		{
			wzrd_box child = g_gui->boxes[parent->children[j]];

			if (parent->h > max_child_h)
			{
				max_child_h = child.h;
			}

			if (parent->w > max_child_w)
			{
				max_child_w = child.w;
			}

			if (parent->row_mode)
			{
				parent->content_size.x += child.w;
			}
			else
			{
				parent->content_size.y += child.h;
			}
		}

		if (parent->row_mode)
		{
			parent->content_size.y += max_child_h;
		}
		else
		{
			parent->content_size.x += max_child_w;
		}

		if (parent->clip)
		{
			float x = parent->x + parent->w - 40;
			float y = parent->y;
			const float button_height = 10.0f;

			// Gray Area
			wzrd_crate(2,
				(wzrd_box) {
				.x = x, .y = y, .w = 40,
					.h = parent->h, .name = str128_create("gray"),
					.border_type = BorderType_None,
					.color = EGUI_GRAY
			});

			// Top button
			wzrd_crate(2,
				(wzrd_box) {
				.x = x, .y = y, .w = 40,
					.h = button_height, .name = str128_create("top"),
			});

			// Bottom button
			wzrd_crate(2,
				(wzrd_box) {
				.x = x,
					.y = parent->y + parent->h - button_height,
					.w = 40,
					.h = button_height, .name = str128_create("bottom"), .color = EGUI_GRAY,
			});

			// TODO: handle x axis scrollbar
			if (parent->content_size.y > parent->h)
			{
				float h = (parent->h / parent->content_size.y) * (parent->h - 2 * button_height);

				wzrd_crate(2,
					(wzrd_box) {
					.x = x, .y = y + button_height + *parent->scrollbar_y, .w = 40,
						.h = h, .name = str128_create("scroly")
				});

				wzrd_box* box = wzrd_box_get_last();
				if (wzrd_box_is_active(box))
				{
					float new_pos = parent->y + button_height + *parent->scrollbar_y + g_wzrd_input.mouse_delta.y;
					if (new_pos >= parent->y + button_height && new_pos + box->h < parent->y + parent->h - button_height)
					{
						*parent->scrollbar_y += g_wzrd_input.mouse_delta.y;
					}
				}

				float ratio = *parent->scrollbar_y / (parent->h - 2 * button_height);
				parent->pad_top -= ratio * parent->content_size.y;
			}
			else
			{
				float h = parent->h - 2 * button_height;

				wzrd_crate(2,
					(wzrd_box) {
					.x = x, .y = parent->y + button_height + *parent->scrollbar_y, .w = 40,
						.h = h, .name = str128_create("scroly")
				});
			}
		}


	}



	// Calculate positions
	for (int i = 0; i < g_gui->boxes_count; ++i) {
		wzrd_box* parent = &g_gui->boxes[i];

		float x = parent->x + parent->pad_left, y = parent->y + parent->pad_top;

		x += 2 * WZRD_BORDER_SIZE;
		y += 2 * WZRD_BORDER_SIZE;

		// Center
		float w = 0, h = 0, max_w = 0, max_h = 0;
		for (int j = 0; j < parent->children_count; ++j) {
			wzrd_box* child = &g_gui->boxes[parent->children[j]];

			if (child->w > max_w)
				max_w = child->w;

			if (child->h > max_h)
				max_h = child->h;

			if (parent->row_mode)
				w += child->w;
			else
				h += child->h;
		}

		if (parent->row_mode)
			w += parent->child_gap * (parent->children_count - 1);
		else
			h += parent->child_gap * (parent->children_count - 1);

		if (parent->center) {
			parent->center_x = true;
			parent->center_y = true;
		}

		if (parent->center_x && parent->row_mode) {
			x += (parent->w - 4 * WZRD_BORDER_SIZE - parent->pad_left - parent->pad_right) / 2 - w / 2;
		}
		if (parent->center_y && !parent->row_mode) {
			y += (parent->h - 4 * WZRD_BORDER_SIZE - parent->pad_top - parent->pad_bottom) / 2 - h / 2;
		}

		// Calc positions
		for (int j = 0; j < parent->children_count; ++j) {
			wzrd_box* child = &g_gui->boxes[parent->children[j]];

			child->x += x;
			child->y += y;

			if (parent->center_y && parent->row_mode) {
				child->y += (parent->h - 4 * WZRD_BORDER_SIZE - parent->pad_top - parent->pad_bottom) / 2 - child->h / 2;
			}
			if (parent->center_x && !parent->row_mode) {
				child->x += (parent->w - 4 * WZRD_BORDER_SIZE - parent->pad_top - parent->pad_bottom) / 2 - child->w / 2;
			}

			if (parent->row_mode) {
				x += child->w;
				x += parent->child_gap;
			}
			else {
				y += child->h;
				y += parent->child_gap;
			}

		}
	}

	// Test child doesn't exceed parent's size
	for (int i = 0; i < g_gui->boxes_count; ++i) {
		for (int j = 0; j < g_gui->boxes[i].children_count; ++j) {
			wzrd_box* owner = g_gui->boxes + i;
			wzrd_box* child = g_gui->boxes + g_gui->boxes[i].children[j];
			if (!IsRectInsideRect((wzrd_rect) { child->x, child->y, child->w, child->h }, (wzrd_rect) { owner->x, owner->y, owner->w, owner->h })) {
				//owner->color = EGUI_ORANGE;
				//child->color = EGUI_RED;
			}
		}
	}



	// Mouse interaction
	//if (g_wzrd_input.mouse_pos.x > 0 && g_wzrd_input.mouse_pos.y > 0)
	{
		wzrd_handle_input();
		wzrd_handle_cursor(cursor);
		wzrd_handle_border_resize(cursor);
	}

	// Mouse position
	g_wzrd_input.previous_mouse_pos = g_wzrd_input.mouse_pos;

	// Drawing
	//if (!g_gui->clean)
	{

		int boxes_indices[MAX_NUM_BOXES] = { 0 };

		for (int i = 0; i < g_gui->boxes_count; ++i)
		{
			boxes_indices[i] = i;
		}

		qsort(boxes_indices, g_gui->boxes_count, sizeof(int), CompareBoxes2);

		buffer->count = 0;
		for (int i = 0; i < g_gui->boxes_count; ++i) {

			wzrd_box box = g_gui->boxes[boxes_indices[i]];

			EguiRectDraw(buffer, (wzrd_rect) {
				.x = box.x,
					.y = box.y,
					.h = box.h,
					.w = box.w
			},
				box.color,
				box.z);


			// Borders (1215 x 810)
			float line_size = WZRD_BORDER_SIZE;

			wzrd_rect top0 = (wzrd_rect){ box.x, box.y, box.w - line_size, line_size };
			wzrd_rect left0 = (wzrd_rect){ box.x, box.y, line_size, box.h };

			wzrd_rect top1 = (wzrd_rect){ box.x + line_size, box.y + line_size, box.w - 3 * line_size, line_size };
			wzrd_rect left1 = (wzrd_rect){ box.x + line_size, box.y + line_size, line_size, box.h - line_size };

			wzrd_rect bottom0 = (wzrd_rect){ box.x, box.y + box.h - line_size, box.w, line_size };
			wzrd_rect right0 = (wzrd_rect){ box.x + box.w - line_size, box.y, line_size, box.h };

			wzrd_rect bottom1 = (wzrd_rect){ box.x + 1 * line_size, box.y + box.h - 2 * line_size, box.w - 3 * line_size, line_size };
			wzrd_rect right1 = (wzrd_rect){ box.x + box.w - 2 * line_size, box.y + 1 * line_size, line_size, box.h - 2 * line_size };

			IsRectInsideRect(top0, (wzrd_rect) { box.x, box.y, box.w, box.h });
			IsRectInsideRect(left0, (wzrd_rect) { box.x, box.y, box.w, box.h });
			IsRectInsideRect(top1, (wzrd_rect) { box.x, box.y, box.w, box.h });
			IsRectInsideRect(left1, (wzrd_rect) { box.x, box.y, box.w, box.h });
			IsRectInsideRect(bottom0, (wzrd_rect) { box.x, box.y, box.w, box.h });
			IsRectInsideRect(right0, (wzrd_rect) { box.x, box.y, box.w, box.h });
			IsRectInsideRect(bottom1, (wzrd_rect) { box.x, box.y, box.w, box.h });
			IsRectInsideRect(right1, (wzrd_rect) { box.x, box.y, box.w, box.h });

			bool debug = false;
			if (debug) {
				// Draw top and left lines
				EguiRectDraw(buffer, top0, EGUI_BLUE, box.z);
				EguiRectDraw(buffer, left0, EGUI_BLUE, box.z);
				EguiRectDraw(buffer, top1, EGUI_ORANGE, box.z);
				EguiRectDraw(buffer, left1, EGUI_ORANGE, box.z);

				// Draw bottom and right lines
				EguiRectDraw(buffer, bottom0, EGUI_GREEN, box.z);
				EguiRectDraw(buffer, right0, EGUI_GREEN, box.z);
				EguiRectDraw(buffer, bottom1, EGUI_PURPLE, box.z);
				EguiRectDraw(buffer, right1, EGUI_PURPLE, box.z);
			}
			else if (box.border_type == BorderType_Default) {
				// Draw top and left lines
				EguiRectDraw(buffer, top0, EGUI_WHITE2, box.z);
				EguiRectDraw(buffer, left0, EGUI_WHITE2, box.z);
				EguiRectDraw(buffer, top1, EGUI_LIGHTGRAY, box.z);
				EguiRectDraw(buffer, left1, EGUI_LIGHTGRAY, box.z);

				// Draw bottom and right lines
				EguiRectDraw(buffer, bottom0, EGUI_BLACK, box.z);
				EguiRectDraw(buffer, right0, EGUI_BLACK, box.z);
				EguiRectDraw(buffer, bottom1, EGUI_GRAY, box.z);
				EguiRectDraw(buffer, right1, EGUI_GRAY, box.z);
			}
			else if (box.border_type == BorderType_Clicked) {
				// Draw top and left lines
				EguiRectDraw(buffer, top0, EGUI_BLACK, box.z);
				EguiRectDraw(buffer, left0, EGUI_BLACK, box.z);
				EguiRectDraw(buffer, top1, EGUI_GRAY, box.z);
				EguiRectDraw(buffer, left1, EGUI_GRAY, box.z);

				// Draw bottom and right lines
				EguiRectDraw(buffer, bottom0, EGUI_WHITE2, box.z);
				EguiRectDraw(buffer, right0, EGUI_WHITE2, box.z);
				EguiRectDraw(buffer, bottom1, EGUI_LIGHTESTGRAY, box.z);
				EguiRectDraw(buffer, right1, EGUI_LIGHTESTGRAY, box.z);
			}
			else if (box.border_type == BorderType_InputBox) {
				// Draw top and left lines
				EguiRectDraw(buffer, top0, EGUI_GRAY, box.z);
				EguiRectDraw(buffer, left0, EGUI_GRAY, box.z);
				EguiRectDraw(buffer, top1, EGUI_BLACK, box.z);
				EguiRectDraw(buffer, left1, EGUI_BLACK, box.z);

				// Draw bottom and right lines
				EguiRectDraw(buffer, bottom0, EGUI_WHITE2, box.z);
				EguiRectDraw(buffer, right0, EGUI_WHITE2, box.z);
				EguiRectDraw(buffer, bottom1, EGUI_LIGHTESTGRAY, box.z);
				EguiRectDraw(buffer, right1, EGUI_LIGHTESTGRAY, box.z);
			}
			else if (box.border_type == BorderType_Black) {
				// Draw top and left lines
				EguiRectDraw(buffer, top0, EGUI_BLACK, box.z);
				EguiRectDraw(buffer, left0, EGUI_BLACK, box.z);
				EguiRectDraw(buffer, top1, EGUI_WHITE2, box.z);
				EguiRectDraw(buffer, left1, EGUI_WHITE2, box.z);

				// Draw bottom and right lines
				EguiRectDraw(buffer, bottom0, EGUI_BLACK, box.z);
				EguiRectDraw(buffer, right0, EGUI_BLACK, box.z);
				EguiRectDraw(buffer, bottom1, EGUI_WHITE2, box.z);
				EguiRectDraw(buffer, right1, EGUI_WHITE2, box.z);
			}
			else if (box.border_type == BorderType_BottomLine) {
				EguiRectDraw(buffer, bottom0, EGUI_WHITE2, box.z);
				EguiRectDraw(buffer, bottom1, EGUI_GRAY, box.z);
			}
			else if (box.border_type == BorderType_LeftLine) {
				EguiRectDraw(buffer, left0, EGUI_GRAY, box.z);
				EguiRectDraw(buffer, left1, EGUI_WHITE2, box.z);
			}

			// Draw content
			for (int j = 0; j < box.items_count; ++j) {
				Item item = box.items[j];
				wzrd_draw_command command = { 0 };

				if (wzrd_float_compare(item.size.x, 0))
					item.size.x = box.w;
				if (wzrd_float_compare(item.size.y, 0))
					item.size.y = box.h;

				if (item.type == wzrd_item_type_str) {
					command = (wzrd_draw_command){
						.type = DrawCommandType_String,
							.str = item.val.str,
							.dest_rect = (wzrd_rect){ box.x, box.y,
							box.w, box.h },
							.color = item.color,
							.z = box.z
					};
				}
				else if (item.type == ItemType_Texture) {
					command = (wzrd_draw_command){
						.type = DrawCommandType_Texture,
						.dest_rect = (wzrd_rect){box.x, box.y, box.w, box.h},
						.src_rect = (wzrd_rect) {0, 0, item.val.texture.w, item.val.texture.h},
						.texture = item.val.texture,
						.z = box.z
					};

					if (item.scissor) {
						command.src_rect = (wzrd_rect){ 0, 0, command.dest_rect.w, command.dest_rect.h };
					}
				}
				else if (item.type == ItemType_Line) {
					command = (wzrd_draw_command){
						.type = DrawCommandType_Line,
							.dest_rect = (wzrd_rect){
								box.x + item.val.rect.x,
								box.y + item.val.rect.y,
								box.x + item.val.rect.w,
								box.y + item.val.rect.h
						},
							.color = item.color,
						.z = box.z

					};
				}
				else if (item.type == ItemType_HorizontalDottedLine) {
					command = (wzrd_draw_command){
						.type = DrawCommandType_HorizontalLine,
							.dest_rect = (wzrd_rect){
								box.x,
								box.y + box.h / 2,
								box.x + box.w,
								box.y + box.h / 2
						},
						.z = box.z

					};
				}
				else if (item.type == ItemType_LeftHorizontalDottedLine) {
					command = (wzrd_draw_command){
						.type = DrawCommandType_HorizontalLine,
							.dest_rect = (wzrd_rect){
								box.x,
								box.y + box.h / 2,
								box.x + box.w / 2,
								box.y + box.h / 2
						},
						.z = box.z

					};
				}
				else if (item.type == ItemType_RightHorizontalDottedLine) {
					command = (wzrd_draw_command){
						.type = DrawCommandType_HorizontalLine,
							.dest_rect = (wzrd_rect){
								box.x + box.w / 2,
								box.y + box.h / 2,
								box.x + box.w,
								box.y + box.h / 2
						},
						.z = box.z

					};
				}
				else if (item.type == ItemType_VerticalDottedLine) {
					command = (wzrd_draw_command){
						.type = DrawCommandType_VerticalLine,
							.dest_rect = (wzrd_rect){
								box.x + box.w / 2,
								box.y,
								box.x + box.w / 2 + box.w,
								box.y + box.h
						},
						.z = box.z

					};
				}
				else if (item.type == ItemType_TopVerticalDottedLine) {
					command = (wzrd_draw_command){
						.type = DrawCommandType_VerticalLine,
							.dest_rect = (wzrd_rect){
								box.x + box.w / 2,
								box.y,
								box.x + box.w / 2 + box.w,
								box.y + box.h / 2
						},
						.z = box.z

					};
				}
				else if (item.type == ItemType_BottomVerticalDottedLine) {
					command = (wzrd_draw_command){
						.type = DrawCommandType_VerticalLine,
							.dest_rect = (wzrd_rect){
								box.x + box.w / 2,
								box.y + box.h / 2,
								box.x + box.w / 2,
								box.y + box.h
						},
						.z = box.z

					};
				}
				else if (item.type == ItemType_Rect) {
					command = (wzrd_draw_command){
						.type = DrawCommandType_Rect,
							.dest_rect = (wzrd_rect){
								box.x + item.val.rect.x,
								box.y + item.val.rect.y,
								item.val.rect.w,
								item.val.rect.h
						},
							.color = item.color,
						.z = box.z

					};
				}

				command.dest_rect.x += item.pad_left;
				command.dest_rect.y += item.pad_top;

				WZRD_ASSERT(command.dest_rect.w >= 0);
				WZRD_ASSERT(command.dest_rect.h >= 0);

				WZRD_ASSERT(buffer->count < MAX_NUM_DRAW_COMMANDS - 1);
				buffer->commands[buffer->count++] = command;
			}

			// Draw clip area
			if (box.clip)
			{
				WZRD_ASSERT(buffer->count < MAX_NUM_DRAW_COMMANDS - 1);
				buffer->commands[buffer->count++] = (wzrd_draw_command){
					.type = DrawCommandType_Clip,
					.dest_rect = (wzrd_rect) {box.x + 2 * WZRD_BORDER_SIZE, box.y + 2 * WZRD_BORDER_SIZE, box.w - 4 * WZRD_BORDER_SIZE, box.h - 4 * WZRD_BORDER_SIZE},
					.z = box.z
				};
			}

		}

		g_gui->clean = true;
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
					.val = { .texture = texture }
			});
		}
		EguiButtonRawEnd();
	}
	EguiButtonRawEnd();

	return result;
}
bool wzrd_button_icon3(wzrd_box box, Item item) {
	bool result = false;
	result |= EguiButtonRawBegin(box);
	{
		result |= EguiButtonRawBegin((wzrd_box) { .border_type = BorderType_None, .w = 16, .h = 16 });
		{
			wzrd_item_add(item);
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
					.val = { .texture = texture }
			});
		}
		EguiButtonRawEnd();
	}
	EguiButtonRawEnd();

	return result;
}

wzrd_rect wzrd_box_get_rect(wzrd_box* box) {
	if (!box)
	{
		return (wzrd_rect) { 0 };
	}

	wzrd_rect result = (wzrd_rect){ box->x, box->y, box->w, box->h };

	return result;
}

bool IsActive() {
	if (str128_equal(wzrd_box_get_from_top_of_stack()->name, g_gui->active_item)) {
		return true;
	}

	return false;
}

void wzrd_toggle_icon(wzrd_texture texture, bool* active) {
	bool result = false;
	result |= EguiButtonRawBegin((wzrd_box) { .w = 24, .h = 22, .center = true });
	{

		result |= EguiButtonRawBegin((wzrd_box) { .border_type = BorderType_None, .w = 16, .h = 16 });
		{
			wzrd_item_add((Item) {
				.type = ItemType_Texture,
					.val = { .texture = texture }
			});
		}
		EguiButtonRawEnd();

		if (result)
		{
			*active = !*active;
		}

		bool a1 = wzrd_box_is_active(&g_gui->boxes[g_gui->boxes_count - 1]);
		bool a2 = wzrd_box_is_active(&g_gui->boxes[g_gui->boxes_count - 2]);


		if (*active || a1 || a2) {
			wzrd_box_get_from_top_of_stack()->border_type = BorderType_Clicked;
		}
	}
	EguiButtonRawEnd();

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
					.val = { .texture = texture }
			});
		}
		EguiButtonRawEnd();

		if (active) {
			wzrd_box_get_from_top_of_stack()->border_type = BorderType_Clicked;
		}

	}
	EguiButtonRawEnd();

	return result;
}

void EguiToggle3(wzrd_box box, str128 str, bool* active) {
	bool b1 = false, b2 = false, a1 = false, a2 = false;
	b1 = EguiButtonRawBegin(box);
	{
		b2 = EguiButtonRawBegin((wzrd_box) {
			.border_type = BorderType_None,
				.color = wzrd_box_get_from_top_of_stack()->color,
				.w = (float)str.len * FONT_WIDTH, .h = WZRD_FONT_HEIGHT
		});
		{
			wzrd_text_add(str);
		}
		EguiButtonRawEnd();
	}
	EguiButtonRawEnd();

	if (b1 || b2)
	{
		*active = !*active;
	}

	a1 = wzrd_box_is_active(&g_gui->boxes[g_gui->boxes_count - 1]);
	a2 = wzrd_box_is_active(&g_gui->boxes[g_gui->boxes_count - 2]);

	if (*active || a1 || a2)
	{
		g_gui->boxes[g_gui->boxes_count - 2].border_type = BorderType_Clicked;
	}
}

bool EguiToggle2(wzrd_box box, str128 str, wzrd_color color, bool active) {
	bool b1 = false, b2 = false;
	b1 = EguiButtonRawBegin(box);
	{
		if (active) {
			wzrd_box_get_from_top_of_stack()->color = color;
		}

		b2 = EguiButtonRawBegin((wzrd_box) {
			.border_type = BorderType_None,
				.color = wzrd_box_get_from_top_of_stack()->color,
				.w = (float)str.len * FONT_WIDTH, .h = WZRD_FONT_HEIGHT
		});
		{
			if (active) wzrd_box_get_from_top_of_stack()->color = color;

			wzrd_text_add(str);
		}
		EguiButtonRawEnd();
	}
	EguiButtonRawEnd();

	if (b1 || b2)
		return true;
	return false;
}

bool EguiButton2(wzrd_box box, str128 str, wzrd_color color) {
	(void)color;
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

			wzrd_text_add(str);
			wzrd_item_add((Item) { .type = wzrd_item_type_str, .val = { .str = str128_create(str.val) } });
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

	if (str128_equal(wzrd_box_get_from_top_of_stack()->name, g_gui->clicked_item)) {
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
	wzrd_box* box = wzrd_box_get_from_top_of_stack();
	WZRD_ASSERT(box->items_count < MAX_NUM_ITEMS - 1);
	box->items[box->items_count++] = item;
}

void wzrd_label(str128 str) {
	float w = 0, h = 0;
	g_gui->get_string_size(str.val, &w, &h);

	wzrd_box_begin((wzrd_box) {
		.border_type = BorderType_None,
			.w = w,
			.h = h,
			.color = g_gui->style.background_color
	});
	{
		wzrd_text_add(str);
	}
	wzrd_box_end();
}

bool wzrd_is_active() {
	bool result = false;
	if (str128_equal(wzrd_box_get_from_top_of_stack()->name, g_gui->active_item)) {
		result = true;
	}

	return result;
}

void wzrd_input_box(str128* str, int max_num_keys) {

	wzrd_box_begin((wzrd_box) {
		.color = EGUI_WHITE,
			.w = FONT_WIDTH * (float)max_num_keys + 8,
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
		wzrd_box_begin((wzrd_box) { .is_input_box = true, .w = FONT_WIDTH * (float)max_num_keys, .h = WZRD_FONT_HEIGHT, .border_type = BorderType_None, .color = EGUI_WHITE });
		{
			str128 str2 = *str;

			if (str128_equal(wzrd_box_get_from_top_of_stack()->name, g_gui->active_input_box)) {

				for (int i = 0; i < g_wzrd_input.keyboard_keys.count; ++i) {
					wzrd_keyboard_key key = g_wzrd_input.keyboard_keys.keys[i];

					if (key.val == '\b' &&
						str->len > 0 &&
						(key.state == WZRD_ACTIVE || key.state == EguiActivating)) {
						str->val[str->len - 1] = 0;
						str->len--;
					}

					if ((key.state == EguiActivating || key.state == WZRD_ACTIVE) &&
						//((key.val <= 'z' && key.val >= 'a') || (key.val <= '9' && key.val >= '0')) &&
						(isgraph(key.val) || key.val == ' ') &&
						str->len < (size_t)max_num_keys - 1 &&
						str->len < 127) {
						char s[2] = { [0] = key.val };
						str128 ss = str128_create(s);
						str128_concat(str, ss);
					}
				}

				static bool show_caret;

				/*		if (g_gui->input_box_timer - g_gui->time > 500) {
							g_gui->input_box_timer = 0;
							show_caret = !show_caret;
						}*/

				if (show_caret) {
					str2 = *str;
					str128 caret = str128_create("|");
					str128_concat(&str2, caret);
				}

			}

			/*if (str128_equal(g_gui->active_input_box, wzrd_box_get_current()->name)) {
				wzrd_box_get_current()->color = EGUI_PINK;
			}*/

			//wzrd_v2 size = { FONT_WIDTH * max_num_keys + 4, WZRD_FONT_HEIGHT + 4 };

			wzrd_text_add(str2);
		}
		wzrd_box_end();
	}
	wzrd_box_end();
}

bool EguiLabelButtonBegin(str128 str) {
	wzrd_v2 v = { FONT_WIDTH * (float)str.len, WZRD_FONT_HEIGHT };
	bool result = wzrd_box_begin((wzrd_box) {
		.border_type = BorderType_None,
			.color = wzrd_box_get_from_top_of_stack()->color,
			.w = v.x,
			.h = v.y,
			.flat_button = true,
			.is_button = true
	});
	{
		wzrd_text_add(str);
	}

	return result;
}

void EguiLabelButtonEnd() {
	wzrd_box_end();
}

bool EguiLabelButton(str128 str) {
	bool result = EguiLabelButtonBegin(str);
	EguiLabelButtonEnd();
	return result;
}


void wzrd_dialog_begin(wzrd_v2* pos, wzrd_v2 size, bool* active, str128 name, int parent, int layer) {

	if (!*active) return;

	wzrd_crate_begin(layer , (wzrd_box) { .parent = parent, .x = pos->x, .y = pos->y, .w = size.x, .h = size.y, .name = name });

	bool close = false;
	wzrd_box_begin((wzrd_box) { .border_type = BorderType_None, .h = 28, .row_mode = true });
	{
		wzrd_box_begin((wzrd_box) {
			.border_type = BorderType_None,
				.color = (wzrd_color){ 57, 77, 205, 255 }
		});
		{
			if (str128_equal(wzrd_box_get_from_top_of_stack()->name, g_gui->active_item)) {
				pos->x += g_wzrd_input.mouse_pos.x - g_wzrd_input.previous_mouse_pos.x;
				pos->y += g_wzrd_input.mouse_pos.y - g_wzrd_input.previous_mouse_pos.y;
			}
		}
		wzrd_box_end();

		wzrd_box_begin((wzrd_box) {
			.border_type = BorderType_None, .w = 28, .row_mode = true,
				.center = true,
				.color = (wzrd_color){ 57, 77, 205, 255 },
		});
		{
			bool b = EguiButtonRawBegin((wzrd_box) { .w = 20, .h = 20, .center = true });
			{
				b |= EguiButtonRawBegin((wzrd_box) { .w = 12, .h = 12, .border_type = BorderType_None });
				{
					wzrd_item_add((Item) { .type = ItemType_CloseIcon });
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

void wzrd_dropdown(int* selected_text, const str128* texts, int texts_count, float w, bool* active) {
	WZRD_ASSERT(texts);
	WZRD_ASSERT(texts_count);
	WZRD_ASSERT(selected_text);
	WZRD_ASSERT(*selected_text >= 0);

	w = 150.0f;

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
				.h = WZRD_FONT_HEIGHT + 4 * WZRD_BORDER_SIZE
		};

		int parent = 0;
		EguiButton2(box, texts[*selected_text], EGUI_DARKBLUE);

		parent = g_gui->boxes_count - 1;

		box.border_type = BorderType_None;

		//bool* toggle2;

		bool button = wzrd_button_icon3((wzrd_box) { .w = 20, .h = WZRD_FONT_HEIGHT + 4 * WZRD_BORDER_SIZE, .center = true }, (Item) { .type = ItemType_DropdownIcon });
		if (button)
		{
			*active = !*active;
		}

		if (*active) {
			//char str[256];
			//sprintf(str, "%s-dropdown", wzrd_box_get_from_top_of_stack()->name.val);
			//static wzrd_v2i pos;

			wzrd_crate_begin(2, (wzrd_box) {
				.parent = parent,
					.y = box.h,
					.w = box.w + 4 * WZRD_BORDER_SIZE,
					.h = box.h * texts_count + 4 * WZRD_BORDER_SIZE,
					.name = str128_create("%s-dropdown", wzrd_box_get_from_top_of_stack()->name.val),
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
	wzrd_v2 v = { FONT_WIDTH * (float)str.len + 12, WZRD_FONT_HEIGHT + 12 };
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
			wzrd_text_add(str);
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
			.fit_h = true, .fit_w = true
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

bool wzrd_game_buttonesque(wzrd_v2 pos, wzrd_v2 size, wzrd_color color, str128 name) {
	bool result = false;

	wzrd_crate_begin(1, (wzrd_box) {
		.is_button = true, .color = color, .border_type = BorderType_None, .x = pos.x, .y = pos.y, .w = size.x, .h = size.y
			, .name = name
	});
	{
		result = IsActive();
	}
	wzrd_crate_end();

	return result;
}

void wzrd_slot() {

}

void wzrd_drag(bool* drag) {

	if (g_wzrd_input.mouse_left == WZRD_INACTIVE)
	{
		*drag = false;
	}

	if (!(*drag)) return;


	g_gui->dragged_box.x += g_wzrd_input.mouse_delta.x;
	g_gui->dragged_box.y += g_wzrd_input.mouse_delta.y;

	wzrd_crate(1, g_gui->dragged_box);
}

bool wzrd_box_is_active(wzrd_box* box) {
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
	//printf("%s\n", g_gui->hot_item.val);
	if (str128_equal(box->name, g_gui->hot_item)) {
		return true;
	}

	return false;
}

bool wzrd_box_is_selected(wzrd_box* box) {
	if (str128_equal(box->name, g_gui->selected_item)) {
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
	float lerp_amount = 0.2f;
	float delta = 0.1f;
	if (fabs(end_pos.x - pos.x) > delta) {
		pos.x = pos.x + lerp_amount * (end_pos.x - pos.x);
	}

	if (fabs(end_pos.y - pos.y) > delta) {
		pos.y = pos.y + lerp_amount * (end_pos.y - pos.y);
	}

	return pos;
}

bool wzrd_v2_is_inside_polygon(wzrd_v2 point, wzrd_polygon polygon) {

	bool inside = false;
	int i, j;

	for (i = 0, j = polygon.count - 1; i < polygon.count; j = i++) {
		if (((polygon.vertices[i].y > point.y) != (polygon.vertices[j].y > point.y)) &&
			(point.x < (polygon.vertices[j].x - polygon.vertices[i].x) * (point.y - polygon.vertices[i].y) / (polygon.vertices[j].y - polygon.vertices[i].y) + polygon.vertices[i].x))
			inside = !inside;
	}

	return inside;
}
