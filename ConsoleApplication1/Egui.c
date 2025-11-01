// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#define WZRD_ASSERT(x) assert(x)
//#define WZRD_ASSERT(x) (void)(x)
#define WZRD_UNUSED(x) (void)x

#include "Egui.h"
#include "Strings.h"

static wzrd_canvas* canvas;

bool wzrd_handle_is_equal(wzrd_handle a, wzrd_handle b);
bool wzrd_handle_is_valid(wzrd_handle handle);
wzrd_handle wzrd_handle_create();
void wzrd_box_end();
wzrd_box* wzrd_box_get_parent();
wzrd_box* wzrd_box_get_previous();
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

int wzrd_space_create(wzrd_space space)
{
	canvas->style_spaces[canvas->style_spaces_count++] = space;

	return canvas->style_spaces_count - 1;
}

int wzrd_structure_create(wzrd_structure structure)
{
	canvas->style_structures[canvas->style_structures_count++] = structure;

	return canvas->style_structures_count - 1;
}

int wzrd_layout_create(wzrd_layout layout)
{
	canvas->style_layouts[canvas->style_layouts_count++] = layout;

	return canvas->style_layouts_count - 1;
}

int wzrd_skin_create(wzrd_skin skin)
{
	canvas->style_skins[canvas->style_skins_count++] = skin;

	return canvas->style_skins_count - 1;
}


wzrd_space wzrd_space_get(wzrd_handle handle)
{
	return canvas->style_spaces[wzrd_box_get_by_handle(handle)->space];
}

wzrd_structure wzrd_structure_get(wzrd_handle handle)
{
	return canvas->style_structures[wzrd_box_get_by_handle(handle)->structure];
}

wzrd_layout wzrd_layout_get(wzrd_handle handle)
{
	return canvas->style_layouts[wzrd_box_get_by_handle(handle)->layout];
}

wzrd_layout wzrd_layout_get_by_handle(int handle)
{
	return canvas->style_layouts[handle];
}

wzrd_skin wzrd_skin_get(wzrd_handle handle)
{
	return canvas->style_skins[wzrd_box_get_by_handle(handle)->skin];
}

void wzrd_skin_set(wzrd_handle handle, int skin)
{
	wzrd_box_get_by_handle(handle)->skin = skin;
}

void wzrd_style_init()
{
	canvas->style_layouts_count = 1;
	canvas->style_structures_count = 1;
	canvas->style_skins_count = 1;
	canvas->style_spaces_count = 1;

	// Panels
	canvas->panel_skin = wzrd_skin_create((wzrd_skin)
	{
		.border_type = BorderType_None,
			.color = EGUI_LIGHTGRAY,
	});
	canvas->panel_border_skin = wzrd_skin_create((wzrd_skin)
	{
		.border_type = BorderType_Default,
			.color = EGUI_LIGHTGRAY,
	});
	canvas->panel_border_click_skin = wzrd_skin_create((wzrd_skin)
	{
		.border_type = BorderType_Clicked,
			.color = EGUI_LIGHTGRAY,
	});
	canvas->panel_structure = wzrd_structure_create((wzrd_structure)
	{
		.pad_left = 5,
		.pad_right = 5,
		.pad_top = 5,
		.pad_bottom = 5
	});
	canvas->v_panel_layout = wzrd_layout_create((wzrd_layout)
	{
		//.center_y = true,
			.child_gap = 10,
	});
	canvas->h_panel_layout = wzrd_layout_create((wzrd_layout)
	{
		//.center_y = true,
			.child_gap = 10,
			.row_mode = true,
	});

	// List
	canvas->list_skin = wzrd_skin_create((wzrd_skin)
	{
		.border_type = BorderType_Clicked,
			.color = EGUI_WHITE,
	});

	// Buttons
	canvas->command_button_space = wzrd_space_create((wzrd_space) {
		.w = 200, .h = 32,
	});

	canvas->command_button_skin = wzrd_skin_create((wzrd_skin) {
		.color = EGUI_LIGHTGRAY,
			.border_type = BorderType_Default
	});

	canvas->command_button_on_skin = wzrd_skin_create((wzrd_skin) {
		.color = EGUI_LIGHTGRAY,
			.border_type = BorderType_Clicked
	});

	canvas->command_button_layout = wzrd_layout_create((wzrd_layout) {
		.center_x = true, .center_y = true,
	});

	// Label List
	canvas->label_item_skin = wzrd_skin_create((wzrd_skin) {
		.color = EGUI_WHITE
	});

	canvas->label_item_selected_skin = wzrd_skin_create((wzrd_skin) {
		.color = EGUI_BLUE
	});

	// Label
	canvas->label_skin = wzrd_skin_create((wzrd_skin) {
		.border_type = BorderType_None,
	});

	canvas->label_structure = wzrd_structure_create((wzrd_structure) {
		.pad_bottom = 2,
		.pad_top = 2,
		.pad_left = 2,
		.pad_right = 2,
	});

	// Toggle Icon
	canvas->toggle_icon_space = wzrd_space_create((wzrd_space) { .w = 24, .h = 22 });

	// Input Box
	const int max_num_keys = 10;
	canvas->input_box_space = wzrd_space_create((wzrd_space)
	{
		.w = FONT_WIDTH * (int)max_num_keys + 8,
			.h = WZRD_FONT_HEIGHT + 8,
	});
	canvas->input_box_skin = wzrd_skin_create((wzrd_skin)
	{
		.border_type = BorderType_InputBox,
			.color = EGUI_WHITE,
	});
	canvas->input_box_structure = wzrd_structure_create((wzrd_structure) {
		.pad_left = 2,
			.pad_top = 2,
			.pad_bottom = 2,
			.pad_right = 2,

	});
	canvas->input_box_layout = wzrd_layout_create((wzrd_layout)
	{
		.center_x = true,
			.center_y = true,
	});

	// Top label panel
	{
		wzrd_layout layout = wzrd_layout_get_by_handle(canvas->v_panel_layout);
		layout.child_gap = 10;
		canvas->top_label_panel_layout = wzrd_layout_create(layout);
	}
}

