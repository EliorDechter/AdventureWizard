#define wz_assert(x) assert(x)
//#define WZ_ASSERT(x) (void)(x)
#define WZRD_UNUSED(x) (void)x

#include "WzGuiCore.h"
#include "Strings.h"
#include "WzLayout.h"

static wzrd_canvas* canvas;

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

void wz_widget_add_source(WzWidget handle, const char* file, unsigned int line)
{
	// We want only the file name
	if (!file) return;
	wz_assert(file);
	const char* f;
	for (f = file + strlen(file); *(f - 1) != '\\'; --f) {}

	wz_widget_get(handle)->file = f;
	wz_widget_get(handle)->line = line;
}

void wz_widget_set_pad_left(WzWidget widget, unsigned int pad)
{
	wz_assert(pad);
	wz_assert(pad <= canvas->window.w);

	wz_widget_get(widget)->pad_left = pad;
}

void wz_widget_set_pad_right(WzWidget widget, unsigned int pad)
{
	wz_assert(pad);
	wz_assert(pad <= canvas->window.w);

	wz_widget_get(widget)->pad_right = pad;
}

void wz_widget_set_pad_top(WzWidget widget, unsigned int pad)
{
	WzWidgetData* d = wz_widget_get(widget);
	wz_assert(pad);

	wz_widget_get(widget)->pad_top = pad;
}

void wz_widget_set_pad_bottom(WzWidget widget, unsigned int pad)
{
	wz_assert(pad);
	wz_assert(pad <= canvas->window.w);

	wz_widget_get(widget)->pad_bottom = pad;
}

void wz_widget_set_child_gap(WzWidget widget, unsigned int child_gap)
{
	wz_assert(child_gap);
	wz_widget_get(widget)->child_gap = child_gap;
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

void wz_widget_set_tight_constraints(WzWidget handle, unsigned int w, unsigned int h)
{
	WzWidgetData* b = wz_widget_get(handle);
	b->constraint_min_w = b->constraint_max_w = w;
	b->constraint_min_h = b->constraint_max_h = h;
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

void wz_widget_set_max_constraint_h(WzWidget w, int height)
{
	wz_assert(height);
	WzWidgetData* b = wz_widget_get(w);
	b->constraint_max_h = height;
}

void wz_widget_set_max_constraint_w(WzWidget w, int width)
{
	wz_assert(width);
	WzWidgetData* b = wz_widget_get(w);
	b->constraint_max_w = width;
}

void wz_widget_set_min_constraint_w(WzWidget w, int width)
{
	wz_assert(width);
	WzWidgetData* b = wz_widget_get(w);
	b->constraint_min_w = width;
}

void wz_widget_set_x(WzWidget w, unsigned int x)
{
	WzWidgetData* widget = wz_widget_get(w);
	widget->x = x;
}

void wz_widget_set_y(WzWidget w, unsigned int y)
{
	WzWidgetData* widget = wz_widget_get(w);
	widget->y = y;
}

void wz_widget_set_pad(WzWidget w, unsigned int pad)
{
	wz_widget_set_pad_top(w, pad);
	wz_widget_set_pad_bottom(w, pad);
	wz_widget_set_pad_left(w, pad);
	wz_widget_set_pad_right(w, pad);
}

void wz_widget_set_border(WzWidget w, WzBorderType border_type)
{
	WzWidgetData* d = wz_widget_get(w);
	d->border_type = border_type;

	d->pad_top += 2;
	d->pad_bottom += 2;
	d->pad_left += 2;
	d->pad_right += 2;
}

void wz_widget_set_pos(WzWidget handle, int x, int y)
{
	wz_widget_set_x(handle, x);
	wz_widget_set_y(handle, y);
}


void wz_widget_set_color_old(WzWidget widget, WzColor color)
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
	wz_assert(canvas->current_crate_index > 0);
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
	wz_assert(canvas->current_crate_index >= 0);
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

void wz_widget_add_offset(WzWidget handle, int x, int y)
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

		box->actual_x += x;
		box->actual_y += y;

		ptr++;
	}
}

bool wz_widget_is_deactivating(WzWidget handle)
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
	wz_assert(p->free_children_count < MAX_NUM_CHILDREN - 1);
	p->free_children[p->free_children_count++] = c->index;

	c->layer = p->layer;
	c->clip_widget = p->clip_widget;
}

void wzrd_box_add_child(WzWidget parent, WzWidget child)
{
	WzWidgetData* p = wz_widget_get(parent);
	WzWidgetData* c = wz_widget_get(child);
	wz_assert(p->children_count < MAX_NUM_CHILDREN - 1);
	p->children[p->children_count++] = c->index;

	c->layer = p->layer;
	c->clip_widget = p->clip_widget;
	c->parent = parent;
}

