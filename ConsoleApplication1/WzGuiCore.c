// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#define WZ_ASSERT(x) assert(x)
//#define WZ_ASSERT(x) (void)(x)
#define WZRD_UNUSED(x) (void)x

#include "WzGuiCore.h"
#include "Strings.h"

static wzrd_canvas* canvas;

WzWidget wz_widget_raw(WzWidget parent);
bool wz_widget_is_equal(WzWidget a, WzWidget b);
bool wz_handle_is_valid(WzWidget handle);
WzWidget wzrd_handle_create();
WzWidgetData* wzrd_box_get_parent();
WzWidgetData* wzrd_box_get_previous();
void wzrd_crate_begin(int window_id, WzWidgetData box);
void wzrd_crate(int window_id, WzWidgetData box);
void wzrd_texture_add(wzrd_texture texture, wzrd_v2 size, WzWidget box);
void wzrd_text_add(wzrd_str str, WzWidget box);
//void wzrd_box_resize(wzrd_v2* size);
void wzrd_crate_end();
WzWidgetData* wz_widget_get(WzWidget str);
void wzrd_drag(bool* drag);
WzWidgetData* wzrd_box_get_last();
bool wzrd_box_is_dragged(WzWidgetData* box);
bool wzrd_box_is_hot_using_canvas(wzrd_canvas* canvas, WzWidgetData* box);
WzWidgetData* wzrd_box_get_released();
WzWidgetData* wzrd_box_find(wzrd_canvas* canvas, wzrd_str name);
wzrd_v2 wzrd_lerp(wzrd_v2 pos, wzrd_v2 end_pos);

bool wz_widget_is_equal(WzWidget a, WzWidget b)
{
	return a.handle == b.handle;
}

typedef struct stylesheet
{
	int button_color;
	int button_color_clicked;
} stylesheet;