wzrd_handle wzrd_box_set_unique_handle(wzrd_handle handle, wzrd_str str)
{
	wzrd_handle new_handle = wzrd_unique_handle_create(str);
	wzrd_box_get_by_handle(handle)->handle = new_handle;
	return new_handle;
}

wzrd_handle wzrd_vbox_border(wzrd_v2 size, wzrd_handle parent)
{
	wzrd_handle handle = wzrd_widget(((wzrd_style) {
		.space = wzrd_space_create((wzrd_space)
		{
			.h = size.y,
				.w = size.x
		}),
			.layout = canvas->v_panel_layout,
			.structure = canvas->panel_structure,
			.skin = canvas->panel_border_skin
	}),
		parent);

	return handle;
}

wzrd_handle wzrd_vbox_border_click(wzrd_v2 size, wzrd_handle parent)
{
	wzrd_handle handle = wzrd_widget(((wzrd_style) {
		.space = wzrd_space_create((wzrd_space)
		{
			.h = size.y,
				.w = size.x
		}),
			.layout = canvas->v_panel_layout,
			.structure = canvas->panel_structure,
			.skin = canvas->panel_border_click_skin
	}),
		parent);

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

		if (wzrd_handle_is_child_of_handle(c2->handle, c1->handle))
		{
			return 1;
		}
		else if (wzrd_handle_is_child_of_handle(c1->handle, c2->handle))
		{
			return - 1;
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
#if 0
	wzrd_box* box = 0;
	//wzrd_box_get_from_top_of_stack();
	wzrd_box* previous_box = wzrd_box_get_parent();

	box->type = wzrd_box_type_resizable;

	int mouse_delta_x = canvas->mouse_pos.x - canvas->previous_mouse_pos.x;
	int mouse_delta_y = canvas->mouse_pos.y - canvas->previous_mouse_pos.y;

	wzrd_style_template previous_box_style = wzrd_style_get(previous_box->style);

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

	//wzrd_box_get_from_top_of_stack()->w_internal += size->x;
	//wzrd_box_get_from_top_of_stack()->h_internal += size->y;
#endif
}

int wzrd_float_compare(float a, float b)
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

void wzrd_box_add_free_child(wzrd_handle parent, wzrd_handle child)
{
	wzrd_box* p = wzrd_box_get_by_handle(parent);
	wzrd_box* c = wzrd_box_get_by_handle(child);
	WZRD_ASSERT(p->free_children_count < MAX_NUM_CHILDREN - 1);
	p->free_children[p->free_children_count++] = c->index;

	c->layer = p->layer;
}

void wzrd_box_add_child(wzrd_handle parent, wzrd_handle child)
{
	wzrd_box* p = wzrd_box_get_by_handle(parent);
	wzrd_box* c = wzrd_box_get_by_handle(child);
	WZRD_ASSERT(p->children_count < MAX_NUM_CHILDREN - 1);
	p->children[p->children_count++] = c->index;

	c->layer = p->layer;

}

void wzrd_box_add_child_using_pointer(wzrd_box* parent, wzrd_box* child)
{
	WZRD_ASSERT(parent->children_count < MAX_NUM_CHILDREN - 1);
	parent->children[parent->children_count++] = child->index;
}

wzrd_box* wzrd_box_create(wzrd_box box) {

	if (box.handle.handle == 0)
		box.handle = wzrd_handle_create();

	WZRD_ASSERT(canvas->boxes_count < MAX_NUM_BOXES - 1);

	box.layer = EguiGetCurrentWindow()->layer;

	/*if (box_style.fit_w) {
		box.w_internal += 4 * WZRD_BORDER_SIZE;
	}
	if (box_style.fit_h) {
		box.h_internal += 4 * WZRD_BORDER_SIZE;
	}*/

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

wzrd_handle wzrd_widget_free(wzrd_style style, wzrd_handle parent)
{
	wzrd_box box_data = { .skin = style.skin, .layout = style.layout, .structure = style.structure, .space = style.space };

	wzrd_box* box = wzrd_box_create(box_data);
	box->layer = wzrd_box_get_by_handle(parent)->layer;

	wzrd_box_add_free_child(parent, box->handle);
	{
		wzrd_space style = wzrd_space_get(box->handle);
		box->x_internal = style.x;
		box->y_internal = style.y;
		box->w_internal = style.w;
		box->h_internal = style.h;
	}

	return box->handle;
}

wzrd_handle wzrd_widget(wzrd_style style, wzrd_handle parent)
{
	wzrd_box box_data = { .skin = style.skin, .layout = style.layout, .structure = style.structure, .space = style.space };

	wzrd_box* box = wzrd_box_create(box_data);

	wzrd_box_add_child(parent, box->handle);
	{
		wzrd_space style = wzrd_space_get(box->handle);
		box->x_internal = style.x;
		box->y_internal = style.y;
		box->w_internal = style.w;
		box->h_internal = style.h;
	}

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

void wzrd_texture_add(wzrd_texture texture, wzrd_v2 size, wzrd_handle parent) {
	wzrd_item_add((Item) {
		.type = ItemType_Texture,
			.size = size,
			.val = { .texture = texture }
	}, parent);
}

void wzrd_crate_begin(int layer, wzrd_box box) {

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

void wzrd_box_set_color(wzrd_handle h, wzrd_color color)
{
	wzrd_skin skin = wzrd_skin_get(h);
	skin.color = color;
	wzrd_skin_set(h, wzrd_skin_create(skin));
}

wzrd_handle wzrd_begin(wzrd_canvas* gui,
	wzrd_rect_struct window,
	void (*get_string_size)(char*, int*, int*),
	wzrd_v2 mouse_pos,
	wzrd_state mouse_left,
	wzrd_keyboard_keys keys,
	bool enable_input)
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

	wzrd_style_init();

	WZRD_UNUSED(gui);

	canvas->boxes_count = 0;
	canvas->input_box_timer += 16.7f;

	// Empty box
	canvas->boxes[canvas->boxes_count++] = (wzrd_box){ 0 };

	// Zero-out boxes
	for (int i = 0; i < MAX_NUM_BOXES; ++i)
	{
		canvas->boxes[i] = (wzrd_box){ 0 };
	}

	// Window
	wzrd_handle h = wzrd_widget((wzrd_style) {
		.space = wzrd_space_create((wzrd_space) {
			.x = window.x,
				.y = window.y,
				.w = window.w,
				.h = window.h,
		}),
			.layout = canvas->v_panel_layout,
			//.skin = canvas->panel_skin,
			.structure = canvas->panel_structure
	}, (wzrd_handle) { 0 });

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

void wzrd_handle_cursor()
{
	canvas->cursor = wzrd_cursor_default;

	wzrd_box* hot_box = wzrd_box_get_by_handle(canvas->hovered_item);
	wzrd_box* active_box = wzrd_box_get_by_handle(canvas->active_item);

	if (canvas->hovered_item.handle) {
		if (hot_box->type == wzrd_box_type_button) {
			canvas->cursor = wzrd_cursor_hand;
		}
	}
	if (canvas->active_item.handle) {
		if (active_box->type == wzrd_box_type_button) {
			canvas->cursor = wzrd_cursor_hand;
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
			wzrd_skin child_style = wzrd_skin_get(child->handle);

			int border_size = 2;

			if (child->type != wzrd_box_type_resizable) continue;

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


void wzrd_handle_input(int* indices, int count)
{
	wzrd_box* hovered_box = canvas->boxes;
	unsigned int max_layer = 0;
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

bool wzrd_box_is_activating(wzrd_box* box) {
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

void wzrd_do_layout()
{
	// Dragging
	if (wzrd_handle_is_valid(canvas->dragged_box.handle))
	{
		canvas->dragged_box.x_internal += canvas->mouse_delta.x;
		canvas->dragged_box.y_internal += canvas->mouse_delta.y;

		wzrd_crate(1, canvas->dragged_box);
	}

	// Calculate size
	for (int i = 1; i < canvas->boxes_count; ++i)
	{
		wzrd_box* parent = &canvas->boxes[i];
		wzrd_space parent_space = wzrd_space_get(parent->handle);
		wzrd_structure parent_structure = wzrd_structure_get(parent->handle);
		wzrd_layout parent_layout = wzrd_layout_get(parent->handle);

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

			available_w = parent->w_internal - parent_structure.pad_left - parent_structure.pad_right - 4 * WZRD_BORDER_SIZE;
			available_h = parent->h_internal - parent_structure.pad_top - parent_structure.pad_bottom - 4 * WZRD_BORDER_SIZE;

			if (parent->children_count)
			{
				if (parent_layout.row_mode)
					available_w -= parent_layout.child_gap * (parent->children_count - 1);
				else
					available_h -= parent_layout.child_gap * (parent->children_count - 1);
			}
		}

		// Handle growing
		for (int j = 0; j < parent->children_count; ++j)
		{
			wzrd_box* child = &canvas->boxes[parent->children[j]];
			wzrd_layout child_layout = wzrd_layout_get(child->handle);

			if (child->w_internal == 0 && !child_layout.fit_w)
			{
				if (parent_layout.row_mode) {
					child->w_internal = available_w - children_w;
				}
				else {
					child->w_internal = available_w;
				}

				WZRD_ASSERT(child->w_internal > 0);
			}

			if (child->h_internal == 0 && !child_layout.fit_h)
			{
				if (!parent_layout.row_mode)
				{
					child->h_internal = available_h - children_h;
				}
				else
				{
					child->h_internal = available_h;
				}

				WZRD_ASSERT(child->h_internal > 0);
			}

			if (child_layout.best_fit && parent->h_internal && child->h_internal)
			{
				float ratio_a = parent->w_internal / parent->h_internal;
				float ratio_b = child->w_internal / child->h_internal;
				float ratio = 0;

				//if (ratio_b >= ratio_a)
				if (wzrd_float_compare(ratio_b, ratio_a) >= 0)
				{
					ratio = (parent->w_internal / child->w_internal);
				}
				else
				{
					ratio = (parent->h_internal / child->h_internal);
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
		wzrd_layout parent_layout = wzrd_layout_get(parent->handle);

		if (!parent->clip)
			continue;

		parent->content_size.x = 2 * WZRD_BORDER_SIZE;
		parent->content_size.y = 2 * WZRD_BORDER_SIZE;

		if (parent_layout.row_mode)
		{
			parent->content_size.x += (parent->children_count + 1) * parent_layout.child_gap;
			parent->content_size.y += parent_layout.child_gap;
		}
		else
		{
			parent->content_size.y += (parent->children_count + 1) * parent_layout.child_gap;
			parent->content_size.x += parent_layout.child_gap;
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

			if (parent_layout.row_mode)
			{
				parent->content_size.x += child.w_internal;
			}
			else
			{
				parent->content_size.y += child.h_internal;
			}
		}

		if (parent_layout.row_mode)
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
					.skin = wzrd_skin_create((wzrd_skin)
				{
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
					.skin = wzrd_skin_create((wzrd_skin) {
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
				//parent_structure.pad_top -= ratio * parent->content_size.y;
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
		wzrd_structure parent_structure = wzrd_structure_get(parent->handle);
		wzrd_layout parent_layout = wzrd_layout_get(parent->handle);

		int x = parent->x_internal + parent_structure.pad_left, y = parent->y_internal + parent_structure.pad_top;

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

			if (parent_layout.row_mode)
				w += child->w_internal;
			else
				h += child->h_internal;
		}

		if (parent_layout.row_mode)
			w += parent_layout.child_gap * (parent->children_count - 1);
		else
			h += parent_layout.child_gap * (parent->children_count - 1);

		if (parent_layout.center_x && parent_layout.row_mode) {
			x += (parent->w_internal - 4 * WZRD_BORDER_SIZE - parent_structure.pad_left - parent_structure.pad_right) / 2 - w / 2;
		}
		if (parent_layout.center_y && !parent_layout.row_mode) {
			y += (parent->h_internal - 4 * WZRD_BORDER_SIZE - parent_structure.pad_top - parent_structure.pad_bottom) / 2 - h / 2;
		}

		// Calc positions
		for (int j = 0; j < parent->children_count; ++j) {
			wzrd_box* child = &canvas->boxes[parent->children[j]];

			child->x_internal += x;
			child->y_internal += y;

			if (parent_layout.center_y && parent_layout.row_mode) {
				child->y_internal += (parent->h_internal - 4 * WZRD_BORDER_SIZE - parent_structure.pad_top
					- parent_structure.pad_bottom) / 2 - child->h_internal / 2;
			}

			if (parent_layout.center_x && !parent_layout.row_mode) {
				child->x_internal += (parent->w_internal - 4 * WZRD_BORDER_SIZE -
					parent_structure.pad_top - parent_structure.pad_bottom) / 2 - child->w_internal / 2;
			}

			if (parent_layout.row_mode) {
				x += child->w_internal;
				x += parent_layout.child_gap;
			}
			else {
				y += child->h_internal;
				y += parent_layout.child_gap;
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
		//wzrd_box_tree_apply(wzrd_box_get_by_handle(canvas->active_item)->index, 0, wzrd_box_bring_to_front);
	}
}

wzrd_draw(int *boxes_indices)
{
	// Draw
	{
		wzrd_draw_commands_buffer* buffer = &canvas->command_buffer;
		buffer->count = 0;
		for (int i = 1; i < canvas->boxes_count; ++i) {

			wzrd_box box = canvas->boxes[boxes_indices[i]];
			wzrd_skin box_skin = wzrd_skin_get(box.handle);

			if (box.type == wzrd_box_type_crate)
			{
				WZRD_ASSERT(buffer->count < MAX_NUM_DRAW_COMMANDS - 1);
				buffer->commands[buffer->count++] = (wzrd_draw_command){
					.type = DrawCommandType_Clip,
					.dest_rect = canvas->window,
					.z = box.layer
				};
			}

			wzrd_color color = { 0 };
			color = box_skin.color;

			EguiRectDraw(buffer, (wzrd_rect_struct) {
				.x = box.x_internal,
					.y = box.y_internal,
					.h = box.h_internal,
					.w = box.w_internal
			},
				color,
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

			if (box_skin.border_type == BorderType_Custom) {
				//// Draw top and left lines
				//EguiRectDraw(buffer, top0, canvas->style.b0, box.layer);
				//EguiRectDraw(buffer, left0, canvas->style.b0, box.layer);
				//EguiRectDraw(buffer, top1, canvas->style.b1, box.layer);
				//EguiRectDraw(buffer, left1, canvas->style.b1, box.layer);

				//// Draw bottom and right lines
				//EguiRectDraw(buffer, bottom0, canvas->style.b2, box.layer);
				//EguiRectDraw(buffer, right0, canvas->style.b2, box.layer);
				//EguiRectDraw(buffer, bottom1, canvas->style.b3, box.layer);
				//EguiRectDraw(buffer, right1, canvas->style.b3, box.layer);
			}
			else if (box_skin.border_type == BorderType_Default) {
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
			else if (box_skin.border_type == BorderType_Clicked) {
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
			else if (box_skin.border_type == BorderType_InputBox) {
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
			else if (box_skin.border_type == BorderType_Black) {
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
			else if (box_skin.border_type == BorderType_BottomLine) {
				EguiRectDraw(buffer, bottom0, EGUI_WHITE2, box.layer);
				EguiRectDraw(buffer, bottom1, EGUI_GRAY, box.layer);
			}
			else if (box_skin.border_type == BorderType_LeftLine) {
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

				// Apply box style to item
				//wzrd_style_template style = wzrd_style_get(box.style);
				wzrd_layout layout = wzrd_layout_get(box.handle);
				wzrd_structure structure = wzrd_structure_get(box.handle);

				wzrd_rect_struct dest = (wzrd_rect_struct){
					box.x_internal,
					box.y_internal,
						box.w_internal,
						box.h_internal
				};

				// String item
				if (item.type == wzrd_item_type_str) {

					int w, h;
					canvas->get_string_size(item.val.str.str, &w, &h);

					if (layout.center_x)
					{
						dest.x += dest.w / 2 - w / 2;
					}

					if (layout.center_y)
					{
						dest.y += dest.h / 2 - h / 2;
					}

					dest.x += structure.pad_left;
					dest.y += structure.pad_top;
					dest.x -= structure.pad_right;
					dest.y -= structure.pad_bottom;

					command = (wzrd_draw_command){
						.type = DrawCommandType_String,
							.str = item.val.str,
							.dest_rect = dest,
							.color = item.color,
							.z = box.layer
					};
				}

				// Texture item
				if (item.type == ItemType_Texture) {
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

				// Rect item
				if (item.type == ItemType_Rect) {
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

				// Line items
				if (item.type == ItemType_Line) {
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
}

void wzrd_end(wzrd_str* debug_str)
{
	wzrd_do_layout();
	
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
		wzrd_handle_cursor();
		/*wzrd_handle_border_resize(cursor);*/
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

	wzrd_draw(boxes_indices);

	if (debug_str)
	{
		char buff[1024];
		sprintf_s(buff, 1024, "hovered: %u\nactive: %u\n hovered items:\n", canvas->hovered_item.handle, canvas->active_item.handle);
		for (int i = 0; i < canvas->hovered_items_list_count; ++i)
		{
			char b[32];
			sprintf_s(b, 32, "hovered item %d %u %d\n", i, canvas->hovered_items_list[i].handle,
				wzrd_box_get_by_handle(canvas->hovered_items_list[i])->layer);
			strcat(debug_str->str, b);
		}
		strcat(debug_str->str, buff);
		debug_str->len = strlen(debug_str->str);
	}
	canvas = 0;
}

void wzrd_box_set_type(wzrd_handle handle, wzrd_box_type type)
{
	wzrd_box_get_by_handle(handle)->type = type;
}

wzrd_handle egui_button_raw_begin(wzrd_style box, bool* released, wzrd_handle parent) {


	wzrd_handle handle = wzrd_widget(box, parent);
	wzrd_box_set_type(handle, wzrd_box_type_button);

	if (wzrd_handle_is_equal(handle, canvas->deactivating_item)) {
		*released = true;
	}

	return handle;
}

wzrd_canvas* wzrd_canvas_get()
{
	return canvas;
}

wzrd_handle wzrd_toggle_icon(wzrd_texture texture, bool* active, wzrd_handle parent) {
	bool b = false;
	bool b1, b2;
	wzrd_handle h1, h2;

	h1 = egui_button_raw_begin((wzrd_style)
	{
		.space = canvas->toggle_icon_space,
			.structure = canvas->command_button_structure
	},
		& b1, parent);

	/*h2 = egui_button_raw_begin((wzrd_box) {
		.style = wzrd_style_create((wzrd_style_template) { .w = 16, .h = 16, .border_type = BorderType_None, }),
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
	bool a2 = wzrd_box_is_active(&canvas->boxes[canvas->boxes_count - 2]);*/

	return h1;
}

wzrd_handle wzrd_button_icon(wzrd_texture texture, bool* result, wzrd_handle parent) {
#if 0
	bool active = false;
	wzrd_handle h1, h2;
	bool b1, b2;
	h1 = egui_button_raw_begin((wzrd_box)
	{
		.style = wzrd_style_create((wzrd_style_space) { .w = 24, .h = 22 }),
			.space = wzrd_space_layout((wzrd_style_layout) { .center_x = true, .center_y = true })
	})
}, & b1, parent);

h2 = egui_button_raw_begin((wzrd_box) {
	.style = wzrd_style_create((wzrd_style_template) { .w = 16, .h = 16, .border_type = BorderType_None, }),
}, & b2, h1);

wzrd_box_add_child(h1, h2);

wzrd_item_add((Item) {
	.type = ItemType_Texture,
		.val = { .texture = texture }
}, h2);

*result |= b1;
*result |= b2;

return h1;
#else
	return (wzrd_handle) { 0 };
#endif
}

wzrd_handle wzrd_command_toggle(wzrd_str str, bool* active, wzrd_handle parent) {
	bool b1 = false, a1 = false;
	wzrd_handle h1;
	wzrd_style style = (wzrd_style){
		.layout = canvas->command_button_layout,
		.skin = canvas->command_button_skin,
		.structure = canvas->command_button_structure,
		.space = canvas->command_button_space,
	};
	h1 = egui_button_raw_begin(style, &b1, parent);

	wzrd_text_add(str, h1);

	if (b1)
	{
		*active = !*active;
	}

	a1 = wzrd_box_is_active(&canvas->boxes[canvas->boxes_count - 1]);

	if (*active || a1)
	{
		wzrd_skin_set(h1, canvas->command_button_on_skin);
	}

	return h1;
}

wzrd_handle egui_button_raw_begin_on_half_click(wzrd_style box, bool* b, wzrd_handle parent) {
	wzrd_handle h = wzrd_widget(box, parent);

	//if (wzrd_handle_is_valid(h))
	if (wzrd_handle_is_equal(h, canvas->activating_item)) {
		*b = true;
	}
	else
	{
		*b = false;
	}

	return h;
}

wzrd_handle wzrd_label_button_activating(wzrd_style box, wzrd_str str, bool* active, wzrd_handle parent)
{
	wzrd_handle h = egui_button_raw_begin_on_half_click(box, active, parent);
	wzrd_box_set_type(h, wzrd_box_type_button);

	wzrd_text_add(str, h);

	return h;
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

void wzrd_item_add(Item item, wzrd_handle box) {
	wzrd_box* b = wzrd_box_get_by_handle(box);
	WZRD_ASSERT(b->items_count < MAX_NUM_ITEMS - 1);
	b->items[b->items_count++] = item;
}

wzrd_handle wzrd_label(wzrd_str str, wzrd_handle parent) {
	int w = 0, h = 0;
	canvas->get_string_size(str.str, &w, &h);

	wzrd_handle widget = wzrd_widget((wzrd_style) {
		.space = wzrd_space_create((wzrd_space) {
			.w = w,
				.h = h,
		})
	}, parent);

	wzrd_text_add(str, widget);

	return widget;
}

wzrd_handle wzrd_input_box(char* str, int* len, int max_num_keys, wzrd_handle parent) {
	wzrd_handle p1 = wzrd_widget((wzrd_style) { .space = wzrd_space_create((wzrd_space){.w = 50, .h = 30}), .skin = canvas->input_box_skin },
		parent);
	//wzrd_box_set_type(p1, wzrd_box_type_input_box);
	
#if 1
			//wzrd_str str2 = *str;

			//if (wzrd_handle_is_equal(wzrd_box_get_from_top_of_stack()->handle, canvas->active_input_box)) {
			{
				//wzrd_style_template style = wzrd_style_get(wzrd_box_get_last()->style);
				//style.color = (wzrd_color){ 255, 230, 230, 255 };
				//wzrd_box_get_last()->style = wzrd_style_create(style);

				for (int i = 0; i < canvas->keyboard_keys.count; ++i) {
					wzrd_keyboard_key key = canvas->keyboard_keys.keys[i];

					if (key.val == '\b' &&
						*len > 0 &&
						(key.state == WZRD_ACTIVE || key.state == EguiActivating)) {
						str[*len - 1] = 0;
						*len = *len - 1;
					}
					else if ((key.state == EguiActivating || key.state == WZRD_ACTIVE) &&
						((key.val <= 'z' && key.val >= 'a') || (key.val <= '9' && key.val >= '0')) &&
						(isgraph(key.val) || key.val == ' ') &&
						*len < max_num_keys - 1 &&
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
			wzrd_text_add((wzrd_str) { .str = str, .len = *len }, p1);
#endif

	return p1;
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
	int w = 0, h = 0;
	canvas->get_string_size(str.str, &w, &h);

	wzrd_handle widget = wzrd_widget((wzrd_style) {
		.skin = canvas->label_skin,
			.structure = canvas->label_structure,
		.space = wzrd_space_create((wzrd_space)
		{
			.w = w,
			.h = h
		})
	}, parent);

	wzrd_box_set_type(widget, wzrd_box_type_button);

	wzrd_text_add(str, widget);

	if (wzrd_box_is_activating(wzrd_box_get_last()))
	{
		*result = true;
	}

	return widget;
}

wzrd_handle wzrd_dialog_begin(wzrd_v2* pos, wzrd_v2 size, bool* active, wzrd_str name, int layer, wzrd_handle parent) {
	WZRD_UNUSED(name);

	if (!*active) return (wzrd_handle) { 0 };

	wzrd_handle window = wzrd_widget_free(
		(wzrd_style) {
		.space = wzrd_space_create((wzrd_space) { .x = pos->x, .y = pos->y, .w = size.x, .h = size.y }),
			.skin = canvas->panel_border_skin,
	},
		parent);

	wzrd_box_get_by_handle(window)->layer = layer;

	bool close = false;

	wzrd_handle top_panel = wzrd_widget((wzrd_style)
	{
		.space = wzrd_space_create((wzrd_space) { .h = 28 }),
			.layout = canvas->v_panel_layout
	},
		window);

	wzrd_handle bar = wzrd_widget((wzrd_style) {
		.skin = wzrd_skin_create((wzrd_skin) {
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


void wzrd_label_list(wzrd_str* item_names, unsigned int count,
	wzrd_v2 size, wzrd_handle* handles, unsigned int* selected, bool* is_selected, wzrd_handle parent)
{
	wzrd_handle panel = wzrd_widget((wzrd_style) {
		.skin = canvas->list_skin,
			.space = wzrd_space_create((wzrd_space) { .w = size.x, .h = size.y })
	}, parent);

	if (wzrd_box_is_activating(wzrd_box_get_by_handle(panel))) {
		*selected = 0;
		*is_selected = false;
	}

	wzrd_handle selected_label = { 0 };

	for (unsigned int i = 0; i < count; ++i)
	{
		char str[32];
		sprintf_s(str, 32, "%d label list %d", wzrd_box_get_last()->handle.handle, i);

		bool is_label_clicked = false;
		wzrd_handle h = wzrd_label_button_activating((wzrd_style) {
			.layout = wzrd_layout_create((wzrd_layout)
			{
				.center_x = true, .center_y = true
			}),
				.space = wzrd_space_create((wzrd_space)
			{
				.h = 32,
			}),
				.skin = wzrd_skin_create((wzrd_skin)
			{
				.border_type = BorderType_None, .color = EGUI_WHITE,
			})
		},
			item_names[i], & is_label_clicked, panel);
		//h = wzrd_box_set_unique_handle(h, wzrd_str_create(str));

		if (handles)
		{
			handles[i] = canvas->boxes[canvas->boxes_count - 2].handle;
		}

		if (is_label_clicked) {
			*selected = i;
			*is_selected = true;
			selected_label = h;
		}

		if (*is_selected && *selected == i)
		{
			wzrd_skin_set(h, canvas->label_item_selected_skin);
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
		int hovered_label_index = -1, released_label_index = -1;

		// Set variables
		for (unsigned int i = 0; i < count; ++i)
		{
			if (wzrd_handle_is_active(handles[i]) || wzrd_handle_is_active_tree(handles[i]))
			{
				active_label = handles[i];
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
			wzrd_space hovered_parent_style = wzrd_space_get(hovered_parent->handle);

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
							.skin = wzrd_skin_create((wzrd_skin)
						{
							.border_type = BorderType_None, .color = EGUI_PURPLE,
						}),
							.space = wzrd_space_create((wzrd_space)
						{
							.y = hovered_parent_style.h - 2, .h = 2,
						})
					});

					wzrd_box_add_free_child(p->handle, c->handle);
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

wzrd_handle wzrd_handle_button(bool* active, wzrd_rect_struct rect, wzrd_color color, wzrd_str name, wzrd_handle parent) {

	wzrd_style box =
		(wzrd_style){
			.skin = wzrd_skin_create((wzrd_skin) {
 .color = color, .border_type = BorderType_Default
}),
		   .space = wzrd_space_create((wzrd_space) { .x = rect.x, .y = rect.y, .w = rect.w, .h = rect.h }),
	};

	wzrd_handle widget = wzrd_widget_free(box, parent);
	wzrd_box_set_type(widget, wzrd_box_type_button);
	//widget = wzrd_box_set_unique_handle(widget, name);
	*active = wzrd_box_is_active(wzrd_box_get_by_handle(widget));

	return widget;
}

void wzrd_drag(bool* drag) {

	if (canvas->mouse_left == WZRD_INACTIVE)
	{
		*drag = false;
	}

	if (!(*drag)) return;

	wzrd_space dragged_box_space = wzrd_space_get(canvas->dragged_box.handle);

	dragged_box_space.x += canvas->mouse_delta.x;
	dragged_box_space.y += canvas->mouse_delta.y;

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
	wzrd_style box = (wzrd_style){
		.space = wzrd_space_create((wzrd_space) {
				.x = rect.x, .y = rect.y, .w = rect.w, .h = rect.h
		}) };

	wzrd_handle h = wzrd_widget_free(box
		, parent);

	h = wzrd_box_set_unique_handle(h, name);

	return h;
}

wzrd_handle wzrd_command_button(wzrd_str str, bool* released, wzrd_handle parent) {
	wzrd_handle h1;
	bool b1 = false;

	h1 = egui_button_raw_begin((wzrd_style) {
		.layout = canvas->command_button_layout,
			.skin = canvas->command_button_skin,
			.structure = canvas->command_button_structure,
			.space = canvas->command_button_space,
	}, & b1, parent);

	//h2 = egui_button_raw_begin((wzrd_box) {
	//	.style = wzrd_style_create((wzrd_style) {

	//		.border_type = BorderType_None,
	//			.w = (int)str.len * FONT_WIDTH, .h = WZRD_FONT_HEIGHT
	//	})
	//}, & b2, h1);

	wzrd_text_add(str, h1);

	*released = b1;

	if (wzrd_handle_is_interacting(h1))
	{
		wzrd_skin_set(h1, canvas->command_button_on_skin);
	}


	return h1;
}

void wzrd_handle_set_layer(wzrd_handle handle, unsigned int layer)
{
	wzrd_box_get_by_handle(handle)->layer = layer;
}