void wzrd_box_add_child_using_pointer(WzWidgetData* parent, WzWidgetData* child)
{
	wz_assert(parent->children_count < MAX_NUM_CHILDREN - 1);
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
	widg->flex_fit = WZ_FLEX_FIT_TIGHT;
}

void wz_widget_set_flex(WzWidget widget)
{
	WzWidgetData* widg = wz_widget_get(widget);
	widg->flex_factor = 1;
	widg->flex_fit = WZ_FLEX_FIT_LOOSE;
}

WzWidgetData* wz_widget_create()
{
	WzWidgetData box;
	box.children_count = 0;
	box.free_children_count = 0;
	box.items_count = 0;
	box.color = WZ_WHITE;
	box.font_color = EGUI_BLACK;
	box.percentage_h = 0;
	box.percentage_w = 0;
	box.cross_axis_alignment = 0;
	box.best_fit = false;
	box.bring_to_front = false;
	box.child_gap = 0;
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
	box.layout = WZ_LAYOUT_NONE;
	box.flex_factor = 0;
	box.constraint_min_w = box.constraint_min_h = 0;
	box.constraint_max_w = box.constraint_max_h = UINT_MAX;
	box.x = box.y = 0;
	box.free_from_parent_horizontally = box.free_from_parent_vertically = 0;

	box.handle = wzrd_handle_create();

	wz_assert(canvas->widgets_count < MAX_NUM_BOXES - 1);

	box.layer = EguiGetCurrentWindow()->layer;

	wz_assert(canvas->widgets_count < 256);

	box.index = canvas->widgets_count;
	canvas->widgets[canvas->widgets_count++] = box;

	return &canvas->widgets[canvas->widgets_count - 1];
}

void wz_widget_set_free_from_parent_horizontally(WzWidget w)
{
	wz_widget_get(w)->free_from_parent_horizontally = true;
}

void wz_widget_set_free_from_parent_vertically(WzWidget w)
{
	wz_widget_get(w)->free_from_parent_horizontally = true;
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
	wz_widget_set_layout(p, WZ_LAYOUT_VERTICAL);

	return p;
}

WzWidget wz_hbox_raw(WzWidget parent, const char* file, unsigned int line)
{
	WzWidget p = wz_widget_raw(parent, file, line);
	wz_widget_set_layout(p, WZ_LAYOUT_HORIZONTAL);

	return p;
}

void wz_widget_add_rect(WzWidget widget, unsigned int w, unsigned int h, WzColor color)
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

#if 0

void wz_log_error(WzLogMessage* arr, unsigned int* count, const char* fmt, ...)
{
	WzLogMessage message;
	message.str[0] = 0;
	va_list args;
	va_start(args, fmt);
	//vsprintf_s(message.str, WZ_LOG_MESSAGE_MAX_SIZE, fmt, args);
	va_end(args);

	arr[*count] = message;
	*count = *count + 1;
}

#endif

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

#define MAX_NUM_SCROLLBARS 32

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

	canvas->scrollbars = malloc(sizeof(*canvas->scrollbars) * MAX_NUM_SCROLLBARS);

	WZRD_UNUSED(gui);

	canvas->widgets_count = 0;
	canvas->input_box_timer += 16.7f;

	canvas->clip_boxes_count = 1;

	// Empty box
	canvas->widgets[canvas->widgets_count++] = (WzWidgetData){ 0 };

	// Window
	WzWidget window_widget = wz_widget((WzWidget) { 0 });
	wz_widget_set_constraints(window_widget, window.w, window.h, window.w, window.h);

	// Stylesheet
	canvas->stylesheet.label_color = EGUI_LIGHTGRAY;
	canvas->stylesheet.label_item_selected_color = EGUI_LIGHTGRAY;

	canvas->scrollbars_count = 0;

	return window_widget;
}

