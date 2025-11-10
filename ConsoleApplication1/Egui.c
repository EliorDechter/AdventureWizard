// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

//#define WZRD_ASSERT(x) assert(x)
#define WZRD_ASSERT(x) (void)(x)
#define WZRD_UNUSED(x) (void)x

#include "Egui.h"
#include "Strings.h"

static wzrd_canvas* canvas;

WzHandle wz_widget(WzHandle parent);
bool wzrd_handle_is_equal(WzHandle a, WzHandle b);
bool wzrd_handle_is_valid(WzHandle handle);
WzHandle wzrd_handle_create();
WzWidget* wzrd_box_get_parent();
WzWidget* wzrd_box_get_previous();
void wzrd_crate_begin(int window_id, WzWidget box);
void wzrd_crate(int window_id, WzWidget box);
void wzrd_texture_add(wzrd_texture texture, wzrd_v2 size, WzHandle box);
void wzrd_text_add(wzrd_str str, WzHandle box);
//void wzrd_box_resize(wzrd_v2* size);
void wzrd_crate_end();
WzWidget* wz_widget_get(WzHandle str);
void wzrd_drag(bool* drag);
WzWidget* wzrd_box_get_last();
bool wzrd_box_is_dragged(WzWidget* box);
bool wzrd_box_is_hot_using_canvas(wzrd_canvas* canvas, WzWidget* box);
WzWidget* wzrd_box_get_released();
WzWidget* wzrd_box_find(wzrd_canvas* canvas, wzrd_str name);
wzrd_v2 wzrd_lerp(wzrd_v2 pos, wzrd_v2 end_pos);

typedef struct stylesheet
{
	int button_color;
	int button_color_clicked;
} stylesheet;

void wz_widget_add_source(WzHandle handle, const char* file, unsigned int line)
{
	wz_widget_get(handle)->file = file;
	wz_widget_get(handle)->line_number = line;
}

void wzrd_style_init()
{
#if 0

	// Panels
	canvas->panel_skin = (wzrd_skin)
	{
		.border_type = BorderType_None,
			.color = EGUI_LIGHTGRAY,
	};
	canvas->panel_border_skin = (wzrd_skin)
	{
		.border_type = BorderType_Default,
			.color = EGUI_LIGHTGRAY,
	};
	canvas->panel_border_click_skin = (wzrd_skin)
	{
		.border_type = BorderType_Clicked,
			.color = EGUI_LIGHTGRAY,
	};
	canvas->panel_structure = ((wzrd_structure)
	{
		.pad_left = 5,
			.pad_right = 5,
			.pad_top = 5,
			.pad_bottom = 5
	});
	canvas->v_panel_layout = ((wzrd_layout)
	{
		//.center_y = true,
		.child_gap = 10,
	});
	canvas->h_panel_layout = ((wzrd_layout)
	{
		//.center_y = true,
		.child_gap = 10,
			.type = WzLayoutRow,
	});

	// List
	canvas->list_skin = (wzrd_skin)
	{
		.border_type = BorderType_Clicked,
			.color = EGUI_WHITE,
	};

	// Buttons
	canvas->command_button_space = (wzrd_space){
		.w = 200, .h = 32,
	};

	canvas->command_button_skin = (wzrd_skin){
		.color = EGUI_LIGHTGRAY,
			.border_type = BorderType_Default
	};

	canvas->command_button_on_skin = (wzrd_skin){
		.color = EGUI_LIGHTGRAY,
			.border_type = BorderType_Clicked
	};

	canvas->command_button_layout = ((wzrd_layout) {
		.center_x = true, .center_y = true,
	});

	// Label List
	canvas->label_item_skin = (wzrd_skin){
		.color = EGUI_WHITE
	};

	canvas->label_item_selected_skin = (wzrd_skin){
		.color = EGUI_BLUE
	};

	// Label
	canvas->label_skin = (wzrd_skin){
		.border_type = BorderType_None,
	};

	canvas->label_structure = ((wzrd_structure) {
		.pad_bottom = 2,
			.pad_top = 2,
			.pad_left = 2,
			.pad_right = 2,
	});

	// Toggle Icon
	canvas->toggle_icon_space = (wzrd_space){ .w = 24, .h = 22 };

	// Input Box
	const int max_num_keys = 10;
	canvas->input_box_space = (wzrd_space)
	{
		.w = FONT_WIDTH * (int)max_num_keys + 8,
			.h = WZRD_FONT_HEIGHT + 8,
	};
	canvas->input_box_skin = (wzrd_skin)
	{
		.border_type = BorderType_InputBox,
			.color = EGUI_WHITE,
	};
	canvas->input_box_structure = ((wzrd_structure) {
		.pad_left = 2,
			.pad_top = 2,
			.pad_bottom = 2,
			.pad_right = 2,

	});
	canvas->input_box_layout = ((wzrd_layout)
	{
		.center_x = true,
			.center_y = true,
	});

	// Top label panel
	{
		wzrd_layout layout = (canvas->v_panel_layout);
		layout.child_gap = 10;
		canvas->top_label_panel_layout = (layout);
	}
#endif
}


WzHandle wzrd_vbox_border_raw(wzrd_v2 size, WzHandle parent, const char* file_name, unsigned int line)
{
	WzHandle handle = wz_widget(parent);
	wz_widget_add_source(handle, file_name, line);
	wz_widget_set_size(handle, size.x, size.y);

	return handle;
}

WzHandle wzrd_vbox_border_click(wzrd_v2 size, WzHandle parent)
{
	WzHandle handle = wz_hbox(parent);
	wz_widget_set_size(handle, size.x, size.y);

	return handle;
}

bool wzrd_handle_is_valid(WzHandle handle)
{
	return (bool)handle.handle;
}

