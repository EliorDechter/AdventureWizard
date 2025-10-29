// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#define WZRD_ASSERT(x) assert(x)
//#define WZRD_ASSERT(x) (void)(x)
#define WZRD_UNUSED(x) (void)x

#include "Egui.h"
#include "Strings.h"

static wzrd_canvas* canvas;

wzrd_style_handle wzrd_style_create(wzrd_style style);
wzrd_style wzrd_style_get(wzrd_style_handle handle);
bool wzrd_handle_is_equal(wzrd_handle a, wzrd_handle b);
bool wzrd_handle_is_valid(wzrd_handle handle);
wzrd_handle wzrd_handle_create();
wzrd_style wzrd_style_get_default();
void wzrd_box_end();
wzrd_box* wzrd_box_get_from_top_of_stack();
wzrd_box* wzrd_box_get_parent();
wzrd_box* wzrd_box_get_previous();
void wzrd_label(wzrd_str str, wzrd_handle parent);
void wzrd_crate_begin(int window_id, wzrd_box box);
void wzrd_crate(int window_id, wzrd_box box);
void wzrd_texture_add(wzrd_texture texture, wzrd_v2 size, wzrd_handle box);
void wzrd_text_add(wzrd_str str, wzrd_handle box);
void wzrd_box_resize(wzrd_v2* size);
void wzrd_crate_end();
wzrd_box* wzrd_box_get_by_handle(wzrd_handle str);
void wzrd_drag(bool* drag);
wzrd_box* wzrd_box_get_last();
bool wzrd_box_is_dragged(wzrd_box* box);
bool wzrd_box_is_hot_using_canvas(wzrd_canvas* canvas, wzrd_box* box);
wzrd_box* wzrd_box_get_released();
wzrd_box* wzrd_box_find(wzrd_canvas* canvas, wzrd_str name);
bool wzrd_is_releasing();
wzrd_v2 wzrd_lerp(wzrd_v2 pos, wzrd_v2 end_pos);
wzrd_handle wzrd_unique_handle_create(wzrd_str str);
bool wzrd_unique_handle_is_equal(wzrd_str str, wzrd_handle handle);

wzrd_style wzrd_style_get(wzrd_style_handle handle)
{
	wzrd_style result = canvas->styles[handle.index];

	return result;
}

wzrd_style_handle wzrd_style_create(wzrd_style style)
{
	wzrd_style_handle result = (wzrd_style_handle){ canvas->styles_count };
	canvas->styles[canvas->styles_count++] = style;

	return result;
}
wzrd_style_handle wzrd_style_set_color(wzrd_style_handle handle, wzrd_color color)
{
	wzrd_style style = wzrd_style_get(handle);
	style.color = color;
	wzrd_style_handle result = wzrd_style_create(style);

	return result;
}

wzrd_handle wzrd_vbox(wzrd_v2 size, wzrd_handle parent)
{
	wzrd_handle handle = wzrd_widget(((wzrd_box) {
		.style = wzrd_style_create((wzrd_style) {
			.h = size.y,
				.w = size.x,
				.pad_left = 5,
				.center_y = true,
				.child_gap = 5,
				.row_mode = true,
				.border_type = BorderType_None,
				.color = EGUI_LIGHTGRAY
		})
	}), parent);

	return handle;
}

bool wzrd_handle_is_valid(wzrd_handle handle)
{
	return (bool)handle.handle;
}

wzrd_handle wzrd_handle_create()
{
	assert(canvas);
	wzrd_handle handle = { canvas->boxes_count };

	return handle;
}

unsigned int wzrd_hash(wzrd_str str)
{
	unsigned int hash = 0;
	for (size_t i = 0; i < str.len; ++i)
	{
		hash = 65599 * hash + str.str[i];
	}

	hash = hash ^ (hash >> 16);

	hash += MAX_NUM_BOXES;

	return hash;
}

wzrd_handle wzrd_unique_handle_create(wzrd_str str)
{
	unsigned int hash = wzrd_hash(str);
	wzrd_handle result = { hash };

	for (int i = 0; i < canvas->boxes_count; ++i)
	{
		assert(canvas->boxes[i].handle.handle != result.handle);
	}

	return result;
}

bool wzrd_is_rect_inside_rect(wzrd_rect_struct a, wzrd_rect_struct b) {
	bool res = a.x >= b.x;
	res &= a.x >= b.x;
	res &= a.x + a.w <= b.x + b.w;
	res &= a.y + a.h <= b.y + b.h;

	return res;
}

int wzrd_v2_is_inside_rect(wzrd_v2 v, wzrd_rect_struct rect) {
	bool result = false;
	if (v.x >= rect.x &&
		v.y >= rect.y &&
		v.x <= rect.x + rect.w &&
		v.y <= rect.y + rect.h) {

		result = true;
	}

	return result;
}

Crate* EguiGetCurrentWindow() {
	Crate* result = &canvas->crates_stack[canvas->current_crate_index];

	return result;
}

Crate* EguiGetPreviousWindow() {
	WZRD_ASSERT(canvas->current_crate_index > 0);
	Crate* result = &canvas->crates_stack[canvas->current_crate_index - 1];

	return result;
}

wzrd_box* wzrd_box_get_parent() {
	if (canvas->crates_stack[canvas->current_crate_index].box_stack_count < 2)
	{
		return &canvas->boxes[0];
	}
	int current_box_index = canvas->crates_stack[canvas->current_crate_index].box_stack_count - 2;
	int final_index = canvas->crates_stack[canvas->current_crate_index].box_stack[current_box_index];
	wzrd_box* result = &canvas->boxes[final_index];

	return result;
}

wzrd_box* wzrd_box_get_from_top_of_stack() {
	//WZRD_ASSERT(canvas->current_crate_index >= 0);
	//int crate_index = canvas->current_crate_index;

	//int current_box_index = canvas->crates_stack[crate_index].box_stack_count - 1;
	//if (current_box_index < 0)
	//{
	//	return &canvas->boxes[0];
	//}
	//int final_index = canvas->crates_stack[crate_index].box_stack[current_box_index];
	//wzrd_box* result = &canvas->boxes[final_index];

	//WZRD_ASSERT(result);

	//return result;
	return &canvas->boxes[0];
}
wzrd_style wzrd_box_style_get_from_top_of_stack() {
	return wzrd_style_get(wzrd_box_get_from_top_of_stack()->style);
}



int wzrd_box_get_current_index() {
	WZRD_ASSERT(canvas->current_crate_index >= 0);
	int current_box_index = canvas->crates_stack[canvas->current_crate_index].box_stack_count - 1;
	if (current_box_index < 0) return 0;
	int final_index = canvas->crates_stack[canvas->current_crate_index].box_stack[current_box_index];
	int result = final_index;

	return result;
}

wzrd_box* wzrd_box_get_last() {
	wzrd_box* result = canvas->boxes + (canvas->boxes_count - 1);

	return result;
}

void wzrd_text_add(wzrd_str str, wzrd_handle parent)
{
	wzrd_item_add((Item) { .type = wzrd_item_type_str, .val = { .str = str }, .color = EGUI_BLACK }, parent);
}

void goo(wzrd_box* box, void* data)
{
	(void)data;
	(void)box;
	//box->color = EGUI_BEIGE;
}

void wzrd_box_tree_apply(int index, void* data, void (*goo)(wzrd_box* box, void* data))
{
	int stack[1024];
	int count = 0;
	int ptr = 0;

	stack[count++] = index;

	while (ptr != count)
	{
		wzrd_box* box = &canvas->boxes[stack[ptr]];

		for (int i = 0; i < box->children_count; ++i)
		{
			stack[count++] = box->children[i];
		}

		goo(box, data);
		ptr++;
	}
}

bool wzrd_handle_is_active_tree(wzrd_handle handle)
{
	int stack[1024];
	int count = 0;
	int ptr = 0;

	stack[count++] = wzrd_box_get_by_handle(handle)->index;

	while (ptr != count)
	{
		wzrd_box* box = &canvas->boxes[stack[ptr]];

		for (int i = 0; i < box->children_count; ++i)
		{
			stack[count++] = box->children[i];
		}

		if (wzrd_box_is_active(box))
		{
			return true;
		}

		ptr++;
	}

	return false;
}

bool wzrd_box_is_released(wzrd_box* box) {
	if (wzrd_handle_is_equal(box->handle, canvas->deactivating_item)) {
		return true;
	}

	return false;
}

bool wzrd_handle_is_released_tree(wzrd_handle handle)
{
	int stack[1024];
	int count = 0;
	int ptr = 0;

	stack[count++] = wzrd_box_get_by_handle(handle)->index;

	while (ptr != count)
	{
		wzrd_box* box = &canvas->boxes[stack[ptr]];

		for (int i = 0; i < box->children_count; ++i)
		{
			stack[count++] = box->children[i];
		}

		if (wzrd_box_is_released(box))
		{
			return true;
		}

		ptr++;
	}

	return false;
}


int wzrd_compare_boxes(const void* element1, const void* element2) {
	int index1 = *(int*)element1;
	int index2 = *(int*)element2;
	wzrd_box* c1 = canvas->boxes + index1;
	wzrd_box* c2 = canvas->boxes + index2;

	if (c1->layer > c2->layer)
	{
		return 1;
	}

	if (c1->layer < c2->layer)
	{
		return -1;
	}

	if (c1->layer == c2->layer)
	{
		if (c1->bring_to_front && !c2->bring_to_front)
		{
			return 1;
		}
		else if (!c1->bring_to_front && c2->bring_to_front)
		{
			return -1;
		}

		if (index1 > index2) return 1;
		else if (index1 < index2) return -1;
	}

	return 0;
}

bool wzrd_handle_is_equal(wzrd_handle a, wzrd_handle b)
{
	return a.handle == b.handle;
}


void wzrd_box_resize(wzrd_v2* size) {
	wzrd_box* box = wzrd_box_get_from_top_of_stack();
	wzrd_box* previous_box = wzrd_box_get_parent();

	box->type = wzrd_box_type_resizable;

	int mouse_delta_x = canvas->mouse_pos.x - canvas->previous_mouse_pos.x;
	int mouse_delta_y = canvas->mouse_pos.y - canvas->previous_mouse_pos.y;

	wzrd_style previous_box_style = wzrd_style_get(previous_box->style);

	if (previous_box_style.row_mode) {
		if (wzrd_handle_is_equal(box->handle, canvas->left_resized_item)) {
			if (size->x - mouse_delta_x >= 0)
				size->x -= mouse_delta_x;
		}

		else if (wzrd_handle_is_equal(box->handle, canvas->right_resized_item)) {
			if (size->x + mouse_delta_x >= 0)
				size->x += mouse_delta_x;
		}
	}
	else {
		if (wzrd_handle_is_equal(box->handle, canvas->top_resized_item)) {
			if (size->y - mouse_delta_y <= 0)
				size->y -= mouse_delta_y;
		}
		else if (wzrd_handle_is_equal(box->handle, canvas->bottom_resized_item)) {
			if (size->y + mouse_delta_y >= 0)
				size->y += mouse_delta_y;
		}
	}

	wzrd_box_get_from_top_of_stack()->w_internal += size->x;
	wzrd_box_get_from_top_of_stack()->h_internal += size->y;
}