void EguiRectDraw(WzDrawCommandBuffer* buffer, WzRect rect, WzColor color,
	int z, unsigned int widget_line_number, const char* file)
{
	wz_assert(rect.w > 0);
	wz_assert(rect.h > 0);
	wz_assert(color.a);

	WzDrawCommand command = (WzDrawCommand){
		.type = DrawCommandType_Rect,
		.dest_rect = rect,
		.color = color,
		.z = z
	};

	wz_layout_log("Rect command: (id: %s %u) (rect: %u %u %u %u)  (color: %u %u %u %u)\n",
		file, widget_line_number,
		rect.x, rect.y, rect.w, rect.h,
		color.r, color.g, color.b, color.a);

	command.widget_line_number = widget_line_number;
	command.file = file;

	wz_assert(buffer->count < MAX_NUM_DRAW_COMMANDS - 1);
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
				canvas->mouse_pos.x >= child->actual_x &&
				canvas->mouse_pos.y >= child->actual_y &&
				canvas->mouse_pos.x < child->actual_x + border_size &&
				canvas->mouse_pos.y < child->actual_y + child->actual_h;
			bool is_inside_right_border =
				canvas->mouse_pos.x >= child->actual_x + child->actual_w - border_size &&
				canvas->mouse_pos.y >= child->actual_y &&
				canvas->mouse_pos.x < child->actual_x + child->actual_w &&
				canvas->mouse_pos.y < child->actual_y + child->actual_h;
			bool is_inside_top_border =
				canvas->mouse_pos.x >= child->actual_x &&
				canvas->mouse_pos.y >= child->actual_y &&
				canvas->mouse_pos.x < child->actual_x + child->actual_w &&
				canvas->mouse_pos.y < child->actual_y + border_size;
			bool is_inside_bottom_border =
				canvas->mouse_pos.x >= child->actual_x &&
				canvas->mouse_pos.y >= child->actual_y + child->actual_h - border_size &&
				canvas->mouse_pos.x < child->actual_x + child->actual_w &&
				canvas->mouse_pos.y < child->actual_y + child->actual_h;


			if (wz_widget_is_equal(canvas->hovered_item, child->handle) || wz_widget_is_equal(canvas->active_item, child->handle)) {
				if (is_inside_top_border || is_inside_bottom_border) {
					//child->color = WZ_BLUE;
					//*cursor = wzrd_cursor_vertical_arrow;
				}
				else if (is_inside_left_border || is_inside_right_border) {
					//child->color = WZ_BLUE;
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

		WzRect scaled_rect = { box->actual_x, box->actual_y, box->actual_w, box->actual_h };

#if 0
		// Input Clipping: Block mouse from reaching clipped widgets
		if (wz_handle_is_valid(box->clip_widget))
		{
			WzWidgetData* clip_widget = wz_widget_get(box->clip_widget);

			if (box->actual_x < clip_widget->actual_w)
			{
				scaled_rect.x = clip_widget->actual_w;
			}

			if (box->actual_y < clip_widget->actual_y)
			{
				scaled_rect.y = clip_widget->actual_y;
			}

			if (box->actual_x + box->actual_w > clip_widget->actual_w + clip_widget->actual_w)
			{
				scaled_rect.x = clip_widget->actual_w;
			}

			if (box->actual_y + box->actual_h > clip_widget->actual_y + clip_widget->actual_h)
			{
				scaled_rect.y = clip_widget->actual_h;
			}
		}
#endif

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

	hot_box->color = EGUI_DARKGREEN;


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
				wz_widget_get(canvas->deactivating_item)->color = WZ_GREEN;
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
			wz_assert(half_clicked_box);
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
		wz_assert(clicked_box);
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

bool wz_widget_is_active(WzWidget handle) {
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

void wz_widget_set_main_axis_size_min(WzWidget w)
{
	WzWidgetData* d = wz_widget_get(w);
	d->main_axis_size_type = MAIN_AXIS_SIZE_TYPE_MIN;
}

void wz_widget_set_layout(WzWidget handle, unsigned int layout)
{
	WzWidgetData* d = wz_widget_get(handle);
	d->layout = layout;
	d->flex_fit = WZ_FLEX_FIT_TIGHT;
	d->main_axis_size_type = MAIN_AXIS_SIZE_TYPE_MAX;
}

void wz_widget_set_constraint_size(WzWidget widget, unsigned int w, unsigned int h)
{
	WzWidgetData* d = wz_widget_get(widget);
	d->constraint_max_w = w;
	d->constraint_max_h = h;
}

void wz_widget_set_constraints(WzWidget widget,
	unsigned int min_w, unsigned int min_h, unsigned int max_w, unsigned int max_h)
{
	WzWidgetData* data = wz_widget_get(widget);
	data->constraint_min_w = min_w;
	data->constraint_min_h = min_h;
	data->constraint_max_w = max_w;
	data->constraint_max_h = max_h;
}

void wz_draw(int* boxes_indices)
{
	WzWidgetData widget;
	WzDrawCommand command;
	WzDrawCommandBuffer* buffer = &canvas->command_buffer;
	buffer->count = 0;
	unsigned int line_size;
	WzWidget current_clip_widget;
	Item item;
	WzRect item_dest_rect;

	current_clip_widget.handle = 0;

	for (int i = 1; i < canvas->widgets_count; ++i)
	{
		widget = canvas->widgets[boxes_indices[i]];

#if 1

		if (widget.actual_w <= 2 || widget.actual_h <= 2)
		{
			continue;
		}

		// Draw Widget
		EguiRectDraw(buffer, (WzRect) {
			.x = widget.actual_x,
				.y = widget.actual_y,
				.w = widget.actual_w,
				.h = widget.actual_h,
		},
			widget.color,
			widget.layer, widget.line, widget.file);

		// Borders (1215 x 810)
		if (!(widget.actual_w <= 2 || widget.actual_h <= 2))
		{
			if (widget.border_type != BorderType_None)
			{
				//WZ_ASSERT(widget.actual_w >= 4);
				//WZ_ASSERT(widget.actual_h >= 4);
			}

			line_size = WZ_BORDER_SIZE;

			WzRect top0 = (WzRect){ widget.actual_x, widget.actual_y, widget.actual_w - line_size, line_size };
			WzRect left0 = (WzRect){ widget.actual_x, widget.actual_y, line_size, widget.actual_h };

			WzRect top1 = (WzRect){ widget.actual_x + line_size, widget.actual_y + line_size, widget.actual_w - 3 * line_size, line_size };
			WzRect left1 = (WzRect){ widget.actual_x + line_size, widget.actual_y + line_size, line_size, widget.actual_h - line_size };

			WzRect bottom0 = (WzRect){ widget.actual_x, widget.actual_y + widget.actual_h - line_size, widget.actual_w, line_size };
			WzRect right0 = (WzRect){ widget.actual_x + widget.actual_w - line_size, widget.actual_y, line_size, widget.actual_h };

			WzRect bottom1 = (WzRect){ widget.actual_x + 1 * line_size, widget.actual_y + widget.actual_h - 2 * line_size, widget.actual_w - 3 * line_size, line_size };
			WzRect right1 = (WzRect){ widget.actual_x + widget.actual_w - 2 * line_size, widget.actual_y + 1 * line_size, line_size, widget.actual_h - 2 * line_size };

			wzrd_is_rect_inside_rect(top0, (WzRect) { widget.actual_x, widget.actual_y, widget.actual_w, widget.actual_h });
			wzrd_is_rect_inside_rect(left0, (WzRect) { widget.actual_x, widget.actual_y, widget.actual_w, widget.actual_h });
			wzrd_is_rect_inside_rect(top1, (WzRect) { widget.actual_x, widget.actual_y, widget.actual_w, widget.actual_h });
			wzrd_is_rect_inside_rect(left1, (WzRect) { widget.actual_x, widget.actual_y, widget.actual_w, widget.actual_h });
			wzrd_is_rect_inside_rect(bottom0, (WzRect) { widget.actual_x, widget.actual_y, widget.actual_w, widget.actual_h });
			wzrd_is_rect_inside_rect(right0, (WzRect) { widget.actual_x, widget.actual_y, widget.actual_w, widget.actual_h });
			wzrd_is_rect_inside_rect(bottom1, (WzRect) { widget.actual_x, widget.actual_y, widget.actual_w, widget.actual_h });
			wzrd_is_rect_inside_rect(right1, (WzRect) { widget.actual_x, widget.actual_y, widget.actual_w, widget.actual_h });

			if (widget.border_type == WZ_BORDER_TYPE_DEFAULT) {
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
			else if (widget.border_type == WZ_BORDER_TYPE_CLICKED) {
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

		for (int j = 0; j < widget.items_count; ++j)
		{
			item = widget.items[j];

			if (item.size.x == 0)
				item.size.x = widget.actual_w;
			if (item.size.y == 0)
				item.size.y = widget.actual_h;

			item_dest_rect = (WzRect){
				widget.actual_x,
				widget.actual_y,
					widget.actual_w,
					widget.actual_h
			};

			// String item
			if (item.type == wzrd_item_type_str)
			{
				int w, h;
				canvas->get_string_size(item.val.str.str, &w, &h);

				item_dest_rect.x += item_dest_rect.w / 2 - w / 2;
				item_dest_rect.y += item_dest_rect.h / 2 - h / 2;

				command = (WzDrawCommand){
					.type = DrawCommandType_String,
						.str = item.val.str,
						.dest_rect = item_dest_rect,
						.color = item.color,
						.z = widget.layer
				};
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

				wz_assert(0); // Log file and line num

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

				wz_assert(command.dest_rect.w > 0);
				wz_assert(command.dest_rect.h > 0);
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

			wz_assert(command.dest_rect.w >= 0);
			wz_assert(command.dest_rect.h >= 0);

			wz_assert(buffer->count < MAX_NUM_DRAW_COMMANDS - 1);
			buffer->commands[buffer->count++] = command;

			buffer->commands[buffer->count - 1].widget_line_number = widget.line;
			buffer->commands[buffer->count - 1].file = widget.file;
		}

		// Draw clip area
		{
			if (wz_handle_is_valid(widget.clip_widget))
			{
				if (!wz_widget_is_equal(current_clip_widget, widget.clip_widget))
				{
					WzWidgetData* clip_box = wz_widget_get(widget.clip_widget);
					wz_assert(buffer->count < MAX_NUM_DRAW_COMMANDS - 1);

					WzRect clip_rect;
					clip_rect.x = clip_box->actual_x;
					clip_rect.y = clip_box->actual_y;
					clip_rect.w = clip_box->actual_w;
					clip_rect.h = clip_box->actual_h;

					if (!clip_rect.w ||
						!clip_rect.h ||
						!(clip_rect.w > (widget.pad_left + widget.pad_right)) ||
						!(clip_rect.h > (widget.pad_top + widget.pad_bottom)))
					{
						printf("clip space too small for widget (%s %u)\n", widget.file, widget.line);
						continue;
					}

					clip_rect.x = clip_rect.x + widget.pad_left;
					clip_rect.w = clip_rect.w - (widget.pad_left + widget.pad_right);
					clip_rect.y = clip_rect.y + widget.pad_top;
					clip_rect.h = clip_rect.h - (widget.pad_top + widget.pad_bottom);

					buffer->commands[buffer->count++] = (WzDrawCommand){
						.type = DrawCommandType_Clip,
						.dest_rect = clip_rect,
						.color = WZ_BLUE
					};
				}
				else
				{
					//printf("wow");
				}

			}
			else
			{
				if (wz_handle_is_valid(current_clip_widget))
				{
					wz_assert(buffer->count < MAX_NUM_DRAW_COMMANDS - 1);
					buffer->commands[buffer->count++] = (WzDrawCommand){
						.type = DrawCommandType_StopClip,
					};
				}
			}

			buffer->commands[buffer->count - 1].widget_line_number = i;

			current_clip_widget = widget.clip_widget;
		}

#endif
	}
}

void wz_widget_clip(WzWidget handle)
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
	return (WzWidgetData) { 0 };
}

void wz_widget_resize(WzWidget handle, int* w_offset, int* h_offset)
{
	WzWidgetData* w = wz_widget_get(handle);
	WzWidgetData* parent = wz_widget_get(w->parent);

	if (parent->layout == WZ_LAYOUT_HORIZONTAL)
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

#define SCROLLBAR_SIZE 20

WzWidget wz_scroll_box(wzrd_v2 size, unsigned int* scroll, WzWidget parent, const char* tag)
{
	WzWidget panel = wz_hbox(parent);
	wz_widget_set_tight_constraints(panel, size.x, size.y);
	wz_widget_set_color_old(panel, EGUI_BEIGE);
	wz_widget_set_main_axis_size_min(panel);
	wz_widget_set_cross_axis_alignment(panel, CROSS_AXIS_ALIGNMENT_STRETCH);

	WzWidget clip_panel = wz_widget(panel);
	wz_widget_clip(clip_panel);
	wz_widget_set_flex(clip_panel);

	WzWidget user_panel = wz_vbox(clip_panel);
	wz_widget_set_color(user_panel, 0x00ffffff);
	wzrd_widget_tag(user_panel, tag);
	wz_widget_set_flex(user_panel);
	wz_widget_set_cross_axis_alignment(user_panel, CROSS_AXIS_ALIGNMENT_STRETCH);

	WzWidget c = wz_widget(user_panel);
	wz_widget_set_max_constraint_h(c, 150);
	wz_widget_set_color(c, 0x0000ffff);

	WzWidget scrollbar_panel = wz_vbox(panel);
	wz_widget_set_max_constraint_w(scrollbar_panel, SCROLLBAR_SIZE + 4);
	wz_widget_set_color(scrollbar_panel, 0xFF00FFFF);
	wz_widget_set_cross_axis_alignment(scrollbar_panel, CROSS_AXIS_ALIGNMENT_STRETCH);

	WzWidget top_button = wz_widget(scrollbar_panel);
	wz_widget_set_tight_constraints(top_button, SCROLLBAR_SIZE, SCROLLBAR_SIZE);
	wz_widget_set_border(top_button, WZ_BORDER_TYPE_DEFAULT);

	WzWidget middle_panel = wz_widget(scrollbar_panel);
	wz_widget_get(middle_panel)->color = EGUI_GRAY;
	wz_widget_get(middle_panel)->tag = tag;
	wz_widget_get(middle_panel)->secondary_tag = "scrollbar_middle";
	wz_widget_set_flex(middle_panel);
	wz_widget_set_border(middle_panel, WZ_BORDER_TYPE_DEFAULT);

	WzWidget bottom_button = wz_widget(scrollbar_panel);
	wz_widget_set_tight_constraints(bottom_button, SCROLLBAR_SIZE, SCROLLBAR_SIZE);
	wz_widget_set_border(bottom_button, WZ_BORDER_TYPE_DEFAULT);

	WzScrollbar* wz_scrollbar = &canvas->scrollbars[canvas->scrollbars_count];
	wz_scrollbar->content = user_panel;
	wz_scrollbar->scrollbar = middle_panel;
	wz_scrollbar->panel = panel;
	wz_scrollbar->top_button = top_button;
	wz_scrollbar->bottom_button = bottom_button;
	wz_scrollbar->scroll = scroll;
	canvas->scrollbars_count++;

#if 0

	//#if 0
	WzWidgetData b = wzrd_widget_get_cached_box(tag);
	WzWidget scrollbar = (WzWidget){ 0 };
	//if (wz_handle_is_valid(b.handle))
	{
		//WzWidgetData* box = &b;
	/*	float ratio = (float)b.actual_h / (float)b.content_h;
		if (ratio > 1)
			ratio = 1;*/

			//scrollbar = wz_hbox(middle_panel);
			//wz_widget_get(scrollbar)->percentage_h = ratio;
		wz_widget_get(scrollbar)->y = *scroll;

		//wz_widget_get(scrollbar)->tag = tag;
		//wz_widget_get(scrollbar)->secondary_tag = "scrollbar";
	}

	WzWidgetData scrollbar_middle_box = wzrd_widget_get_cached_box_with_secondary_tag(tag, "scrollbar_middle");
	WzWidgetData scrollbar_box = wzrd_widget_get_cached_box_with_secondary_tag(tag, "scrollbar");

	if (wzrd_widget_is_active(top_button))
	{
		if (*scroll > 0)
			*scroll -= 1;
	}

	if (wzrd_widget_is_active(bottom_button))
	{
		if (*scroll + scrollbar_box.actual_h < scrollbar_middle_box.actual_h - 2)
			*scroll += 1;
	}

	if (wzrd_widget_is_active(scrollbar))
	{
		int scroll_temp = *scroll + wzrd_canvas_get()->mouse_delta.y;

		if (scroll_temp + scrollbar_box.actual_h < scrollbar_middle_box.actual_h - 2 && scroll_temp > 0)
		{
			*scroll = scroll_temp;
		}
	}
#endif

	//printf("%d %d\n", wzrd_canvas_get()->mouse_pos.x, wzrd_canvas_get()->mouse_pos.y);
	return user_panel;
}

void wz_handle_input()
{
	// Mouse interaction
	if (canvas->enable_input)
	{
		wzrd_handle_input(canvas->boxes_indices, canvas->widgets_count);
		wzrd_handle_cursor();
		wzrd_handle_border_resize();
	}
	else
	{
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
}

void wz_end(wzrd_str* debug_str)
{
	unsigned int widgets_stack[MAX_NUM_BOXES];
	unsigned int widgets_visits[MAX_NUM_BOXES];
	unsigned int widgets_children[MAX_NUM_BOXES];
	WzRect rects[MAX_NUM_BOXES];
	unsigned int widgets_children_count = 0;
	WzWidgetDescriptor descriptors[MAX_NUM_BOXES];

	for (unsigned int i = 0; i < canvas->widgets_count; ++i)
	{
		descriptors[i].children = widgets_children + widgets_children_count;
		descriptors[i].children_count = canvas->widgets[i].children_count;

		for (unsigned int j = 0; j < canvas->widgets[i].children_count; ++j)
		{
			widgets_children[widgets_children_count] = canvas->widgets[i].children[j];
			widgets_children_count++;
		}

		descriptors[i].constraint_min_h = canvas->widgets[i].constraint_min_h;
		descriptors[i].constraint_min_w = canvas->widgets[i].constraint_min_w;
		descriptors[i].constraint_max_h = canvas->widgets[i].constraint_max_h;
		descriptors[i].constraint_max_w = canvas->widgets[i].constraint_max_w;
		descriptors[i].gap = canvas->widgets[i].child_gap;
		descriptors[i].pad_bottom = canvas->widgets[i].pad_bottom;
		descriptors[i].pad_top = canvas->widgets[i].pad_top;
		descriptors[i].pad_left = canvas->widgets[i].pad_left;
		descriptors[i].pad_right = canvas->widgets[i].pad_right;
		descriptors[i].flex_factor = canvas->widgets[i].flex_factor;
		descriptors[i].free_from_parent_horizontally = canvas->widgets[i].free_from_parent_horizontally;
		descriptors[i].free_from_parent_vertically = canvas->widgets[i].free_from_parent_vertically;
		descriptors[i].flex_fit = canvas->widgets[i].flex_fit;
		descriptors[i].main_axis_size_type = canvas->widgets[i].main_axis_size_type;
		descriptors[i].layout = canvas->widgets[i].layout;
		descriptors[i].cross_axis_alignment = canvas->widgets[i].cross_axis_alignment;
		descriptors[i].file = canvas->widgets[i].file;
		descriptors[i].line = canvas->widgets[i].line;
		descriptors[i].x = canvas->widgets[i].x;
		descriptors[i].y = canvas->widgets[i].y;
	}

	unsigned int layout_failed = 0;
	wz_do_layout(1, descriptors, rects, canvas->widgets_count, &layout_failed);

	if (!layout_failed)
	{
		for (unsigned int i = 0; i < canvas->widgets_count; ++i)
		{
			canvas->rects[i] = rects[i];
		}
	}

	for (unsigned int i = 0; i < canvas->widgets_count; ++i)
	{
		canvas->widgets[i].actual_x = canvas->rects[i].x;
		canvas->widgets[i].actual_y = canvas->rects[i].y;
		canvas->widgets[i].actual_w = canvas->rects[i].w;
		canvas->widgets[i].actual_h = canvas->rects[i].h;
	}

	// Second pass: handle scrollbars
	for (int i = 0; i < canvas->scrollbars_count; ++i)
	{
		WzScrollbar scrollbar = canvas->scrollbars[i];
		WzWidgetData* content_panel = wz_widget_get(scrollbar.content);
		unsigned int content_panel_h = content_panel->actual_h;
		unsigned int content_h = 0;

		for (int i = 0; i < content_panel->children_count; ++i)
		{
			content_h += canvas->widgets[content_panel->children[i]].actual_h;
		}

		if (!content_h)
		{
			continue;
		}

		wz_assert(content_panel_h);
		if (content_panel_h < 2 * SCROLLBAR_SIZE)
		{
			continue;
		}
		float ratio = (float)content_panel_h / (float)content_h;
		if (ratio > 1)
		{
			ratio = 1;
		}
		unsigned int scrollbar_h = (unsigned int)(ratio * (float)(content_panel_h - 2 * SCROLLBAR_SIZE));
		WzWidgetData* scrollbar_data = wz_widget_get(canvas->scrollbars[i].scrollbar);
		scrollbar_data->actual_h = scrollbar_h;

		if (wz_widget_is_active(scrollbar.top_button))
		{
			if (scrollbar_data->actual_y + *scrollbar.scroll + scrollbar_data->actual_h <
				content_panel->actual_y + content_panel->actual_h - SCROLLBAR_SIZE - 1)
			{
				*scrollbar.scroll += 1;
			}
		}

		scrollbar_data->actual_y += *scrollbar.scroll;
		wz_widget_add_offset(content_panel->handle, 0, -1 * *scrollbar.scroll);
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
	for (int i = 0; i < canvas->widgets_count; ++i)
	{
		canvas->boxes_indices[i] = i;
	}

	qsort(canvas->boxes_indices, canvas->widgets_count, sizeof(int), wzrd_compare_boxes);

	wz_handle_input();

	wz_draw(canvas->boxes_indices);
}

void wzrd_box_set_type(WzWidget handle, wzrd_box_type type)
{
	wz_widget_get(handle)->layout = type;
}

WzWidget egui_button_raw_begin(bool* released, WzWidget parent, const char* file, unsigned int line) {

	WzWidget handle = wz_widget_raw(parent, file, line);

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

WzWidget wz_command_toggle_raw(wzrd_str str, bool* active, WzWidget parent, const char* file_name, unsigned int line)
{
	WzWidget widget = wz_label_raw(str, parent, file_name, line);
	wz_widget_set_border(widget, WZ_BORDER_TYPE_DEFAULT);

	if (wz_widget_is_deactivating(widget))
	{
		*active = !*active;
	}

	if (*active)
	{
		wz_widget_get(widget)->border_type = WZ_BORDER_TYPE_CLICKED;
	}

	return widget;
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
	WzWidget h = wz_label_raw(str, parent, file, line);

	//if (wzrd_handle_is_valid(h))
	if (wz_widget_is_equal(h, canvas->activating_item)) {
		*active = true;
	}
	else
	{
		*active = false;
	}

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
	wz_assert(b->items_count < MAX_NUM_ITEMS - 1);
	b->items[b->items_count++] = item;
}

WzWidget wz_label_raw(wzrd_str str, WzWidget handle, const char* file, unsigned int line) {
	int w = 0, h = 0;
	canvas->get_string_size(str.str, &w, &h);

	w += 20;
	h += 20;

	WzWidget parent = wz_widget_raw(handle, file, line);

	wz_widget_set_tight_constraints(parent, w, h);

	wzrd_text_add(str, parent);

	return parent;
}

WzWidget wzrd_input_box_raw(char* str, int* len, int max_num_keys, WzWidget handle, const char* file, unsigned int line) {
	WzWidget parent = wz_widget_raw(handle, file, line);

	WzWidgetData* box = wz_widget_get(parent);
	wz_widget_set_max_constraint_w(box->handle, 50);
	wz_widget_set_max_constraint_h(box->handle, 30);
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

	wz_widget_set_max_constraint_w(parent, w);
	wz_widget_set_max_constraint_h(parent, h);

	wzrd_text_add(str, parent);

	if (wzrd_box_is_activating(wzrd_box_get_last()))
	{
		*result = true;
	}

	return parent;
}

WzWidget wzrd_dialog_begin_raw(wzrd_v2* pos, wzrd_v2 size,
	bool* active, wzrd_str name, int layer,
	WzWidget parent, const char* file, unsigned int line) {
	WZRD_UNUSED(name);

	if (!*active) return (WzWidget) { 0 };

	WzWidget window = wz_vbox(parent);
	wz_widget_set_free_from_parent_horizontally(window);
	wz_widget_set_free_from_parent_vertically(window);
	wz_widget_set_max_constraint_w(window, size.x);
	wz_widget_set_max_constraint_h(window, size.y);
	wz_widget_set_layer(window, layer);

	/*WzWidget top_panel = wz_widget_raw(window, file, line);
	wz_widget_set_constraint_h(top_panel, 28);
	wz_widget_set_constraint_w(top_panel, 28);*/

	WzWidget bar = wz_widget_raw(window, file, line);
	wz_widget_set_color_old(bar, (WzColor) { 57, 77, 205, 255 });
	wz_widget_set_max_constraint_h(bar, 28);
	//wz_widget_set_constraint_w(bar, 28);

	if (wz_widget_is_equal(bar, canvas->active_item))
	{
		pos->x += canvas->mouse_pos.x - canvas->previous_mouse_pos.x;
		pos->y += canvas->mouse_pos.y - canvas->previous_mouse_pos.y;
	}

	wz_widget_set_x(window, pos->x);
	wz_widget_set_y(window, pos->y);

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

void wz_widget_set_color(WzWidget widget, unsigned int color)
{
	wz_assert((color & 0x000000ff) != 0);
	WzColor c = (WzColor){ (color & 0xff000000) >> 24, (color & 0x00ff0000) >> 16, (color & 0x0000ff00) >> 8, (color & 0x000000ff) >> 0 };
	wz_widget_set_color_old(widget, c);
}

void wz_widget_set_cross_axis_alignment(WzWidget widget, unsigned int cross_axis_alignment)
{
	wz_assert(CROSS_AXIS_ALIGNMENT_TOTAL);
	wz_widget_get(widget)->cross_axis_alignment = cross_axis_alignment;
}

void wzrd_label_list_raw(wzrd_str* item_names, unsigned int count,
	unsigned int width, unsigned int height, unsigned int color,
	WzWidget* handles, unsigned int* selected, bool* is_selected,
	WzWidget parent, const char* file, unsigned int line)
{
	wz_assert(width);
	wz_assert(height);

	WzWidget panel = wz_vbox_raw(parent, file, line);
	wz_widget_set_main_axis_size_min(panel);
	wz_widget_set_color(panel, 0x00ffffff);

	if (wzrd_box_is_activating(wz_widget_get(panel)))
	{
		*selected = 0;
		*is_selected = false;
	}

	WzWidget selected_label = { 0 };

	for (unsigned int i = 0; i < count; ++i)
	{
		char str[32];
		sprintf_s(str, 32, "%d label list %d", wzrd_box_get_last()->handle.handle, i);

		bool is_label_clicked = false;
		WzWidget wdg = wzrd_label_button_activating(item_names[i], &is_label_clicked, panel, file, line);
		wz_widget_set_max_constraint_w(wdg, width);
		wz_widget_set_max_constraint_h(wdg, height);
		wz_widget_set_color(wdg, color);

		//wz_set_cross_axis_alignment(wdg);

		if (handles)
		{
			handles[i] = canvas->widgets[canvas->widgets_count - 2].handle;
		}

		if (is_label_clicked) {
			*selected = i;
			*is_selected = true;
			selected_label = wdg;
		}

		if (*is_selected && *selected == i)
		{
			wz_widget_get(wdg)->color = canvas->stylesheet.label_item_selected_color;
		}
	}
}

void wzrd_label_list_sorted_raw(wzrd_str* item_names, unsigned int count, int* items,
	unsigned int width, unsigned int height, unsigned int color, unsigned int* selected,
	bool* is_selected, WzWidget parent, const char* file_name, unsigned int line) {

	WzWidget handles[MAX_NUM_LABELS] = { 0 };

	wzrd_label_list_raw(item_names, count, width, height, color, handles, selected, is_selected, parent, file_name, line);

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
					wz_widget_set_y(c->handle, hovered_parent->actual_h - 2);
					wz_widget_set_max_constraint_h(c->handle, 2);
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

	WzWidget parent = wz_widget(handle);
	wz_widget_set_free_from_parent_horizontally(parent);
	wz_widget_set_free_from_parent_vertically(parent);
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

WzWidget wz_command_button_raw(wzrd_str str, bool* released, WzWidget parent, const char* file_name, unsigned int line)
{
	WzWidget button = wz_label_raw(str, parent, file_name, line);
	wz_widget_set_border(button, WZ_BORDER_TYPE_DEFAULT);

	*released = wz_widget_is_deactivating(button);

	if (wz_widget_is_active(button) || wzrd_widget_is_activating(button))
	{
		wz_widget_set_border(button, WZ_BORDER_TYPE_CLICKED);
	}

	return button;
}

void wz_widget_set_layer(WzWidget handle, unsigned int layer)
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