WzWidget* wz_widget_get(WzHandle handle) {
	if (!wzrd_handle_is_valid(handle))
	{
		return canvas->boxes;
	}

	WzWidget* result = 0;
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

void wz_widget_set_size(WzHandle handle, int w, int h)
{
	WzWidget* b = wz_widget_get(handle);
	b->w = w;
	b->h = h;
}


WzHandle wzrd_handle_create()
{
	assert(canvas);
	WzHandle handle = { canvas->boxes_count };

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

bool wzrd_is_rect_inside_rect(WzRect a, WzRect b) {
	bool res = a.x >= b.x;
	res &= a.x >= b.x;
	res &= a.x + a.w <= b.x + b.w;
	res &= a.y + a.h <= b.y + b.h;

	return res;
}

void wz_widget_set_h(WzHandle w, int height)
{
	wz_widget_get(w)->h = height;
}

void wz_widget_set_w(WzHandle w, int width)
{
	wz_widget_get(w)->w = width;
}

void wz_widget_set_x(WzHandle w, int x)
{
	WzWidget* widget = wz_widget_get(w);
	widget->x = x;
}

void wz_widget_set_y(WzHandle w, int y)
{
	WzWidget* widget = wz_widget_get(w);
	widget->y = y;
}

void wz_widget_set_border(WzHandle w, WzBorderType border_type)
{
	wz_widget_get(w)->border_type = border_type;
}

void wz_widget_set_pos(WzHandle handle, int x, int y)
{
	wz_widget_set_x(handle, x);
	wz_widget_set_y(handle, y);
}


void wzrd_widget_set_color(WzHandle widget, wzrd_color color)
{
	wz_widget_get(widget)->color = color;
}

int wzrd_v2_is_inside_rect(wzrd_v2 v, WzRect rect) {
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

WzWidget* wzrd_box_get_parent() {
	if (canvas->crates_stack[canvas->current_crate_index].box_stack_count < 2)
	{
		return &canvas->boxes[0];
	}
	int current_box_index = canvas->crates_stack[canvas->current_crate_index].box_stack_count - 2;
	int final_index = canvas->crates_stack[canvas->current_crate_index].box_stack[current_box_index];
	WzWidget* result = &canvas->boxes[final_index];

	return result;
}

bool wzrd_handle_is_child_of_handle(WzHandle a, WzHandle b)
{
	int stack[MAX_NUM_BOXES];
	int count = 0;
	int ptr = 0;

	stack[count++] = wz_widget_get(a)->index;

	while (ptr != count)
	{
		WzWidget* box = &canvas->boxes[stack[ptr]];

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

WzWidget* wzrd_box_get_last() {
	WzWidget* result = canvas->boxes + (canvas->boxes_count - 1);

	return result;
}

void wzrd_text_add(wzrd_str str, WzHandle parent)
{
	wzrd_item_add((Item) { .type = wzrd_item_type_str, .val = { .str = str }, .color = EGUI_BLACK }, parent);
}

void goo(WzWidget* box, void* data)
{
	(void)data;
	(void)box;
	//box->color = EGUI_BEIGE;
}

void wzrd_box_tree_apply(int index, void* data, void (*goo)(WzWidget* box, void* data))
{
	int stack[1024];
	int count = 0;
	int ptr = 0;

	stack[count++] = index;

	while (ptr != count)
	{
		WzWidget* box = &canvas->boxes[stack[ptr]];

		for (int i = 0; i < box->children_count; ++i)
		{
			stack[count++] = box->children[i];
		}

		goo(box, data);
		ptr++;
	}
}

bool wzrd_handle_is_active_tree(WzHandle handle)
{
	int stack[1024];
	int count = 0;
	int ptr = 0;

	stack[count++] = wz_widget_get(handle)->index;

	while (ptr != count)
	{
		WzWidget* box = &canvas->boxes[stack[ptr]];

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

bool wzrd_widget_is_deactivating(WzHandle handle)
{
	if (wzrd_handle_is_equal(handle, canvas->deactivating_item))
	{
		return true;
	}

	return false;
}

bool wzrd_box_is_released(WzWidget* box) {
	if (wzrd_handle_is_equal(box->handle, canvas->deactivating_item)) {
		return true;
	}

	return false;
}

bool wzrd_handle_is_released_tree(WzHandle handle)
{
	int stack[1024];
	int count = 0;
	int ptr = 0;

	stack[count++] = wz_widget_get(handle)->index;

	while (ptr != count)
	{
		WzWidget* box = &canvas->boxes[stack[ptr]];

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
	WzWidget* c1 = canvas->boxes + index1;
	WzWidget* c2 = canvas->boxes + index2;

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
			return -1;
		}

		if (index1 > index2) return 1;
		else if (index1 < index2) return -1;
	}

	return 0;
}

bool wzrd_handle_is_equal(WzHandle a, WzHandle b)
{
	return a.handle == b.handle;
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

void wzrd_box_add_free_child(WzHandle parent, WzHandle child)
{
	WzWidget* p = wz_widget_get(parent);
	WzWidget* c = wz_widget_get(child);
	WZRD_ASSERT(p->free_children_count < MAX_NUM_CHILDREN - 1);
	p->free_children[p->free_children_count++] = c->index;

	c->layer = p->layer;
	c->clip_widget = p->clip_widget;

}

void wzrd_box_add_child(WzHandle parent, WzHandle child)
{


	WzWidget* p = wz_widget_get(parent);
	WzWidget* c = wz_widget_get(child);
	WZRD_ASSERT(p->children_count < MAX_NUM_CHILDREN - 1);
	p->children[p->children_count++] = c->index;

	c->layer = p->layer;
	c->clip_widget = p->clip_widget;

}

void wzrd_box_add_child_using_pointer(WzWidget* parent, WzWidget* child)
{
	WZRD_ASSERT(parent->children_count < MAX_NUM_CHILDREN - 1);
	parent->children[parent->children_count++] = child->index;
}

WzWidget* wz_widget_create()
{
	WzWidget box;
	box.children_count = 0;
	box.free_children_count = 0;
	box.items_count = 0;
	box.color = EGUI_LIGHTGRAY;
	box.font_color = EGUI_BLACK;
	box.percentage_h = 0;
	box.percentage_w = 0;
	box.alignment = 0;
	box.best_fit = false;
	box.border_type = BorderType_None;
	box.bring_to_front = false;
	box.child_gap = 0;
	box.content_h = 0;
	box.content_w = 0;
	box.fit_h = false;
	box.fit_w = false;
	box.pad_left = box.pad_right = box.pad_top = box.pad_bottom = 0;
	box.x = box.y = box.h = box.w = 0;
	box.file = 0;
	box.line_number = 0;
	box.clip_widget.handle = 0;

	box.handle = wzrd_handle_create();

	WZRD_ASSERT(canvas->boxes_count < MAX_NUM_BOXES - 1);

	box.layer = EguiGetCurrentWindow()->layer;

	WZRD_ASSERT(canvas->boxes_count < 256);

	box.index = canvas->boxes_count;
	canvas->boxes[canvas->boxes_count++] = box;

	return &canvas->boxes[canvas->boxes_count - 1];
}

WzHandle wzrd_widget_free(WzHandle parent)
{
	WzWidget* box = wz_widget_create();
	box->layer = wz_widget_get(parent)->layer;


	return box->handle;
}

WzHandle wz_widget(WzHandle parent)
{
	WzWidget* widget = wz_widget_create();

	wzrd_box_add_child(parent, widget->handle);

	return widget->handle;
}

WzHandle wz_vbox_raw(WzHandle parent, const char* file, unsigned int line)
{
	WzHandle p = wz_widget(parent);
	wz_widget_set_layout(p, WzLayoutVertical);
	wz_widget_add_source(p, file, line);
	wz_widget_set_strech_factor(p, 1);

	return p;
}

WzHandle wz_hbox_raw(WzHandle parent, const char* file, unsigned int line)
{
	WzHandle p = wz_widget(parent);
	wz_widget_set_layout(p, WzLayoutHorizontal);
	wz_widget_add_source(p, file, line);
	wz_widget_set_strech_factor(p, 1);

	return p;
}

void wzrd_texture_add(wzrd_texture texture, wzrd_v2 size, WzHandle parent) {
	wzrd_item_add((Item) {
		.type = ItemType_Texture,
			.size = size,
			.val = { .texture = texture }
	}, parent);
}

void wzrd_crate_begin(int layer, WzWidget box) {

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

void wzrd_crate(int window_id, WzWidget box) {
	wzrd_crate_begin(window_id, box);
	wzrd_crate_end();
}

WzHandle wz_begin(wzrd_canvas* gui,
	WzRect window,
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

	canvas->clip_boxes_count = 1;

	// Empty box
	canvas->boxes[canvas->boxes_count++] = (WzWidget){ 0 };

	// Zero-out boxes
	for (int i = 0; i < MAX_NUM_BOXES; ++i)
	{
		//canvas->boxes[i] = (wzrd_box){ 0 };
	}

	// Window
	WzHandle h = wz_vbox((WzHandle) { 0 });

	wz_widget_set_size(h, window.w, window.h);
	wz_widget_set_pos(h, window.x, window.y);

	// Stylesheet
	canvas->stylesheet.label_color = EGUI_LIGHTGRAY;
	canvas->stylesheet.label_item_selected_color = EGUI_LIGHTGRAY;


	return h;
}


void EguiRectDraw(wzrd_draw_commands_buffer* buffer, WzRect rect, wzrd_color color, int z, int box_index) {

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

	buffer->commands[buffer->count - 1].box_index = box_index;

}

WzWidget* wzrd_box_find(wzrd_canvas* c, wzrd_str name)
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


WzWidget* wzrd_box_get_previous() {
	WzWidget* result = &canvas->boxes[canvas->boxes_count - 1];

	return result;
}

void wzrd_handle_cursor()
{
#if 0
	canvas->cursor = wzrd_cursor_default;

	WzWidget* hot_box = wz_widget_get(canvas->hovered_item);
	WzWidget* active_box = wz_widget_get(canvas->active_item);

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
#endif
}

void wzrd_handle_border_resize()
{
#if 0
	wzrd_cursor* cursor = 0;

	canvas->left_resized_item = (wzrd_handle){ 0 };
	canvas->right_resized_item = (wzrd_handle){ 0 };
	canvas->top_resized_item = (wzrd_handle){ 0 };
	canvas->bottom_resized_item = (wzrd_handle){ 0 };

	for (int i = 0; i < canvas->boxes_count; ++i) {
		wzrd_box* owner = canvas->boxes + i;
		for (int j = 0; j < owner->children_count; ++j) {
			wzrd_box* child = &canvas->boxes[owner->children[j]];
			wzrd_skin child_style = child->skin;

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
#endif
}

bool wzrd_handle_is_active(WzHandle handle) {
	if (wzrd_handle_is_equal(handle, canvas->active_item)) {
		return true;
	}

	return false;
}

bool wzrd_handle_is_released(WzHandle handle) {
	if (wzrd_handle_is_equal(handle, canvas->deactivating_item)) {
		return true;
	}

	return false;
}

bool wzrd_handle_is_hovered(WzHandle handle) {
	if (wzrd_handle_is_equal(handle, canvas->hovered_item)) {
		return true;
	}

	return false;
}

bool wzrd_handle_is_hovered_from_list(WzHandle handle)
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

bool wzrd_handle_is_interacting(WzHandle handle) {
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
	WzWidget* hovered_box = canvas->boxes;
	unsigned int max_layer = 0;
	canvas->hovered_items_list_count = 0;
	canvas->hovered_boxes_count = 0;
	for (int i = 0; i < count; ++i) {
		WzWidget* box = canvas->boxes + indices[i];

		WzRect scaled_rect = { box->x_internal, box->y_internal, box->w_internal, box->h_internal };

		// Input Clipping: Block mouse from reaching clipped widgets
		if (wzrd_handle_is_valid(box->clip_widget))
		{
			WzWidget* clip_widget = wz_widget_get(box->clip_widget);

			if (box->x_internal < clip_widget->x_internal)
			{
				scaled_rect.x = clip_widget->x_internal;
			}

			if (box->y_internal < clip_widget->y_internal)
			{
				scaled_rect.y = clip_widget->y_internal;
			}

			if (box->x_internal + box->w_internal > clip_widget->x_internal + clip_widget->w_internal)
			{
				scaled_rect.w = clip_widget->w_internal;
			}

			if (box->y_internal + box->h_internal > clip_widget->y_internal + clip_widget->h_internal)
			{
				scaled_rect.y = clip_widget->h_internal;
			}
		}

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
	WzWidget* half_clicked_box = wz_widget_get(canvas->activating_item);
	if (half_clicked_box && canvas->mouse_left == WZRD_ACTIVE)
	{
		canvas->activating_item = (WzHandle){ 0 };
	}

	if (canvas->mouse_left == WZRD_DEACTIVATING)
	{
		//canvas->released_item = canvas->dragged_item;
		canvas->dragged_box = (WzWidget){ 0 };
		canvas->dragged_item = (WzHandle){ 0 };

		canvas->clean = false;
	}

	if (canvas->mouse_left == WZRD_INACTIVE)
	{
		canvas->deactivating_item = (WzHandle){ 0 };
	}

	if (wzrd_handle_is_valid(hovered_box->handle)) {
		if (!wzrd_handle_is_equal(hovered_box->handle, canvas->hovered_item))
		{
			canvas->clean = false;
		}

		canvas->hovered_item = hovered_box->handle;
	}
	else {
		canvas->hovered_item = (WzHandle){ 0 };
	}

	WzWidget* hot_box = wz_widget_get(canvas->hovered_item);
	WzWidget* active_box = wz_widget_get(canvas->active_item);

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
				wz_widget_get(canvas->deactivating_item)->color = EGUI_PURPLE;
				canvas->active_item = (WzHandle){ 0 };
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
		canvas->clicked_item = (WzHandle){ 0 };

		canvas->clean = false;
	}

	// Input box
	if (wzrd_handle_is_valid(canvas->clicked_item)) {
		WzWidget* clicked_box = wz_widget_get(canvas->clicked_item);
		WZRD_ASSERT(clicked_box);
		/*	if (clicked_box->type == wzrd_box_type_input_box) {
				canvas->active_input_box = clicked_box->handle;
			}
			else {
				canvas->active_input_box = (wzrd_handle){ 0 };
			}*/
	}
}

bool wzrd_widget_is_activating(WzHandle handle) {
	if (wzrd_handle_is_equal(handle, canvas->activating_item)) {
		return true;
	}

	return false;
}

bool wzrd_widget_is_active(WzHandle handle) {
	if (wzrd_handle_is_equal(handle, canvas->active_item)) {
		return true;
	}

	return false;
}

bool wzrd_box_is_activating(WzWidget* box) {
	if (wzrd_handle_is_equal(box->handle, canvas->activating_item)) {
		return true;
	}

	return false;
}

void wzrd_box_bring_to_front(WzWidget* box, void* data)
{
	(void)data;
	box->bring_to_front = true;
}

void wz_widget_set_strech_factor(WzHandle handle, unsigned int strech_factor)
{
	wz_widget_get(handle)->strech_factor = strech_factor;
}

void wz_widget_set_layout(WzHandle handle, WzLayout layout_type)
{
	wz_widget_get(handle)->layout_type = layout_type;
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
		WzWidget* parent = &canvas->boxes[i];

		WZRD_ASSERT(parent->w_internal > 0);
		WZRD_ASSERT(parent->h_internal > 0);

		// Calculate the total size of the children
		unsigned int available_w = 0, available_h = 0, children_w = 0, children_h = 0, children_strech_factor = 0;
		{
			for (int j = 0; j < parent->children_count; ++j)
			{
				WzWidget* child = &canvas->boxes[parent->children[j]];
				children_w += child->w_internal;
				children_h += child->h_internal;

				children_strech_factor += child->strech_factor;
			}

			WZRD_ASSERT(parent->w_internal);
			WZRD_ASSERT(parent->h_internal);

			available_w = parent->w_internal - parent->pad_left - parent->pad_right - 4 * WZRD_BORDER_SIZE;
			available_h = parent->h_internal - parent->pad_top - parent->pad_bottom - 4 * WZRD_BORDER_SIZE;

			if (parent->children_count)
			{
				if (parent->layout_type == WzLayoutHorizontal)
					available_w -= parent->child_gap * (parent->children_count - 1);
				else
					available_h -= parent->child_gap * (parent->children_count - 1);
			}
		}

		// Handle growing
		{
			unsigned int strech_w = 0;
			unsigned int strech_h = 0;
			if (children_strech_factor)
			{
				strech_w = available_w / children_strech_factor;
				strech_h = available_h / children_strech_factor;
			}

			for (int j = 0; j < parent->children_count; ++j)
			{
				WzWidget* child = &canvas->boxes[parent->children[j]];

				if (1)
				{
					if (child->strech_factor)
					{
						if (parent->layout_type == WzLayoutHorizontal)
						{
							WZRD_ASSERT(strech_w);
							child->w_internal = strech_w * child->strech_factor;

							if (!child->h)
							{
								child->h_internal = available_h;
							}

						}
						else if (parent->layout_type == WzLayoutVertical)
						{
							WZRD_ASSERT(strech_h);
							child->h_internal = strech_h * child->strech_factor;

							if (!child->w)
							{
								child->w_internal = available_w;
							}

						}
					}
				}
				else
				{
					// OLD CODE
					if (child->strech_factor && !child->fit_w && !child->percentage_w)
					{
						if (parent->layout_type == WzLayoutHorizontal)
						{
							WZRD_ASSERT(available_w >= children_w);
							child->w_internal = available_w - children_w;
						}
						else
						{
							child->w_internal = available_w;
						}

					}

					if (child->strech_factor && !child->fit_h && !child->percentage_h)
					{
						if (parent->layout_type == WzLayoutHorizontal)
						{
							WZRD_ASSERT(available_h >= children_h);
							child->h_internal = available_h - children_h;
						}
						else
						{
							child->h_internal = available_h;
						}

					}
				}

				if (child->best_fit && parent->h_internal && child->h_internal)
				{
					float ratio_a = parent->w_internal / parent->h_internal;
					float ratio_b = child->w_internal / child->h_internal;
					float ratio = 0;

					if (wzrd_float_compare(ratio_b, ratio_a) >= 0)
					{
						WZRD_ASSERT(child->w_internal);
						ratio = (parent->w_internal / child->w_internal);
					}
					else
					{
						WZRD_ASSERT(child->h_internal);
						ratio = (parent->h_internal / child->h_internal);
					}

					child->w_internal = child->w_internal * ratio;
					child->h_internal = child->h_internal * ratio;
				}

				if (child->percentage_w)
				{
					child->w_internal = parent->w_internal * child->percentage_w;
				}
				if (child->percentage_h)
				{
					child->h_internal = parent->h_internal * child->percentage_h;
				}

				WZRD_ASSERT(child->w_internal > 0);
				WZRD_ASSERT(child->h_internal > 0);
			}
		}

		// Calcuate size for free children
		for (int j = 0; j < parent->free_children_count; ++j)
		{
			WzWidget* child = &canvas->boxes[parent->free_children[j]];

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
		WzWidget* parent = canvas->boxes + i;

		parent->content_w = 2 * WZRD_BORDER_SIZE;
		parent->content_h = 2 * WZRD_BORDER_SIZE;

		if (parent->layout_type == WzLayoutHorizontal)
		{
			parent->content_w += (parent->children_count + 1) * parent->child_gap;
			parent->content_h += parent->child_gap;
		}
		else
		{
			parent->content_h += (parent->children_count + 1) * parent->child_gap;
			parent->content_w += parent->child_gap;
		}

		int max_child_w = 0, max_child_h = 0;
		for (int j = 0; j < parent->children_count; ++j)
		{
			WzWidget child = canvas->boxes[parent->children[j]];

			if (parent->h_internal > max_child_h)
			{
				max_child_h = child.h_internal;
			}

			if (parent->w_internal > max_child_w)
			{
				max_child_w = child.w_internal;
			}

			if (parent->layout_type == WzLayoutHorizontal)
			{
				parent->content_w += child.w_internal;
			}
			else
			{
				parent->content_h += child.h_internal;
			}
		}

		if (parent->layout_type == WzLayoutHorizontal)
		{
			parent->content_h += max_child_h;
		}
		else
		{
			parent->content_w += max_child_w;
		}
	}

	// Calculate positions
	for (int i = 1; i < canvas->boxes_count; ++i) {
		WzWidget* parent = &canvas->boxes[i];

		int x = parent->x_internal + parent->pad_left, y = parent->y_internal + parent->pad_top;

		x += 2 * WZRD_BORDER_SIZE;
		y += 2 * WZRD_BORDER_SIZE;

		// Center
		int w = 0, h = 0, max_w = 0, max_h = 0;
		for (int j = 0; j < parent->children_count; ++j) {
			WzWidget* child = &canvas->boxes[parent->children[j]];

			if (child->w_internal > max_w)
				max_w = child->w_internal;

			if (child->h_internal > max_h)
				max_h = child->h_internal;

			if (parent->layout_type == WzLayoutHorizontal)
				w += child->w_internal;
			else
				h += child->h_internal;
		}

		if (parent->layout_type == WzLayoutHorizontal)
			w += parent->child_gap * (parent->children_count - 1);
		else
			h += parent->child_gap * (parent->children_count - 1);

		if ((parent->alignment & WzAlignVCenter) && parent->layout_type == WzLayoutHorizontal)
		{
			x += (parent->w_internal - 4 * WZRD_BORDER_SIZE - parent->pad_left - parent->pad_right) / 2 - w / 2;
		}

		if ((parent->alignment & WzAlignHCenter) && parent->layout_type == WzLayoutHorizontal)
		{
			y += (parent->h_internal - 4 * WZRD_BORDER_SIZE - parent->pad_top - parent->pad_bottom) / 2 - h / 2;
		}

		// Calc positions
		for (int j = 0; j < parent->children_count; ++j) {
			WzWidget* child = &canvas->boxes[parent->children[j]];

			child->x_internal += x;
			child->y_internal += y;

			if ((parent->alignment & WzAlignHCenter) && parent->layout_type == WzLayoutHorizontal) {
				child->y_internal += (parent->h_internal - 4 * WZRD_BORDER_SIZE - parent->pad_top
					- parent->pad_bottom) / 2 - child->h_internal / 2;
			}

			if ((parent->alignment & WzAlignVCenter) && parent->layout_type == WzLayoutHorizontal) {
				child->x_internal += (parent->w_internal - 4 * WZRD_BORDER_SIZE -
					parent->pad_top - parent->pad_bottom) / 2 - child->w_internal / 2;
			}

			if (parent->layout_type == WzLayoutHorizontal) {
				x += child->w_internal;
				x += parent->child_gap;
			}
			else {
				y += child->h_internal;
				y += parent->child_gap;
			}
		}

		// Calculate positions for free children
		for (int j = 0; j < parent->free_children_count; ++j) {
			WzWidget* child = &canvas->boxes[parent->free_children[j]];

			child->x_internal += parent->x_internal;
			child->y_internal += parent->y_internal;
		}
	}

	// Test child doesn't exceed parent's size
	for (int i = 0; i < canvas->boxes_count; ++i) {
		for (int j = 0; j < canvas->boxes[i].children_count; ++j) {
			WzWidget* owner = canvas->boxes + i;
			WzWidget* child = canvas->boxes + canvas->boxes[i].children[j];
			if (!wzrd_is_rect_inside_rect((WzRect) { child->x_internal, child->y_internal, child->w_internal, child->h_internal }, (WzRect) { owner->x_internal, owner->y_internal, owner->w_internal, owner->h_internal })) {
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

void wzrd_draw(int* boxes_indices)
{
	wzrd_draw_commands_buffer* buffer = &canvas->command_buffer;
	buffer->count = 0;
	for (int i = 1; i < canvas->boxes_count; ++i)
	{
		WzWidget box = canvas->boxes[boxes_indices[i]];

		// Draw clip area
		{
			static WzHandle current_clip_widget;

			if (wzrd_handle_is_valid(box.clip_widget))
			{
				if (!wzrd_handle_is_equal(current_clip_widget, box.clip_widget))
				{
					WzWidget* clip_box = wz_widget_get(box.clip_widget);
					WZRD_ASSERT(buffer->count < MAX_NUM_DRAW_COMMANDS - 1);
					buffer->commands[buffer->count++] = (wzrd_draw_command){
						.type = DrawCommandType_Clip,
						.dest_rect = (WzRect) {clip_box->x_internal + 2 * WZRD_BORDER_SIZE, clip_box->y_internal + 2 * WZRD_BORDER_SIZE, clip_box->w_internal - 4 * WZRD_BORDER_SIZE, clip_box->h_internal - 4 * WZRD_BORDER_SIZE},
						.color = EGUI_BLUE
					};
				}
			}
			else {
				if (wzrd_handle_is_valid(current_clip_widget))
				{
					WZRD_ASSERT(buffer->count < MAX_NUM_DRAW_COMMANDS - 1);
					buffer->commands[buffer->count++] = (wzrd_draw_command){
						.type = DrawCommandType_StopClip,
					};
				}
			}

			buffer->commands[buffer->count - 1].box_index = i;

			current_clip_widget = box.clip_widget;
		}


		EguiRectDraw(buffer, (WzRect) {
			.x = box.x_internal,
				.y = box.y_internal,
				.h = box.h_internal,
				.w = box.w_internal
		},
			box.color,
			box.layer, i);


		// Borders (1215 x 810)
		unsigned int line_size = WZRD_BORDER_SIZE;

		WzRect top0 = (WzRect){ box.x_internal, box.y_internal, box.w_internal - line_size, line_size };
		WzRect left0 = (WzRect){ box.x_internal, box.y_internal, line_size, box.h_internal };

		WzRect top1 = (WzRect){ box.x_internal + line_size, box.y_internal + line_size, box.w_internal - 3 * line_size, line_size };
		WzRect left1 = (WzRect){ box.x_internal + line_size, box.y_internal + line_size, line_size, box.h_internal - line_size };

		WzRect bottom0 = (WzRect){ box.x_internal, box.y_internal + box.h_internal - line_size, box.w_internal, line_size };
		WzRect right0 = (WzRect){ box.x_internal + box.w_internal - line_size, box.y_internal, line_size, box.h_internal };

		WzRect bottom1 = (WzRect){ box.x_internal + 1 * line_size, box.y_internal + box.h_internal - 2 * line_size, box.w_internal - 3 * line_size, line_size };
		WzRect right1 = (WzRect){ box.x_internal + box.w_internal - 2 * line_size, box.y_internal + 1 * line_size, line_size, box.h_internal - 2 * line_size };

		wzrd_is_rect_inside_rect(top0, (WzRect) { box.x_internal, box.y_internal, box.w_internal, box.h_internal });
		wzrd_is_rect_inside_rect(left0, (WzRect) { box.x_internal, box.y_internal, box.w_internal, box.h_internal });
		wzrd_is_rect_inside_rect(top1, (WzRect) { box.x_internal, box.y_internal, box.w_internal, box.h_internal });
		wzrd_is_rect_inside_rect(left1, (WzRect) { box.x_internal, box.y_internal, box.w_internal, box.h_internal });
		wzrd_is_rect_inside_rect(bottom0, (WzRect) { box.x_internal, box.y_internal, box.w_internal, box.h_internal });
		wzrd_is_rect_inside_rect(right0, (WzRect) { box.x_internal, box.y_internal, box.w_internal, box.h_internal });
		wzrd_is_rect_inside_rect(bottom1, (WzRect) { box.x_internal, box.y_internal, box.w_internal, box.h_internal });
		wzrd_is_rect_inside_rect(right1, (WzRect) { box.x_internal, box.y_internal, box.w_internal, box.h_internal });

		if (box.border_type == BorderType_Default) {
			// Draw top and left lines
			EguiRectDraw(buffer, top0, EGUI_WHITE2, box.layer, i);
			EguiRectDraw(buffer, left0, EGUI_WHITE2, box.layer, i);
			EguiRectDraw(buffer, top1, EGUI_LIGHTGRAY, box.layer, i);
			EguiRectDraw(buffer, left1, EGUI_LIGHTGRAY, box.layer, i);

			// Draw bottom and right lines
			EguiRectDraw(buffer, bottom0, EGUI_BLACK, box.layer, i);
			EguiRectDraw(buffer, right0, EGUI_BLACK, box.layer, i);
			EguiRectDraw(buffer, bottom1, EGUI_GRAY, box.layer, i);
			EguiRectDraw(buffer, right1, EGUI_GRAY, box.layer, i);
		}
		else if (box.border_type == BorderType_Clicked) {
			// Draw top and left lines
			EguiRectDraw(buffer, top0, EGUI_BLACK, box.layer, i);
			EguiRectDraw(buffer, left0, EGUI_BLACK, box.layer, i);
			EguiRectDraw(buffer, top1, EGUI_GRAY, box.layer, i);
			EguiRectDraw(buffer, left1, EGUI_GRAY, box.layer, i);

			// Draw bottom and right lines
			EguiRectDraw(buffer, bottom0, EGUI_WHITE2, box.layer, i);
			EguiRectDraw(buffer, right0, EGUI_WHITE2, box.layer, i);
			EguiRectDraw(buffer, bottom1, EGUI_LIGHTESTGRAY, box.layer, i);
			EguiRectDraw(buffer, right1, EGUI_LIGHTESTGRAY, box.layer, i);
		}
		else if (box.border_type == BorderType_InputBox) {
			// Draw top and left lines
			EguiRectDraw(buffer, top0, EGUI_GRAY, box.layer, i);
			EguiRectDraw(buffer, left0, EGUI_GRAY, box.layer, i);
			EguiRectDraw(buffer, top1, EGUI_BLACK, box.layer, i);
			EguiRectDraw(buffer, left1, EGUI_BLACK, box.layer, i);

			// Draw bottom and right lines
			EguiRectDraw(buffer, bottom0, EGUI_WHITE2, box.layer, i);
			EguiRectDraw(buffer, right0, EGUI_WHITE2, box.layer, i);
			EguiRectDraw(buffer, bottom1, EGUI_LIGHTESTGRAY, box.layer, i);
			EguiRectDraw(buffer, right1, EGUI_LIGHTESTGRAY, box.layer, i);
		}
		else if (box.border_type == BorderType_Black) {
			// Draw top and left lines
			EguiRectDraw(buffer, top0, EGUI_BLACK, box.layer, i);
			EguiRectDraw(buffer, left0, EGUI_BLACK, box.layer, i);
			EguiRectDraw(buffer, top1, EGUI_WHITE2, box.layer, i);
			EguiRectDraw(buffer, left1, EGUI_WHITE2, box.layer, i);

			// Draw bottom and right lines
			EguiRectDraw(buffer, bottom0, EGUI_BLACK, box.layer, i);
			EguiRectDraw(buffer, right0, EGUI_BLACK, box.layer, i);
			EguiRectDraw(buffer, bottom1, EGUI_WHITE2, box.layer, i);
			EguiRectDraw(buffer, right1, EGUI_WHITE2, box.layer, i);
		}
		else if (box.border_type == BorderType_BottomLine) {
			EguiRectDraw(buffer, bottom0, EGUI_WHITE2, box.layer, i);
			EguiRectDraw(buffer, bottom1, EGUI_GRAY, box.layer, i);
		}
		else if (box.border_type == BorderType_LeftLine) {
			EguiRectDraw(buffer, left0, EGUI_GRAY, box.layer, i);
			EguiRectDraw(buffer, left1, EGUI_WHITE2, box.layer, i);
		}

		// Draw content
		for (int j = 0; j < box.items_count; ++j) {
			Item item = box.items[j];
			wzrd_draw_command command = { 0 };

			if (item.size.x == 0)
				item.size.x = box.w_internal;
			if (item.size.y == 0)
				item.size.y = box.h_internal;


			WzRect dest = (WzRect){
				box.x_internal,
				box.y_internal,
					box.w_internal,
					box.h_internal
			};

			// String item
			if (item.type == wzrd_item_type_str) {

				int w, h;
				canvas->get_string_size(item.val.str.str, &w, &h);

				if ((box.alignment & WzAlignHCenter))
				{
					dest.x += dest.w / 2 - w / 2;
				}

				if ((box.alignment & WzAlignVCenter))
				{
					dest.y += dest.h / 2 - h / 2;
				}

				dest.x += box.pad_left;
				dest.y += box.pad_top;
				dest.x -= box.pad_right;
				dest.y -= box.pad_bottom;

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
					.dest_rect = (WzRect){box.x_internal, box.y_internal, box.w_internal, box.h_internal},
					.src_rect = (WzRect) {0, 0, (int)item.val.texture.w, (int)item.val.texture.h},
					.texture = item.val.texture,
					.z = box.layer
				};

				if (item.scissor) {
					command.src_rect = (WzRect){ 0, 0, command.dest_rect.w, command.dest_rect.h };
				}
			}

			// Rect item
			if (item.type == ItemType_Rect) {
				command = (wzrd_draw_command){
					.type = DrawCommandType_Rect,
						.dest_rect = (WzRect){
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
						.dest_rect = (WzRect){
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
						.dest_rect = (WzRect){
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
						.dest_rect = (WzRect){
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
						.dest_rect = (WzRect){
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
						.dest_rect = (WzRect){
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
						.dest_rect = (WzRect){
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
						.dest_rect = (WzRect){
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

			buffer->commands[buffer->count - 1].box_index = i;
		}


	}

	canvas->clean = true;

}

void wzrd_widget_clip(WzHandle handle)
{
	WzWidget* box = wz_widget_get(handle);
	box->clip_widget = box->handle;
}

WzWidget wzrd_widget_get_cached_box_with_secondary_tag(const char* tag, const char* secondary_tag)
{
	for (int i = 0; i < canvas->cached_boxes_count; ++i)
	{
		if (canvas->cached_boxes[i].tag == tag && canvas->cached_boxes[i].secondary_tag == secondary_tag)
		{
			return canvas->cached_boxes[i];
		}
	}

	return (WzWidget) { 0 };
}

WzWidget wzrd_widget_get_cached_box(const char* tag)
{
	for (int i = 0; i < canvas->cached_boxes_count; ++i)
	{
		if (canvas->cached_boxes[i].tag == tag && !canvas->cached_boxes[i].secondary_tag)
		{
			return canvas->cached_boxes[i];
		}
	}

	return (WzWidget) { 0 };
}

void wzrd_widget_tag(WzHandle widget, const char* str)
{
	wz_widget_get(widget)->tag = str;
}

void wzrd_end(wzrd_str* debug_str)
{
	// Set internal size
	for (int i = 1; i < canvas->boxes_count; ++i)
	{
		WzWidget* box = &canvas->boxes[i];
		box->x_internal = box->x;
		box->y_internal = box->y;
		box->w_internal = box->w;
		box->h_internal = box->h;
	}

	// box
	wzrd_do_layout();

	// Cache tagged elements
	canvas->cached_boxes_count = 0;
	for (int i = 1; i < canvas->boxes_count; ++i)
	{
		assert(canvas->cached_boxes_count < MAX_NUM_CACHED_BOXES - 1);
		WzWidget* box = &canvas->boxes[i];
		if (box->tag)
		{
			canvas->cached_boxes[canvas->cached_boxes_count++] = *box;
		}
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
		wzrd_handle_cursor();
		wzrd_handle_border_resize();
	}
	else {
		if (canvas->mouse_left == WZRD_DEACTIVATING)
		{
			//canvas->released_item = canvas->dragged_item;
			//canvas->released_item = canvas->dragged_item;
			canvas->dragged_box = (WzWidget){ 0 };
			canvas->dragged_item = (WzHandle){ 0 };
			canvas->active_item = (WzHandle){ 0 };
		}
	}

	canvas->previous_mouse_pos = canvas->mouse_pos;

	wzrd_draw(boxes_indices);

	canvas = 0;
}

void wzrd_box_set_type(WzHandle handle, wzrd_box_type type)
{
	wz_widget_get(handle)->layout_type = type;
}

WzHandle egui_button_raw_begin(bool* released, WzHandle parent, const char* file_name, unsigned int line) {

	WzHandle handle = wz_widget(parent);
	wz_widget_add_source(handle, file_name, line);
	wzrd_box_set_type(handle, wzrd_box_type_button);

	if (wzrd_handle_is_equal(handle, canvas->deactivating_item)) {
		*released = true;
	}
	else {
		*released = false;
	}

	return handle;
}

wzrd_canvas* wzrd_canvas_get()
{
	return canvas;
}

WzHandle wzrd_toggle_icon_raw(wzrd_texture texture, bool* active, WzHandle parent, const char* file_name, unsigned int line) {
	bool b = false;
	WzHandle widget = egui_button_raw_begin(&b, parent, file_name, line);
	wz_widget_add_source(widget, file_name, line);

	return widget;
}

WzHandle wzrd_button_icon_raw(wzrd_texture texture, bool* result, WzHandle parent, const char* file_name, unsigned int line) {
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

	WzHandle widget = wz_widget(parent);

	return widget;
#endif
}

WzHandle wzrd_command_toggle_raw(wzrd_str str, bool* active, WzHandle parent, const char* file_name, unsigned int line) {
	bool b1 = false, a1 = false;

	WzHandle widget = egui_button_raw_begin(&b1, parent, file_name, line);
	wz_widget_add_source(widget, file_name, line);
	wzrd_text_add(str, widget);

	if (b1)
	{
		*active = !*active;
	}

	a1 = wzrd_box_is_active(&canvas->boxes[canvas->boxes_count - 1]);

	if (*active || a1)
	{
		wz_widget_get(widget)->border_type = BorderType_Clicked;
	}

	return widget;
}

WzHandle egui_button_raw_begin_on_half_click(bool* b, WzHandle parent) {
	WzHandle h = wz_widget(parent);

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

WzHandle wzrd_label_button_activating(wzrd_str str, bool* active, WzHandle parent)
{
	WzHandle h = egui_button_raw_begin_on_half_click(active, parent);
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

void wzrd_item_add(Item item, WzHandle box) {
	WzWidget* b = wz_widget_get(box);
	WZRD_ASSERT(b->items_count < MAX_NUM_ITEMS - 1);
	b->items[b->items_count++] = item;
}

WzHandle wzrd_label_raw(wzrd_str str, WzHandle parent, const char* file_name, unsigned int line) {
	int w = 0, h = 0;
	canvas->get_string_size(str.str, &w, &h);

	WzHandle widget = wz_widget(parent);
	wz_widget_add_source(widget, file_name, line);

	wz_widget_set_size(widget, w, h);

	wzrd_text_add(str, widget);

	return widget;
}

WzHandle wzrd_input_box_raw(char* str, int* len, int max_num_keys, WzHandle parent, const char* file_name, unsigned int line) {
	WzHandle widget = wz_widget(parent);
	wz_widget_add_source(widget, file_name, line);

	WzWidget* box = wz_widget_get(widget);
	box->w = 50;
	box->h = 30;
	box->color = EGUI_GREEN;

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
	wzrd_text_add((wzrd_str) { .str = str, .len = *len }, widget);
#endif

	return widget;
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

WzHandle wzrd_label_button_raw(wzrd_str str, bool* result, WzHandle parent, const char* file_name, unsigned  int line) {
	int w = 0, h = 0;
	canvas->get_string_size(str.str, &w, &h);

	WzHandle widget = wz_widget(parent);
	wz_widget_add_source(widget, file_name, line);

	wz_widget_set_w(widget, w);
	wz_widget_set_h(widget, h);

	wzrd_box_set_type(widget, wzrd_box_type_button);

	wzrd_text_add(str, widget);

	if (wzrd_box_is_activating(wzrd_box_get_last()))
	{
		*result = true;
	}

	return widget;
}

WzHandle wzrd_dialog_begin_raw(wzrd_v2* pos, wzrd_v2 size,
	bool* active, wzrd_str name, int layer, WzHandle parent, const char* file_name, unsigned int line) {
	WZRD_UNUSED(name);

	if (!*active) return (WzHandle) { 0 };

	WzHandle window = wzrd_widget_free(
		/*	(wzrd_style) {
			.space = (wzrd_space){ .x = pos->x, .y = pos->y, .w = size.x, .h = size.y },
				.skin = canvas->panel_border_skin,
		},*/
		parent);
	wz_widget_add_source(window, file_name, line);

	wz_widget_get(window)->layer = layer;

	bool close = false;

	WzHandle top_panel = wz_widget(window);

	wz_widget_set_h(top_panel, 28);

	wz_widget_get(top_panel)->h = 28;
	//wzrd_box_get(top_panel)->canvas = v_panel_layout;

	WzHandle bar = wz_widget(top_panel);

	wzrd_widget_set_color(bar, (wzrd_color) { 57, 77, 205, 255 });

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

WzHandle wzrd_dropdown_raw(int* selected_text, const wzrd_str* texts, int texts_count, int w, bool* active, WzHandle parent, const char* file_name, unsigned int line)
{
#if 0
	WZRD_ASSERT(texts);
	WZRD_ASSERT(texts_count);
	WZRD_ASSERT(selected_text);
	WZRD_ASSERT(*selected_text >= 0);

	w = 150;

	wzrd_handle panel = wzrd_widget(
		(wzrd_style) {
		.layout.fit_w = true,
			.layout.fit_h = true,
			.skin.color = EGUI_WHITE,
			.skin.border_type = BorderType_InputBox,
			.layout.type = WzLayoutRow
	}, parent);

	bool released = false;
	wzrd_command_button(texts[*selected_text], &released, panel);
	wzrd_command_button(wzrd_str_create("b"), &released, panel);

	if (*active) {

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
#endif

	WzHandle widget = wz_widget(parent);

	return widget;
}


void wz_set_alignment(WzHandle widget, WzAlignment alignment)
{
	wz_widget_get(widget)->alignment |= alignment;
}

void wzrd_label_list_raw(wzrd_str* item_names, unsigned int count,
	wzrd_v2 size, WzHandle* handles, unsigned int* selected, bool* is_selected, WzHandle parent, const char* file_name, unsigned int line)
{
	WzHandle panel = wz_widget(parent);
	wz_widget_add_source(panel, file_name, line);

	wz_widget_set_size(panel, size.x, size.y);

	if (wzrd_box_is_activating(wz_widget_get(panel))) {
		*selected = 0;
		*is_selected = false;
	}

	WzHandle selected_label = { 0 };

	for (unsigned int i = 0; i < count; ++i)
	{
		char str[32];
		sprintf_s(str, 32, "%d label list %d", wzrd_box_get_last()->handle.handle, i);

		bool is_label_clicked = false;
		WzHandle h = wzrd_label_button_activating(item_names[i], &is_label_clicked, panel);

		wz_widget_set_h(h, 32);
		wz_set_alignment(h, WzAlignVCenter | WzAlignHCenter);


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
			wz_widget_get(h)->color = canvas->stylesheet.label_item_selected_color;
		}
	}
}

void wzrd_label_list_sorted_raw(wzrd_str* item_names, unsigned int count, int* items,
	wzrd_v2 size, unsigned int* selected, bool* is_selected, WzHandle parent, const char* file_name, unsigned int line) {

	WzHandle handles[MAX_NUM_LABELS] = { 0 };

	wzrd_label_list_raw(item_names, count, size, handles, selected, is_selected, parent, file_name, line);

	// Ordering
	{
		assert(items);
		WzHandle active_label = { 0 }, hovered_label = { 0 }, released_label = { 0 };
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
		WzWidget* hovered_parent = 0;
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

			if (canvas->mouse_pos.y > hovered_parent->y + hovered_parent->h / 2)
			{
				is_bottom = true;
			}

			// Label grabbed and hovering over another one
			if (wzrd_handle_is_valid(active_label) && wzrd_handle_is_valid(hovered_label) && !wzrd_handle_is_equal(hovered_label, active_label))
			{

				WzWidget* p = wz_widget_get(hovered_label);
				WzWidget* c = 0;
				if (is_bottom)
				{
					c = wz_widget_create();
					wz_widget_add_source(c->handle, __FILE__, __LINE__);
					wzrd_widget_set_color(c->handle, EGUI_PURPLE);
					wz_widget_set_y(c->handle, hovered_parent->h - 2);
					wz_widget_set_h(c->handle, 2);

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

WzHandle wzrd_handle_button_raw(bool* active, WzRect rect,
	wzrd_color color, wzrd_str name, WzHandle parent, const char* file_name, unsigned int line) {

	WzHandle widget = wzrd_widget_free(parent);
	wz_widget_add_source(widget, file_name, line);
	wz_widget_set_pos(widget, rect.x, rect.y);
	wz_widget_set_size(widget, rect.w, rect.h);
	wzrd_box_set_type(widget, wzrd_box_type_button);
	*active = wzrd_box_is_active(wz_widget_get(widget));

	return widget;
}

void wzrd_drag(bool* drag) {

	if (canvas->mouse_left == WZRD_INACTIVE)
	{
		*drag = false;
	}

	if (!(*drag)) return;

	canvas->dragged_box.x += canvas->mouse_delta.x;
	canvas->dragged_box.y += canvas->mouse_delta.y;

	wzrd_crate(1, canvas->dragged_box);
}

bool wzrd_box_is_active(WzWidget* box) {
	if (wzrd_handle_is_equal(box->handle, canvas->active_item)) {
		return true;
	}

	return false;
}

bool wzrd_box_is_dragged(WzWidget* box) {
	if (wzrd_handle_is_equal(box->handle, canvas->dragged_item)) {
		return true;
	}

	return false;
}

bool wzrd_box_is_hot_using_canvas(wzrd_canvas* c, WzWidget* box) {
	if (wzrd_handle_is_equal(box->handle, c->hovered_item)) {
		return true;
	}

	return false;
}

bool wzrd_box_is_hot(WzWidget* box) {
	if (wzrd_handle_is_equal(box->handle, canvas->hovered_item)) {
		return true;
	}

	return false;
}


WzWidget* wzrd_box_get_released()
{
	WzWidget* result = 0;

	if (wzrd_handle_is_valid(canvas->deactivating_item))
	{
		result = wz_widget_get(canvas->deactivating_item);
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

void wzrd_widget_set_rect(WzHandle widget, WzRect rect)
{
	WzWidget* w = wz_widget_get(widget);
	w->x = rect.x;
	w->y = rect.y;
	w->w = rect.w;
	w->h = rect.h;
}

WzHandle wzrd_command_button_raw(wzrd_str str, bool* released, WzHandle parent, const char* file_name, unsigned int line)
{
	WzHandle button = egui_button_raw_begin(released, parent, file_name, line);
	wz_widget_add_source(button, file_name, line);

	wzrd_text_add(str, button);

	if (wzrd_handle_is_interacting(button))
	{
		//wzrd_skin_set(button, canvas->command_button_on_skin);
	}

	return button;
}

void wzrd_handle_set_layer(WzHandle handle, unsigned int layer)
{
	wz_widget_get(handle)->layer = layer;
}