int wzrd_int_compare(int a, int b)
{
	if (fabs(a - b) < FLT_EPSILON)
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

void wzrd_box_add_child_free(wzrd_handle parent, wzrd_handle child)
{
	wzrd_box* p = wzrd_box_get_by_handle(parent);
	wzrd_box* c = wzrd_box_get_by_handle(child);
	WZRD_ASSERT(p->free_children_count < MAX_NUM_CHILDREN);
	p->free_children[p->free_children_count++] = c->index;
}

void wzrd_box_add_child(wzrd_handle parent, wzrd_handle child)
{
	wzrd_box* p = wzrd_box_get_by_handle(parent);
	wzrd_box* c = wzrd_box_get_by_handle(child);
	WZRD_ASSERT(p->children_count < MAX_NUM_CHILDREN);
	p->children[p->children_count++] = c->index;
}

void wzrd_box_add_child_using_pointer(wzrd_box* parent, wzrd_box* child)
{
	WZRD_ASSERT(parent->children_count < MAX_NUM_CHILDREN);
	parent->children[parent->children_count++] = child->index;
}

void wzrd_box_add_free_child(wzrd_box* parent, wzrd_box* child)
{
	WZRD_ASSERT(parent->free_children_count < MAX_NUM_CHILDREN);
	parent->free_children[parent->free_children_count++] = child->index;
}

wzrd_box* wzrd_box_create(wzrd_box box) {

	if (box.handle.handle == 0)
		box.handle = wzrd_handle_create();

	WZRD_ASSERT(canvas->boxes_count < MAX_NUM_BOXES - 1);

	box.layer = EguiGetCurrentWindow()->layer;

	wzrd_style box_style = wzrd_style_get(box.style);

	if (box_style.fit_w) {
		box.w_internal += 4 * WZRD_BORDER_SIZE;
	}
	if (box_style.fit_h) {
		box.h_internal += 4 * WZRD_BORDER_SIZE;
	}

	// Default color 
	if (box_style.color.r == 0 && box_style.color.g == 0 && box_style.color.b == 0 && box_style.color.a == 0) {
		box_style.color = canvas->style.background_color;
	}

	// Add box
	if (!memcmp(&canvas->boxes[canvas->boxes_count], &box, sizeof(wzrd_box)))
	{
		canvas->clean = false;
	}

	WZRD_ASSERT(canvas->boxes_count < 256);

	box.index = canvas->boxes_count;
	canvas->boxes[canvas->boxes_count++] = box;

	return &canvas->boxes[canvas->boxes_count - 1];
}

wzrd_handle wzrd_widget_free(wzrd_box box_data, wzrd_handle parent)
{
	{
		wzrd_style style = wzrd_style_get(box_data.style);
		box_data.x_internal = style.x;
		box_data.y_internal = style.y;
		box_data.w_internal = style.w;
		box_data.h_internal = style.h;
	}

	wzrd_box* box = wzrd_box_create(box_data);

	wzrd_box_add_child_free(parent, box->handle);
	box->layer = wzrd_box_get_by_handle(parent)->layer;

	return box->handle;
}

wzrd_handle wzrd_widget(wzrd_box box_data, wzrd_handle parent)
{
	// TODO: Is this the right place for this?
	//for (int i = 0; i < canvas->boxes_count;++i)
	{
		wzrd_style style = wzrd_style_get(box_data.style);
		box_data.x_internal = style.x;
		box_data.y_internal = style.y;
		box_data.w_internal = style.w;
		box_data.h_internal = style.h;
	}

	wzrd_box* box = wzrd_box_create(box_data);
	box->layer = wzrd_box_get_by_handle(parent)->layer;

	wzrd_box_add_child(parent, box->handle);

	return box->handle;
}

void wzrd_box_close()
{

}

void wzrd_box_end() {
#if 0
	canvas->boxes_in_stack_count--;
	//WZRD_ASSERT(canvas->boxes_in_stack_count >= 0);

	wzrd_box* current_box = wzrd_box_get_from_top_of_stack();
	Crate* current_crate = EguiGetCurrentWindow();

	//set to previous panel
	wzrd_box* previous_box = wzrd_box_get_parent();
	wzrd_style previous_box_style = wzrd_style_get(previous_box->style);
	if (previous_box) {

		// Handle fitting
		if (previous_box_style.fit_h) {
			WZRD_ASSERT(current_box->h_internal);
			WZRD_ASSERT(current_box->w_internal);

			if (!previous_box_style.row_mode) {
				previous_box->h_internal += current_box->h_internal;
			}
			else {
				if (current_box->h_internal > previous_box->h_internal) {
					previous_box->h_internal += current_box->h_internal;
				}
			}
		}
		if (previous_box_style.fit_w) {
			WZRD_ASSERT(current_box->h_internal);
			WZRD_ASSERT(current_box->w_internal);

			if (previous_box_style.row_mode) {
				previous_box->w_internal += current_box->w_internal;
			}
			else {
				if (current_box->w_internal > previous_box->w_internal) {
					previous_box->w_internal += current_box->w_internal;
				}
			}
		}
	}

	current_crate->box_stack_count--;
	WZRD_ASSERT(current_crate->box_stack_count >= 0);
#endif
}

//void wzrd_box_do(wzrd_box box) {
//	wzrd_box_begin(box);
//	wzrd_box_end();
//}

bool IsClicked() {
	if (wzrd_handle_is_equal(wzrd_box_get_from_top_of_stack()->handle, canvas->clicked_item)) {
		return true;
	}

	return false;
}

bool IsHovered() {
	if (wzrd_handle_is_equal(wzrd_box_get_from_top_of_stack()->handle, canvas->hovered_item)) {
		return true;
	}

	return false;
}

void wzrd_texture_add(wzrd_texture texture, wzrd_v2 size, wzrd_handle parent) {
	wzrd_item_add((Item) {
		.type = ItemType_Texture,
			.size = size,
			.val = { .texture = texture }
	}, parent);
}

void wzrd_crate_begin(int layer, wzrd_box box) {

	wzrd_style box_style = wzrd_style_get(box.style);
	(void)box_style;

	box.type = wzrd_box_type_crate;
	canvas->total_num_windows++;
	canvas->current_crate_index++;

	Crate* current_window = EguiGetCurrentWindow();

	// Set new window
	*current_window = (Crate){ .layer = layer, .index = canvas->current_crate_index, };

	// Begin drawing panel
	box.layer = layer;
	//wzrd_box_begin(box);
}

void wzrd_crate_end() {
#if 0
	// Count number of windows for debugging
	canvas->total_num_windows--;

	wzrd_box_end();

	if (canvas->current_crate_index > 0) {
		//Crate* previous_window = EguiGetPreviousWindow();

		//WZRD_ASSERT(previous_window->layer != 0);
	}

	canvas->current_crate_index--;
	WZRD_ASSERT(canvas->current_crate_index >= -1);
#endif
}

void wzrd_crate(int window_id, wzrd_box box) {
	wzrd_crate_begin(window_id, box);
	wzrd_crate_end();
}

wzrd_handle wzrd_begin_ex(wzrd_canvas* gui, wzrd_box box, int layer)
{

	WZRD_UNUSED(gui);

	canvas->boxes_count = 0;
	canvas->layer = layer;
	canvas->input_box_timer += 16.7f;

	// Empty box
	canvas->boxes[canvas->boxes_count++] = (wzrd_box){ 0 };

	// Zero-out boxes
	for (int i = 0; i < MAX_NUM_BOXES; ++i)
	{
		canvas->boxes[i] = (wzrd_box){ 0 };
	}

	// Window
	wzrd_handle h = wzrd_widget(box, (wzrd_handle) { 0 });

	return h;
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


wzrd_handle wzrd_begin(wzrd_canvas* gui, wzrd_rect_struct window, void (*get_string_size)(char*, int*, int*), int layer, wzrd_v2 mouse_pos, wzrd_state mouse_left, wzrd_keyboard_keys keys, bool enable_input)
{
	canvas = gui;

	gui->keyboard_keys = keys;
	gui->mouse_left = mouse_left;
	gui->mouse_pos = mouse_pos;
	gui->mouse_delta.x = gui->mouse_pos.x - gui->previous_mouse_pos.x;
	gui->mouse_delta.y = gui->mouse_pos.y - gui->previous_mouse_pos.y;
	gui->get_string_size = get_string_size;
	gui->window = window;
	gui->enable_input = enable_input;
	canvas->styles_count = 0;

	wzrd_style_create((wzrd_style) { .color = EGUI_LIGHTGRAY });

	//wzrd_v2 v = { FONT_WIDTH * (int)str.len + 12, WZRD_FONT_HEIGHT + 12 };
	gui->button_style = wzrd_style_create((wzrd_style) {
		.w = 200,
			.h = 32,
			.pad_left = 2,
			.pad_right = 2,
			.pad_bottom = 2,
			.pad_top = 2,
			.color = EGUI_LIGHTGRAY,
			.center_x = true, .center_y = true,
	});

	wzrd_box box =
		(wzrd_box){
		.disable_input = true,
		.style = wzrd_style_create((wzrd_style) {
			//.pad_left = 5, .pad_top = 5,
			.x = window.x,
			.y = window.y,
			.w = window.w,
			.h = window.h,
			/*.pad_right = 5,
			.pad_bottom = 5,
			.child_gap = 5,*/
				//.border_type = style.window_border_type,
				//.color = style.background_color
		})
	};

	wzrd_handle h = wzrd_begin_ex(gui, box, layer);

	return h;
}


void EguiRectDraw(wzrd_draw_commands_buffer* buffer, wzrd_rect_struct rect, wzrd_color color, int z) {

	WZRD_ASSERT(rect.w > 0);
	WZRD_ASSERT(rect.h > 0);

	wzrd_draw_command command = (wzrd_draw_command){
		.type = DrawCommandType_Rect,
		.dest_rect = rect,
		.color = color,
		.z = z
	};

	WZRD_ASSERT(buffer->count < MAX_NUM_DRAW_COMMANDS - 1);
	buffer->commands[buffer->count++] = command;

}

wzrd_box* wzrd_box_find(wzrd_canvas* c, wzrd_str name)
{
	// TODO: optimize	
	unsigned int hash = wzrd_hash(name);

	for (int i = 0; i < c->boxes_count; ++i)
	{
		if (hash == c->boxes[i].handle.handle)
		{
			return c->boxes + i;
		}
	}

	return &c->boxes[0];
}


wzrd_box* wzrd_box_get_previous() {
	wzrd_box* result = &canvas->boxes[canvas->boxes_count - 1];

	return result;
}

void wzrd_handle_cursor(wzrd_cursor* cursor)
{
	*cursor = wzrd_cursor_default;

	//if (g_gui->is_hovering)
	{

		wzrd_box* hot_box = wzrd_box_get_by_handle(canvas->hovered_item);
		wzrd_box* active_box = wzrd_box_get_by_handle(canvas->active_item);

		if (hot_box) {
			/*	if (hot_box->type == wzrd_box_type_button) {
					*cursor = wzrd_cursor_hand;
				}*/
		}
		if (active_box) {
			/*	if (active_box->type == wzrd_box_type_button) {
					*cursor = wzrd_cursor_hand;
				}*/
		}
	}
}

void wzrd_handle_border_resize(wzrd_cursor* cursor)
{
	canvas->left_resized_item = (wzrd_handle){ 0 };
	canvas->right_resized_item = (wzrd_handle){ 0 };
	canvas->top_resized_item = (wzrd_handle){ 0 };
	canvas->bottom_resized_item = (wzrd_handle){ 0 };

	for (int i = 0; i < canvas->boxes_count; ++i) {
		wzrd_box* owner = canvas->boxes + i;
		for (int j = 0; j < owner->children_count; ++j) {
			wzrd_box* child = &canvas->boxes[owner->children[j]];
			wzrd_style child_style = wzrd_style_get(child->style);

			int border_size = 2;

			if (!child->type == wzrd_box_type_resizable) continue;

			bool is_inside_left_border =
				canvas->mouse_pos.x >= child->x_internal &&
				canvas->mouse_pos.y >= child->y_internal &&
				canvas->mouse_pos.x < child->x_internal + border_size &&
				canvas->mouse_pos.y < child->y_internal + child->h_internal;
			bool is_inside_right_border =
				canvas->mouse_pos.x >= child->x_internal + child->w_internal - border_size &&
				canvas->mouse_pos.y >= child->y_internal &&
				canvas->mouse_pos.x < child->x_internal + child->w_internal &&
				canvas->mouse_pos.y < child->y_internal + child->h_internal;
			bool is_inside_top_border =
				canvas->mouse_pos.x >= child->x_internal &&
				canvas->mouse_pos.y >= child->y_internal &&
				canvas->mouse_pos.x < child->x_internal + child->w_internal &&
				canvas->mouse_pos.y < child->y_internal + border_size;
			bool is_inside_bottom_border =
				canvas->mouse_pos.x >= child->x_internal &&
				canvas->mouse_pos.y >= child->y_internal + child->h_internal - border_size &&
				canvas->mouse_pos.x < child->x_internal + child->w_internal &&
				canvas->mouse_pos.y < child->y_internal + child->h_internal;

			if (wzrd_handle_is_equal(canvas->hovered_item, child->handle) || wzrd_handle_is_equal(canvas->active_item, child->handle)) {

				if (is_inside_top_border || is_inside_bottom_border) {
					*cursor = wzrd_cursor_vertical_arrow;
				}
				else if (is_inside_left_border || is_inside_right_border) {
					*cursor = wzrd_cursor_horizontal_arrow;
				}
			}

			if (wzrd_handle_is_equal(canvas->active_item, child->handle)) {
				if (is_inside_bottom_border) {
					child_style.color = EGUI_PURPLE;
					canvas->bottom_resized_item = child->handle;
				}
				else if (is_inside_top_border) {
					child_style.color = EGUI_PURPLE;
					canvas->top_resized_item = child->handle;
				}
				else if (is_inside_left_border) {
					child_style.color = EGUI_PURPLE;
					canvas->left_resized_item = child->handle;
				}
				else if (is_inside_right_border) {
					child_style.color = EGUI_PURPLE;
					canvas->right_resized_item = child->handle;
				}


			}
		}
	}
}

void wzrd_handle_input(int* indices, int count)
{
	wzrd_box* hovered_box = canvas->boxes;
	int max_layer = 0;
	canvas->hovered_items_list_count = 0;
	canvas->hovered_boxes_count = 0;
	for (int i = 0; i < count; ++i) {
		wzrd_box* box = canvas->boxes + indices[i];

		wzrd_rect_struct scaled_rect = { box->x_internal, box->y_internal, box->w_internal, box->h_internal };

		bool is_hover = wzrd_v2_is_inside_rect((wzrd_v2) { canvas->mouse_pos.x, canvas->mouse_pos.y },
			scaled_rect);

		if (is_hover && !box->disable_hover)
		{
			assert(canvas->hovered_items_list_count < MAX_NUM_HOVERED_ITEMS);
			canvas->hovered_items_list[canvas->hovered_items_list_count++] = box->handle;
			canvas->hovered_boxes[canvas->hovered_boxes_count++] = *box;

			if (box->layer >= max_layer)
			{
				hovered_box = box;
				max_layer = box->layer;
			}
			else if (box->layer == max_layer)
			{
				if (box->bring_to_front)
				{
					hovered_box = box;
				}
			}
		}
	}



	// ...
	wzrd_box* half_clicked_box = wzrd_box_get_by_handle(canvas->activating_item);
	if (half_clicked_box && canvas->mouse_left == WZRD_ACTIVE)
	{
		canvas->activating_item = (wzrd_handle){ 0 };
	}

	if (canvas->mouse_left == WZRD_DEACTIVATING)
	{
		//canvas->released_item = canvas->dragged_item;
		canvas->dragged_box = (wzrd_box){ 0 };
		canvas->dragged_item = (wzrd_handle){ 0 };

		canvas->clean = false;
	}

	if (canvas->mouse_left == WZRD_INACTIVE)
	{
		canvas->deactivating_item = (wzrd_handle){ 0 };
	}

	if (wzrd_handle_is_valid(hovered_box->handle)) {
		if (!wzrd_handle_is_equal(hovered_box->handle, canvas->hovered_item))
		{
			canvas->clean = false;
		}

		canvas->hovered_item = hovered_box->handle;
	}
	else {
		canvas->hovered_item = (wzrd_handle){ 0 };
	}

	wzrd_box* hot_box = wzrd_box_get_by_handle(canvas->hovered_item);
	wzrd_box* active_box = wzrd_box_get_by_handle(canvas->active_item);

	if (wzrd_handle_is_valid(active_box->handle)) {

		if (canvas->mouse_left == WZRD_DEACTIVATING) {
			if (wzrd_handle_is_valid(canvas->active_item))
			{
				if (hot_box == active_box)
				{
					canvas->clicked_item = active_box->handle;
					//canvas->selected_item = hot_box->handle;
				}

				canvas->deactivating_item = canvas->active_item;
				canvas->active_item = (wzrd_handle){ 0 };
			}
		}

		canvas->clean = false;
	}

	if (wzrd_handle_is_valid(hot_box->handle)) {

		/*	if (hot_box->type == wzrd_box_type_flat_button)
			{
				hot_box->color = (wzrd_color){ 0, 255, 255, 255 };
			}*/
		if (canvas->mouse_left == EguiActivating) {
			canvas->active_item = hot_box->handle;

			canvas->activating_item = hot_box->handle;

			// Dragging
			WZRD_ASSERT(half_clicked_box);
			if (half_clicked_box->is_draggable) {
				canvas->dragged_item = half_clicked_box->handle;

				canvas->dragged_box = *half_clicked_box;
				//g_gui->dragged_box.x = g_gui->dragged_box.x;
				//g_gui->dragged_box.y = g_gui->dragged_box.y;
				//g_gui->dragged_box.w = g_gui->dragged_box.w;
				//g_gui->dragged_box.h = g_gui->dragged_box.h;
				//g_gui->dragged_box.name = wzrd_str_create("drag");
				//g_gui->dragged_box.absolute_rect = (wzrd_rect){ 0 };
				canvas->dragged_box.disable_hover = true;
			}
		}
	}

	// Clicked item
	if (wzrd_handle_is_valid(canvas->clicked_item) && canvas->mouse_left == WZRD_INACTIVE) {
		canvas->clicked_item = (wzrd_handle){ 0 };

		canvas->clean = false;
	}

	// Input box
	if (wzrd_handle_is_valid(canvas->clicked_item)) {
		wzrd_box* clicked_box = wzrd_box_get_by_handle(canvas->clicked_item);
		WZRD_ASSERT(clicked_box);
		/*	if (clicked_box->type == wzrd_box_type_input_box) {
				canvas->active_input_box = clicked_box->handle;
			}
			else {
				canvas->active_input_box = (wzrd_handle){ 0 };
			}*/
	}
}

bool wzrd_box_is_half_clicked(wzrd_box* box) {
	if (wzrd_handle_is_equal(box->handle, canvas->activating_item)) {
		return true;
	}

	return false;
}

void wzrd_box_bring_to_front(wzrd_box* box, void* data)
{
	(void)data;
	box->bring_to_front = true;
}

void wzrd_end(wzrd_cursor* cursor, wzrd_draw_commands_buffer* buffer, wzrd_str* debug_str)
{
	(void)cursor;

	// Dragging
	if (wzrd_handle_is_valid(canvas->dragged_box.handle))
	{
		canvas->dragged_box.x_internal += canvas->mouse_delta.x;
		canvas->dragged_box.y_internal += canvas->mouse_delta.y;

		wzrd_crate(1, canvas->dragged_box);
	}

	wzrd_crate_end();

	// Calculate size
	for (int i = 1; i < canvas->boxes_count; ++i)
	{
		wzrd_box* parent = &canvas->boxes[i];
		wzrd_style parent_style = wzrd_style_get(parent->style);

		WZRD_ASSERT(parent->w_internal > 0);
		WZRD_ASSERT(parent->h_internal > 0);

		// Calculate the total size of the children
		int available_w = 0, available_h = 0, children_w = 0, children_h = 0;
		{
			for (int j = 0; j < parent->children_count; ++j)
			{
				wzrd_box* child = &canvas->boxes[parent->children[j]];
				children_w += child->w_internal;
				children_h += child->h_internal;
			}

			available_w = parent->w_internal - parent_style.pad_left - parent_style.pad_right - 4 * WZRD_BORDER_SIZE;
			available_h = parent->h_internal - parent_style.pad_top - parent_style.pad_bottom - 4 * WZRD_BORDER_SIZE;

			if (parent->children_count)
			{
				if (parent_style.row_mode)
					available_w -= parent_style.child_gap * (parent->children_count - 1);
				else
					available_h -= parent_style.child_gap * (parent->children_count - 1);
			}
		}

		// Handle growing
		for (int j = 0; j < parent->children_count; ++j)
		{
			wzrd_box* child = &canvas->boxes[parent->children[j]];
			wzrd_style child_style = wzrd_style_get(child->style);

			if (child->w_internal == 0 && !child_style.fit_w)
			{
				if (parent_style.row_mode) {
					child->w_internal = available_w - children_w;
				}
				else {
					child->w_internal = available_w;
				}

				WZRD_ASSERT(child->w_internal > 0);
			}

			if (child->h_internal == 0 && !child_style.fit_h)
			{
				if (!parent_style.row_mode)
				{
					child->h_internal = available_h - children_h;
				}
				else
				{
					child->h_internal = available_h;
				}

				WZRD_ASSERT(child->h_internal > 0);
			}

			if (child_style.best_fit)
			{
				int ratio_a = parent->w_internal / parent->h_internal;
				int ratio_b = child->w_internal / child->h_internal;
				int ratio = 0;

				//if (ratio_b >= ratio_a)
				if (wzrd_int_compare(ratio_b, ratio_a) >= 0)
				{
					ratio = (int)(parent->w_internal / child->w_internal);
				}
				else
				{
					ratio = (int)(parent->h_internal / child->h_internal);
				}

				child->w_internal = child->w_internal * ratio;
				child->h_internal = child->h_internal * ratio;

			}

			WZRD_ASSERT(child->w_internal > 0);
			WZRD_ASSERT(child->h_internal > 0);
		}

		// Calcuate size for free children
		for (int j = 0; j < parent->free_children_count; ++j)
		{
			wzrd_box* child = &canvas->boxes[parent->free_children[j]];

			if (!child->w_internal)
			{
				child->w_internal = parent->w_internal;
			}
			if (!child->h_internal)
			{
				child->h_internal = parent->h_internal;
			}

			WZRD_ASSERT(child->w_internal > 0);
			WZRD_ASSERT(child->h_internal > 0);
		}

	}

	// Calculate content size
	for (int i = 1; i < canvas->boxes_count; ++i)
	{
		wzrd_box* parent = canvas->boxes + i;
		wzrd_style parent_style = wzrd_style_get(parent->style);

		if (!parent->clip)
			continue;

		parent->content_size.x = 2 * WZRD_BORDER_SIZE;
		parent->content_size.y = 2 * WZRD_BORDER_SIZE;

		if (parent_style.row_mode)
		{
			parent->content_size.x += (parent->children_count + 1) * parent_style.child_gap;
			parent->content_size.y += parent_style.child_gap;
		}
		else
		{
			parent->content_size.y += (parent->children_count + 1) * parent_style.child_gap;
			parent->content_size.x += parent_style.child_gap;
		}

		int max_child_w = 0, max_child_h = 0;
		for (int j = 0; j < parent->children_count; ++j)
		{
			wzrd_box child = canvas->boxes[parent->children[j]];

			if (parent->h_internal > max_child_h)
			{
				max_child_h = child.h_internal;
			}

			if (parent->w_internal > max_child_w)
			{
				max_child_w = child.w_internal;
			}

			if (parent_style.row_mode)
			{
				parent->content_size.x += child.w_internal;
			}
			else
			{
				parent->content_size.y += child.h_internal;
			}
		}

		if (parent_style.row_mode)
		{
			parent->content_size.y += max_child_h;
		}
		else
		{
			parent->content_size.x += max_child_w;
		}

		// Handle scrollbar
		if (parent->clip && parent->scrollbar_x && parent->scrollbar_y)
		{
			int x = parent->x_internal + parent->w_internal - 40;
			int y = parent->y_internal;
			const int button_height = 10;

			// Gray Area
			wzrd_crate(2,
				(wzrd_box) {
				.x_internal = x, .y_internal = y, .w_internal = 40,
					.h_internal = parent->h_internal,
					.style = wzrd_style_create((wzrd_style) {
					.border_type = BorderType_None,
						.color = EGUI_GRAY
				})
			});

			// Top button
			wzrd_crate(2,
				(wzrd_box) {
				.x_internal = x, .y_internal = y, .w_internal = 40,
					.h_internal = button_height,
			});

			// Bottom button
			wzrd_crate(2,
				(wzrd_box) {
				.x_internal = x,
					.y_internal = parent->y_internal + parent->h_internal - button_height,
					.w_internal = 40,
					.h_internal = button_height,
					.style = wzrd_style_create((wzrd_style) {
					.color = EGUI_GRAY,
				})
			});

			// TODO: handle x axis scrollbar
			if (parent->content_size.y > parent->h_internal)
			{
				int h = (parent->h_internal / parent->content_size.y) * (parent->h_internal - 2 * button_height);

				wzrd_crate(2,
					(wzrd_box) {
					.x_internal = x, .y_internal = y + button_height + *parent->scrollbar_y, .w_internal = 40,
						.h_internal = h,
				});

				wzrd_box* box = wzrd_box_get_last();
				if (wzrd_box_is_active(box))
				{
					int new_pos = parent->y_internal + button_height + *parent->scrollbar_y + canvas->mouse_delta.y;
					if (new_pos >= parent->y_internal + button_height && new_pos + box->h_internal < parent->y_internal + parent->h_internal - button_height)
					{
						*parent->scrollbar_y += canvas->mouse_delta.y;
					}
				}

				int ratio = *parent->scrollbar_y / (parent->h_internal - 2 * button_height);
				parent_style.pad_top -= ratio * parent->content_size.y;
			}
			else
			{
				int h = parent->h_internal - 2 * button_height;

				wzrd_crate(2,
					(wzrd_box) {
					.x_internal = x, .y_internal = parent->y_internal + button_height + *parent->scrollbar_y, .w_internal = 40,
						.h_internal = h,
						//.name = wzrd_str_create("scroly")
				});
			}
		}
	}

	// Calculate positions
	for (int i = 1; i < canvas->boxes_count; ++i) {
		wzrd_box* parent = &canvas->boxes[i];
		wzrd_style parent_style = wzrd_style_get(parent->style);

		int x = parent->x_internal + parent_style.pad_left, y = parent->y_internal + parent_style.pad_top;

		x += 2 * WZRD_BORDER_SIZE;
		y += 2 * WZRD_BORDER_SIZE;

		// Center
		int w = 0, h = 0, max_w = 0, max_h = 0;
		for (int j = 0; j < parent->children_count; ++j) {
			wzrd_box* child = &canvas->boxes[parent->children[j]];

			if (child->w_internal > max_w)
				max_w = child->w_internal;

			if (child->h_internal > max_h)
				max_h = child->h_internal;

			if (parent_style.row_mode)
				w += child->w_internal;
			else
				h += child->h_internal;
		}

		if (parent_style.row_mode)
			w += parent_style.child_gap * (parent->children_count - 1);
		else
			h += parent_style.child_gap * (parent->children_count - 1);

		if (parent_style.center_x && parent_style.row_mode) {
			x += (parent->w_internal - 4 * WZRD_BORDER_SIZE - parent_style.pad_left - parent_style.pad_right) / 2 - w / 2;
		}
		if (parent_style.center_y && !parent_style.row_mode) {
			y += (parent->h_internal - 4 * WZRD_BORDER_SIZE - parent_style.pad_top - parent_style.pad_bottom) / 2 - h / 2;
		}

		// Calc positions
		for (int j = 0; j < parent->children_count; ++j) {
			wzrd_box* child = &canvas->boxes[parent->children[j]];

			child->x_internal += x;
			child->y_internal += y;

			if (parent_style.center_y && parent_style.row_mode) {
				child->y_internal += (parent->h_internal - 4 * WZRD_BORDER_SIZE - parent_style.pad_top
					- parent_style.pad_bottom) / 2 - child->h_internal / 2;
			}

			if (parent_style.center_x && !parent_style.row_mode) {
				child->x_internal += (parent->w_internal - 4 * WZRD_BORDER_SIZE -
					parent_style.pad_top - parent_style.pad_bottom) / 2 - child->w_internal / 2;
			}

			if (parent_style.row_mode) {
				x += child->w_internal;
				x += parent_style.child_gap;
			}
			else {
				y += child->h_internal;
				y += parent_style.child_gap;
			}
		}

		// Calculate positions for free children
		for (int j = 0; j < parent->free_children_count; ++j) {
			wzrd_box* child = &canvas->boxes[parent->free_children[j]];

			child->x_internal += parent->x_internal;
			child->y_internal += parent->y_internal;
		}
	}

	// Test child doesn't exceed parent's size
	for (int i = 0; i < canvas->boxes_count; ++i) {
		for (int j = 0; j < canvas->boxes[i].children_count; ++j) {
			wzrd_box* owner = canvas->boxes + i;
			wzrd_box* child = canvas->boxes + canvas->boxes[i].children[j];
			if (!wzrd_is_rect_inside_rect((wzrd_rect_struct) { child->x_internal, child->y_internal, child->w_internal, child->h_internal }, (wzrd_rect_struct) { owner->x_internal, owner->y_internal, owner->w_internal, owner->h_internal })) {
				//owner->color = EGUI_ORANGE;
				//child->color = EGUI_RED;
			}
		}
	}

	// Bring to front
	if (wzrd_handle_is_valid(canvas->active_item))
	{
		wzrd_box_tree_apply(wzrd_box_get_by_handle(canvas->active_item)->index, 0, wzrd_box_bring_to_front);
	}

	// Sort
	int boxes_indices[MAX_NUM_BOXES] = { 0 };

	for (int i = 0; i < canvas->boxes_count; ++i)
	{
		boxes_indices[i] = i;
	}

	qsort(boxes_indices, canvas->boxes_count, sizeof(int), wzrd_compare_boxes);

	// Mouse interaction
	if (canvas->enable_input)
	{
		wzrd_handle_input(boxes_indices, canvas->boxes_count);
		/*wzrd_handle_cursor(cursor);
		wzrd_handle_border_resize(cursor);*/
	}
	else {
		if (canvas->mouse_left == WZRD_DEACTIVATING)
		{
			//canvas->released_item = canvas->dragged_item;
			//canvas->released_item = canvas->dragged_item;
			canvas->dragged_box = (wzrd_box){ 0 };
			canvas->dragged_item = (wzrd_handle){ 0 };
			canvas->active_item = (wzrd_handle){ 0 };
		}
	}

	canvas->previous_mouse_pos = canvas->mouse_pos;

	// Draw
	{
		buffer->count = 0;
		for (int i = 1; i < canvas->boxes_count; ++i) {

			wzrd_box box = canvas->boxes[boxes_indices[i]];
			wzrd_style box_style = wzrd_style_get(box.style);

			if (box.type == wzrd_box_type_crate)
			{
				WZRD_ASSERT(buffer->count < MAX_NUM_DRAW_COMMANDS - 1);
				buffer->commands[buffer->count++] = (wzrd_draw_command){
					.type = DrawCommandType_Clip,
					.dest_rect = canvas->window,
					.z = box.layer
				};
			}

			EguiRectDraw(buffer, (wzrd_rect_struct) {
				.x = box.x_internal,
					.y = box.y_internal,
					.h = box.h_internal,
					.w = box.w_internal
			},
				box_style.color,
				box.layer);

			// Borders (1215 x 810)
			int line_size = WZRD_BORDER_SIZE;

			wzrd_rect_struct top0 = (wzrd_rect_struct){ box.x_internal, box.y_internal, box.w_internal - line_size, line_size };
			wzrd_rect_struct left0 = (wzrd_rect_struct){ box.x_internal, box.y_internal, line_size, box.h_internal };

			wzrd_rect_struct top1 = (wzrd_rect_struct){ box.x_internal + line_size, box.y_internal + line_size, box.w_internal - 3 * line_size, line_size };
			wzrd_rect_struct left1 = (wzrd_rect_struct){ box.x_internal + line_size, box.y_internal + line_size, line_size, box.h_internal - line_size };

			wzrd_rect_struct bottom0 = (wzrd_rect_struct){ box.x_internal, box.y_internal + box.h_internal - line_size, box.w_internal, line_size };
			wzrd_rect_struct right0 = (wzrd_rect_struct){ box.x_internal + box.w_internal - line_size, box.y_internal, line_size, box.h_internal };

			wzrd_rect_struct bottom1 = (wzrd_rect_struct){ box.x_internal + 1 * line_size, box.y_internal + box.h_internal - 2 * line_size, box.w_internal - 3 * line_size, line_size };
			wzrd_rect_struct right1 = (wzrd_rect_struct){ box.x_internal + box.w_internal - 2 * line_size, box.y_internal + 1 * line_size, line_size, box.h_internal - 2 * line_size };

			wzrd_is_rect_inside_rect(top0, (wzrd_rect_struct) { box.x_internal, box.y_internal, box.w_internal, box.h_internal });
			wzrd_is_rect_inside_rect(left0, (wzrd_rect_struct) { box.x_internal, box.y_internal, box.w_internal, box.h_internal });
			wzrd_is_rect_inside_rect(top1, (wzrd_rect_struct) { box.x_internal, box.y_internal, box.w_internal, box.h_internal });
			wzrd_is_rect_inside_rect(left1, (wzrd_rect_struct) { box.x_internal, box.y_internal, box.w_internal, box.h_internal });
			wzrd_is_rect_inside_rect(bottom0, (wzrd_rect_struct) { box.x_internal, box.y_internal, box.w_internal, box.h_internal });
			wzrd_is_rect_inside_rect(right0, (wzrd_rect_struct) { box.x_internal, box.y_internal, box.w_internal, box.h_internal });
			wzrd_is_rect_inside_rect(bottom1, (wzrd_rect_struct) { box.x_internal, box.y_internal, box.w_internal, box.h_internal });
			wzrd_is_rect_inside_rect(right1, (wzrd_rect_struct) { box.x_internal, box.y_internal, box.w_internal, box.h_internal });

			if (box_style.border_type == BorderType_Custom) {
				// Draw top and left lines
				EguiRectDraw(buffer, top0, canvas->style.b0, box.layer);
				EguiRectDraw(buffer, left0, canvas->style.b0, box.layer);
				EguiRectDraw(buffer, top1, canvas->style.b1, box.layer);
				EguiRectDraw(buffer, left1, canvas->style.b1, box.layer);

				// Draw bottom and right lines
				EguiRectDraw(buffer, bottom0, canvas->style.b2, box.layer);
				EguiRectDraw(buffer, right0, canvas->style.b2, box.layer);
				EguiRectDraw(buffer, bottom1, canvas->style.b3, box.layer);
				EguiRectDraw(buffer, right1, canvas->style.b3, box.layer);
			}
			else if (box_style.border_type == BorderType_Default) {
				// Draw top and left lines
				EguiRectDraw(buffer, top0, EGUI_WHITE2, box.layer);
				EguiRectDraw(buffer, left0, EGUI_WHITE2, box.layer);
				EguiRectDraw(buffer, top1, EGUI_LIGHTGRAY, box.layer);
				EguiRectDraw(buffer, left1, EGUI_LIGHTGRAY, box.layer);

				// Draw bottom and right lines
				EguiRectDraw(buffer, bottom0, EGUI_BLACK, box.layer);
				EguiRectDraw(buffer, right0, EGUI_BLACK, box.layer);
				EguiRectDraw(buffer, bottom1, EGUI_GRAY, box.layer);
				EguiRectDraw(buffer, right1, EGUI_GRAY, box.layer);
			}
			else if (box_style.border_type == BorderType_Clicked) {
				// Draw top and left lines
				EguiRectDraw(buffer, top0, EGUI_BLACK, box.layer);
				EguiRectDraw(buffer, left0, EGUI_BLACK, box.layer);
				EguiRectDraw(buffer, top1, EGUI_GRAY, box.layer);
				EguiRectDraw(buffer, left1, EGUI_GRAY, box.layer);

				// Draw bottom and right lines
				EguiRectDraw(buffer, bottom0, EGUI_WHITE2, box.layer);
				EguiRectDraw(buffer, right0, EGUI_WHITE2, box.layer);
				EguiRectDraw(buffer, bottom1, EGUI_LIGHTESTGRAY, box.layer);
				EguiRectDraw(buffer, right1, EGUI_LIGHTESTGRAY, box.layer);
			}
			else if (box_style.border_type == BorderType_InputBox) {
				// Draw top and left lines
				EguiRectDraw(buffer, top0, EGUI_GRAY, box.layer);
				EguiRectDraw(buffer, left0, EGUI_GRAY, box.layer);
				EguiRectDraw(buffer, top1, EGUI_BLACK, box.layer);
				EguiRectDraw(buffer, left1, EGUI_BLACK, box.layer);

				// Draw bottom and right lines
				EguiRectDraw(buffer, bottom0, EGUI_WHITE2, box.layer);
				EguiRectDraw(buffer, right0, EGUI_WHITE2, box.layer);
				EguiRectDraw(buffer, bottom1, EGUI_LIGHTESTGRAY, box.layer);
				EguiRectDraw(buffer, right1, EGUI_LIGHTESTGRAY, box.layer);
			}
			else if (box_style.border_type == BorderType_Black) {
				// Draw top and left lines
				EguiRectDraw(buffer, top0, EGUI_BLACK, box.layer);
				EguiRectDraw(buffer, left0, EGUI_BLACK, box.layer);
				EguiRectDraw(buffer, top1, EGUI_WHITE2, box.layer);
				EguiRectDraw(buffer, left1, EGUI_WHITE2, box.layer);

				// Draw bottom and right lines
				EguiRectDraw(buffer, bottom0, EGUI_BLACK, box.layer);
				EguiRectDraw(buffer, right0, EGUI_BLACK, box.layer);
				EguiRectDraw(buffer, bottom1, EGUI_WHITE2, box.layer);
				EguiRectDraw(buffer, right1, EGUI_WHITE2, box.layer);
			}
			else if (box_style.border_type == BorderType_BottomLine) {
				EguiRectDraw(buffer, bottom0, EGUI_WHITE2, box.layer);
				EguiRectDraw(buffer, bottom1, EGUI_GRAY, box.layer);
			}
			else if (box_style.border_type == BorderType_LeftLine) {
				EguiRectDraw(buffer, left0, EGUI_GRAY, box.layer);
				EguiRectDraw(buffer, left1, EGUI_WHITE2, box.layer);
			}

			// Draw content
			for (int j = 0; j < box.items_count; ++j) {
				Item item = box.items[j];
				wzrd_draw_command command = { 0 };

				if (item.size.x == 0)
					item.size.x = box.w_internal;
				if (item.size.y == 0)
					item.size.y = box.h_internal;

				if (item.type == wzrd_item_type_str) {
					command = (wzrd_draw_command){
						.type = DrawCommandType_String,
							.str = item.val.str,
							.dest_rect = (wzrd_rect_struct){ box.x_internal, box.y_internal,
							box.w_internal, box.h_internal },
							.color = item.color,
							.z = box.layer
					};
				}
				else if (item.type == ItemType_Texture) {
					command = (wzrd_draw_command){
						.type = DrawCommandType_Texture,
						.dest_rect = (wzrd_rect_struct){box.x_internal, box.y_internal, box.w_internal, box.h_internal},
						.src_rect = (wzrd_rect_struct) {0, 0, (int)item.val.texture.w, (int)item.val.texture.h},
						.texture = item.val.texture,
						.z = box.layer
					};

					if (item.scissor) {
						command.src_rect = (wzrd_rect_struct){ 0, 0, command.dest_rect.w, command.dest_rect.h };
					}
				}
				else if (item.type == ItemType_Line) {
					command = (wzrd_draw_command){
						.type = DrawCommandType_Line,
							.dest_rect = (wzrd_rect_struct){
								box.x_internal + item.val.rect.x,
								box.y_internal + item.val.rect.y,
								box.x_internal + item.val.rect.w,
								box.y_internal + item.val.rect.h
						},
							.color = item.color,
						.z = box.layer

					};
				}
				else if (item.type == ItemType_HorizontalDottedLine) {
					command = (wzrd_draw_command){
						.type = DrawCommandType_HorizontalLine,
							.dest_rect = (wzrd_rect_struct){
								box.x_internal,
								box.y_internal + box.h_internal / 2,
								box.x_internal + box.w_internal,
								box.y_internal + box.h_internal / 2
						},
						.z = box.layer

					};
				}
				else if (item.type == ItemType_LeftHorizontalDottedLine) {
					command = (wzrd_draw_command){
						.type = DrawCommandType_HorizontalLine,
							.dest_rect = (wzrd_rect_struct){
								box.x_internal,
								box.y_internal + box.h_internal / 2,
								box.x_internal + box.w_internal / 2,
								box.y_internal + box.h_internal / 2
						},
						.z = box.layer

					};
				}
				else if (item.type == ItemType_RightHorizontalDottedLine) {
					command = (wzrd_draw_command){
						.type = DrawCommandType_HorizontalLine,
							.dest_rect = (wzrd_rect_struct){
								box.x_internal + box.w_internal / 2,
								box.y_internal + box.h_internal / 2,
								box.x_internal + box.w_internal,
								box.y_internal + box.h_internal / 2
						},
						.z = box.layer

					};
				}
				else if (item.type == ItemType_VerticalDottedLine) {
					command = (wzrd_draw_command){
						.type = DrawCommandType_VerticalLine,
							.dest_rect = (wzrd_rect_struct){
								box.x_internal + box.w_internal / 2,
								box.y_internal,
								box.x_internal + box.w_internal / 2 + box.w_internal,
								box.y_internal + box.h_internal
						},
						.z = box.layer

					};
				}
				else if (item.type == ItemType_TopVerticalDottedLine) {
					command = (wzrd_draw_command){
						.type = DrawCommandType_VerticalLine,
							.dest_rect = (wzrd_rect_struct){
								box.x_internal + box.w_internal / 2,
								box.y_internal,
								box.x_internal + box.w_internal / 2 + box.w_internal,
								box.y_internal + box.h_internal / 2
						},
						.z = box.layer
					};
				}
				else if (item.type == ItemType_BottomVerticalDottedLine) {
					command = (wzrd_draw_command){
						.type = DrawCommandType_VerticalLine,
							.dest_rect = (wzrd_rect_struct){
								box.x_internal + box.w_internal / 2,
								box.y_internal + box.h_internal / 2,
								box.x_internal + box.w_internal / 2,
								box.y_internal + box.h_internal
						},
						.z = box.layer

					};
				}
				else if (item.type == ItemType_Rect) {
					command = (wzrd_draw_command){
						.type = DrawCommandType_Rect,
							.dest_rect = (wzrd_rect_struct){
								box.x_internal + item.val.rect.x,
								box.y_internal + item.val.rect.y,
								item.val.rect.w,
								item.val.rect.h
						},
							.color = item.color,
						.z = box.layer
					};

					WZRD_ASSERT(command.dest_rect.w > 0);
					WZRD_ASSERT(command.dest_rect.h > 0);
				}

				command.dest_rect.x += item.pad_left;
				command.dest_rect.y += item.pad_top;

				WZRD_ASSERT(command.dest_rect.w >= 0);
				WZRD_ASSERT(command.dest_rect.h >= 0);

				WZRD_ASSERT(buffer->count < MAX_NUM_DRAW_COMMANDS - 1);
				buffer->commands[buffer->count++] = command;
			}

			// Draw clip area
	/*		if (box.type == wzrd_box_type_crate)
			{
				WZRD_ASSERT(buffer->count < MAX_NUM_DRAW_COMMANDS - 1);
				buffer->commands[buffer->count++] = (wzrd_draw_command){
					.type = DrawCommandType_Clip,
					.dest_rect = (wzrd_rect) {box.x_internal + 2 * WZRD_BORDER_SIZE, box.y_internal + 2 * WZRD_BORDER_SIZE, box.w_internal - 4 * WZRD_BORDER_SIZE, box.h_internal - 4 * WZRD_BORDER_SIZE},
					.z = box.layer
				};
			}*/
		}

		canvas->clean = true;
	}

	if (debug_str)
	{
		char buff[1024];
		sprintf_s(buff, 1024, "hovered: %u\nactive: %u\n hovered items:\n", canvas->hovered_item.handle, canvas->active_item.handle);
		for (int i = 0; i < canvas->hovered_items_list_count; ++i)
		{
			char b[32];
			sprintf_s(b, 32, "%u\n", canvas->hovered_items_list[i].handle);
			strcat(debug_str->str, b);
		}
		strcat(debug_str->str, buff);
		debug_str->len = strlen(debug_str->str);
	}
	canvas = 0;
}

wzrd_handle egui_button_raw_begin(wzrd_box box, bool* released, wzrd_handle parent) {

	wzrd_style style = wzrd_style_get(box.style);
	box.type = wzrd_box_type_button;
	box.style = wzrd_style_create(style);

	wzrd_handle handle = wzrd_widget(box, parent);

	if (wzrd_handle_is_equal(handle, canvas->deactivating_item)) {
		*released = true;
	}

	return handle;
}

//bool EguiButtonIcon2(wzrd_texture texture) {
//	bool result = false;
//	bool b1, b2;
//	result |= egui_button_raw_begin((wzrd_box) {
//
//		.style = wzrd_style_create((wzrd_style) { .w = 24, .h = 22, .center_x = true, .center_y = true })
//	}, &b1);
//	{
//		result |= egui_button_raw_begin((wzrd_box) {
//			.style = wzrd_style_create((wzrd_style) {
//				.border_type = BorderType_None, .w = 10, .h = 10
//			}),
//		}, &b2);
//		{
//			wzrd_item_add((Item) {
//				.type = ItemType_Texture,
//					.val = { .texture = texture }
//			});
//		}
//		egui_button_raw_end();
//	}
//	egui_button_raw_end();
//
//	return result;
//}

//bool wzrd_button_sized(wzrd_box box, Item item) {
//	bool result = false;
//	result |= egui_button_raw_begin(box);
//	{
//		result |= egui_button_raw_begin((wzrd_box) {
//			.style = wzrd_style_create((wzrd_style) {
//				.border_type = BorderType_None, .w = 16, .h = 16
//			})
//		});
//		{
//			wzrd_item_add(item);
//		}
//		egui_button_raw_end();
//	}
//	egui_button_raw_end();
//
//	return result;
//}

//bool wzrd_button_icon2(wzrd_box box, wzrd_texture texture) {
//	bool result = false;
//	result |= egui_button_raw_begin(box);
//	{
//		result |= egui_button_raw_begin((wzrd_box) {
//			.style = wzrd_style_create((wzrd_style) { .w = 16, .h = 16, .border_type = BorderType_None, }),
//
//		});
//		{
//			wzrd_item_add((Item) {
//				.type = ItemType_Texture,
//					.val = { .texture = texture }
//			});
//		}
//		egui_button_raw_end();
//	}
//	egui_button_raw_end();
//
//	return result;
//}

//wzrd_rect wzrd_box_get_rect(wzrd_box* box) {
//	if (!box)
//	{
//		return (wzrd_rect) { 0 };
//	}
//
//	wzrd_rect result = (wzrd_rect){ box->x_do_not_touch, box->y_do_not_touch, box->w_do_not_touch, box->h_do_not_touch };
//
//	return result;
//}

bool IsActive() {
	if (wzrd_handle_is_equal(wzrd_box_get_from_top_of_stack()->handle, canvas->active_item)) {
		return true;
	}

	return false;
}

wzrd_handle wzrd_toggle_icon(wzrd_texture texture, bool* active, wzrd_handle parent) {
	bool b = false;
	bool b1, b2;
	wzrd_handle h1, h2;

	h1 = egui_button_raw_begin((wzrd_box) { .style = wzrd_style_create((wzrd_style) { .w = 24, .h = 22, .center_x = true, .center_y = true }) },
		& b1, parent);

	h2 = egui_button_raw_begin((wzrd_box) {
		.style = wzrd_style_create((wzrd_style) { .w = 16, .h = 16, .border_type = BorderType_None, }),
	}, & b2, h1);

	wzrd_item_add((Item) {
		.type = ItemType_Texture,
			.val = { .texture = texture }
	}, h2);

	b |= b1;
	b |= b2;

	if (b)
	{
		*active = !*active;
	}

	bool a1 = wzrd_box_is_active(&canvas->boxes[canvas->boxes_count - 1]);
	bool a2 = wzrd_box_is_active(&canvas->boxes[canvas->boxes_count - 2]);

	if (*active || a1 || a2) {
		//wzrd_box_get_from_top_of_stack()->border_type = BorderType_Clicked;
	}

	return h1;

}

wzrd_handle wzrd_button_icon(wzrd_texture texture, bool* result, wzrd_handle parent) {
	bool active = false;
	wzrd_handle h1, h2;
	bool b1, b2;
	h1 = egui_button_raw_begin((wzrd_box) {
		.style = wzrd_style_create((wzrd_style) { .w = 24, .h = 22, .center_x = true, .center_y = true })
	}, & b1, parent);
	active = IsActive();

	h2 = egui_button_raw_begin((wzrd_box) {
		.style = wzrd_style_create((wzrd_style) { .w = 16, .h = 16, .border_type = BorderType_None, }),
	}, & b2, h1);

	active |= IsActive();

	wzrd_box_add_child(h1, h2);

	wzrd_item_add((Item) {
		.type = ItemType_Texture,
			.val = { .texture = texture }
	}, h2);

	if (active) {
		//wzrd_box_get_from_top_of_stack()->border_type = BorderType_Clicked;
	}

	*result |= b1;
	*result |= b2;

	return h1;
}

wzrd_handle wzrd_command_toggle(wzrd_str str, bool* active, wzrd_handle parent) {
	bool b1 = false, b2 = false, a1 = false, a2 = false;
	wzrd_handle h1, h2;
	wzrd_box box = (wzrd_box){
		.style = wzrd_style_create((wzrd_style) {
			.center_x = true, .center_y = true, .w = 200, .h = 32, .color = EGUI_LIGHTGRAY
})
	};
	h1 = egui_button_raw_begin(box, &b1, parent);
	h2 = egui_button_raw_begin((wzrd_box) {
		.style = wzrd_style_create((wzrd_style) {
			.border_type = BorderType_None,
				.w = (int)str.len * FONT_WIDTH, .h = WZRD_FONT_HEIGHT
		}),
	}, & b2, h1);
	{
		wzrd_text_add(str, h2);
	}

	if (b1 || b2)
	{
		*active = !*active;
	}

	a1 = wzrd_box_is_active(&canvas->boxes[canvas->boxes_count - 1]);
	a2 = wzrd_box_is_active(&canvas->boxes[canvas->boxes_count - 2]);

	if (*active || a1 || a2)
	{
		wzrd_box* b = wzrd_box_get_by_handle(h1);
		wzrd_style style = wzrd_style_get(b->style);
		style.border_type = BorderType_Clicked;
		wzrd_box_get_by_handle(h1)->style = wzrd_style_create(style);
	}

	return h1;
}

wzrd_handle egui_button_raw_begin_on_half_click(wzrd_box box, bool* b, wzrd_handle parent) {
	wzrd_handle h = wzrd_widget(box, parent);

	if (wzrd_handle_is_equal(wzrd_box_get_from_top_of_stack()->handle, canvas->activating_item)) {
		*b = true;
	}

	return h;
}

bool wzrd_label_button_half_click(wzrd_box box, wzrd_str str, wzrd_handle parent)
{
	bool b1 = false, b2 = false;
	wzrd_handle h1, h2;
	h1 = egui_button_raw_begin_on_half_click(box, &b1, parent);

	h2 = egui_button_raw_begin_on_half_click((wzrd_box) {
		.style = wzrd_style_create((wzrd_style) {
			.border_type = BorderType_None,
				.w = (int)str.len * FONT_WIDTH, .h = WZRD_FONT_HEIGHT,
		})
	}, & b2, h1);

	wzrd_text_add(str, h2);

	if (b1 || b2)
		return true;
	return false;
}

#if 0
bool EguiButton2(wzrd_box box, wzrd_str str, wzrd_color color) {
	(void)color;
	bool flag = false;
	bool result = egui_button_raw_begin(box);
	{
		if (IsHovered()) {
			flag = true;
		}

		result |= egui_button_raw_begin((wzrd_box) {
			.style = wzrd_style_create((wzrd_style) {
				.border_type = BorderType_None,
					.color = EGUI_WHITE,
			})
		});
		{
			if (IsHovered()) {
				flag = true;
			}

			wzrd_text_add(str);
			wzrd_item_add((Item) { .type = wzrd_item_type_str, .val = { .str = str } });
		}
		egui_button_raw_end();
	}
	egui_button_raw_end();

	if (flag) {
		//canvas->boxes[canvas->boxes_count - 1].color = EGUI_DARKBLUE;
		//canvas->boxes[canvas->boxes_count - 2].color = EGUI_DARKBLUE;
	}

	return result;
}
#endif

//bool wzrd_button_raw(wzrd_box box)
//{
//	bool result = egui_button_raw_begin(box);
//	egui_button_raw_end();
//
//	return result;
//}

void wzrd_item_add(Item item, wzrd_handle box) {
	//wzrd_box* box = wzrd_box_get_from_top_of_stack();
	wzrd_box* b = wzrd_box_get_by_handle(box);
	WZRD_ASSERT(b->items_count < MAX_NUM_ITEMS - 1);
	b->items[b->items_count++] = item;
}

void wzrd_label(wzrd_str str, wzrd_handle parent) {
	int w = 0, h = 0;
	canvas->get_string_size(str.str, &w, &h);

	wzrd_handle widget = wzrd_widget((wzrd_box) {

		.style = wzrd_style_create((wzrd_style) {
			.w = w,
				.h = h,
				.border_type = BorderType_None,
				.color = canvas->style.background_color
		})
	}, parent);
	wzrd_text_add(str, widget);
}

bool wzrd_is_active() {
	bool result = false;
	if (wzrd_handle_is_equal(wzrd_box_get_from_top_of_stack()->handle, canvas->active_item)) {
		result = true;
	}

	return result;
}

void wzrd_input_box(char* str, int* len, int max_num_keys, wzrd_handle parent) {

	wzrd_handle p1 = wzrd_widget((wzrd_box) {
		.type = wzrd_box_type_input_box,
			.style = wzrd_style_create((wzrd_style) {
			.w = FONT_WIDTH * (int)max_num_keys + 8,
				.h = WZRD_FONT_HEIGHT + 8,
				.color = EGUI_WHITE,
				.pad_left = 2,
				.pad_top = 2,
				.pad_bottom = 2,
				.pad_right = 2,
				.center_x = true,
				.center_y = true,
				.border_type = BorderType_InputBox,
		})
	}, parent);
	{
		wzrd_handle p2 = wzrd_widget((wzrd_box) {
			.type = wzrd_box_type_input_box,
				.style = wzrd_style_create((wzrd_style) {
				.w = FONT_WIDTH * (int)max_num_keys, .h = WZRD_FONT_HEIGHT,
					.border_type = BorderType_None, .color = EGUI_WHITE
			}),
		}, p1);
		{
#if 1
			//wzrd_str str2 = *str;

			if (wzrd_handle_is_equal(wzrd_box_get_from_top_of_stack()->handle, canvas->active_input_box)) {

				wzrd_style style = wzrd_style_get(wzrd_box_get_last()->style);
				style.color = (wzrd_color){ 255, 230, 230, 255 };
				wzrd_box_get_last()->style = wzrd_style_create(style);

				for (int i = 0; i < canvas->keyboard_keys.count; ++i) {
					wzrd_keyboard_key key = canvas->keyboard_keys.keys[i];

					if (key.val == '\b' &&
						*len > 0 &&
						(key.state == WZRD_ACTIVE || key.state == EguiActivating)) {
						str[*len - 1] = 0;
						*len--;
					}
					else if ((key.state == EguiActivating || key.state == WZRD_ACTIVE) &&
						((key.val <= 'z' && key.val >= 'a') || (key.val <= '9' && key.val >= '0')) &&
						(isgraph(key.val) || key.val == ' ') &&
						*len < (size_t)max_num_keys - 1 &&
						*len < 127) {
						char s[2] = { [0] = key.val };
						strcat_s(str, 128, s);
					}
				}

#if 0
				static bool show_caret;

				/*		if (g_gui->input_box_timer - g_gui->time > 500) {
							g_gui->input_box_timer = 0;
							show_caret = !show_caret;
						}*/

				if (show_caret) {
					str2 = *str;
					wzrd_str caret = wzrd_str_create("|");
					//wzrd_str_concat(&str2, caret);
				}
#endif
			}

			/*if (wzrd_handle_equal(g_gui->active_input_box, wzrd_box_get_current()->name)) {
				wzrd_box_get_current()->color = EGUI_PINK;
			}*/

			//wzrd_v2 size = { FONT_WIDTH * max_num_keys + 4, WZRD_FONT_HEIGHT + 4 };
			*len = (int)strlen(str);
			wzrd_text_add((wzrd_str) { .str = str, .len = *len }, p2);
#endif
		}
	}
}

wzrd_str wzrd_str_create(char* str)
{
	wzrd_str result =
	{
		.str = str,
		.len = strlen(str)
	};

	return result;
}

wzrd_handle wzrd_label_button(wzrd_str str, bool* result, wzrd_handle parent) {
	wzrd_v2 v = { FONT_WIDTH * (int)str.len, WZRD_FONT_HEIGHT };
	wzrd_handle h = wzrd_widget((wzrd_box) {

		.type = wzrd_box_type_button,

			.style = wzrd_style_create((wzrd_style) {
			.border_type = BorderType_None,
				.w = v.x,
				.h = v.y,
				//.color = wzrd_box_get_from_top_of_stack()->color,
		})
			//.flat_button = true,
			//.is_button = true
	}, parent);
	{
		wzrd_text_add(str, h);
	}

	if (wzrd_box_is_half_clicked(wzrd_box_get_last()))
	{
		*result = true;
	}

	return h;
}

void wzrd_label_button_end() {
	wzrd_box_end();
}

//bool wzrd_label_button(wzrd_str str) {
//	bool result = wzrd_label_button_begin(str);
//	//wzrd_label_button_end();
//	return result;
//}


wzrd_handle wzrd_dialog_begin(wzrd_v2* pos, wzrd_v2 size, bool* active, wzrd_str name, int layer, wzrd_handle parent) {
	WZRD_UNUSED(name);

	if (!*active) return (wzrd_handle) { 0 };

	wzrd_handle window = wzrd_widget_free((wzrd_box) {
		.style = wzrd_style_create((wzrd_style) { .x = pos->x, .y = pos->y, .w = size.x, .h = size.y, .color = EGUI_GRAY})
	}, parent);

	wzrd_box_get_by_handle(window)->layer = layer;

	bool close = false;

	wzrd_handle top_panel = wzrd_widget((wzrd_box) {
		.style = wzrd_style_create((wzrd_style) { .h = 28, .border_type = BorderType_None, .row_mode = true })
	}, window);

	wzrd_handle bar = wzrd_widget((wzrd_box) {
		.style = wzrd_style_create((wzrd_style) {
			.border_type = BorderType_None,
				.color = (wzrd_color){ 57, 77, 205, 255 }
		})
	}, top_panel);

	if (wzrd_handle_is_equal(bar, canvas->active_item)) {
		pos->x += canvas->mouse_pos.x - canvas->previous_mouse_pos.x;
		pos->y += canvas->mouse_pos.y - canvas->previous_mouse_pos.y;
	}

	//	wzrd_box_begin((wzrd_box) {

	//		.style = wzrd_style_create((wzrd_style) {
	//			.w = 28,
	//				.border_type = BorderType_None, .row_mode = true,
	//				.center_x = true, .center_y = true,
	//				.color = (wzrd_color){ 57, 77, 205, 255 },
	//		})
	//	});
	//	{
	//		bool b = egui_button_raw_begin((wzrd_box) {

	//			.style = wzrd_style_create((wzrd_style) {
	//				.center_x = true, .center_y = true, .w = 20, .h = 20,
	//			})
	//		});
	//		{
	//			b |= egui_button_raw_begin((wzrd_box) {
	//				.style = wzrd_style_create((wzrd_style) {
	//					.border_type = BorderType_None,
	//						.w = 12, .h = 12,
	//				})
	//			});
	//			{
	//				wzrd_item_add((Item) { .type = ItemType_CloseIcon });
	//			}
	//			egui_button_raw_end();
	//		}
	//		egui_button_raw_end();

	//		if (b) {
	//			*active = false;
	//			close = true;
	//		}

	//	}
	//	wzrd_box_end();
	//}
	//wzrd_box_end();

	if (close)
	{
		wzrd_crate_end();
	}

	return window;
}

void wzrd_dialog_end(bool active) {
	if (active)
	{
		wzrd_crate_end();
	}
}

void wzrd_dropdown(int* selected_text, const wzrd_str* texts, int texts_count, int w, bool* active) {

	(void)selected_text;
	(void)texts;
	(void)texts_count;
	(void)active;
	(void)w;

#if 0
	WZRD_ASSERT(texts);
	WZRD_ASSERT(texts_count);
	WZRD_ASSERT(selected_text);
	WZRD_ASSERT(*selected_text >= 0);

	w = 150;

	wzrd_box_begin((wzrd_box) {
		.style = wzrd_style_create((wzrd_style) {

			.fit_w = true,
				.fit_h = true,
				.color = EGUI_WHITE, .border_type = BorderType_InputBox, .row_mode = true
		})
	});
	{
		wzrd_box box = (wzrd_box){


				.style = wzrd_style_create((wzrd_style) {
						.w = w,
				.h = WZRD_FONT_HEIGHT + 4 * WZRD_BORDER_SIZE,
			.center_x = true, .center_y = true,
				.pad_left = 2,
				.pad_top = 2,
				.pad_bottom = 2, .pad_right = 2,
				.border_type = BorderType_None,
				.color = EGUI_WHITE,
			})
		};

		wzrd_style box_style = wzrd_style_get(box.style);

		int parent = 0;
		//EguiButton2(box, texts[*selected_text], EGUI_DARKBLUE);

		parent = canvas->boxes_count - 1;

		//box.border_type = BorderType_None;

		//bool* toggle2;

	/*	bool button = wzrd_button_sized((wzrd_box) {

			.style = wzrd_style_create((wzrd_style) {
				.center_x = true, .center_y = true,
					.w = 20, .h = WZRD_FONT_HEIGHT + 4 * WZRD_BORDER_SIZE,
			})
		}, (Item) { .type = ItemType_DropdownIcon });
		if (button)
		{
			*active = !*active;
		}*/

		if (*active) {
			//char str[256];
			//sprintf(str, "%s-dropdown", wzrd_box_get_from_top_of_stack()->name.val);
			//static wzrd_v2i pos;

			wzrd_crate_begin(2, (wzrd_box) {

				//.name = wzrd_str_create("%s-dropdown", wzrd_box_get_from_top_of_stack()->name.val),
				.style = wzrd_style_create((wzrd_style) {
					.border_type = BorderType_Black, .y = box_style.h,
						.w = box_style.w + 4 * WZRD_BORDER_SIZE,
						.h = box_style.h * texts_count + 4 * WZRD_BORDER_SIZE,
				})
			});
			{
				for (int i = 0; i < texts_count; ++i) {

					//bool pressed = EguiButton2(box, texts[i], EGUI_DARKBLUE);

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
#endif
}



bool wzrd_handle_is_active(wzrd_handle handle) {
	if (wzrd_handle_is_equal(handle, canvas->active_item)) {
		return true;
	}

	return false;
}



bool wzrd_handle_is_released(wzrd_handle handle) {
	if (wzrd_handle_is_equal(handle, canvas->deactivating_item)) {
		return true;
	}

	return false;
}

bool wzrd_handle_is_hovered(wzrd_handle handle) {
	if (wzrd_handle_is_equal(handle, canvas->hovered_item)) {
		return true;
	}

	return false;
}

bool wzrd_handle_is_hovered_from_list(wzrd_handle handle)
{

	for (int i = 0; i < canvas->hovered_items_list_count; ++i)
	{
		if (wzrd_handle_is_equal(handle, canvas->hovered_items_list[i]))
		{
			return true;
		}
	}

	return false;
}

bool wzrd_handle_is_interacting(wzrd_handle handle) {
	if (wzrd_handle_is_equal(handle, canvas->activating_item) ||
		wzrd_handle_is_equal(handle, canvas->active_item) ||
		wzrd_handle_is_equal(handle, canvas->deactivating_item))
	{
		return true;
	}

	return false;
}

bool wzrd_handle_is_child_of_handle(wzrd_handle a, wzrd_handle b)
{
	int stack[MAX_NUM_BOXES];
	int count = 0;
	int ptr = 0;

	stack[count++] = wzrd_box_get_by_handle(a)->index;

	while (ptr != count)
	{
		wzrd_box* box = &canvas->boxes[stack[ptr]];

		for (int i = 0; i < box->children_count; ++i)
		{
			stack[count++] = box->children[i];
		}

		if (wzrd_handle_is_equal(box->handle, b))
			return true;

		ptr++;
	}

	return false;
}

void wzrd_label_list(wzrd_str* item_names, unsigned int count,
	wzrd_v2 size, wzrd_handle* handles, unsigned int* selected, bool* is_selected, wzrd_handle parent)
{

	wzrd_handle panel = wzrd_widget((wzrd_box) {
		.style = wzrd_style_create((wzrd_style) { .color = EGUI_WHITE, .w = size.x, .h = size.y })
	}, parent);

	if (wzrd_box_is_half_clicked(wzrd_box_get_by_handle(panel))) {
		*selected = 0;
		*is_selected = false;
	}

	wzrd_box* active_box = 0;

	for (unsigned int i = 0; i < count; ++i)
	{
		bool active = false;
		if (*selected == i && *is_selected)
			active = true;

		char str[32];
		sprintf_s(str, 32, "%d label list %d", wzrd_box_get_last()->handle.handle, i);

		bool is_label_clicked = false;
		wzrd_label_button_half_click((wzrd_box) {
			.handle = wzrd_unique_handle_create(wzrd_str_create(str)),

				.style = wzrd_style_create((wzrd_style) {
				.center_x = true, .center_y = true, .h = 32,
					.border_type = BorderType_None, .color = EGUI_WHITE,
			})
		},
			item_names[i], panel);

		if (handles)
		{
			handles[i] = canvas->boxes[canvas->boxes_count - 2].handle;
		}

		if (is_label_clicked) {
			*selected = i;
			*is_selected = true;
		}

		wzrd_box* current_box = wzrd_box_get_last();
		if (wzrd_box_is_active(current_box))
		{
			active_box = current_box;
		}

	}
}

void wzrd_label_list_sorted(wzrd_str* item_names, unsigned int count, int* items,
	wzrd_v2 size, unsigned int* selected, bool* is_selected, wzrd_handle parent) {

	wzrd_handle handles[MAX_NUM_LABELS] = { 0 };

	wzrd_label_list(item_names, count, size, handles, selected, is_selected, parent);

	// Ordering
	{
		assert(items);
		wzrd_handle active_label = { 0 }, hovered_label = { 0 }, released_label = { 0 };
		int active_label_index = -1, hovered_label_index = -1, released_label_index = -1;

		// Set variables
		for (unsigned int i = 0; i < count; ++i)
		{
			if (wzrd_handle_is_active(handles[i]) || wzrd_handle_is_active_tree(handles[i]))
			{
				active_label = handles[i];
				active_label_index = i;
			}
			else if (wzrd_handle_is_hovered_from_list(handles[i]))
			{
				hovered_label = handles[i];
				hovered_label_index = i;
			}
			else if (wzrd_handle_is_hovered(handles[i]))
			{
				// ...
			}
			else if (wzrd_handle_is_released(handles[i]) || wzrd_handle_is_released_tree(handles[i]))
			{
				released_label = handles[i];
				released_label_index = i;
			}
		}

		// Get hovered widget
		bool is_bottom = false;
		wzrd_box* hovered_parent = 0;
		for (int i = 0; i < canvas->hovered_boxes_count; ++i)
		{
			if (wzrd_handle_is_equal(canvas->hovered_boxes[i].handle, hovered_label))
			{
				hovered_parent = canvas->hovered_boxes + i;
				break;
			}
		}

		// Set hover position
		if (hovered_parent)
		{
			wzrd_style hovered_parent_style = wzrd_style_get(hovered_parent->style);

			if (canvas->mouse_pos.y > hovered_parent_style.y + hovered_parent_style.h / 2)
			{
				is_bottom = true;
			}

			// Label grabbed and hovering over another one
			if (wzrd_handle_is_valid(active_label) && wzrd_handle_is_valid(hovered_label) && !wzrd_handle_is_equal(hovered_label, active_label))
			{

				wzrd_box* p = wzrd_box_get_by_handle(hovered_label);
				wzrd_box* c = 0;
				if (is_bottom)
				{
					c = wzrd_box_create((wzrd_box) {
						.free = true,
							.style = wzrd_style_create((wzrd_style) {
							.border_type = BorderType_None, .color = EGUI_PURPLE,
								.y = hovered_parent_style.h - 2, .h = 2,
						})
					});

					wzrd_box_add_free_child(p, c);
				}
				else
				{
					/*	 c = wzrd_box_create((wzrd_box) {
							.h = 10, .border_type = BorderType_None, .color = EGUI_BROWN, .free = true
						});*/
				}
			}
		}



		// Label released over another label
		if (wzrd_handle_is_valid(released_label) && wzrd_handle_is_valid(hovered_label) && !wzrd_handle_is_equal(hovered_label, released_label))
		{
			int val = items[released_label_index];

			if (is_bottom)
			{
				if (released_label_index < hovered_label_index)
				{
					for (int i = released_label_index + 1; i <= hovered_label_index; ++i)
					{
						items[i - 1] = items[i];
					}

					items[hovered_label_index] = val;
				}
				else if (released_label_index > hovered_label_index)
				{
					for (int i = released_label_index; i > hovered_label_index; --i)
					{
						items[i] = items[i - 1];
					}

					items[hovered_label_index + 1] = val;
				}
			}

			*selected = hovered_label_index;
		}
	}

}

//(wzrd_str) {
//	.str = name.val, .len = name.len
// }

bool wzrd_button_handle(wzrd_rect_struct rect, wzrd_color color, wzrd_str name, wzrd_handle parent) {

	bool result = false;

	wzrd_box box =
		(wzrd_box){
		.type = wzrd_box_type_button,
			.style = wzrd_style_create((wzrd_style) {
			.color = color, .border_type = BorderType_None, .x = rect.x, .y = rect.y, .w = rect.w, .h = rect.h,
		}),
			.handle = wzrd_unique_handle_create(name)
	};

	wzrd_handle widget = wzrd_widget_free(box, parent);
	result = wzrd_box_is_active(wzrd_box_get_by_handle(widget));

	return result;
}

void wzrd_drag(bool* drag) {

	if (canvas->mouse_left == WZRD_INACTIVE)
	{
		*drag = false;
	}

	if (!(*drag)) return;

	wzrd_style dragged_box_style = wzrd_style_get(canvas->dragged_box.style);

	dragged_box_style.x += canvas->mouse_delta.x;
	dragged_box_style.y += canvas->mouse_delta.y;

	wzrd_crate(1, canvas->dragged_box);
}

bool wzrd_box_is_active(wzrd_box* box) {
	if (wzrd_handle_is_equal(box->handle, canvas->active_item)) {
		return true;
	}

	return false;
}

bool wzrd_box_is_dragged(wzrd_box* box) {
	if (wzrd_handle_is_equal(box->handle, canvas->dragged_item)) {
		return true;
	}

	return false;
}

bool wzrd_box_is_hot_using_canvas(wzrd_canvas* c, wzrd_box* box) {
	if (wzrd_handle_is_equal(box->handle, c->hovered_item)) {
		return true;
	}

	return false;
}

bool wzrd_box_is_hot(wzrd_box* box) {
	if (wzrd_handle_is_equal(box->handle, canvas->hovered_item)) {
		return true;
	}

	return false;
}

bool wzrd_unique_handle_is_equal(wzrd_str str, wzrd_handle handle) {
	wzrd_handle unique_handle = wzrd_unique_handle_create(str);
	if (wzrd_handle_is_equal(unique_handle, handle)) {
		return true;
	}

	return false;
}

//
//bool wzrd_box_is_selected(wzrd_box* box) {
//	if (wzrd_handle_equal(box->handle, canvas->selected_item)) {
//		return true;
//	}
//
//	return false;
//}

wzrd_box* wzrd_box_get_by_handle(wzrd_handle handle) {
	if (!wzrd_handle_is_valid(handle))
	{
		return canvas->boxes;
	}

	wzrd_box* result = 0;
	for (int i = 0; i < canvas->boxes_count; ++i)
	{
		if (canvas->boxes[i].handle.handle == handle.handle)
		{
			result = canvas->boxes + i;
			break;
		}
	}

	assert(result);

	return result;
}

wzrd_box* wzrd_box_get_released()
{
	wzrd_box* result = 0;

	if (wzrd_handle_is_valid(canvas->deactivating_item))
	{
		result = wzrd_box_get_by_handle(canvas->deactivating_item);
	}

	return result;
}


bool wzrd_is_releasing() {
	bool result = false;
	if (wzrd_handle_is_valid(canvas->deactivating_item))
	{
		result = true;
	}

	return result;
}

wzrd_v2 wzrd_lerp(wzrd_v2 pos, wzrd_v2 end_pos) {
	(void)pos;
	(void)end_pos;
	/*float lerp_amount = 0.2f;
	int delta = 0.1f;
	if (fabs((float)end_pos.x - pos.x) > delta) {
		pos.x = pos.x + (int)(lerp_amount * (end_pos.x - pos.x));
	}

	if (fabs(end_pos.y - pos.y) > delta) {
		pos.y = pos.y + (int)(lerp_amount * (end_pos.y - pos.y));
	}*/

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

wzrd_handle wzrd_rect_unique(wzrd_rect_struct rect, wzrd_str name, wzrd_handle parent)
{
	wzrd_box box = (wzrd_box){
		.handle = wzrd_unique_handle_create(name),
		.style = wzrd_style_create((wzrd_style) {
			.border_type = BorderType_None, .x = rect.x, .y = rect.y, .w = rect.w, .h = rect.h
		}) };

	wzrd_handle h = wzrd_widget_free(box
	, parent);

	return h;
}

wzrd_handle wzrd_command_button(wzrd_str str, bool* released, wzrd_handle parent) {
	wzrd_handle h1, h2;
	bool b1 = false, b2 = false;

	h1 = egui_button_raw_begin((wzrd_box) {
		.style = canvas->button_style
	}, & b1, parent);

	h2 = egui_button_raw_begin((wzrd_box) {
		.style = wzrd_style_create((wzrd_style) {

			.border_type = BorderType_None,
				.w = (int)str.len * FONT_WIDTH, .h = WZRD_FONT_HEIGHT
		})
	}, & b2, h1);

	wzrd_text_add(str, h2);

	*released = b1 || b2;

	if (wzrd_handle_is_interacting(h1) || wzrd_handle_is_interacting(h2))
	{
		wzrd_box* b = wzrd_box_get_by_handle(h1);
		wzrd_style style = wzrd_style_get(b->style);
		style.border_type = BorderType_Clicked;
		wzrd_box_get_by_handle(h1)->style = wzrd_style_create(style);
	}

	return h1;
}