void wz_widget_add_source(WzWidget handle, const char* file, unsigned int line)
{
	// We want only the file name
	const char* f;
	for (f = file + strlen(file); *(f - 1) != '\\'; --f) {}

	wz_widget_get(handle)->file = f;
	wz_widget_get(handle)->line = line;
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

//void wz_widget_set_size_policy(WzWidget widget, WzLayoutSizePolicy size_policy)
//{
//	WzWidgetData* w = wz_widget_get(widget);
//
//	if (size_policy == WzSizePolicyPreferred)
//	{
//		w->size_policy_flag_vertical |= WzSizePolicyFlagGrow;
//		w->size_policy_flag_vertical |= WzSizePolicyFlagShrink;
//	}
//}

WzWidget wzrd_vbox_border_raw(wzrd_v2 size, WzWidget parent, const char* file, unsigned int line)
{
	WzWidget handle = wz_widget_raw(parent, file, line);
	wz_widget_set_tight_constraints(handle, size.x, size.y);
	//wz_widget_set_size_policy(handle, WzSizePolicyPreferred);

	return handle;
}

WzWidget wzrd_vbox_border_click(wzrd_v2 size, WzWidget parent)
{
	WzWidget handle = wz_hbox(parent);
	//wz_widget_set_size(handle, size.x, size.y);

	return handle;
}

bool wz_handle_is_valid(WzWidget handle)
{
	return (bool)handle.handle;
}

WzWidgetData* wz_widget_get(WzWidget handle) {
	if (!wz_handle_is_valid(handle))
	{
		return canvas->widgets;
	}

	WzWidgetData* result = 0;
	for (int i = 0; i < canvas->widgets_count; ++i)
	{
		if (canvas->widgets[i].handle.handle == handle.handle)
		{
			result = canvas->widgets + i;
			break;
		}
	}

	assert(result);

	return result;
}

void wz_widget_set_tight_constraints(WzWidget handle, int w, int h)
{
	WzWidgetData* b = wz_widget_get(handle);
	b->constraint_min_w = b->constraint_max_w = w;
	b->constraint_min_h = b->constraint_max_h = h;
}

void wz_widget_set_size(WzWidget handle, unsigned int w, unsigned int h)
{
	WzWidgetData* b = wz_widget_get(handle);
	b->actual_w = w;
	b->actual_h = h;
}

WzWidget wzrd_handle_create()
{
	assert(canvas);
	WzWidget handle = { canvas->widgets_count };

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

void wz_widget_set_constraint_h(WzWidget w, int height)
{
	WzWidgetData* b = wz_widget_get(w);
	b->constraint_max_h = height;
}

void wz_widget_set_constraint_w(WzWidget w, int width)
{
	WzWidgetData* b = wz_widget_get(w);
	b->constraint_max_w = width;
}

void wz_widget_set_x(WzWidget w, int x)
{
	WzWidgetData* widget = wz_widget_get(w);
	widget->x = x;
}

void wz_widget_set_y(WzWidget w, int y)
{
	WzWidgetData* widget = wz_widget_get(w);
	widget->y = y;
}

void wz_widget_set_border(WzWidget w, WzBorderType border_type)
{
	wz_widget_get(w)->border_type = border_type;
}

void wz_widget_set_pos(WzWidget handle, int x, int y)
{
	wz_widget_set_x(handle, x);
	wz_widget_set_y(handle, y);
}


void wz_widget_set_color(WzWidget widget, WzColor color)
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
	WZ_ASSERT(canvas->current_crate_index > 0);
	Crate* result = &canvas->crates_stack[canvas->current_crate_index - 1];

	return result;
}

WzWidgetData* wzrd_box_get_parent() {
	if (canvas->crates_stack[canvas->current_crate_index].box_stack_count < 2)
	{
		return &canvas->widgets[0];
	}
	int current_box_index = canvas->crates_stack[canvas->current_crate_index].box_stack_count - 2;
	int final_index = canvas->crates_stack[canvas->current_crate_index].box_stack[current_box_index];
	WzWidgetData* result = &canvas->widgets[final_index];

	return result;
}

bool wzrd_handle_is_child_of_handle(WzWidget a, WzWidget b)
{
	int stack[MAX_NUM_BOXES];
	int count = 0;
	int ptr = 0;

	stack[count++] = wz_widget_get(a)->index;

	while (ptr != count)
	{
		WzWidgetData* box = &canvas->widgets[stack[ptr]];

		for (int i = 0; i < box->children_count; ++i)
		{
			stack[count++] = box->children[i];
		}

		if (wz_widget_is_equal(box->handle, b))
			return true;

		ptr++;
	}

	return false;
}


int wzrd_box_get_current_index() {
	WZ_ASSERT(canvas->current_crate_index >= 0);
	int current_box_index = canvas->crates_stack[canvas->current_crate_index].box_stack_count - 1;
	if (current_box_index < 0) return 0;
	int final_index = canvas->crates_stack[canvas->current_crate_index].box_stack[current_box_index];
	int result = final_index;

	return result;
}

WzWidgetData* wzrd_box_get_last() {
	WzWidgetData* result = canvas->widgets + (canvas->widgets_count - 1);

	return result;
}

void wzrd_text_add(wzrd_str str, WzWidget parent)
{
	wzrd_item_add((Item) { .type = wzrd_item_type_str, .val = { .str = str }, .color = EGUI_BLACK }, parent);
}

void goo(WzWidgetData* box, void* data)
{
	(void)data;
	(void)box;
	//box->color = EGUI_BEIGE;
}

void wzrd_box_tree_apply(int index, void* data, void (*goo)(WzWidgetData* box, void* data))
{
	int stack[1024];
	int count = 0;
	int ptr = 0;

	stack[count++] = index;

	while (ptr != count)
	{
		WzWidgetData* box = &canvas->widgets[stack[ptr]];

		for (int i = 0; i < box->children_count; ++i)
		{
			stack[count++] = box->children[i];
		}

		goo(box, data);
		ptr++;
	}
}

bool wzrd_handle_is_active_tree(WzWidget handle)
{
	int stack[1024];
	int count = 0;
	int ptr = 0;

	stack[count++] = wz_widget_get(handle)->index;

	while (ptr != count)
	{
		WzWidgetData* box = &canvas->widgets[stack[ptr]];

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

bool wzrd_widget_is_deactivating(WzWidget handle)
{
	if (wz_widget_is_equal(handle, canvas->deactivating_item))
	{
		return true;
	}

	return false;
}

bool wzrd_box_is_released(WzWidgetData* box) {
	if (wz_widget_is_equal(box->handle, canvas->deactivating_item)) {
		return true;
	}

	return false;
}
bool wzrd_handle_is_released_tree(WzWidget handle)
{
	int stack[1024];
	int count = 0;
	int ptr = 0;

	stack[count++] = wz_widget_get(handle)->index;

	while (ptr != count)
	{
		WzWidgetData* box = &canvas->widgets[stack[ptr]];

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
	WzWidgetData* c1 = canvas->widgets + index1;
	WzWidgetData* c2 = canvas->widgets + index2;

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

void wzrd_box_add_free_child(WzWidget parent, WzWidget child)
{
	WzWidgetData* p = wz_widget_get(parent);
	WzWidgetData* c = wz_widget_get(child);
	WZ_ASSERT(p->free_children_count < MAX_NUM_CHILDREN - 1);
	p->free_children[p->free_children_count++] = c->index;

	c->layer = p->layer;
	c->clip_widget = p->clip_widget;

}

void wzrd_box_add_child(WzWidget parent, WzWidget child)
{
	WzWidgetData* p = wz_widget_get(parent);
	WzWidgetData* c = wz_widget_get(child);
	WZ_ASSERT(p->children_count < MAX_NUM_CHILDREN - 1);
	p->children[p->children_count++] = c->index;

	c->layer = p->layer;
	c->clip_widget = p->clip_widget;
	c->parent = parent;
}

void wzrd_box_add_child_using_pointer(WzWidgetData* parent, WzWidgetData* child)
{
	WZ_ASSERT(parent->children_count < MAX_NUM_CHILDREN - 1);
	parent->children[parent->children_count++] = child->index;
}

void wz_widget_set_flex_factor(WzWidget widget, unsigned int flex_factor)
{
	wz_widget_get(widget)->flex_factor = flex_factor;
}

void wz_widget_set_expanded(WzWidget widget)
{
	WzWidgetData* widg = wz_widget_get(widget);
	widg->flex_factor = 1;
	widg->flex_fit = FlexFitTight;
}

void wz_widget_set_flex(WzWidget widget)
{
	WzWidgetData* widg = wz_widget_get(widget);
	widg->flex_factor = 1;
	widg->flex_fit = FlexFitLoose;
}

WzWidgetData* wz_widget_create()
{
	WzWidgetData box;
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
	box.file = 0;
	box.line = 0;
	box.clip_widget.handle = 0;
	box.w_offset = 0;
	box.h_offset = 0;
	box.disable_hover = false;
	box.layer = 0;
	box.actual_w = box.actual_h = 0;
	box.actual_x = box.actual_y = 0;
	box.layout = WzLayoutNone;
	box.flex_factor = 0;
	box.constraint_min_w = box.constraint_min_h = 0;
	box.constraint_max_w = box.constraint_max_h = UINT_MAX;
	box.x = box.y = 0;
	box.border_type = BorderType_Black;

	box.handle = wzrd_handle_create();

	WZ_ASSERT(canvas->widgets_count < MAX_NUM_BOXES - 1);

	box.layer = EguiGetCurrentWindow()->layer;

	WZ_ASSERT(canvas->widgets_count < 256);

	box.index = canvas->widgets_count;
	canvas->widgets[canvas->widgets_count++] = box;

	return &canvas->widgets[canvas->widgets_count - 1];
}

WzWidget wzrd_widget_free(WzWidget parent)
{
	WzWidgetData* box = wz_widget_create();
	box->layer = wz_widget_get(parent)->layer;


	return box->handle;
}

WzWidget wz_widget_raw(WzWidget parent, const char* file, unsigned int line)
{
	WzWidgetData* widget = wz_widget_create();
	wz_widget_add_source(widget->handle, file, line);

	wzrd_box_add_child(parent, widget->handle);

	return widget->handle;
}

WzWidget wz_vbox_raw(WzWidget parent, const char* file, unsigned int line)
{
	WzWidget p = wz_widget_raw(parent, file, line);
	wz_widget_set_layout(p, WzLayoutVertical);
	wz_widget_set_expanded(p);

	return p;
}

WzWidget wz_hbox_raw(WzWidget parent, const char* file, unsigned int line)
{
	WzWidget p = wz_widget_raw(parent, file, line);
	wz_widget_set_layout(p, WzLayoutHorizontal);
	wz_widget_set_expanded(p);

	return p;
}

wz_widget_add_rect(WzWidget widget, unsigned int w, unsigned int h, WzColor color)
{
	Item item;
	item.size.x = w;
	item.size.y = h;
	item.type = ItemType_Rect;
	item.color = color;

	wzrd_item_add(item, widget);
}

void wzrd_texture_add(wzrd_texture texture, wzrd_v2 size, WzWidget parent) {
	wzrd_item_add((Item) {
		.type = ItemType_Texture,
			.size = size,
			.val = { .texture = texture }
	}, parent);
}

void wzrd_crate_begin(int layer, WzWidgetData box) {

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

void wzrd_crate(int window_id, WzWidgetData box) {
	wzrd_crate_begin(window_id, box);
	wzrd_crate_end();
}

//void wz_widget_set_fixed_size(WzWidget widget, unsigned int w, unsigned int h)
//{
//	WzWidgetData* wd = wz_widget_get(widget);
//	wd->size_hint_w = w;
//	wd->size_hint_h = h;
//}

WzWidget wz_begin(wzrd_canvas* gui,
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

	canvas->widgets_count = 0;
	canvas->input_box_timer += 16.7f;

	canvas->clip_boxes_count = 1;

	// Empty box
	canvas->widgets[canvas->widgets_count++] = (WzWidgetData){ 0 };

	// Window
	WzWidget window_widget = wz_widget((WzWidget) { 0 });
	wz_widget_add_constraints(window_widget, window.w, window.h, window.w, window.h);

	// Stylesheet
	canvas->stylesheet.label_color = EGUI_LIGHTGRAY;
	canvas->stylesheet.label_item_selected_color = EGUI_LIGHTGRAY;

	return window_widget;
}

void EguiRectDraw(WzDrawCommandBuffer* buffer, WzRect rect, WzColor color, int z, unsigned int widget_line_number, const char* file)
{
	WZ_ASSERT(rect.w > 0);
	WZ_ASSERT(rect.h > 0);
	WZ_ASSERT(color.a);

	WzDrawCommand command = (WzDrawCommand){
		.type = DrawCommandType_Rect,
		.dest_rect = rect,
		.color = color,
		.z = z
	};

	command.widget_line_number = widget_line_number;
	command.file = file;

	WZ_ASSERT(buffer->count < MAX_NUM_DRAW_COMMANDS - 1);
	buffer->commands[buffer->count++] = command;

}

WzWidgetData* wzrd_box_find(wzrd_canvas* c, wzrd_str name)
{
	// TODO: optimize	
	unsigned int hash = wzrd_hash(name);

	for (int i = 0; i < c->widgets_count; ++i)
	{
		if (hash == c->widgets[i].handle.handle)
		{
			return c->widgets + i;
		}
	}

	return &c->widgets[0];
}


WzWidgetData* wzrd_box_get_previous() {
	WzWidgetData* result = &canvas->widgets[canvas->widgets_count - 1];

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
	canvas->left_resized_item = (WzWidget){ 0 };
	canvas->right_resized_item = (WzWidget){ 0 };
	canvas->top_resized_item = (WzWidget){ 0 };
	canvas->bottom_resized_item = (WzWidget){ 0 };

	for (int i = 0; i < canvas->widgets_count; ++i) {
		WzWidgetData* owner = canvas->widgets + i;
		for (int j = 0; j < owner->children_count; ++j) {
			WzWidgetData* child = &canvas->widgets[owner->children[j]];

			int border_size = 10;

			//if (child->type != wzrd_box_type_resizable) continue;

			bool is_inside_left_border =
				canvas->mouse_pos.x >= child->actual_w &&
				canvas->mouse_pos.y >= child->actual_y &&
				canvas->mouse_pos.x < child->actual_w + border_size &&
				canvas->mouse_pos.y < child->actual_y + child->actual_h;
			bool is_inside_right_border =
				canvas->mouse_pos.x >= child->actual_w + child->actual_w - border_size &&
				canvas->mouse_pos.y >= child->actual_y &&
				canvas->mouse_pos.x < child->actual_w + child->actual_w &&
				canvas->mouse_pos.y < child->actual_y + child->actual_h;
			bool is_inside_top_border =
				canvas->mouse_pos.x >= child->actual_w &&
				canvas->mouse_pos.y >= child->actual_y &&
				canvas->mouse_pos.x < child->actual_w + child->actual_w &&
				canvas->mouse_pos.y < child->actual_y + border_size;
			bool is_inside_bottom_border =
				canvas->mouse_pos.x >= child->actual_w &&
				canvas->mouse_pos.y >= child->actual_y + child->actual_h - border_size &&
				canvas->mouse_pos.x < child->actual_w + child->actual_w &&
				canvas->mouse_pos.y < child->actual_y + child->actual_h;


			if (wz_widget_is_equal(canvas->hovered_item, child->handle) || wz_widget_is_equal(canvas->active_item, child->handle)) {

				if (is_inside_top_border || is_inside_bottom_border) {
					child->color = EGUI_PURPLE;
					//*cursor = wzrd_cursor_vertical_arrow;
				}
				else if (is_inside_left_border || is_inside_right_border) {
					child->color = EGUI_PURPLE;
					//*cursor = wzrd_cursor_horizontal_arrow;
				}
			}

			if (wz_widget_is_equal(canvas->active_item, child->handle)) {
				if (is_inside_bottom_border) {
					child->color = EGUI_PURPLE;
					canvas->bottom_resized_item = child->handle;
				}
				else if (is_inside_top_border) {
					child->color = EGUI_PURPLE;
					canvas->top_resized_item = child->handle;
				}
				else if (is_inside_left_border) {
					child->color = EGUI_PURPLE;
					canvas->left_resized_item = child->handle;
				}
				else if (is_inside_right_border) {
					child->color = EGUI_PURPLE;
					canvas->right_resized_item = child->handle;
				}
			}
		}
	}
}

bool wzrd_handle_is_active(WzWidget handle) {
	if (wz_widget_is_equal(handle, canvas->active_item)) {
		return true;
	}

	return false;
}

bool wzrd_handle_is_released(WzWidget handle) {
	if (wz_widget_is_equal(handle, canvas->deactivating_item)) {
		return true;
	}

	return false;
}

bool wzrd_handle_is_hovered(WzWidget handle) {
	if (wz_widget_is_equal(handle, canvas->hovered_item)) {
		return true;
	}

	return false;
}

bool wzrd_handle_is_hovered_from_list(WzWidget handle)
{

	for (int i = 0; i < canvas->hovered_items_list_count; ++i)
	{
		if (wz_widget_is_equal(handle, canvas->hovered_items_list[i]))
		{
			return true;
		}
	}

	return false;
}

bool wzrd_handle_is_interacting(WzWidget handle) {
	if (wz_widget_is_equal(handle, canvas->activating_item) ||
		wz_widget_is_equal(handle, canvas->active_item) ||
		wz_widget_is_equal(handle, canvas->deactivating_item))
	{
		return true;
	}

	return false;
}

void wzrd_handle_input(int* indices, int count)
{
	WzWidgetData* hovered_box = canvas->widgets;
	unsigned int max_layer = 0;
	canvas->hovered_items_list_count = 0;
	canvas->hovered_boxes_count = 0;
	for (int i = 0; i < count; ++i) {
		WzWidgetData* box = canvas->widgets + indices[i];

		WzRect scaled_rect = { box->actual_w, box->actual_y, box->actual_w, box->actual_h };

		// Input Clipping: Block mouse from reaching clipped widgets
		if (wz_handle_is_valid(box->clip_widget))
		{
			WzWidgetData* clip_widget = wz_widget_get(box->clip_widget);

			if (box->actual_w < clip_widget->actual_w)
			{
				scaled_rect.x = clip_widget->actual_w;
			}

			if (box->actual_y < clip_widget->actual_y)
			{
				scaled_rect.y = clip_widget->actual_y;
			}

			if (box->actual_w + box->actual_w > clip_widget->actual_w + clip_widget->actual_w)
			{
				scaled_rect.w = clip_widget->actual_w;
			}

			if (box->actual_y + box->actual_h > clip_widget->actual_y + clip_widget->actual_h)
			{
				scaled_rect.y = clip_widget->actual_h;
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
	WzWidgetData* half_clicked_box = wz_widget_get(canvas->activating_item);
	if (half_clicked_box && canvas->mouse_left == WZRD_ACTIVE)
	{
		canvas->activating_item = (WzWidget){ 0 };
	}

	if (canvas->mouse_left == WZRD_DEACTIVATING)
	{
		//canvas->released_item = canvas->dragged_item;
		canvas->dragged_box = (WzWidgetData){ 0 };
		canvas->dragged_item = (WzWidget){ 0 };

		canvas->clean = false;
	}

	if (canvas->mouse_left == WZRD_INACTIVE)
	{
		canvas->deactivating_item = (WzWidget){ 0 };
	}

	if (wz_handle_is_valid(hovered_box->handle))
	{
		canvas->hovered_item = hovered_box->handle;
	}
	else {
		canvas->hovered_item = (WzWidget){ 0 };
	}

	WzWidgetData* hot_box = wz_widget_get(canvas->hovered_item);
	WzWidgetData* active_box = wz_widget_get(canvas->active_item);

	if (wz_handle_is_valid(active_box->handle)) {

		if (canvas->mouse_left == WZRD_DEACTIVATING) {
			if (wz_handle_is_valid(canvas->active_item))
			{
				if (hot_box == active_box)
				{
					canvas->clicked_item = active_box->handle;
					//canvas->selected_item = hot_box->handle;
				}

				canvas->deactivating_item = canvas->active_item;
				wz_widget_get(canvas->deactivating_item)->color = EGUI_PURPLE;
				canvas->active_item = (WzWidget){ 0 };
			}
		}

		canvas->clean = false;
	}

	if (wz_handle_is_valid(hot_box->handle)) {

		/*	if (hot_box->type == wzrd_box_type_flat_button)
			{
				hot_box->color = (wzrd_color){ 0, 255, 255, 255 };
			}*/
		if (canvas->mouse_left == EguiActivating) {
			canvas->active_item = hot_box->handle;

			canvas->activating_item = hot_box->handle;

			// Dragging
			WZ_ASSERT(half_clicked_box);
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
	if (wz_handle_is_valid(canvas->clicked_item) && canvas->mouse_left == WZRD_INACTIVE) {
		canvas->clicked_item = (WzWidget){ 0 };

		canvas->clean = false;
	}

	// Input box
	if (wz_handle_is_valid(canvas->clicked_item)) {
		WzWidgetData* clicked_box = wz_widget_get(canvas->clicked_item);
		WZ_ASSERT(clicked_box);
		/*	if (clicked_box->type == wzrd_box_type_input_box) {
				canvas->active_input_box = clicked_box->handle;
			}
			else {
				canvas->active_input_box = (wzrd_handle){ 0 };
			}*/
	}
}

bool wzrd_widget_is_activating(WzWidget handle) {
	if (wz_widget_is_equal(handle, canvas->activating_item)) {
		return true;
	}

	return false;
}

bool wzrd_widget_is_active(WzWidget handle) {
	if (wz_widget_is_equal(handle, canvas->active_item)) {
		return true;
	}

	return false;
}

bool wzrd_box_is_activating(WzWidgetData* box) {
	if (wz_widget_is_equal(box->handle, canvas->activating_item)) {
		return true;
	}

	return false;
}

void wzrd_box_bring_to_front(WzWidgetData* box, void* data)
{
	(void)data;
	box->bring_to_front = true;
}

void wz_widget_set_stretch_factor(WzWidget handle, unsigned int flex_factor)
{
	WzWidgetData* w = wz_widget_get(handle);
	w->flex_factor = flex_factor;
}

void widget_set_main_axis_size_min(WzWidget w)
{
	WzWidgetData* d = wz_widget_get(w);
	d->main_axis_size_type = MAIN_AXIS_SIZE_TYPE_MIN;
}

void wz_widget_set_layout(WzWidget handle, WzLayout layout)
{
	WzWidgetData* d = wz_widget_get(handle);
	d->layout = layout;
	d->flex_fit = FlexFitTight;
	d->main_axis_size_type = MAIN_AXIS_SIZE_TYPE_MAX;
}

void wz_widget_add_constraints(WzWidget widget,
	unsigned int min_w, unsigned int min_h, unsigned int max_w, unsigned int max_h)
{
	WzWidgetData* data = wz_widget_get(widget);
	data->constraint_min_w = min_w;
	data->constraint_min_h = min_h;
	data->constraint_max_w = max_w;
	data->constraint_max_h = max_h;
}

#define WZ_LOG(...) printf(__VA_ARGS__)

void wzrd_do_layout(int index)
{
	unsigned int widgets_stack[MAX_NUM_BOXES];
	unsigned int widgets_stack_count = 0;

	unsigned int widgets_visits[MAX_NUM_BOXES];

	unsigned int size_per_flex_factor;
	WzWidgetData* widget;
	WzWidgetData* child;

	unsigned int w;
	unsigned int h;
	unsigned int available_size_main_axis, available_size_cross_axis;
	int i;
	unsigned int children_flex_factor;
	unsigned int children_size, max_child_h;
	unsigned int children_h, max_child_w;

	memset(widgets_visits, 0, sizeof(*widgets_visits) * MAX_NUM_BOXES);

	widgets_stack[widgets_stack_count++] = index;

	unsigned int* constraint_min_main_axis, * constraint_max_main_axis,
		* constraint_min_cross_axis, * constraint_max_cross_axis,
		* actual_size_main_axis, * actual_size_cross_axis;

	unsigned int* child_constraint_min_main_axis, * child_constraint_max_main_axis,
		* child_constraint_min_cross_axis, * child_constraint_max_cross_axis,
		* child_actual_size_main_axis, * child_cross_axis_actual_size;

	unsigned int screen_size_main_axis, screen_size_cross_axis;

	WZ_LOG("---------------------------\n");


	// Constraints pass
	while (widgets_stack_count)
	{
		widget = &canvas->widgets[widgets_stack[widgets_stack_count - 1]];

		WZ_ASSERT(widget->constraint_min_w >= 0);
		WZ_ASSERT(widget->constraint_min_h >= 0);
		WZ_ASSERT(widget->constraint_max_w > 0);
		WZ_ASSERT(widget->constraint_max_h > 0);
		WZ_ASSERT(widget->actual_w <= canvas->window.w);
		WZ_ASSERT(widget->actual_h <= canvas->window.h);

		if (!widget->children_count)
		{
			// Size leaf widgets, and pop immediately
			// For now all leaf widgets must have a finite constraint
			// Later on we'll let them decide their own size based on their content
			WZ_ASSERT(widget->constraint_max_w != UINT_MAX);
			widget->actual_w = widget->constraint_max_w;

			WZ_ASSERT(widget->constraint_max_h != UINT_MAX);
			widget->actual_h = widget->constraint_max_h;

			WZ_ASSERT(widget->actual_w);
			WZ_ASSERT(widget->actual_h);
			WZ_ASSERT(widget->actual_w <= canvas->window.w);
			WZ_ASSERT(widget->actual_h <= canvas->window.h);
			WZ_ASSERT(widget->actual_w <= widget->constraint_max_w);
			WZ_ASSERT(widget->actual_h <= widget->constraint_max_h);
			WZ_ASSERT(widget->actual_w <= canvas->window.w);
			WZ_ASSERT(widget->actual_h <= canvas->window.h);

			WZ_LOG("Leaf widget (%s, %d) with constraints (%u, %u) determined its size (%u, %u)\n", widget->file, widget->line,
				widget->constraint_max_w, widget->constraint_max_h, widget->actual_w, widget->actual_h);

			widgets_stack_count--;
		}
		else
		{
			// Handle widgets with children
			if (widget->layout == WzLayoutHorizontal || widget->layout == WzLayoutVertical)
			{
				// delete this line of code, only exists to appease the compiler
				constraint_min_cross_axis = constraint_max_cross_axis = 0;

				if (widget->layout == WzLayoutHorizontal)
				{
					constraint_min_main_axis = &widget->constraint_min_w;
					constraint_max_main_axis = &widget->constraint_max_w;
					constraint_min_cross_axis = &widget->constraint_min_h;
					constraint_max_cross_axis = &widget->constraint_max_h;
					actual_size_main_axis = &widget->actual_w;
					actual_size_cross_axis = &widget->actual_h;
					screen_size_main_axis = canvas->window.w;
					screen_size_cross_axis = canvas->window.h;
				}
				else if (widget->layout == WzLayoutVertical)
				{
					constraint_min_main_axis = &widget->constraint_min_h;
					constraint_max_main_axis = &widget->constraint_max_h;
					constraint_min_cross_axis = &widget->constraint_min_w;
					constraint_max_cross_axis = &widget->constraint_max_w;
					actual_size_main_axis = &widget->actual_h;
					actual_size_cross_axis = &widget->actual_w;
					screen_size_main_axis = canvas->window.h;
					screen_size_cross_axis = canvas->window.w;
				}
				else
				{
					child_constraint_min_main_axis = child_constraint_max_main_axis =
						child_constraint_max_cross_axis = child_constraint_min_cross_axis = 0;
					constraint_max_main_axis = 0;
					actual_size_main_axis = actual_size_cross_axis = 0;

					WZ_ASSERT(0);
				}

				// You got 3 visits for layout widget.
				// 1. Non Flex children get fixed constraints
				// 2. Above children determine their desired size, and now we allocate available space to flex children
				// 3. It's the turn of the flex children to determine their size, and then we can finally assess the 
				//    layout widget's size using it's children's 
				if (widgets_visits[widget->index] == 0)
				{
					const char* widget_type = 0;
					if (widget->layout == WzLayoutHorizontal)
					{
						widget_type = "Row";
					}
					else if (widget->layout == WzLayoutVertical)
					{
						widget_type = "Column";
					}

					WZ_LOG("%s (%s, %d) begins constrains non-flex children\n", widget_type, widget->file, widget->line);

					// Give constraints to non flex children
					// A child with flex factor 0 recieves unbounded constraints in the main axis
					for (int i = 0; i < widget->children_count; ++i)
					{
						child = &canvas->widgets[widget->children[i]];

						if (child->flex_factor == 0)
						{

							if (widget->layout == WzLayoutHorizontal)
							{
								child_constraint_min_main_axis = &child->constraint_min_w;
								child_constraint_max_main_axis = &child->constraint_max_w;
								child_constraint_min_cross_axis = &child->constraint_min_h;
								child_constraint_max_cross_axis = &child->constraint_max_h;
							}
							else if (widget->layout == WzLayoutVertical)
							{
								child_constraint_min_main_axis = &child->constraint_min_h;
								child_constraint_max_main_axis = &child->constraint_max_h;
								child_constraint_min_cross_axis = &child->constraint_min_w;
								child_constraint_max_cross_axis = &child->constraint_max_w;
							}
							else
							{
								child_constraint_min_main_axis = child_constraint_max_main_axis =
									child_constraint_max_cross_axis = child_constraint_min_cross_axis = 0;
								child_constraint_max_cross_axis = 0;
								WZ_ASSERT(0);
							}

							// The child must have min axis size if it's unbounded
							WZ_ASSERT(!(*child_constraint_max_main_axis == UINT_MAX && child->main_axis_size_type == MAIN_AXIS_SIZE_TYPE_MAX));

							// Leave by default all constraints as they are
							// Child will only inherit the cross axis constraint
							if (*constraint_max_cross_axis < *child_constraint_max_cross_axis)
							{
								*child_constraint_max_cross_axis = *constraint_max_cross_axis;
							}

							WZ_ASSERT(*child_constraint_max_main_axis);
							WZ_ASSERT(*child_constraint_max_cross_axis);

							WZ_LOG("Non-flex widget (%s, %d) with constraints (%u, %u)\n", child->file, child->line,
								child->constraint_max_w, child->constraint_max_h);

							widgets_stack[widgets_stack_count] = child->index;
							widgets_stack_count++;
						}

					}
					WZ_LOG("%s (%s, %d) ends constrains non-flex children\n", widget_type, widget->file, widget->line);

					widgets_visits[widget->index] = 1;
				}
				else if (widgets_visits[widget->index] == 1)
				{
					// Give constraints to flex children, allocating from the availble space
					if (widget->layout == WzLayoutHorizontal)
					{
						available_size_main_axis = widget->constraint_max_w;
						available_size_cross_axis = widget->constraint_max_h;
					}
					else if (widget->layout == WzLayoutVertical)
					{
						available_size_main_axis = widget->constraint_max_h;
						available_size_cross_axis = widget->constraint_max_w;
					}
					children_flex_factor = 0;

					for (i = 0; i < widget->children_count; ++i)
					{
						child = &canvas->widgets[widget->children[i]];

						if (widget->layout == WzLayoutHorizontal)
						{
							child_actual_size_main_axis = &child->actual_w;
						}
						else if (widget->layout == WzLayoutVertical)
						{
							child_actual_size_main_axis = &child->actual_h;
						}
						else
						{
							child_actual_size_main_axis = 0;
						}

						if (!child->flex_factor)
						{
							WZ_ASSERT(*child_actual_size_main_axis);
							available_size_main_axis -= *child_actual_size_main_axis;
						}
						else
						{
							children_flex_factor += child->flex_factor;
						}

					}

					if (children_flex_factor)
					{
						size_per_flex_factor = available_size_main_axis / children_flex_factor;
					}

					const char* widget_type = 0;
					if (widget->layout == WzLayoutHorizontal)
					{
						widget_type = "Row";
					}
					else if (widget->layout == WzLayoutVertical)
					{
						widget_type = "Column";
					}

					WZ_LOG("%s (%s, %d) begins constrains flex children\n", widget_type, widget->file, widget->line);

					for (int i = 0; i < widget->children_count; ++i)
					{
						child = &canvas->widgets[widget->children[i]];

						if (widget->layout == WzLayoutHorizontal)
						{
							child_constraint_min_main_axis = &child->constraint_min_w;
							child_constraint_max_main_axis = &child->constraint_max_w;
							child_constraint_min_cross_axis = &child->constraint_min_h;
							child_constraint_max_cross_axis = &child->constraint_max_h;
						}
						else if (widget->layout == WzLayoutVertical)
						{
							child_constraint_min_main_axis = &child->constraint_min_h;
							child_constraint_max_main_axis = &child->constraint_max_h;
							child_constraint_min_cross_axis = &child->constraint_min_w;
							child_constraint_max_cross_axis = &child->constraint_max_w;
						}
						else
						{
							child_constraint_min_main_axis = child_constraint_max_main_axis =
								child_constraint_max_cross_axis = child_constraint_min_cross_axis = 0;
							WZ_ASSERT(0);
						}

						if (child->flex_factor)
						{
							unsigned int main_axis_size = size_per_flex_factor * child->flex_factor;

							if (widget->flex_fit == FlexFitTight)
							{
								child_constraint_min_main_axis = main_axis_size;
							}

							if (*constraint_min_cross_axis < *child_constraint_min_cross_axis)
							{
								*child_constraint_min_cross_axis = *constraint_min_cross_axis;
							}
							if (main_axis_size < *child_constraint_max_main_axis)
							{
								*child_constraint_max_main_axis = main_axis_size;
							}
							if (*constraint_max_cross_axis < *child_constraint_max_cross_axis)
							{
								*child_constraint_max_cross_axis = *constraint_max_cross_axis;
							}

							WZ_LOG("Flex widget (%s, %d) with constraints (%u, %u) \n", child->file, child->line,
								child->constraint_max_w, child->constraint_max_h);

							widgets_stack[widgets_stack_count] = child->index;
							widgets_stack_count++;
						}
					}

					WZ_LOG("%s (%s, %d) ends constrains flex children\n", widget_type, widget->file, widget->line);

					widgets_visits[widget->index] = 2;
				}
				else if (widgets_visits[widget->index] == 2)
				{
					// We finally determined the size of all the children of a widget with a layout
					// Now we determine it's size

					if (*constraint_max_main_axis == UINT_MAX)
					{
						// Layout widget is unconstrained in the main axis
						// It must shrink-wrap
						WZ_ASSERT(widget->main_axis_size_type == MAIN_AXIS_SIZE_TYPE_MIN);

						children_size = 0;
						for (i = 0; i < widget->children_count; ++i)
						{
							child = &canvas->widgets[widget->children[i]];

							child_actual_size_main_axis = &child->actual_w;

							children_size += *child_actual_size_main_axis;
						}

						*actual_size_main_axis = children_size;
					}
					else
					{
						// Determine size of widget that is constrained in the main axis
						*actual_size_main_axis = *constraint_max_main_axis;
					}

					*actual_size_cross_axis = *constraint_max_cross_axis;

					if (widget->layout == WzLayoutHorizontal)
					{
						WZ_LOG("Row widget (%s, %d) with constraints (%u, %u) determined its size (%u, %u)\n", widget->file, widget->line,
							widget->constraint_max_w, widget->constraint_max_h, widget->actual_w, widget->actual_h);
					}
					else if (widget->layout == WzLayoutVertical)
					{
						WZ_LOG("Column widget (%s, %d) with constraints (%u, %u) determined its size (%u, %u)\n", widget->file, widget->line,
							widget->constraint_max_w, widget->constraint_max_h, widget->actual_w, widget->actual_h);
					}

					WZ_ASSERT(*actual_size_main_axis <= screen_size_main_axis);
					WZ_ASSERT(*actual_size_cross_axis <= screen_size_cross_axis);

					// Give positions for children
					unsigned int offset = 0;
					for (int i = 0; i < widget->children_count; ++i)
					{
						child = &canvas->widgets[widget->children[i]];
						if (widget->layout == WzLayoutHorizontal)
						{
							actual_size_main_axis = &child->actual_w;
							child->x += offset;
							child->y = 0;
						}
						else if (widget->layout == WzLayoutVertical)
						{
							actual_size_main_axis = &child->actual_h;
							child->y += offset;
							child->x = 0;
						}
						else
						{
							WZ_ASSERT(0);
						}

						offset += *actual_size_main_axis;

						WZ_ASSERT(child->x <= canvas->window.w);
						WZ_ASSERT(child->y <= canvas->window.h);
					}

					widgets_stack_count--;
				}
			}
			else if (widget->layout == WzLayoutNone)
			{
				if (widget->children_count == 1)
				{
					child = &canvas->widgets[widget->children[0]];

					if (widgets_visits[widget->index] == 0)
					{
						if (widget->constraint_min_w > child->constraint_min_w)
							child->constraint_min_w = widget->constraint_min_w;
						if (widget->constraint_min_h > child->constraint_min_h)
							child->constraint_min_h = widget->constraint_min_h;
						if (widget->constraint_max_w < child->constraint_max_w)
							child->constraint_max_w = widget->constraint_max_w;
						if (widget->constraint_max_h < child->constraint_max_h)
							child->constraint_max_h = widget->constraint_max_h;

						WZ_LOG("Widget (%s, %d) with constraints (%u, %u)\n", child->file, child->line,
							child->constraint_max_w, child->constraint_max_h, child->actual_w, child->actual_h);

						widgets_stack[widgets_stack_count] = child->index;
						widgets_stack_count++;
						widgets_visits[widget->index] = 1;
					}
					else if (widgets_visits[widget->index] == 1)
					{
						if (!widget->actual_w)
							widget->actual_w = child->actual_w;
						if (!widget->actual_h)
							widget->actual_h = child->actual_h;

						WZ_LOG("Widget (%s, %d) with constraints (%u, %u) determined its size (%u, %u)\n", widget->file, widget->line,
							widget->constraint_max_w, widget->constraint_max_h, widget->actual_w, widget->actual_h);

						WZ_ASSERT(widget->actual_w <= canvas->window.w);
						WZ_ASSERT(widget->actual_h <= canvas->window.h);

						widgets_stack_count--;
					}
				}
				else
				{
					// A widget without layout cannot have more than one child
					WZ_ASSERT(0);
				}
			}
		}
	}


	// Final stage. Calculate the widgets non-relative final position 
	for (int i = 1; i < canvas->widgets_count; ++i)
	{
		widget = &canvas->widgets[i];

		for (int j = 0; j < widget->children_count; ++j)
		{
			child = &canvas->widgets[widget->children[j]];
			child->actual_x = widget->x + child->x;
			child->actual_y = widget->y + child->y;

			// Check the widgets size doesnt exceeds its parents
			WZ_ASSERT(child->actual_x >= 0);
			WZ_ASSERT(child->actual_y >= 0);
			WZ_ASSERT(child->actual_w);
			WZ_ASSERT(child->actual_h);
			WZ_ASSERT(widget->actual_w);
			WZ_ASSERT(widget->actual_h);
			WZ_ASSERT(child->actual_x + child->actual_w <= widget->actual_x + widget->actual_w);
			WZ_ASSERT(child->actual_y + child->actual_h <= widget->actual_y + widget->actual_h);
		}
	}
	WZ_LOG("---------------------------\n");
	WZ_LOG("Final Layout:\n");
	for (int i = 0; i < canvas->widgets_count; ++i)
	{
		widget = &canvas->widgets[i];
		WZ_LOG("(%s %u) : (%u %u %u %u)\n", widget->file, widget->line, widget->actual_x, widget->actual_y, widget->actual_w, widget->actual_h);
	}
	WZ_LOG("---------------------------\n");


}

void wzrd_do_layout2()
{

#if 0
	// Dragging
	if (wz_handle_is_valid(canvas->dragged_box.handle))
	{
		canvas->dragged_box.actual_w += canvas->mouse_delta.x;
		canvas->dragged_box.actual_y += canvas->mouse_delta.y;

		wzrd_crate(1, canvas->dragged_box);
	}

	// Calculate size
	for (int i = 1; i < canvas->widgets_count; ++i)
	{
		WzWidgetData* parent = &canvas->widgets[i];

		WZ_ASSERT(parent->actual_w > 0);
		WZ_ASSERT(parent->actual_h > 0);

		// Calculate available size
		unsigned int available_w = 0, available_h = 0, children_w = 0, children_h = 0, children_stretch_factor = 0;
		{
			for (int j = 0; j < parent->children_count; ++j)
			{
				WzWidgetData* child = &canvas->widgets[parent->children[j]];
				children_w += child->actual_w;
				children_h += child->actual_h;

				children_stretch_factor += child->flex_factor;
			}

			WZ_ASSERT(parent->actual_w);
			WZ_ASSERT(parent->actual_h);

			available_w = parent->actual_w - parent->pad_left - parent->pad_right - 4 * WZRD_BORDER_SIZE;
			available_h = parent->actual_h - parent->pad_top - parent->pad_bottom - 4 * WZRD_BORDER_SIZE;

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
			if (children_stretch_factor)
			{
				strech_w = available_w / children_stretch_factor;
				strech_h = available_h / children_stretch_factor;
			}

			for (int j = 0; j < parent->children_count; ++j)
			{
				WzWidgetData* child = &canvas->widgets[parent->children[j]];

				if (1)
				{
					if (child->flex_factor)
					{
						if (parent->layout_type == WzLayoutHorizontal)
						{
							WZ_ASSERT(strech_w);
							child->actual_w = strech_w * child->flex_factor;

							if (!child->h)
							{
								child->actual_h = available_h;
							}

						}
						else if (parent->layout_type == WzLayoutVertical)
						{
							WZ_ASSERT(strech_h);
							child->actual_h = strech_h * child->flex_factor;

							if (!child->w)
							{
								child->actual_w = available_w;
							}

						}
					}
				}
				else
				{
					// OLD CODE
					if (child->flex_factor && !child->fit_w && !child->percentage_w)
					{
						if (parent->layout_type == WzLayoutHorizontal)
						{
							WZ_ASSERT(available_w >= children_w);
							child->actual_w = available_w - children_w;
						}
						else
						{
							child->actual_w = available_w;
						}

					}

					if (child->flex_factor && !child->fit_h && !child->percentage_h)
					{
						if (parent->layout_type == WzLayoutHorizontal)
						{
							WZ_ASSERT(available_h >= children_h);
							child->actual_h = available_h - children_h;
						}
						else
						{
							child->actual_h = available_h;
						}

					}
				}

				if (child->best_fit && parent->actual_h && child->actual_h)
				{
					float ratio_a = parent->actual_w / parent->actual_h;
					float ratio_b = child->actual_w / child->actual_h;
					float ratio = 0;

					if (wzrd_float_compare(ratio_b, ratio_a) >= 0)
					{
						WZ_ASSERT(child->actual_w);
						ratio = (parent->actual_w / child->actual_w);
					}
					else
					{
						WZ_ASSERT(child->actual_h);
						ratio = (parent->actual_h / child->actual_h);
					}

					child->actual_w = child->actual_w * ratio;
					child->actual_h = child->actual_h * ratio;
				}

				if (child->percentage_w)
				{
					child->actual_w = parent->actual_w * child->percentage_w;
				}
				if (child->percentage_h)
				{
					child->actual_h = parent->actual_h * child->percentage_h;
				}

				WZ_ASSERT(child->actual_w > 0);
				WZ_ASSERT(child->actual_h > 0);
			}
		}

		// Calcuate size for free children
		for (int j = 0; j < parent->free_children_count; ++j)
		{
			WzWidgetData* child = &canvas->widgets[parent->free_children[j]];

			if (!child->actual_w)
			{
				child->actual_w = parent->actual_w;
			}
			if (!child->actual_h)
			{
				child->actual_h = parent->actual_h;
			}

			WZ_ASSERT(child->actual_w > 0);
			WZ_ASSERT(child->actual_h > 0);
		}
	}

	// Calculate content size
	for (int i = 1; i < canvas->widgets_count; ++i)
	{
		WzWidgetData* parent = canvas->widgets + i;

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
			WzWidgetData child = canvas->widgets[parent->children[j]];

			if (parent->actual_h > max_child_h)
			{
				max_child_h = child.actual_h;
			}

			if (parent->actual_w > max_child_w)
			{
				max_child_w = child.actual_w;
			}

			if (parent->layout_type == WzLayoutHorizontal)
			{
				parent->content_w += child.actual_w;
			}
			else
			{
				parent->content_h += child.actual_h;
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
	for (int i = 1; i < canvas->widgets_count; ++i) {
		WzWidgetData* parent = &canvas->widgets[i];

		int x = parent->actual_w + parent->pad_left, y = parent->actual_y + parent->pad_top;

		x += 2 * WZRD_BORDER_SIZE;
		y += 2 * WZRD_BORDER_SIZE;

		// Center
		int w = 0, h = 0, max_w = 0, max_h = 0;
		for (int j = 0; j < parent->children_count; ++j) {
			WzWidgetData* child = &canvas->widgets[parent->children[j]];

			if (child->actual_w > max_w)
				max_w = child->actual_w;

			if (child->actual_h > max_h)
				max_h = child->actual_h;

			if (parent->layout_type == WzLayoutHorizontal)
				w += child->actual_w;
			else
				h += child->actual_h;
		}

		if (parent->layout_type == WzLayoutHorizontal)
			w += parent->child_gap * (parent->children_count - 1);
		else
			h += parent->child_gap * (parent->children_count - 1);

		if ((parent->alignment & WzAlignVCenter) && parent->layout_type == WzLayoutHorizontal)
		{
			x += (parent->actual_w - 4 * WZRD_BORDER_SIZE - parent->pad_left - parent->pad_right) / 2 - w / 2;
		}

		if ((parent->alignment & WzAlignHCenter) && parent->layout_type == WzLayoutHorizontal)
		{
			y += (parent->actual_h - 4 * WZRD_BORDER_SIZE - parent->pad_top - parent->pad_bottom) / 2 - h / 2;
		}

		// Calc positions
		for (int j = 0; j < parent->children_count; ++j) {
			WzWidgetData* child = &canvas->widgets[parent->children[j]];

			child->actual_w += x;
			child->actual_y += y;

			if ((parent->alignment & WzAlignHCenter) && parent->layout_type == WzLayoutHorizontal) {
				child->actual_y += (parent->actual_h - 4 * WZRD_BORDER_SIZE - parent->pad_top
					- parent->pad_bottom) / 2 - child->actual_h / 2;
			}

			if ((parent->alignment & WzAlignVCenter) && parent->layout_type == WzLayoutHorizontal) {
				child->actual_w += (parent->actual_w - 4 * WZRD_BORDER_SIZE -
					parent->pad_top - parent->pad_bottom) / 2 - child->actual_w / 2;
			}

			if (parent->layout_type == WzLayoutHorizontal) {
				x += child->actual_w;
				x += parent->child_gap;
			}
			else {
				y += child->actual_h;
				y += parent->child_gap;
			}
		}

		// Calculate positions for free children
		for (int j = 0; j < parent->free_children_count; ++j) {
			WzWidgetData* child = &canvas->widgets[parent->free_children[j]];

			child->actual_w += parent->actual_w;
			child->actual_y += parent->actual_y;
		}
	}

	// Test child doesn't exceed parent's size
	for (int i = 0; i < canvas->widgets_count; ++i) {
		for (int j = 0; j < canvas->widgets[i].children_count; ++j) {
			WzWidgetData* owner = canvas->widgets + i;
			WzWidgetData* child = canvas->widgets + canvas->widgets[i].children[j];
			if (!wzrd_is_rect_inside_rect((WzRect) { child->actual_w, child->actual_y, child->actual_w, child->actual_h }, (WzRect) { owner->actual_w, owner->actual_y, owner->actual_w, owner->actual_h })) {
				//owner->color = EGUI_ORANGE;
				//child->color = EGUI_RED;
			}
		}
	}

	// Bring to front
	if (wz_handle_is_valid(canvas->active_item))
	{
		//wzrd_box_tree_apply(wzrd_box_get_by_handle(canvas->active_item)->index, 0, wzrd_box_bring_to_front);
	}
#endif
}


void wzrd_draw(int* boxes_indices)
{
	WzWidgetData widget;
	WzDrawCommand command;
	WzDrawCommandBuffer* buffer = &canvas->command_buffer;
	buffer->count = 0;
	unsigned int line_size;

	for (int i = 1; i < canvas->widgets_count; ++i)
	{
		widget = canvas->widgets[boxes_indices[i]];

#if 1
		// Draw clip area
		{
			static WzWidget current_clip_widget;

			if (wz_handle_is_valid(widget.clip_widget))
			{
				if (!wz_widget_is_equal(current_clip_widget, widget.clip_widget))
				{
					WzWidgetData* clip_box = wz_widget_get(widget.clip_widget);
					WZ_ASSERT(buffer->count < MAX_NUM_DRAW_COMMANDS - 1);
					buffer->commands[buffer->count++] = (WzDrawCommand){
						.type = DrawCommandType_Clip,
						.dest_rect = (WzRect) {clip_box->actual_w + 2 * WZRD_BORDER_SIZE, clip_box->actual_y + 2 * WZRD_BORDER_SIZE, clip_box->actual_w - 4 * WZRD_BORDER_SIZE, clip_box->actual_h - 4 * WZRD_BORDER_SIZE},
						.color = WZ_BLUE
					};
				}
			}
			else
			{
				if (wz_handle_is_valid(current_clip_widget))
				{
					WZ_ASSERT(buffer->count < MAX_NUM_DRAW_COMMANDS - 1);
					buffer->commands[buffer->count++] = (WzDrawCommand){
						.type = DrawCommandType_StopClip,
					};
				}
			}

			buffer->commands[buffer->count - 1].widget_line_number = i;

			current_clip_widget = widget.clip_widget;
		}

		// Draw Widget
		if (1)
		{
			EguiRectDraw(buffer, (WzRect) {
				.x = widget.actual_x,
					.y = widget.actual_y,
					.w = widget.actual_w,
					.h = widget.actual_h,
			},
				widget.color,
				widget.layer, widget.line, widget.file);

			continue;
		}

		// Borders (1215 x 810)
		if (1)
		{
			line_size = WZRD_BORDER_SIZE;

			WzRect top0 = (WzRect){ widget.actual_w, widget.actual_y, widget.actual_w - line_size, line_size };
			WzRect left0 = (WzRect){ widget.actual_w, widget.actual_y, line_size, widget.actual_h };

			WzRect top1 = (WzRect){ widget.actual_w + line_size, widget.actual_y + line_size, widget.actual_w - 3 * line_size, line_size };
			WzRect left1 = (WzRect){ widget.actual_w + line_size, widget.actual_y + line_size, line_size, widget.actual_h - line_size };

			WzRect bottom0 = (WzRect){ widget.actual_w, widget.actual_y + widget.actual_h - line_size, widget.actual_w, line_size };
			WzRect right0 = (WzRect){ widget.actual_w + widget.actual_w - line_size, widget.actual_y, line_size, widget.actual_h };

			WzRect bottom1 = (WzRect){ widget.actual_w + 1 * line_size, widget.actual_y + widget.actual_h - 2 * line_size, widget.actual_w - 3 * line_size, line_size };
			WzRect right1 = (WzRect){ widget.actual_w + widget.actual_w - 2 * line_size, widget.actual_y + 1 * line_size, line_size, widget.actual_h - 2 * line_size };

			wzrd_is_rect_inside_rect(top0, (WzRect) { widget.actual_w, widget.actual_y, widget.actual_w, widget.actual_h });
			wzrd_is_rect_inside_rect(left0, (WzRect) { widget.actual_w, widget.actual_y, widget.actual_w, widget.actual_h });
			wzrd_is_rect_inside_rect(top1, (WzRect) { widget.actual_w, widget.actual_y, widget.actual_w, widget.actual_h });
			wzrd_is_rect_inside_rect(left1, (WzRect) { widget.actual_w, widget.actual_y, widget.actual_w, widget.actual_h });
			wzrd_is_rect_inside_rect(bottom0, (WzRect) { widget.actual_w, widget.actual_y, widget.actual_w, widget.actual_h });
			wzrd_is_rect_inside_rect(right0, (WzRect) { widget.actual_w, widget.actual_y, widget.actual_w, widget.actual_h });
			wzrd_is_rect_inside_rect(bottom1, (WzRect) { widget.actual_w, widget.actual_y, widget.actual_w, widget.actual_h });
			wzrd_is_rect_inside_rect(right1, (WzRect) { widget.actual_w, widget.actual_y, widget.actual_w, widget.actual_h });

			if (widget.border_type == BorderType_Default) {
				// Draw top and left lines
				EguiRectDraw(buffer, top0, EGUI_WHITE2, widget.layer, widget.line, widget.file);
				EguiRectDraw(buffer, left0, EGUI_WHITE2, widget.layer, widget.line, widget.file);
				EguiRectDraw(buffer, top1, EGUI_LIGHTGRAY, widget.layer, widget.line, widget.file);
				EguiRectDraw(buffer, left1, EGUI_LIGHTGRAY, widget.layer, widget.line, widget.file);

				// Draw bottom and right lines
				EguiRectDraw(buffer, bottom0, EGUI_BLACK, widget.layer, widget.line, widget.file);
				EguiRectDraw(buffer, right0, EGUI_BLACK, widget.layer, widget.line, widget.file);
				EguiRectDraw(buffer, bottom1, EGUI_GRAY, widget.layer, widget.line, widget.file);
				EguiRectDraw(buffer, right1, EGUI_GRAY, widget.layer, widget.line, widget.file);
			}
			else if (widget.border_type == BorderType_Clicked) {
				// Draw top and left lines
				EguiRectDraw(buffer, top0, EGUI_BLACK, widget.layer, widget.line, widget.file);
				EguiRectDraw(buffer, left0, EGUI_BLACK, widget.layer, widget.line, widget.file);
				EguiRectDraw(buffer, top1, EGUI_GRAY, widget.layer, widget.line, widget.file);
				EguiRectDraw(buffer, left1, EGUI_GRAY, widget.layer, widget.line, widget.file);

				// Draw bottom and right lines
				EguiRectDraw(buffer, bottom0, EGUI_WHITE2, widget.layer, widget.line, widget.file);
				EguiRectDraw(buffer, right0, EGUI_WHITE2, widget.layer, widget.line, widget.file);
				EguiRectDraw(buffer, bottom1, EGUI_LIGHTESTGRAY, widget.layer, widget.line, widget.file);
				EguiRectDraw(buffer, right1, EGUI_LIGHTESTGRAY, widget.layer, widget.line, widget.file);
			}
			else if (widget.border_type == BorderType_InputBox) {
				// Draw top and left lines
				EguiRectDraw(buffer, top0, EGUI_GRAY, widget.layer, widget.line, widget.file);
				EguiRectDraw(buffer, left0, EGUI_GRAY, widget.layer, widget.line, widget.file);
				EguiRectDraw(buffer, top1, EGUI_BLACK, widget.layer, widget.line, widget.file);
				EguiRectDraw(buffer, left1, EGUI_BLACK, widget.layer, widget.line, widget.file);

				// Draw bottom and right lines
				EguiRectDraw(buffer, bottom0, EGUI_WHITE2, widget.layer, widget.line, widget.file);
				EguiRectDraw(buffer, right0, EGUI_WHITE2, widget.layer, widget.line, widget.file);
				EguiRectDraw(buffer, bottom1, EGUI_LIGHTESTGRAY, widget.layer, widget.line, widget.file);
				EguiRectDraw(buffer, right1, EGUI_LIGHTESTGRAY, widget.layer, widget.line, widget.file);
			}
			else if (widget.border_type == BorderType_Black) {
				// Draw top and left lines
				EguiRectDraw(buffer, top0, EGUI_BLACK, widget.layer, widget.line, widget.file);
				EguiRectDraw(buffer, left0, EGUI_BLACK, widget.layer, widget.line, widget.file);
				EguiRectDraw(buffer, top1, EGUI_BLACK, widget.layer, widget.line, widget.file);
				EguiRectDraw(buffer, left1, EGUI_BLACK, widget.layer, widget.line, widget.file);

				// Draw bottom and right lines
				EguiRectDraw(buffer, bottom0, EGUI_BLACK, widget.layer, widget.line, widget.file);
				EguiRectDraw(buffer, right0, EGUI_BLACK, widget.layer, widget.line, widget.file);
				EguiRectDraw(buffer, bottom1, EGUI_BLACK, widget.layer, widget.line, widget.file);
				EguiRectDraw(buffer, right1, EGUI_BLACK, widget.layer, widget.line, widget.file);
			}
			else if (widget.border_type == BorderType_BottomLine) {
				EguiRectDraw(buffer, bottom0, EGUI_WHITE2, widget.layer, widget.line, widget.file);
				EguiRectDraw(buffer, bottom1, EGUI_GRAY, widget.layer, widget.line, widget.file);
			}
			else if (widget.border_type == BorderType_LeftLine) {
				EguiRectDraw(buffer, left0, EGUI_GRAY, widget.layer, widget.line, widget.file);
				EguiRectDraw(buffer, left1, EGUI_WHITE2, widget.layer, widget.line, widget.file);
			}
		}

		// Draw content
		for (int j = 0; j < widget.items_count; ++j) {
			Item item = widget.items[j];

			if (item.size.x == 0)
				item.size.x = widget.actual_w;
			if (item.size.y == 0)
				item.size.y = widget.actual_h;

			WzRect dest = (WzRect){
				widget.actual_w,
				widget.actual_y,
					widget.actual_w,
					widget.actual_h
			};

			// String item
			if (item.type == wzrd_item_type_str)
			{
#if 0
				int w, h;
				canvas->get_string_size(item.val.str.str, &w, &h);

				if ((widget.alignment & WzAlignHCenter))
				{
					dest.x += dest.w / 2 - w / 2;
				}

				if ((widget.alignment & WzAlignVCenter))
				{
					dest.y += dest.h / 2 - h / 2;
				}

				dest.x += widget.pad_left;
				dest.y += widget.pad_top;
				dest.x -= widget.pad_right;
				dest.y -= widget.pad_bottom;

				command = (WzDrawCommand){
					.type = DrawCommandType_String,
						.str = item.val.str,
						.dest_rect = dest,
						.color = item.color,
						.z = widget.layer
				};
#else
				continue;
#endif
			}

			// Texture item
			if (item.type == ItemType_Texture) {
				command = (WzDrawCommand){
					.type = DrawCommandType_Texture,
					.dest_rect = (WzRect){widget.actual_w, widget.actual_y, widget.actual_w, widget.actual_h},
					.src_rect = (WzRect) {0, 0, (int)item.val.texture.w, (int)item.val.texture.h},
					.texture = item.val.texture,
					.z = widget.layer
				};

				if (item.scissor) {
					command.src_rect = (WzRect){ 0, 0, command.dest_rect.w, command.dest_rect.h };
				}
			}

			// Rect item
			if (item.type == ItemType_Rect) {
				command = (WzDrawCommand){
					.type = DrawCommandType_Rect,
						.dest_rect = (WzRect){
							widget.actual_w + item.val.rect.x,
							widget.actual_y + item.val.rect.y,
							item.val.rect.w,
							item.val.rect.h
					},
						.color = item.color,
					.z = widget.layer
				};

				WZ_ASSERT(command.dest_rect.w > 0);
				WZ_ASSERT(command.dest_rect.h > 0);
			}

			// Line items
			if (item.type == ItemType_Line) {
				command = (WzDrawCommand){
					.type = DrawCommandType_Line,
						.dest_rect = (WzRect){
							widget.actual_w + item.val.rect.x,
							widget.actual_y + item.val.rect.y,
							widget.actual_w + item.val.rect.w,
							widget.actual_y + item.val.rect.h
					},
						.color = item.color,
					.z = widget.layer

				};
			}
			else if (item.type == ItemType_HorizontalDottedLine) {
				command = (WzDrawCommand){
					.type = DrawCommandType_HorizontalLine,
						.dest_rect = (WzRect){
							widget.actual_w,
							widget.actual_y + widget.actual_h / 2,
							widget.actual_w + widget.actual_w,
							widget.actual_y + widget.actual_h / 2
					},
					.z = widget.layer

				};
			}
			else if (item.type == ItemType_LeftHorizontalDottedLine) {
				command = (WzDrawCommand){
					.type = DrawCommandType_HorizontalLine,
						.dest_rect = (WzRect){
							widget.actual_w,
							widget.actual_y + widget.actual_h / 2,
							widget.actual_w + widget.actual_w / 2,
							widget.actual_y + widget.actual_h / 2
					},
					.z = widget.layer

				};
			}
			else if (item.type == ItemType_RightHorizontalDottedLine) {
				command = (WzDrawCommand){
					.type = DrawCommandType_HorizontalLine,
						.dest_rect = (WzRect){
							widget.actual_w + widget.actual_w / 2,
							widget.actual_y + widget.actual_h / 2,
							widget.actual_w + widget.actual_w,
							widget.actual_y + widget.actual_h / 2
					},
					.z = widget.layer

				};
			}
			else if (item.type == ItemType_VerticalDottedLine) {
				command = (WzDrawCommand){
					.type = DrawCommandType_VerticalLine,
						.dest_rect = (WzRect){
							widget.actual_w + widget.actual_w / 2,
							widget.actual_y,
							widget.actual_w + widget.actual_w / 2 + widget.actual_w,
							widget.actual_y + widget.actual_h
					},
					.z = widget.layer

				};
			}
			else if (item.type == ItemType_TopVerticalDottedLine) {
				command = (WzDrawCommand){
					.type = DrawCommandType_VerticalLine,
						.dest_rect = (WzRect){
							widget.actual_w + widget.actual_w / 2,
							widget.actual_y,
							widget.actual_w + widget.actual_w / 2 + widget.actual_w,
							widget.actual_y + widget.actual_h / 2
					},
					.z = widget.layer
				};
			}
			else if (item.type == ItemType_BottomVerticalDottedLine) {
				command = (WzDrawCommand){
					.type = DrawCommandType_VerticalLine,
						.dest_rect = (WzRect){
							widget.actual_w + widget.actual_w / 2,
							widget.actual_y + widget.actual_h / 2,
							widget.actual_w + widget.actual_w / 2,
							widget.actual_y + widget.actual_h
					},
					.z = widget.layer

				};
			}

			command.dest_rect.x += item.pad_left;
			command.dest_rect.y += item.pad_top;

			WZ_ASSERT(command.dest_rect.w >= 0);
			WZ_ASSERT(command.dest_rect.h >= 0);

			WZ_ASSERT(buffer->count < MAX_NUM_DRAW_COMMANDS - 1);
			buffer->commands[buffer->count++] = command;

			buffer->commands[buffer->count - 1].widget_line_number = widget.line;
			buffer->commands[buffer->count - 1].file = widget.file;
		}
#endif
	}

	canvas->clean = true;

}

void wzrd_widget_clip(WzWidget handle)
{
	WzWidgetData* box = wz_widget_get(handle);
	box->clip_widget = box->handle;
}

WzWidgetData wzrd_widget_get_cached_box_with_secondary_tag(const char* tag, const char* secondary_tag)
{
	for (int i = 0; i < canvas->cached_boxes_count; ++i)
	{
		if (canvas->cached_boxes[i].tag == tag && canvas->cached_boxes[i].secondary_tag == secondary_tag)
		{
			return canvas->cached_boxes[i];
		}
	}

	return (WzWidgetData) { 0 };
}

WzWidgetData wzrd_widget_get_cached_box(const char* tag)
{
	for (int i = 0; i < canvas->cached_boxes_count; ++i)
	{
		if (canvas->cached_boxes[i].tag == tag && !canvas->cached_boxes[i].secondary_tag)
		{
			return canvas->cached_boxes[i];
		}
	}

	return (WzWidgetData) { 0 };
}

void wzrd_widget_tag(WzWidget parent, const char* str)
{
	wz_widget_get(parent)->tag = str;
}

WzWidgetData wz_widget_get_parent(WzWidget parent)
{
	//return wz_widget_get(wz_widget_get(widget)->parent);
}

void wz_widget_resize(WzWidget handle, int* w_offset, int* h_offset)
{
	WzWidgetData* w = wz_widget_get(handle);
	WzWidgetData* parent = wz_widget_get(w->parent);

	if (parent->layout == WzLayoutHorizontal)
	{
		if (wz_widget_is_equal(w->handle, canvas->right_resized_item)) {
			*w_offset += canvas->mouse_delta.x;
		}
	}
	else {
		if (wz_widget_is_equal(w->handle, canvas->bottom_resized_item)) {
			*h_offset += canvas->mouse_delta.y;
		}
	}

	w->w_offset = *w_offset;
	w->h_offset = *h_offset;
}

void wzrd_end(wzrd_str* debug_str)
{

	wzrd_do_layout(1);

	for (int i = 0; i < canvas->widgets_count; ++i)
	{
		unsigned int x = i % 4;
		if (x == 0)
		{
			canvas->widgets[i].color.r = 255;
			canvas->widgets[i].color.g = 0;
			canvas->widgets[i].color.b = 0;
			canvas->widgets[i].color.a = 255;
		}
		else if (x == 1)
		{
			canvas->widgets[i].color.r = 0;
			canvas->widgets[i].color.g = 255;
			canvas->widgets[i].color.b = 0;
			canvas->widgets[i].color.a = 255;
		}
		if (x == 2)
		{
			canvas->widgets[i].color.r = 0;
			canvas->widgets[i].color.g = 0;
			canvas->widgets[i].color.b = 255;
			canvas->widgets[i].color.a = 255;
		}
		if (x == 2)
		{
			canvas->widgets[i].color.r = 0;
			canvas->widgets[i].color.g = 255;
			canvas->widgets[i].color.b = 255;
			canvas->widgets[i].color.a = 255;
		}
	}

	// Cache tagged elements
	canvas->cached_boxes_count = 0;
	for (int i = 1; i < canvas->widgets_count; ++i)
	{
		assert(canvas->cached_boxes_count < MAX_NUM_CACHED_BOXES - 1);
		WzWidgetData* box = &canvas->widgets[i];
		if (box->tag)
		{
			canvas->cached_boxes[canvas->cached_boxes_count++] = *box;
		}
	}

	// Sort
	int boxes_indices[MAX_NUM_BOXES] = { 0 };

	for (int i = 0; i < canvas->widgets_count; ++i)
	{
		boxes_indices[i] = i;
	}

	qsort(boxes_indices, canvas->widgets_count, sizeof(int), wzrd_compare_boxes);

	// Mouse interaction
	if (canvas->enable_input)
	{
		wzrd_handle_input(boxes_indices, canvas->widgets_count);
		wzrd_handle_cursor();
		wzrd_handle_border_resize();
	}
	else {
		if (canvas->mouse_left == WZRD_DEACTIVATING)
		{
			//canvas->released_item = canvas->dragged_item;
			//canvas->released_item = canvas->dragged_item;
			canvas->dragged_box = (WzWidgetData){ 0 };
			canvas->dragged_item = (WzWidget){ 0 };
			canvas->active_item = (WzWidget){ 0 };
		}
	}

	canvas->previous_mouse_pos = canvas->mouse_pos;

	wzrd_draw(boxes_indices);

	canvas = 0;
}

void wzrd_box_set_type(WzWidget handle, wzrd_box_type type)
{
	wz_widget_get(handle)->layout = type;
}

WzWidget egui_button_raw_begin(bool* released, WzWidget parent, const char* file, unsigned int line) {

	WzWidget handle = wz_widget_raw(parent, file, line);
	wzrd_box_set_type(handle, wzrd_box_type_button);

	if (wz_widget_is_equal(handle, canvas->deactivating_item)) {
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

WzWidget wzrd_toggle_icon_raw(wzrd_texture texture, bool* active, WzWidget parent, const char* file_name, unsigned int line) {
	bool b = false;
	WzWidget handle = egui_button_raw_begin(&b, parent, file_name, line);
	wz_widget_add_source(handle, file_name, line);

	return handle;
}

WzWidget wzrd_button_icon_raw(wzrd_texture texture, bool* result, WzWidget handle, const char* file, unsigned int line) {
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

	WzWidget parent = wz_widget_raw(handle, file, line);

	return parent;
#endif
}

WzWidget wzrd_command_toggle_raw(wzrd_str str, bool* active, WzWidget handle, const char* file_name, unsigned int line) {
	bool b1 = false, a1 = false;

	WzWidget parent = egui_button_raw_begin(&b1, handle, file_name, line);
	wz_widget_add_source(parent, file_name, line);
	wzrd_text_add(str, parent);
	wz_widget_set_constraint_w(parent, 100);
	wz_widget_set_constraint_w(parent, 100);

	if (b1)
	{
		*active = !*active;
	}

	a1 = wzrd_box_is_active(&canvas->widgets[canvas->widgets_count - 1]);

	if (*active || a1)
	{
		wz_widget_get(parent)->border_type = BorderType_Clicked;
	}

	return parent;
}

WzWidget egui_button_raw_begin_on_half_click(bool* b, WzWidget parent, const char* file, unsigned int line) {
	WzWidget h = wz_widget_raw(parent, file, line);

	//if (wzrd_handle_is_valid(h))
	if (wz_widget_is_equal(h, canvas->activating_item)) {
		*b = true;
	}
	else
	{
		*b = false;
	}

	return h;
}

WzWidget wzrd_label_button_activating(wzrd_str str, bool* active, WzWidget parent, const char* file, unsigned int line)
{
	WzWidget h = egui_button_raw_begin_on_half_click(active, parent, file, line);
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
		//canvas->widgets[canvas->widgets_count - 1].color = EGUI_DARKBLUE;
		//canvas->widgets[canvas->widgets_count - 2].color = EGUI_DARKBLUE;
	}

	return result;
}
#endif

void wzrd_item_add(Item item, WzWidget box) {
	WzWidgetData* b = wz_widget_get(box);
	WZ_ASSERT(b->items_count < MAX_NUM_ITEMS - 1);
	b->items[b->items_count++] = item;
}

WzWidget wzrd_label_raw(wzrd_str str, WzWidget handle, const char* file, unsigned int line) {
	int w = 0, h = 0;
	canvas->get_string_size(str.str, &w, &h);

	WzWidget parent = wz_widget_raw(handle, file, line);

	wz_widget_set_tight_constraints(parent, w, h);

	wzrd_text_add(str, parent);

	return parent;
}

WzWidget wzrd_input_box_raw(char* str, int* len, int max_num_keys, WzWidget handle, const char* file, unsigned int line) {
	WzWidget parent = wz_widget_raw(handle, file, line);

	WzWidgetData* box = wz_widget_get(parent);
	wz_widget_set_constraint_w(box->handle, 50);
	wz_widget_set_constraint_h(box->handle, 30);
	box->color = WZ_GREEN;

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
	wzrd_text_add((wzrd_str) { .str = str, .len = *len }, parent);
#endif

	return parent;
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

WzWidget wzrd_label_button_raw(wzrd_str str, bool* result, WzWidget handle, const char* file, unsigned  int line) {
	int w = 0, h = 0;
	canvas->get_string_size(str.str, &w, &h);

	WzWidget parent = wz_widget_raw(handle, file, line);

	wz_widget_set_constraint_w(parent, w);
	wz_widget_set_constraint_h(parent, h);

	wzrd_box_set_type(parent, wzrd_box_type_button);

	wzrd_text_add(str, parent);

	if (wzrd_box_is_activating(wzrd_box_get_last()))
	{
		*result = true;
	}

	return parent;
}

WzWidget wzrd_dialog_begin_raw(wzrd_v2* pos, wzrd_v2 size,
	bool* active, wzrd_str name, int layer, WzWidget parent, const char* file, unsigned int line) {
	WZRD_UNUSED(name);

	if (!*active) return (WzWidget) { 0 };

	WzWidget window = wzrd_widget_free(
		/*	(wzrd_style) {
			.space = (wzrd_space){ .x = pos->x, .y = pos->y, .w = size.x, .h = size.y },
				.skin = canvas->panel_border_skin,
		},*/
		parent);

	wz_widget_get(window)->layer = layer;

	bool close = false;

	WzWidget top_panel = wz_widget_raw(window, file, line);

	wz_widget_set_constraint_h(top_panel, 28);

	//wz_widget_get(top_panel)->h = 28;
	//wzrd_box_get(top_panel)->canvas = v_panel_layout;

	WzWidget bar = wz_widget_raw(top_panel, file, line);

	wz_widget_set_color(bar, (WzColor) { 57, 77, 205, 255 });

	if (wz_widget_is_equal(bar, canvas->active_item)) {
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

WzWidget wzrd_dropdown_raw(int* selected_text, const wzrd_str* texts, int texts_count, int w, bool* active, WzWidget handle, const char* file, unsigned int line)
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

	WzWidget parent = wz_widget_raw(handle, file, line);

	return parent;
}


void wz_set_alignment(WzWidget parent, WzAlignment alignment)
{
	wz_widget_get(parent)->alignment |= alignment;
}

void wzrd_label_list_raw(wzrd_str* item_names, unsigned int count,
	wzrd_v2 size, WzWidget* handles, unsigned int* selected, bool* is_selected, WzWidget parent, const char* file, unsigned int line)
{
	WzWidget panel = wz_vbox(parent, file, line);

	//wz_widget_set_tight_constraints(panel, size.x, size.y);

	if (wzrd_box_is_activating(wz_widget_get(panel))) {
		*selected = 0;
		*is_selected = false;
	}

	WzWidget selected_label = { 0 };

	for (unsigned int i = 0; i < count; ++i)
	{
		char str[32];
		sprintf_s(str, 32, "%d label list %d", wzrd_box_get_last()->handle.handle, i);

		bool is_label_clicked = false;
		WzWidget h = wzrd_label_button_activating(item_names[i], &is_label_clicked, panel, file, line);

		wz_widget_set_constraint_h(h, 32);
		wz_set_alignment(h, WzAlignVCenter | WzAlignHCenter);


		if (handles)
		{
			handles[i] = canvas->widgets[canvas->widgets_count - 2].handle;
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
	wzrd_v2 size, unsigned int* selected, bool* is_selected, WzWidget parent, const char* file_name, unsigned int line) {

	WzWidget handles[MAX_NUM_LABELS] = { 0 };

	wzrd_label_list_raw(item_names, count, size, handles, selected, is_selected, parent, file_name, line);

	// Ordering
	{
		assert(items);
		WzWidget active_label = { 0 }, hovered_label = { 0 }, released_label = { 0 };
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
		WzWidgetData* hovered_parent = 0;
		for (int i = 0; i < canvas->hovered_boxes_count; ++i)
		{
			if (wz_widget_is_equal(canvas->hovered_boxes[i].handle, hovered_label))
			{
				hovered_parent = canvas->hovered_boxes + i;
				break;
			}
		}

		// Set hover position
		if (hovered_parent)
		{

			if (canvas->mouse_pos.y > hovered_parent->y + hovered_parent->actual_h / 2)
			{
				is_bottom = true;
			}

			// Label grabbed and hovering over another one
			if (wz_handle_is_valid(active_label) && wz_handle_is_valid(hovered_label) && !wz_widget_is_equal(hovered_label, active_label))
			{

				WzWidgetData* p = wz_widget_get(hovered_label);
				WzWidgetData* c = 0;
				if (is_bottom)
				{
					c = wz_widget_create();
					wz_widget_add_source(c->handle, __FILE__, __LINE__);
					wz_widget_set_color(c->handle, EGUI_PURPLE);
					wz_widget_set_y(c->handle, hovered_parent->actual_h - 2);
					wz_widget_set_constraint_h(c->handle, 2);

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
		if (wz_handle_is_valid(released_label) && wz_handle_is_valid(hovered_label) && !wz_widget_is_equal(hovered_label, released_label))
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

WzWidget wzrd_handle_button_raw(bool* active, WzRect rect,
	WzColor color, wzrd_str name, WzWidget handle, const char* file_name, unsigned int line) {

	WzWidget parent = wzrd_widget_free(handle);
	wz_widget_add_source(parent, file_name, line);
	wz_widget_set_pos(parent, rect.x, rect.y);
	wz_widget_set_tight_constraints(parent, rect.w, rect.h);
	wzrd_box_set_type(parent, wzrd_box_type_button);
	*active = wzrd_box_is_active(wz_widget_get(parent));

	return parent;
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

bool wzrd_box_is_active(WzWidgetData* box) {
	if (wz_widget_is_equal(box->handle, canvas->active_item)) {
		return true;
	}

	return false;
}

bool wzrd_box_is_dragged(WzWidgetData* box) {
	if (wz_widget_is_equal(box->handle, canvas->dragged_item)) {
		return true;
	}

	return false;
}

bool wzrd_box_is_hot_using_canvas(wzrd_canvas* c, WzWidgetData* box) {
	if (wz_widget_is_equal(box->handle, c->hovered_item)) {
		return true;
	}

	return false;
}

bool wzrd_box_is_hot(WzWidgetData* box) {
	if (wz_widget_is_equal(box->handle, canvas->hovered_item)) {
		return true;
	}

	return false;
}


WzWidgetData* wzrd_box_get_released()
{
	WzWidgetData* result = 0;

	if (wz_handle_is_valid(canvas->deactivating_item))
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

void wzrd_widget_set_rect(WzWidget parent, WzRect rect)
{
#if 0
	WzWidgetData* w = wz_widget_get(widget);
	w->x = rect.x;
	w->y = rect.y;
	w->w = rect.w;
	w->h = rect.h;
#endif
}

WzWidget wzrd_command_button_raw(wzrd_str str, bool* released, WzWidget parent, const char* file_name, unsigned int line)
{
	WzWidget button = egui_button_raw_begin(released, parent, file_name, line);
	wz_widget_add_source(button, file_name, line);

	wzrd_text_add(str, button);

	if (wzrd_handle_is_interacting(button))
	{
		//wzrd_skin_set(button, canvas->command_button_on_skin);
	}

	return button;
}

void wzrd_handle_set_layer(WzWidget handle, unsigned int layer)
{
	wz_widget_get(handle)->layer = layer;
}

#if 0		
			else
			{
				// Constraints on the main axis are unbounded
				// Widget must must not demand the entire space
				WZ_ASSERT(widget->main_axis_size_type == MAIN_AXIS_SIZE_MIN);

				for (int i = 0; i < widget->children_count; ++i)
				{
					child = &canvas->widgets[widget->children[i]];

					if (widget->layout == WzLayoutHorizontal)
					{
						child_constraint_min_main_axis = &child->constraint_min_w;
						child_constraint_max_main_axis = &child->constraint_max_w;
						child_constraint_min_cross_axis = &child->constraint_min_h;
						child_constraint_max_cross_axis = &child->constraint_max_h;
					}
					else if (widget->layout == WzLayoutVertical)
					{
						child_constraint_min_main_axis = &child->constraint_min_h;
						child_constraint_max_main_axis = &child->constraint_max_h;
						child_constraint_min_cross_axis = &child->constraint_min_w;
						child_constraint_max_cross_axis = &child->constraint_max_w;
					}
					else
					{
						child_constraint_min_main_axis = child_constraint_max_main_axis =
							child_constraint_max_cross_axis = child_constraint_min_cross_axis = 0;
						WZ_ASSERT(0);
					}

					WZ_ASSERT(child->flex_fit == FlexFitLoose);

					*child_constraint_min_main_axis = 0;
					*child_constraint_min_cross_axis = 0;
					*child_constraint_max_main_axis = UINT_MAX;
					*child_constraint_max_cross_axis = *constraint_max_cross_axis;

					widgets_stack[widgets_stack_count] = child->index;
					widgets_stack_count++;
				}

				}

		}
#endif