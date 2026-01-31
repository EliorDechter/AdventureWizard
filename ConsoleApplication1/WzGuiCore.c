#define wz_assert(x) assert(x)
//#define WZ_ASSERT(x) (void)(x)
#define WZRD_UNUSED(x) (void)x

#include "WzGuiCore.h"
#include "Strings.h"
#include "WzLayout.h"

static WzGui* gui;

bool wz_widget_is_equal(WzWidget a, WzWidget b);
bool wz_handle_is_valid(WzWidget handle);
WzWidgetData* wzrd_box_get_parent();
WzWidgetData* wzrd_box_get_previous();
void wzrd_crate_begin(int window_id, WzWidgetData box);
void wzrd_crate(int window_id, WzWidgetData box);
void wz_widget_add_texture(WzTexture texture, wzrd_v2 size, WzWidget box);
void wz_widget_add_text(WzStr str, WzWidget box);
//void wzrd_box_resize(wzrd_v2* size);
WzWidgetData* wz_widget_get(WzWidget str);
void wzrd_drag(bool* drag);
bool wzrd_box_is_dragged(WzWidgetData* box);
bool wzrd_box_is_hot_using_canvas(WzGui* canvas, WzWidgetData* box);
WzWidgetData* wzrd_box_get_released();
WzWidgetData* wzrd_box_find(WzGui* canvas, WzStr name);
wzrd_v2 wzrd_lerp(wzrd_v2 pos, wzrd_v2 end_pos);

bool wz_widget_is_equal(WzWidget a, WzWidget b)
{
	//WzWidgetData* widget_a = wz_widget_get(a);
	//WzWidgetData* widget_b = wz_widget_get(b);

#if 0
	if (strcmp(widget_a->source, widget_b->source) == 0 && widget_a->tag == widget_b->tag)
	{
		return true;
	}
#else
	if (a.handle == b.handle)
	{
		return true;
	}
#endif
	return false;
}

void wz_widget_add_source(WzWidget handle, const char* file, unsigned int line)
{
	if (!file) return;
	wz_assert(file);
	const char* f;
	for (f = file + strlen(file); *(f - 1) != '\\'; --f) {}

	WzWidgetData* w = wz_widget_get(handle);

	char temp_buffer[128];
	sprintf(temp_buffer, "%s %d ", f, line);
	strcat(w->source, temp_buffer);
}

void wz_widget_set_pad(WzWidget widget, unsigned pad)
{
	WzWidgetData* data = wz_widget_get(widget);

	data->pad_left = data->pad_right = data->pad_top = data->pad_bottom = pad;
	data->constraint_max_w += pad * 2;
	data->constraint_max_h += pad * 2;
}

void wz_widget_set_margin_left(WzWidget widget, unsigned int pad)
{
	wz_assert(pad);
	wz_assert(pad <= gui->window.w);

	wz_widget_get(widget)->margin_left = pad;
}

void wz_widget_set_margin_right(WzWidget widget, unsigned int pad)
{
	wz_assert(pad);
	wz_assert(pad <= gui->window.w);

	wz_widget_get(widget)->margin_right = pad;
}

void wz_widget_set_margin_top(WzWidget widget, unsigned int pad)
{
	WzWidgetData* d = wz_widget_get(widget);
	wz_assert(pad);

	wz_widget_get(widget)->margin_top = pad;
}

void wz_widget_set_margin_bottom(WzWidget widget, unsigned int pad)
{
	wz_assert(pad);
	wz_assert(pad <= gui->window.w);

	wz_widget_get(widget)->margin_bottom = pad;
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

WzWidgetData* wz_widget_get(WzWidget handle)
{
	WzWidgetData* result = &gui->widgets[0];
	for (int i = 0; i < MAX_NUM_WIDGETS; ++i)
	{
		if (gui->widgets[i].handle.handle == handle.handle)
		{
			result = gui->widgets + i;
			break;
		}
	}

	return result;
}

void wz_widget_get_actual_rect(WzWidget widget, int* x, int* y, unsigned* w, unsigned* h)
{
	WzWidgetData* data = wz_widget_get(widget);
	*x = data->actual_x;
	*y = data->actual_y;
	*w = data->actual_w;
	*h = data->actual_h;
}

void wz_widget_data_set_tight_constraints(WzWidgetData* b, unsigned int w, unsigned int h)
{
	b->constraint_min_w = b->constraint_max_w = w;
	b->constraint_min_h = b->constraint_max_h = h;
}

void wz_widget_set_tight_constraints(WzWidget handle, unsigned  w, unsigned  h)
{
	WzWidgetData* b = wz_widget_get(handle);
	wz_widget_data_set_tight_constraints(b, w, h);
}

WzWidget wz_create_handle()
{
	assert(gui);
	WzWidget handle = { 0 };
	for (unsigned i = 1; i < MAX_NUM_WIDGETS; ++i)
	{
		if (gui->free_widgets[i])
		{
			handle.handle = i;
			gui->free_widgets[i] = false;
			return handle;
		}
	}

	wz_assert(0);

	return handle;
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

void wz_widget_data_set_x(WzWidgetData* widget, int x)
{
	widget->x = x;
}

void wz_widget_data_set_y(WzWidgetData* widget, int y)
{
	widget->y = y;
}

void wz_widget_set_x(WzWidget w, int x)
{
	WzWidgetData* widget = wz_widget_get(w);
	wz_widget_data_set_x(widget, x);
}

void wz_widget_set_y(WzWidget w, int y)
{
	WzWidgetData* widget = wz_widget_get(w);
	wz_widget_data_set_y(widget, y);
}

void wz_widget_set_margins(WzWidget w, unsigned int pad)
{
	wz_widget_set_margin_top(w, pad);
	wz_widget_set_margin_bottom(w, pad);
	wz_widget_set_margin_left(w, pad);
	wz_widget_set_margin_right(w, pad);
}

void wz_widget_data_set_border(WzWidgetData* d, WzBorderType border_type)
{
	d->border_type = border_type;

	/*d->pad_top += 2;
	d->pad_bottom += 2;
	d->pad_left += 2;
	d->pad_right += 2;*/
}

void wz_widget_set_border(WzWidget w, WzBorderType border_type)
{
	WzWidgetData* d = wz_widget_get(w);
	wz_widget_data_set_border(d, border_type);
}

void wz_widget_set_pos(WzWidget handle, int x, int y)
{
	wz_widget_set_x(handle, x);
	wz_widget_set_y(handle, y);
}

void wz_widget_data_set_pos(WzWidgetData* handle, int x, int y)
{
	handle->x = x;
	handle->y = y;
}

void wz_widget_set_color_old(WzWidget widget, WzColor color)
{
	wz_widget_get(widget)->color = color;
}

Crate* EguiGetCurrentWindow() {
	Crate* result = &gui->crates_stack[gui->current_crate_index];

	return result;
}

Crate* EguiGetPreviousWindow() {
	wz_assert(gui->current_crate_index > 0);
	Crate* result = &gui->crates_stack[gui->current_crate_index - 1];

	return result;
}

WzWidgetData* wzrd_box_get_parent() {
	if (gui->crates_stack[gui->current_crate_index].box_stack_count < 2)
	{
		return &gui->widgets[0];
	}
	int current_box_index = gui->crates_stack[gui->current_crate_index].box_stack_count - 2;
	int final_index = gui->crates_stack[gui->current_crate_index].box_stack[current_box_index];
	WzWidgetData* result = &gui->widgets[final_index];

	return result;
}

bool wzrd_handle_is_child_of_handle(WzWidget a, WzWidget b)
{
	if (a.handle == 0) return false;
	if (b.handle == 0) return false;

	int stack[MAX_NUM_WIDGETS] = { 0 };
	int count = 0;
	int ptr = 0;

	stack[count++] = wz_widget_get(a)->handle.handle;

	while (ptr != count)
	{
		WzWidgetData* widget = &gui->widgets[stack[ptr]];

		for (int i = 0; i < widget->children_count; ++i)
		{
			wz_assert(count < MAX_NUM_WIDGETS);
			stack[count++] = widget->children[i].handle;
		}

		if (wz_widget_is_equal(widget->handle, b))
			return true;

		ptr++;
	}

	return false;
}


int wzrd_box_get_current_index() {
	wz_assert(gui->current_crate_index >= 0);
	int current_box_index = gui->crates_stack[gui->current_crate_index].box_stack_count - 1;
	if (current_box_index < 0) return 0;
	int final_index = gui->crates_stack[gui->current_crate_index].box_stack[current_box_index];
	int result = final_index;

	return result;
}

void wz_widget_add_text(WzWidget parent, WzStr str)
{
	WzWidgetItem item;

	item.type = WZ_WIDGET_ITEM_TYPE_STRING;
	item.val.str = str;
	//item.color = wz_widget_get(parent)->font_color;
	item.margin_bottom = item.margin_top = item.margin_right = item.margin_left = 0;
	wz_widget_add_item(parent, item);
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
		WzWidgetData* box = &gui->widgets[stack[ptr]];

		for (int i = 0; i < box->children_count; ++i)
		{
			stack[count++] = box->children[i].handle;
		}

		goo(box, data);
		ptr++;
	}
}

bool wzrd_handle_is_active_tree(WzWidget handle)
{
	if (!handle.handle) return false;

	int stack[1024];
	int count = 0;
	int ptr = 0;

	stack[count++] = wz_widget_get(handle)->handle.handle;

	while (ptr != count)
	{
		WzWidgetData* box = &gui->widgets[stack[ptr]];

		for (int i = 0; i < box->children_count; ++i)
		{
			wz_assert(count < 1023);
			stack[count++] = box->children[i].handle;
		}

		if (wzrd_box_is_active(box))
		{
			return true;
		}

		ptr++;
	}

	return false;
}

bool wz_widget_is_interacting_tree(WzWidget handle)
{
	if (!handle.handle) return false;

	int stack[1024];
	int count = 0;
	int ptr = 0;

	stack[count++] = wz_widget_get(handle)->handle.handle;

	while (ptr != count)
	{
		WzWidgetData* box = &gui->widgets[stack[ptr]];

		for (int i = 0; i < box->children_count; ++i)
		{
			wz_assert(count < 1023);
			stack[count++] = box->children[i].handle;
		}

		if (wz_widget_is_interacting(box->handle))
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

	stack[count++] = wz_widget_get(handle)->handle.handle;

	while (ptr != count)
	{
		WzWidgetData* box = &gui->widgets[stack[ptr]];

		for (int i = 0; i < box->children_count; ++i)
		{
			stack[count++] = box->children[i].handle;
		}

		box->actual_x += x;
		box->actual_y += y;

		ptr++;
	}
}

bool wz_widget_is_deactivating(WzWidget handle)
{
	if (wz_widget_is_equal(handle, gui->deactivating_item))
	{
		return true;
	}

	return false;
}

bool wzrd_box_is_released(WzWidgetData* box) {
	if (wz_widget_is_equal(box->handle, gui->deactivating_item)) {
		return true;
	}

	return false;
}

bool wzrd_handle_is_released_tree(WzWidget handle)
{
	int stack[1024];
	int count = 0;
	int ptr = 0;

	stack[count++] = wz_widget_get(handle)->handle.handle;

	while (ptr != count)
	{
		WzWidgetData* box = &gui->widgets[stack[ptr]];

		for (int i = 0; i < box->children_count; ++i)
		{
			stack[count++] = box->children[i].handle;
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
	WzWidgetData* c1 = gui->widgets + index1;
	WzWidgetData* c2 = gui->widgets + index2;

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

void wz_widget_set_free_from_parent(WzWidget w)
{
	wz_widget_get(w)->free_from_parent = true;
}

void wz_widget_add_child(WzWidget parent, WzWidget child)
{
	//wz_assert(parent.handle);
	wz_assert(child.handle);

	if (!parent.handle) return;

	WzWidgetData* p = wz_widget_get(parent);
	WzWidgetData* c = wz_widget_get(child);
	wz_assert(p->children_count < MAX_NUM_CHILDREN - 1);
	for (unsigned i = 0; i < p->children_count; ++i)
	{
		wz_assert(c->handle.handle != p->children[i].handle);
	}

	p->children[p->children_count++] = c->handle;
	c->layer = p->layer;
	c->clip_widget = p->clip_widget;
	c->parent = parent;
}

WzWidgetData wz_widget_create(WzWidget parent)
{
	WzWidgetData box = { 0 };
	box.children_count = 0;
	box.free_children_count = 0;
	box.items_count = 0;
	box.color = (WzColor){ 0xc0, 0xc0, 0xc0, 0xff };
	box.font_color = WZ_BLACK;
	box.percentage_h = 0;
	box.percentage_w = 0;
	box.cross_axis_alignment = 0;
	box.best_fit = false;
	box.bring_to_front = false;
	box.child_gap = 0;
	box.fit_h = false;
	box.fit_w = false;
	box.margin_left = box.margin_right = box.margin_top = box.margin_bottom = 0;
	box.clip_widget.handle = 0;
	box.w_offset = 0;
	box.h_offset = 0;
	box.disable_hover = false;
	box.layer = 0;
	box.actual_w = box.actual_h = 0;
	box.actual_x = box.actual_y = 0;
	box.layout = WZ_LAYOUT_NONE;
	box.flex_factor = 0;
#if 0
	box.constraint_min_w = box.constraint_max_w = 50;
	box.constraint_min_h = box.constraint_max_h = 20;
#else
	box.constraint_min_w = box.constraint_min_h = 0;
	box.constraint_max_w = box.constraint_max_h = UINT_MAX;
#endif
	box.x = box.y = 0;
	box.free_from_parent = 0;
	box.cull = false;
	box.source[0] = 0;
	box.tag = 0;
	box.layer = EguiGetCurrentWindow()->layer;

	box.border_type = WZ_BORDER_TYPE_DEFAULT;
	box.margin_top += 2;
	box.margin_bottom += 2;
	box.margin_left += 2;
	box.margin_right += 2;

	return box;
}

WzWidget wz_widget_add_to_frame(WzWidget parent, WzWidgetData widget)
{
	widget.handle = wz_create_handle();
	gui->widgets[widget.handle.handle] = widget;

	wz_widget_add_child(parent, widget.handle);

	return widget.handle;
}

WzWidget wz_widget_persistent(WzWidget parent, WzWidgetData widget_data)
{
	WzWidget widget = wz_widget_add_to_frame(parent, widget_data);

	return widget;
}

WzWidget wz_widget_raw(WzWidget parent, const char* file, unsigned int line)
{
	WzWidgetData box = wz_widget_create(parent);
	WzWidget widget = wz_widget_add_to_frame(parent, box);
	wz_widget_add_source(widget, file, line);

	return widget;
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
	WzWidgetItem item;
	item.w = w;
	item.h = h;
	item.type = ItemType_Rect;
	item.color = color;
	item.margin_left = item.margin_right = item.margin_bottom = item.margin_top = 0;
	item.center_h = item.center_w = true;

	wz_widget_add_item(widget, item);
}

void wz_widget_add_rect_absolute(WzWidget widget, int x, int y, unsigned w, unsigned h, WzColor color)
{
	WzWidgetItem item;
	item.w = w;
	item.h = h;
	item.x = x;
	item.y = y;
	item.type = ItemType_RectAbsolute;
	item.color = color;
	item.margin_left = item.margin_right = item.margin_bottom = item.margin_top = 0;
	item.center_h = item.center_w = true;

	wz_widget_add_item(widget, item);
}

void wz_widget_add_texture(WzWidget parent, WzTexture texture, unsigned w, unsigned h) {

	WzWidgetItem item;
	item.type = ItemType_Texture;
	item.size = (wzrd_v2){ w, h };
	item.val.texture = texture;
	item.margin_left = item.margin_right = item.margin_bottom = item.margin_top = 0;
	item.center_h = item.center_w = true;
	item.w = w;
	item.h = h;

	wz_widget_add_item(parent, item);
}

void wzrd_crate_begin(int layer, WzWidgetData box) {

	gui->total_num_windows++;
	gui->current_crate_index++;

	Crate* current_window = EguiGetCurrentWindow();

	// Set new window
	*current_window = (Crate){ .layer = layer, .index = gui->current_crate_index, };

	// Begin drawing panel
	box.layer = layer;
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
}

#define MAX_NUM_SCROLLBARS 32

void wz_get_string_size(const char* str, int* w, int* h)
{

}

WzWidget wz_gui_begin(WzGui* gui_in,
	unsigned window_w, unsigned  window_h,
	unsigned mouse_x, unsigned mouse_y,
	void (*get_string_size)(char*, int*, int*),
	WzState left_mouse_state,
	WzKeyboardKeys keys,
	bool enable_input)
{
	gui = gui_in;

	memset(gui->free_widgets, 1, sizeof(*gui->free_widgets) * MAX_NUM_WIDGETS);
	for (unsigned i = 0; i < gui->persistent_widgets_count; ++i)
	{
		gui->free_widgets[gui->persistent_widgets[i].handle.handle] = false;
	}

	gui->keyboard_keys = keys;
	gui->mouse_left = left_mouse_state;
	gui->mouse_pos = (wzrd_v2){ mouse_x, mouse_y };
	gui->mouse_delta.x = gui->mouse_pos.x - gui->previous_mouse_pos.x;
	gui->mouse_delta.y = gui->mouse_pos.y - gui->previous_mouse_pos.y;

	if (get_string_size)
	{
		gui->get_string_size = get_string_size;
	}
	else
	{
		gui->get_string_size = wz_get_string_size;
	}

	gui->window = (WzRect){ 0, 0, window_w, window_h };
	gui->enable_input = enable_input;
	gui->styles_count = 0;
	gui->current_crate_index = 0;

	WZRD_UNUSED(gui);

	gui->input_box_timer += 16.7f;

	// Empty box
	//gui->widgets_count = 0;
	//gui->widgets[gui->widgets_count++] = (WzWidgetData){ 0 };

	// Window
	WzWidget window_widget = wz_widget((WzWidget) { 0 });
	wz_widget_set_constraints(window_widget, window_w, window_h, window_w, window_h);
	wz_widget_set_border(window_widget, WZ_BORDER_TYPE_NONE);

	// Stylesheet
	gui->stylesheet.label_color = EGUI_LIGHTGRAY;
	gui->stylesheet.label_item_selected_color = EGUI_LIGHTGRAY;

	gui->scrollbars_count = 0;

	return window_widget;
}

void wz_draw_rect(WzDrawCommandBuffer* buffer, WzRect rect, WzColor color,
	int z, char* source)
{
	//wz_assert(rect.w > 0);
	//wz_assert(rect.h > 0);

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

	command.source = source;

	wz_assert(buffer->count < MAX_NUM_DRAW_COMMANDS - 1);
	buffer->commands[buffer->count++] = command;
}

WzWidgetData* wzrd_box_get_previous() {
	//WzWidgetData* result = &gui->widgets[gui->widgets_count - 1];

	//return result;
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
	gui->left_resized_item = (WzWidget){ 0 };
	gui->right_resized_item = (WzWidget){ 0 };
	gui->top_resized_item = (WzWidget){ 0 };
	gui->bottom_resized_item = (WzWidget){ 0 };

	for (int i = 0; i < MAX_NUM_WIDGETS; ++i) {
		WzWidgetData* owner = gui->widgets + i;
		for (int j = 0; j < owner->children_count; ++j) {
			WzWidgetData* child = &gui->widgets[owner->children[j].handle];

			int border_size = 10;

			bool is_inside_left_border =
				gui->mouse_pos.x >= child->actual_x &&
				gui->mouse_pos.y >= child->actual_y &&
				gui->mouse_pos.x < child->actual_x + border_size &&
				gui->mouse_pos.y < child->actual_y + child->actual_h;
			bool is_inside_right_border =
				gui->mouse_pos.x >= child->actual_x + child->actual_w - border_size &&
				gui->mouse_pos.y >= child->actual_y &&
				gui->mouse_pos.x < child->actual_x + child->actual_w &&
				gui->mouse_pos.y < child->actual_y + child->actual_h;
			bool is_inside_top_border =
				gui->mouse_pos.x >= child->actual_x &&
				gui->mouse_pos.y >= child->actual_y &&
				gui->mouse_pos.x < child->actual_x + child->actual_w &&
				gui->mouse_pos.y < child->actual_y + border_size;
			bool is_inside_bottom_border =
				gui->mouse_pos.x >= child->actual_x &&
				gui->mouse_pos.y >= child->actual_y + child->actual_h - border_size &&
				gui->mouse_pos.x < child->actual_x + child->actual_w &&
				gui->mouse_pos.y < child->actual_y + child->actual_h;

			if (wz_widget_is_equal(gui->hovered_item, child->handle) || wz_widget_is_equal(gui->active_item, child->handle))
			{
				if (is_inside_top_border || is_inside_bottom_border)
				{
					//child->color = WZ_BLUE;
					//*cursor = wzrd_cursor_vertical_arrow;
				}
				else if (is_inside_left_border || is_inside_right_border)
				{
					//child->color = WZ_BLUE;
					//*cursor = wzrd_cursor_horizontal_arrow;
				}
			}

			if (wz_widget_is_equal(gui->active_item, child->handle)) {
				if (is_inside_bottom_border) {
					//child->color = EGUI_PURPLE;
					gui->bottom_resized_item = child->handle;
				}
				else if (is_inside_top_border) {
					//child->color = EGUI_PURPLE;
					gui->top_resized_item = child->handle;
				}
				else if (is_inside_left_border) {
					//child->color = EGUI_PURPLE;
					gui->left_resized_item = child->handle;
				}
				else if (is_inside_right_border) {
					//child->color = EGUI_PURPLE;
					gui->right_resized_item = child->handle;
				}
			}
		}
	}
}

bool wzrd_handle_is_active(WzWidget handle) {
	if (wz_widget_is_equal(handle, gui->active_item)) {
		return true;
	}

	return false;
}

bool wzrd_handle_is_released(WzWidget handle) {
	if (wz_widget_is_equal(handle, gui->deactivating_item)) {
		return true;
	}

	return false;
}

bool wzrd_handle_is_hovered(WzWidget handle) {
	if (wz_widget_is_equal(handle, gui->hovered_item)) {
		return true;
	}

	return false;
}

bool wzrd_handle_is_hovered_from_list(WzWidget handle)
{

	for (int i = 0; i < gui->hovered_items_list_count; ++i)
	{
		if (wz_widget_is_equal(handle, gui->hovered_items_list[i]))
		{
			return true;
		}
	}

	return false;
}

bool wzrd_handle_is_interacting(WzWidget handle) {
	if (wz_widget_is_equal(handle, gui->activating_item) ||
		wz_widget_is_equal(handle, gui->active_item) ||
		wz_widget_is_equal(handle, gui->deactivating_item))
	{
		return true;
	}

	return false;
}



void wzrd_handle_input(int* indices, int count)
{

	// For Debugging
	{
		/*canvas->mouse_pos.x = 312;
		canvas->mouse_pos.y = 204;*/
	}

	WzWidgetData* hovered_box = gui->widgets;
	unsigned int max_layer = 0;
	gui->hovered_items_list_count = 0;
	gui->hovered_boxes_count = 0;
	for (int i = 0; i < count; ++i) {
		WzWidgetData* box = gui->widgets + indices[i];

		WzRect scaled_rect = { box->actual_x, box->actual_y, box->actual_w, box->actual_h };

#if 0
		// Input Clipping: Block mouse from reaching clipped widgets
		if (wz_handle_is_valid(box->clip_widget))
		{
			WzWidgetData* clip_widget = wz_widget_get(box->clip_widget);

			if (box->actual_x < clip_widget->actual_w)
			{
				y			scaled_rect.x = clip_widget->actual_w;
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

		bool is_hover = false;
		{
			// Clamp rect size to some arbitrary number in case it's uint_max
			int w, h;
			if (scaled_rect.w < 8000)
			{
				w = scaled_rect.w;
			}
			else
			{
				w = 8000;
			}

			if (scaled_rect.h < 8000)
			{
				h = scaled_rect.h;
			}
			else
			{
				h = 8000;
			}

			if (gui->mouse_pos.x >= scaled_rect.x &&
				gui->mouse_pos.x <= scaled_rect.x + w &&
				gui->mouse_pos.y >= scaled_rect.y &&
				gui->mouse_pos.y <= scaled_rect.y + h)
			{
				is_hover = true;
			}
		}

		if (is_hover && !box->disable_hover)
		{
			wz_assert(gui->hovered_items_list_count < MAX_NUM_WIDGETS);
			gui->hovered_items_list[gui->hovered_items_list_count++] = box->handle;
			gui->hovered_boxes[gui->hovered_boxes_count++] = *box;

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
	WzWidgetData* half_clicked_box = wz_widget_get(gui->activating_item);
	if (half_clicked_box && (gui->mouse_left == WZ_ACTIVE || gui->mouse_left == WZ_DEACTIVATING))
	{
		gui->activating_item = (WzWidget){ 0 };
	}

	if (gui->mouse_left == WZ_DEACTIVATING)
	{
		//canvas->released_item = canvas->dragged_item;
		gui->dragged_box = (WzWidgetData){ 0 };
		gui->dragged_item = (WzWidget){ 0 };

		gui->clean = false;
	}

	if (gui->mouse_left == WZ_INACTIVE)
	{
		gui->deactivating_item = (WzWidget){ 0 };
	}

	if (wz_handle_is_valid(hovered_box->handle))
	{
		gui->hovered_item = hovered_box->handle;
	}
	else {
		gui->hovered_item = (WzWidget){ 0 };
	}

	WzWidgetData* hot_box = wz_widget_get(gui->hovered_item);
	WzWidgetData* active_box = wz_widget_get(gui->active_item);

	if (wz_handle_is_valid(active_box->handle))
	{
		if (gui->mouse_left == WZ_DEACTIVATING)
		{
			if (wz_handle_is_valid(gui->active_item))
			{
				if (hot_box == active_box)
				{
					gui->clicked_item = active_box->handle;
					//canvas->selected_item = hot_box->handle;
				}

				gui->deactivating_item = gui->active_item;
				gui->active_item = (WzWidget){ 0 };
			}
		}
	}

	if (wz_handle_is_valid(hot_box->handle)) {

		/*	if (hot_box->type == wzrd_box_type_flat_button)
			{
				hot_box->color = (wzrd_color){ 0, 255, 255, 255 };
			}*/
		if (gui->mouse_left == WZ_ACTIVATING) {
			gui->active_item = hot_box->handle;

			gui->activating_item = hot_box->handle;

			// Dragging
			wz_assert(half_clicked_box);
			if (half_clicked_box->is_draggable) {
				gui->dragged_item = half_clicked_box->handle;

				gui->dragged_box = *half_clicked_box;
				//g_gui->dragged_box.x = g_gui->dragged_box.x;
				//g_gui->dragged_box.y = g_gui->dragged_box.y;
				//g_gui->dragged_box.w = g_gui->dragged_box.w;
				//g_gui->dragged_box.h = g_gui->dragged_box.h;
				//g_gui->dragged_box.name = wzrd_str_create("drag");
				//g_gui->dragged_box.absolute_rect = (wzrd_rect){ 0 };
				gui->dragged_box.disable_hover = true;
			}
		}
	}

	// Clicked item
	if (wz_handle_is_valid(gui->clicked_item) && gui->mouse_left == WZ_INACTIVE) {
		gui->clicked_item = (WzWidget){ 0 };

		gui->clean = false;
	}

	// Input box
	if (wz_handle_is_valid(gui->clicked_item)) {
		WzWidgetData* clicked_box = wz_widget_get(gui->clicked_item);
		wz_assert(clicked_box);
		/*	if (clicked_box->type == wzrd_box_type_input_box) {
				canvas->active_input_box = clicked_box->handle;
			}
			else {
				canvas->active_input_box = (wzrd_handle){ 0 };
			}*/
	}
}

bool wz_widget_is_activating(WzWidget handle) {
	if (wz_widget_is_equal(handle, gui->activating_item)) {
		return true;
	}

	return false;
}


bool wz_widget_is_active(WzWidget handle) {
	if (wz_widget_is_equal(handle, gui->active_item)) {
		return true;
	}

	return false;
}

bool wz_widget_is_interacting(WzWidget handle)
{
	return wz_widget_is_activating(handle) || wz_widget_is_active(handle);
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

void wz_draw_string(const char* string, WzRect rect)
{
	int w, h;
	gui->get_string_size(string, &w, &h);

	WzDrawCommand command = (WzDrawCommand){
		.type = DrawCommandType_String,
			.str = wz_str_create(string),
			.dest_rect = rect,
			.color = WZ_BLACK,
			.z = 10
	};

	wz_assert(gui->commands_buffer.count < MAX_NUM_DRAW_COMMANDS - 1);
	gui->commands_buffer.commands[gui->commands_buffer.count++] = command;
}

void wz_widget_set_max_constraints(WzWidget widget, unsigned int w, unsigned int h)
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
	WzDrawCommandBuffer* buffer = &gui->commands_buffer;
	buffer->count = 0;
	unsigned int line_size;
	WzWidget current_clip_widget;
	WzWidgetItem item;
	WzRect item_dest_rect;

	current_clip_widget.handle = 0;

	for (int i = 1; i < MAX_NUM_WIDGETS; ++i)
	{
		widget = gui->widgets[boxes_indices[i]];

		if (widget.cull)
		{
			//continue;
		}
#if 1

		if (widget.actual_w <= 2 || widget.actual_h <= 2)
		{
			continue;
		}

		// Draw Widget
		wz_draw_rect(buffer, (WzRect) {
			.x = widget.actual_x,
				.y = widget.actual_y,
				.w = widget.actual_w,
				.h = widget.actual_h,
		},
			widget.color,
			widget.layer, widget.source);

		// Borders (1215 x 810)
		if (!(widget.actual_w <= 2 || widget.actual_h <= 2))
		{
			if (widget.border_type != WZ_BORDER_TYPE_NONE)
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
				wz_draw_rect(buffer, top0, EGUI_WHITE2, widget.layer, widget.source);
				wz_draw_rect(buffer, left0, EGUI_WHITE2, widget.layer, widget.source);
				wz_draw_rect(buffer, top1, EGUI_LIGHTGRAY, widget.layer, widget.source);
				wz_draw_rect(buffer, left1, EGUI_LIGHTGRAY, widget.layer, widget.source);

				// Draw bottom and right lines
				wz_draw_rect(buffer, bottom0, WZ_BLACK, widget.layer, widget.source);
				wz_draw_rect(buffer, right0, WZ_BLACK, widget.layer, widget.source);
				wz_draw_rect(buffer, bottom1, EGUI_GRAY, widget.layer, widget.source);
				wz_draw_rect(buffer, right1, EGUI_GRAY, widget.layer, widget.source);
			}
			else if (widget.border_type == WZ_BORDER_TYPE_CLICKED) {
				// Draw top and left lines
				wz_draw_rect(buffer, top0, WZ_BLACK, widget.layer, widget.source);
				wz_draw_rect(buffer, left0, WZ_BLACK, widget.layer, widget.source);
				wz_draw_rect(buffer, top1, EGUI_GRAY, widget.layer, widget.source);
				wz_draw_rect(buffer, left1, EGUI_GRAY, widget.layer, widget.source);

				// Draw bottom and right lines
				wz_draw_rect(buffer, bottom0, EGUI_WHITE2, widget.layer, widget.source);
				wz_draw_rect(buffer, right0, EGUI_WHITE2, widget.layer, widget.source);
				wz_draw_rect(buffer, bottom1, EGUI_LIGHTESTGRAY, widget.layer, widget.source);
				wz_draw_rect(buffer, right1, EGUI_LIGHTESTGRAY, widget.layer, widget.source);
			}
			else if (widget.border_type == BorderType_InputBox) {
				// Draw top and left lines
				wz_draw_rect(buffer, top0, EGUI_GRAY, widget.layer, widget.source);
				wz_draw_rect(buffer, left0, EGUI_GRAY, widget.layer, widget.source);
				wz_draw_rect(buffer, top1, WZ_BLACK, widget.layer, widget.source);
				wz_draw_rect(buffer, left1, WZ_BLACK, widget.layer, widget.source);

				// Draw bottom and right lines
				wz_draw_rect(buffer, bottom0, EGUI_WHITE2, widget.layer, widget.source);
				wz_draw_rect(buffer, right0, EGUI_WHITE2, widget.layer, widget.source);
				wz_draw_rect(buffer, bottom1, EGUI_LIGHTESTGRAY, widget.layer, widget.source);
				wz_draw_rect(buffer, right1, EGUI_LIGHTESTGRAY, widget.layer, widget.source);
			}
			else if (widget.border_type == WZ_BORDER_TYPE_BLACK) {
				// Draw top and left lines
				wz_draw_rect(buffer, top0, WZ_BLACK, widget.layer, widget.source);
				wz_draw_rect(buffer, left0, WZ_BLACK, widget.layer, widget.source);
				wz_draw_rect(buffer, top1, WZ_BLACK, widget.layer, widget.source);
				wz_draw_rect(buffer, left1, WZ_BLACK, widget.layer, widget.source);

				// Draw bottom and right lines
				wz_draw_rect(buffer, bottom0, WZ_BLACK, widget.layer, widget.source);
				wz_draw_rect(buffer, right0, WZ_BLACK, widget.layer, widget.source);
				wz_draw_rect(buffer, bottom1, WZ_BLACK, widget.layer, widget.source);
				wz_draw_rect(buffer, right1, WZ_BLACK, widget.layer, widget.source);
			}
			else if (widget.border_type == BorderType_BottomLine) {
				wz_draw_rect(buffer, bottom0, EGUI_WHITE2, widget.layer, widget.source);
				wz_draw_rect(buffer, bottom1, EGUI_GRAY, widget.layer, widget.source);
			}
			else if (widget.border_type == BorderType_LeftLine) {
				wz_draw_rect(buffer, left0, EGUI_GRAY, widget.layer, widget.source);
				wz_draw_rect(buffer, left1, EGUI_WHITE2, widget.layer, widget.source);
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
				item.w,
				item.h
			};

			if (item.center_w)
			{
				item_dest_rect.x = widget.actual_x + widget.actual_w / 2 - item.w / 2;
			}
			if (item.center_h)
			{
				item_dest_rect.y = widget.actual_y + widget.actual_h / 2 - item.h / 2;
			}

			// String item
			if (item.type == WZ_WIDGET_ITEM_TYPE_STRING)
			{
				int w, h;
				gui->get_string_size(item.val.str.str, &w, &h);

				item_dest_rect.x = widget.actual_x + widget.actual_w / 2 - w / 2;
				item_dest_rect.y = widget.actual_y + widget.actual_h / 2 - h / 2;

				command = (WzDrawCommand){
					.type = DrawCommandType_String,
						.str = item.val.str,
						.dest_rect = item_dest_rect,
						.color = widget.font_color,
						.z = widget.layer
				};
			}

			// Texture item
			if (item.type == ItemType_Texture)
			{
				command = (WzDrawCommand){
					.type = DrawCommandType_Texture,
					.dest_rect = (WzRect){widget.actual_x, widget.actual_y, widget.actual_w, widget.actual_h},
					.src_rect = (WzRect) {0, 0, item.val.texture.w, item.val.texture.h},
					.texture = item.val.texture,
					.z = widget.layer
				};
			}

			// Rect item
			if (item.type == ItemType_Rect)
			{
				command = (WzDrawCommand){
					.type = DrawCommandType_Rect,
					.dest_rect = item_dest_rect,
					.color = item.color,
					.z = widget.layer
				};

				wz_assert(command.dest_rect.w > 0);
				wz_assert(command.dest_rect.h > 0);
			}

			if (item.type == ItemType_RectAbsolute)
			{

				item_dest_rect.x = item.x;
				item_dest_rect.y = item.y;
				item_dest_rect.w = item.w;
				item_dest_rect.h = item.h;

				command = (WzDrawCommand){
					.type = DrawCommandType_Rect,
					.dest_rect = item_dest_rect,
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
			else if (item.type == ItemType_HorizontalDottedLine)
			{
				command = (WzDrawCommand){
					.type = DrawCommandType_HorizontalLine,
						.dest_rect = item_dest_rect,
					.z = widget.layer
				};
			}
			else if (item.type == ItemType_LeftHorizontalDottedLine)
			{
				command = (WzDrawCommand){
					.type = DrawCommandType_HorizontalLine,
						.dest_rect = (WzRect){
							widget.actual_x,
							widget.actual_y + widget.actual_h / 2,
							widget.actual_x + widget.actual_w / 2,
							widget.actual_y + widget.actual_h / 2
					},
					.z = widget.layer

				};
			}
			else if (item.type == ItemType_RightHorizontalDottedLine)
			{
				command = (WzDrawCommand){
					.type = DrawCommandType_HorizontalLine,
						.dest_rect = (WzRect){
							widget.actual_x + widget.actual_w / 2,
							widget.actual_y + widget.actual_h / 2,
							widget.actual_x + widget.actual_w,
							widget.actual_y + widget.actual_h / 2
					},
					.z = widget.layer

				};
			}
			else if (item.type == ItemType_VerticalDottedLine)
			{
				command = (WzDrawCommand){
					.type = DrawCommandType_VerticalLine,
						.dest_rect = item_dest_rect,
					.z = widget.layer
				};
			}
			else if (item.type == ItemType_VerticalLine)
			{
				command = (WzDrawCommand){
					.type = DrawCommandType_VerticalLine,
						.dest_rect = item_dest_rect,
					.z = widget.layer
				};
			}
			else if (item.type == ItemType_LineAbsolute)
			{
				command = (WzDrawCommand){
					.type = DrawCommandType_Line,
					.line = item.val.line,
					.z = widget.layer
				};
			}
			else if (item.type == ItemType_DottedLineAbsolute)
			{
				command = (WzDrawCommand){
					.type = DrawCommandType_LineDotted,
					.line = item.val.line,
					.z = widget.layer
				};
			}
			else if (item.type == ItemType_HorizontalLine)
			{
				command = (WzDrawCommand){
					.type = DrawCommandType_HorizontalLine,
						.dest_rect = item_dest_rect,
					.z = widget.layer
				};
			}
			else if (item.type == ItemType_TopVerticalDottedLine)
			{
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
			else if (item.type == ItemType_BottomVerticalDottedLine)
			{
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

			command.dest_rect.x += item.margin_left;
			command.dest_rect.y += item.margin_top;

			wz_assert(command.dest_rect.w >= 0);
			wz_assert(command.dest_rect.h >= 0);

			wz_assert(buffer->count < MAX_NUM_DRAW_COMMANDS - 1);
			buffer->commands[buffer->count++] = command;

			buffer->commands[buffer->count - 1].source = widget.source;
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
						!(clip_rect.w > (widget.margin_left + widget.margin_right)) ||
						!(clip_rect.h > (widget.margin_top + widget.margin_bottom)))
					{
						//printf("clip space too small for widget (%s %u)\n", widget.file, widget.line);
						printf("erorr!\n");
						continue;
					}

					clip_rect.x = clip_rect.x + widget.margin_left;
					clip_rect.w = clip_rect.w - (widget.margin_left + widget.margin_right);
					clip_rect.y = clip_rect.y + widget.margin_top;
					clip_rect.h = clip_rect.h - (widget.margin_top + widget.margin_bottom);

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

			//buffer->commands[buffer->count - 1].widget_line_number = i;

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
	for (int i = 0; i < gui->cached_boxes_count; ++i)
	{
		if (gui->cached_boxes[i].tag == tag && gui->cached_boxes[i].secondary_tag == secondary_tag)
		{
			return gui->cached_boxes[i];
		}
	}

	return (WzWidgetData) { 0 };
}

WzWidgetData wzrd_widget_get_cached_box(const char* tag)
{
	for (int i = 0; i < gui->cached_boxes_count; ++i)
	{
		if (gui->cached_boxes[i].tag == tag && !gui->cached_boxes[i].secondary_tag)
		{
			return gui->cached_boxes[i];
		}
	}

	return (WzWidgetData) { 0 };
}

void wz_widget_add_tag(WzWidget parent, const void* tag)
{
	wz_widget_get(parent)->tag = tag;
}

WzWidgetData* wz_widget_get_parent(WzWidget widget)
{
	return wz_widget_get(wz_widget_get(widget)->parent);
}

void wz_widget_resize(WzWidget handle, int* w_offset, int* h_offset)
{
	WzWidgetData* w = wz_widget_get(handle);
	WzWidgetData* parent = wz_widget_get(w->parent);

	if (parent->layout == WZ_LAYOUT_HORIZONTAL)
	{
		if (wz_widget_is_equal(w->handle, gui->right_resized_item)) {
			*w_offset += gui->mouse_delta.x;
		}
	}
	else {
		if (wz_widget_is_equal(w->handle, gui->bottom_resized_item)) {
			*h_offset += gui->mouse_delta.y;
		}
	}

	w->w_offset = *w_offset;
	w->h_offset = *h_offset;
}

#define SCROLLBAR_SIZE 20

WzWidget wz_scroll_box(wzrd_v2 size, unsigned int* scroll, WzWidget parent, const void* tag)
{
	WzWidget panel = wz_hbox(parent);
	wz_widget_set_tight_constraints(panel, size.x, size.y);
	wz_widget_set_color_old(panel, EGUI_BEIGE);
	wz_widget_set_main_axis_size_min(panel);
	wz_widget_set_cross_axis_alignment(panel, WZ_CROSS_AXIS_ALIGNMENT_STRETCH);

	WzWidget clip_panel = wz_widget(panel);
	wz_widget_clip(clip_panel);
	wz_widget_set_flex(clip_panel);

	WzWidget user_panel = wz_vbox(clip_panel);
	wz_widget_set_color(user_panel, 0x00ffffff);
	wz_widget_add_tag(user_panel, tag);
	wz_widget_set_flex(user_panel);
	wz_widget_set_cross_axis_alignment(user_panel, WZ_CROSS_AXIS_ALIGNMENT_STRETCH);

	WzWidget c = wz_widget(user_panel);
	wz_widget_set_max_constraint_h(c, 150);
	wz_widget_set_color(c, 0x0000ffff);

	WzWidget scrollbar_panel = wz_vbox(panel);
	wz_widget_set_max_constraint_w(scrollbar_panel, SCROLLBAR_SIZE + 4);
	wz_widget_set_color(scrollbar_panel, 0xFF00FFFF);
	wz_widget_set_cross_axis_alignment(scrollbar_panel, WZ_CROSS_AXIS_ALIGNMENT_STRETCH);

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

	WzScrollbar* wz_scrollbar = &gui->scrollbars[gui->scrollbars_count];
	wz_scrollbar->content = user_panel;
	wz_scrollbar->scrollbar = middle_panel;
	wz_scrollbar->panel = panel;
	wz_scrollbar->top_button = top_button;
	wz_scrollbar->bottom_button = bottom_button;
	wz_scrollbar->scroll = scroll;
	gui->scrollbars_count++;

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
	if (gui->enable_input)
	{
		wzrd_handle_input(gui->boxes_indices, MAX_NUM_WIDGETS);
		wzrd_handle_cursor();
		wzrd_handle_border_resize();
	}
	else
	{
		if (gui->mouse_left == WZ_DEACTIVATING)
		{
			//canvas->released_item = canvas->dragged_item;
			//canvas->released_item = canvas->dragged_item;
			gui->dragged_box = (WzWidgetData){ 0 };
			gui->dragged_item = (WzWidget){ 0 };
			gui->active_item = (WzWidget){ 0 };
		}
	}

	gui->previous_mouse_pos = gui->mouse_pos;
}

#define WZ_TREE_ROW_HEIGHT 32
#define WZ_TREE_BOX_SIZE 32

void wz_widget_ignore_unique_id(WzWidget widget)
{
	wz_widget_get(widget)->ignore_unique_id = true;
}

void wz_widget_add_horizontal_dotted_line(WzWidget widget, unsigned w)
{
	WzWidgetItem item;
	item.type = ItemType_HorizontalDottedLine;
	item.size.x = 20;
	item.margin_bottom = item.margin_top = item.margin_left = item.margin_right = 0;
	item.center_h = item.center_w = true;
	item.w = w;
	item.h = 1;
	wz_widget_add_item(widget, item);
}

void wz_widget_add_vertical_dotted_line(WzWidget widget, unsigned h)
{
	WzWidgetItem item;
	item.type = ItemType_VerticalDottedLine;
	item.margin_bottom = item.margin_top = item.margin_left = item.margin_right = 0;
	item.center_h = item.center_w = true;
	item.w = 1;
	item.h = h;
	wz_widget_add_item(widget, item);
}

void wz_widget_add_horizontal_line(WzWidget widget, unsigned w)
{
	WzWidgetItem item;
	item.type = ItemType_HorizontalLine;
	item.size.x = 20;
	item.margin_bottom = item.margin_top = item.margin_left = item.margin_right = 0;
	item.center_h = item.center_w = true;
	item.w = w;
	item.h = 1;
	wz_widget_add_item(widget, item);
}

void wz_widget_add_vertical_line(WzWidget widget, unsigned h)
{
	WzWidgetItem item;
	item.type = ItemType_VerticalLine;
	item.margin_bottom = item.margin_top = item.margin_left = item.margin_right = 0;
	item.center_h = item.center_w = true;
	item.w = 1;
	item.h = h;
	wz_widget_add_item(widget, item);
}

void wz_widget_add_line_absolute(WzWidget widget, int x0, int y0, int x1, int y1)
{
	WzWidgetItem item;
	item.type = ItemType_LineAbsolute;
	item.margin_bottom = item.margin_top = item.margin_left = item.margin_right = 0;
	item.center_h = item.center_w = true;
	item.val.line.x0 = x0;
	item.val.line.y0 = y0;
	item.val.line.x1 = x1;
	item.val.line.y1 = y1;
	wz_widget_add_item(widget, item);
}

void wz_widget_add_dotted_line_absolute(WzWidget widget, int x0, int y0, int x1, int y1)
{
	WzWidgetItem item;
	item.type = ItemType_DottedLineAbsolute;
	item.margin_bottom = item.margin_top = item.margin_left = item.margin_right = 0;
	item.center_h = item.center_w = true;
	item.val.line.x0 = x0;
	item.val.line.y0 = y0;
	item.val.line.x1 = x1;
	item.val.line.y1 = y1;
	wz_widget_add_item(widget, item);
}

void wz_widget_add_horizontal_line_absolute(WzWidget widget, unsigned h)
{
	WzWidgetItem item;
	item.type = ItemType_HorizontalLineAbsolute;
	item.margin_bottom = item.margin_top = item.margin_left = item.margin_right = 0;
	item.center_h = item.center_w = true;
	item.w = 1;
	item.h = h;
	wz_widget_add_item(widget, item);
}

void wz_widget_add_content_margins(WzWidget widget, unsigned left, unsigned right, unsigned top, unsigned bottom)
{
	WzWidgetData* d = wz_widget_get(widget);
	d->constraint_max_h += top + bottom;
	d->constraint_max_w += left + right;
}

void wz_widget_set_cross_axis_alignment(WzWidget widget, unsigned int cross_axis_alignment)
{
	wz_widget_get(widget)->cross_axis_alignment = cross_axis_alignment;
}

void wz_widget_set_main_axis_alignment(WzWidget widget, unsigned int main_axis_alignment)
{
	wz_widget_get(widget)->main_axis_alignment = main_axis_alignment;
}

void wz_widget_add_content_margin_left(WzWidget widget, unsigned left)
{
	WzWidgetData* d = wz_widget_get(widget);
	d->constraint_max_w += left;
}

void wz_widget_add_content_margin_right(WzWidget widget, unsigned right)
{
	WzWidgetData* d = wz_widget_get(widget);
	d->constraint_max_w += right;
}

void wz_widget_get_all_children(WzWidget widget, WzWidget* children, unsigned* children_count)
{
	WzWidget stack[1024];
	int stack_count = 0;
	*children_count = 0;
	stack[stack_count++] = widget;

	while (stack_count > 0)
	{
		WzWidgetData* box = wz_widget_get(stack[stack_count - 1]);
		stack_count--;

		for (int i = 0; i < box->children_count; ++i)
		{
			WzWidget child = gui->widgets[box->children[i].handle].handle;
			stack[stack_count++] = child;
			children[*children_count] = child;
			*children_count = *children_count + 1;
		}
	}
}

WzWidget wz_tree_add_row_raw(WzTree* tree, WzStr str, WzTexture texture, unsigned depth,
	bool* expand, bool* selected, WzTreeNodeData* node, const char* file, unsigned line)
{
	const toggle_size = 20;
	const icon_size = 20;

	WzWidget row = wz_hbox(tree->menu);
	wz_widget_add_source(row, file, line);
	wz_widget_set_cross_axis_alignment(row, WZ_CROSS_AXIS_ALIGNMENT_CENTER);
	wz_widget_set_margins(row, 5);

	WzWidget box = wz_widget(row);
	wz_widget_set_max_constraints(box, depth * (icon_size + 10), icon_size);
	wz_widget_ignore_unique_id(box);

	WzWidget toggle;
	if (node->children_count)
	{
		toggle = wz_toggle(row, toggle_size, toggle_size, (WzColor) { 0 }, expand);
		wz_widget_add_source(toggle, file, line);
		wz_widget_add_rect(toggle, toggle_size - 2, toggle_size - 2, WZ_WHITE);
		wz_widget_set_border(toggle, WZ_BORDER_TYPE_BLACK);
	}
	else
	{
		toggle = wz_widget(row);
		wz_widget_set_tight_constraints(toggle, toggle_size, toggle_size);
		wz_widget_add_source(toggle, file, line);
	}

	WzWidget hline = wz_widget(row);
	wz_widget_set_max_constraints(hline, 10, 10);
	wz_widget_add_source(hline, file, line);

	WzWidget icon = wz_texture(row, texture, icon_size, icon_size);
	wz_widget_add_source(icon, file, line);

	WzWidget label = wz_label(row, str);
	wz_widget_add_source(label, file, line);
	wz_widget_add_content_margin_left(label, 5);
	wz_widget_add_content_margin_right(label, 5);

	WzWidget children[16];
	unsigned children_count;
	wz_widget_get_all_children(row, children, &children_count);

	*selected = false;
	*selected |= wz_widget_is_deactivating(row);

	for (unsigned i = 0; i < children_count; ++i)
	{
		if (wz_widget_is_deactivating(children[i]))
		{
			*selected |= true;
		}
	}

	node->row_widget = row;
	node->icon_widget = icon;
	node->expand_widget = toggle;

	return row;
}

WzTreeNodeData* wz_tree_get_node(WzTree* tree, WzTreeNode node)
{
	return &tree->nodes[node.index];
}

void wz_tree_node_get_children(WzTree* tree, WzTreeNode node, WzTreeNodeData** children,
	unsigned* children_count)
{
	WzTreeNodeData* n = wz_tree_get_node(tree, node);
	*children = &tree->children_indices[n->children_index];
	*children_count = tree->children_count;
}

void wz_do_layout_refactor_me()
{
	unsigned int layout_failed = 0;

	wz_do_layout(1, gui->widgets, gui->rects, MAX_NUM_WIDGETS, &layout_failed);
}

void wz_gui_end(WzStr* debug_str)
{
	unsigned int widgets_stack[MAX_NUM_WIDGETS];
	unsigned int widgets_visits[MAX_NUM_WIDGETS];

	wz_do_layout_refactor_me();

	// Check all ids are unique
	// WARNING O(N^2)
	for (unsigned int i = 0; i < MAX_NUM_WIDGETS; ++i)
	{
		for (unsigned int j = 0; j < MAX_NUM_WIDGETS; ++j)
		{
			if (i != j)
			{
				if (!gui->widgets[j].ignore_unique_id)
				{
					//wz_assert(!wz_widget_is_equal(gui->widgets[i].handle, gui->widgets[j].handle));
				}
			}
		}
	}

	// Second pass: handle scrollbars
	for (int i = 0; i < gui->scrollbars_count; ++i)
	{
		WzScrollbar scrollbar = gui->scrollbars[i];
		WzWidgetData* content_panel = wz_widget_get(scrollbar.content);
		unsigned int content_panel_h = content_panel->actual_h;
		unsigned int content_h = 0;

		for (int i = 0; i < content_panel->children_count; ++i)
		{
			content_h += gui->widgets[content_panel->children[i].handle].actual_h;
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
		WzWidgetData* scrollbar_data = wz_widget_get(gui->scrollbars[i].scrollbar);
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

	// Cull
	for (unsigned int i = 1; i < MAX_NUM_WIDGETS; ++i)
	{
		WzWidgetData* widget = &gui->widgets[i];
		WzWidgetData* parent = wz_widget_get_parent(gui->widgets[i].handle);
		if (parent->actual_h < parent->margin_bottom + parent->margin_top || parent->actual_w < parent->margin_left + parent->margin_right)
		{
			widget->cull = true;
		}
		if (!widget->actual_w || !widget->actual_h)
		{
			widget->cull = true;
		}
		if (widget->actual_x > parent->actual_x + parent->actual_w - parent->margin_right - parent->margin_left)
		{
			widget->cull = true;
		}
		if (widget->actual_y > parent->actual_y + parent->actual_h - parent->margin_bottom - parent->margin_top)
		{
			widget->cull = true;
		}
		if (parent->cull)
		{
			widget->cull = true;
		}
	}

	// Cache tagged elements
	gui->cached_boxes_count = 0;
	for (int i = 1; i < MAX_NUM_WIDGETS; ++i)
	{
		assert(gui->cached_boxes_count < MAX_NUM_CACHED_BOXES - 1);
		WzWidgetData* box = &gui->widgets[i];
		if (box->tag)
		{
			gui->cached_boxes[gui->cached_boxes_count++] = *box;
		}
	}

	// Sort
	for (int i = 1; i < MAX_NUM_WIDGETS; ++i)
	{
		gui->boxes_indices[i] = i;
	}

	qsort(gui->boxes_indices + 1, MAX_NUM_WIDGETS - 1, sizeof(int), wzrd_compare_boxes);

	wz_handle_input();

	// Handle trees
	WzTree* tree = &gui->trees[0];

	for (unsigned i = 0; i < tree->nodes_count; ++i)
	{
		WzTreeNodeData* node = &tree->nodes[i];

		if (wz_widget_is_equal(node->row_widget, tree->selected_row))
		{
			WzWidgetData* data = wz_widget_get(node->row_widget);
			wz_widget_add_rect_absolute(tree->menu, data->actual_x, data->actual_y,
				data->actual_w, data->actual_h, EGUI_DARKGREEN);
		}
	}

	for (unsigned i = 0; i < tree->nodes_count; ++i)
	{
		WzTreeNodeData* node = &tree->nodes[i];

		if (node->visible)
		{
			WzWidgetData* icon_widget = wz_widget_get(node->icon_widget);
			WzWidgetData* expand_widget = wz_widget_get(node->expand_widget);

			int x0 = icon_widget->actual_x + icon_widget->actual_w / 2;
			int y0 = icon_widget->actual_y + icon_widget->actual_h / 2;
			int x1 = expand_widget->actual_x + expand_widget->actual_w / 2;
			int y1 = expand_widget->actual_y + expand_widget->actual_h / 2;

			wz_widget_add_dotted_line_absolute(tree->menu, x0, y0, x1, y1);

			unsigned children_count = node->children_count;

			if (children_count)
			{
				if (node->expand)
				{
					WzTreeNode* node_children = &tree->children_indices[tree->nodes[i].children_index];

					WzTreeNodeData* child_first = &tree->nodes[tree->children_indices[0].index];
					WzTreeNodeData* child_last = &tree->nodes[tree->children_indices[children_count - 1].index];
					WzWidgetData* child_expand_widget = wz_widget_get(child_last->expand_widget);

					x1 = child_expand_widget->actual_x + child_expand_widget->actual_w / 2;
					y1 = child_expand_widget->actual_y + child_expand_widget->actual_h / 2;

					wz_widget_add_dotted_line_absolute(tree->menu, x0, y0, x1, y1);
				}

				unsigned size = expand_widget->actual_w / 2;
				wz_widget_add_line_absolute(expand_widget->handle,
					expand_widget->actual_x + (expand_widget->actual_w - size) / 2,
					expand_widget->actual_y + expand_widget->actual_h / 2,
					expand_widget->actual_x + (expand_widget->actual_w - size) / 2 + size,
					expand_widget->actual_y + expand_widget->actual_h / 2);

				if (node->expand)
				{
					wz_widget_add_line_absolute(expand_widget->handle,
						expand_widget->actual_x + expand_widget->actual_w / 2,
						expand_widget->actual_y + (expand_widget->actual_h - size) / 2,
						expand_widget->actual_x + expand_widget->actual_w / 2,
						expand_widget->actual_y + (expand_widget->actual_h - size) / 2 + size);
				}
			}
		}
	}

	wz_draw(gui->boxes_indices);

	// Debug string
	{
#if 1

		WzWidgetData* widget = wz_widget_get(gui->hovered_item);
		char line_str[128];
		sprintf(line_str, "%s %u", widget->source, widget->handle.handle);
		int w = 0, h = 0;
		if (gui->get_string_size)
		{
			gui->get_string_size(line_str, &w, &h);
		}

		WzRect rect = {
			.x = gui->mouse_pos.x + 20,
				.y = gui->mouse_pos.y + 20,
				.w = (unsigned int)w,
				.h = (unsigned int)h,
		};

		wz_draw_rect(&gui->commands_buffer, rect,
			(WzColor) {
			255, 0, 0, 100
		},
			3, widget->source);

		wz_draw_string(line_str, rect);
#endif
	}
}

WzGui* wzrd_canvas_get()
{
	return gui;
}

WzWidget wz_toggle_raw(WzWidget parent, unsigned w, unsigned h, WzColor color,
	bool* active, const char* file_name, unsigned int line)
{
	bool b = false;
	WzWidget handle = wz_widget_raw(parent, file_name, line);

	wz_widget_add_source(handle, file_name, line);
	wz_widget_set_max_constraints(handle, w, h);
	wz_widget_set_color_old(handle, color);

	if (wz_widget_is_deactivating(handle))
	{
		*active = !*active;
	}

	if (*active)
	{
		//wz_widget_get(widget)->border_type = WZ_BORDER_TYPE_CLICKED;
	}

	return handle;
}

WzWidget wz_icon_toggle_raw(WzWidget parent, WzTexture texture, unsigned w, unsigned h, bool* active,
	const char* file_name, unsigned int line) {
	WzWidget handle = wz_widget(parent, file_name, line);
	wz_widget_add_source(handle, file_name, line);
	wz_widget_set_max_constraints(handle, w, h);
	wz_widget_add_texture(handle, texture, w, h);


	if (wz_widget_is_deactivating(handle))
	{
		*active = !*active;
	}

	if (*active)
	{
		//wz_widget_get(widget)->border_type = WZ_BORDER_TYPE_CLICKED;
	}

	return handle;
}

WzWidget wz_texture_raw(WzWidget parent, WzTexture texture,
	unsigned w, unsigned h, const char* file_name, unsigned int line)
{
	WzWidget handle = wz_widget(parent, file_name, line);
	wz_widget_add_source(handle, file_name, line);
	wz_widget_set_tight_constraints(handle, w, h);
	wz_widget_add_texture(handle, texture, w, h);

	return handle;
}

WzWidget wz_button_icon_raw(WzWidget parent, bool* result, WzTexture texture,
	const char* file, unsigned int line) {
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

	const int icon_size = 32;
	WzWidget icon = wz_texture(parent, texture, icon_size, icon_size);
	wz_widget_add_source(icon, file, line);
	wz_widget_set_border(icon, WZ_BORDER_TYPE_DEFAULT);

	*result = wz_widget_is_deactivating(icon);

	if (wz_widget_is_active(icon) || wz_widget_is_activating(icon))
	{
		wz_widget_set_border(icon, WZ_BORDER_TYPE_CLICKED);
	}

	return icon;
#endif
}

WzWidget wz_command_toggle_raw(WzWidget parent, WzStr str, bool* active,
	const char* file_name, unsigned int line)
{
	WzWidget widget = wz_label_raw(parent, str, file_name, line);
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
	if (wz_widget_is_equal(h, gui->activating_item)) {
		*b = true;
	}
	else
	{
		*b = false;
	}

	return h;
}

WzWidget wzrd_label_button_activating_raw(WzStr str, bool* active, WzWidget parent, const char* file, unsigned int line)
{
	WzWidget h = wz_label_raw(parent, str, file, line);

	//if (wzrd_handle_is_valid(h))
	if (wz_widget_is_equal(h, gui->activating_item)) {
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

void wz_widget_add_item(WzWidget widget, WzWidgetItem item)
{
	WzWidgetData* b = wz_widget_get(widget);
	wz_assert(b->items_count < MAX_NUM_ITEMS - 1);
	b->items[b->items_count++] = item;
}

WzWidget wz_label_raw(WzWidget handle, WzStr str, const char* file, unsigned int line)
{
	int w = 0, h = 0;
	gui->get_string_size(str.str, &w, &h);

	WzWidget parent = wz_widget_raw(handle, file, line);
	wz_widget_set_tight_constraints(parent, w, h);
	wz_widget_add_text(parent, str);
	wz_widget_set_color(parent, 0);
	wz_widget_set_border(parent, WZ_BORDER_TYPE_NONE);

	return parent;
}

WzWidget wzrd_input_box_raw(char* str, int* len, int max_num_keys, WzWidget handle, const char* file, unsigned int line) {
	WzWidget parent = wz_widget_raw(handle, file, line);
	wz_widget_set_expanded(parent);
	wz_widget_set_max_constraint_h(parent, 20);
	//wzrd_box_set_type(p1, wzrd_box_type_input_box);

#if 1
			//wzrd_str str2 = *str;

			//if (wzrd_handle_is_equal(wzrd_box_get_from_top_of_stack()->handle, canvas->active_input_box)) {
	{
		//wzrd_style_template style = wzrd_style_get(wzrd_box_get_last()->style);
		//style.color = (wzrd_color){ 255, 230, 230, 255 };
		//wzrd_box_get_last()->style = wzrd_style_create(style);

		for (int i = 0; i < gui->keyboard_keys.count; ++i) {
			wzrd_keyboard_key key = gui->keyboard_keys.keys[i];

			if (key.val == '\b' &&
				*len > 0 &&
				(key.state == WZ_ACTIVE || key.state == WZ_ACTIVATING)) {
				str[*len - 1] = 0;
				*len = *len - 1;
			}
			else if ((key.state == WZ_ACTIVATING || key.state == WZ_ACTIVE) &&
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
	wz_widget_add_text(parent, wz_str_create(str));
#endif

	return parent;
}

WzStr wz_str_create(const char* str)
{
	WzStr result =
	{
		.len = strlen(str)
	};

	strcpy(result.str, str);

	return result;
}

WzWidget wzrd_label_button_raw(WzStr str, bool* result, WzWidget handle,
	const char* file, unsigned  int line) {
	int w = 0, h = 0;
	gui->get_string_size(str.str, &w, &h);

	WzWidget parent = wz_label_raw(handle, str, file, line);

	wz_widget_set_max_constraint_w(parent, w);
	wz_widget_set_max_constraint_h(parent, h);

	if (wz_widget_is_activating(parent))
	{
		*result = true;
	}

	return parent;
}

void wz_spacer(WzWidget parent)
{
	WzWidget spacer = wz_widget(parent);
	wz_widget_set_color(spacer, 0);
	wz_widget_set_border(spacer, WZ_BORDER_TYPE_NONE);
	wz_widget_set_flex(spacer);
	wz_widget_set_tight_constraints(spacer, 0, 0);
}

void wz_widget_set_font_color(WzWidget widget, unsigned color)
{
	WzColor color_new = {
			(0xff000000 & color) >> 24,
			(0x00ff0000 & color) >> 16,
			(0x0000ff00 & color) >> 8,
			(0x000000ff & color) >> 0 };

	wz_widget_get(widget)->font_color = color_new;
}

WzWidget wzrd_dialog_begin_raw(int* x, int* y, unsigned* w, unsigned* h,
	bool* active, WzStr name_str, int layer,
	WzWidget parent, const char* file, unsigned int line)
{

	*active = true;

	WzWidget window = wz_vbox(parent);
	wz_widget_set_free_from_parent(window);
	wz_widget_set_tight_constraints(window, *w, *h);
	wz_widget_set_layer(window, layer);
	wz_widget_add_source(window, file, line);
	wz_widget_set_cross_axis_alignment(window, WZ_CROSS_AXIS_ALIGNMENT_STRETCH);

#if 1
	WzWidget bar = wz_hbox(window);
	wz_widget_set_color(bar, 0x000080FF);
	wz_widget_add_source(bar, file, line);
	wz_widget_set_border(bar, WZ_BORDER_TYPE_NONE);
	wz_widget_set_margin_left(bar, 5);
	wz_widget_set_cross_axis_alignment(bar, WZ_CROSS_AXIS_ALIGNMENT_CENTER);

	WzWidget name = wz_label(bar, name_str);
	wz_widget_set_font_color(name, 0xffffffff);

	wz_spacer(bar);
	WzWidget quit_button = wz_button_icon(bar, &active, (WzTexture) { 0 });

	if (wz_widget_is_interacting_tree(bar) && !wz_widget_is_interacting(quit_button))
	{
		*x += gui->mouse_pos.x - gui->previous_mouse_pos.x;
		*y += gui->mouse_pos.y - gui->previous_mouse_pos.y;
	}

	wz_widget_set_pos(window, *x, *y);

	WzWidget user_section = wz_vbox(window);
	wz_widget_set_border(user_section, WZ_BORDER_TYPE_NONE);
	wz_widget_set_flex(user_section);

	return user_section;
#else
	return window;
#endif

}

WzWidget wzrd_dropdown_raw(int* selected_text, const WzStr* texts, int texts_count, int w, bool* active, WzWidget handle, const char* file, unsigned int line)
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
	WzColor c = (WzColor){ (color & 0xff000000) >> 24, (color & 0x00ff0000) >> 16, (color & 0x0000ff00) >> 8, (color & 0x000000ff) >> 0 };
	wz_widget_set_color_old(widget, c);
}


void wz_widget_focus(WzWidget widget)
{
	gui->focused_widget = widget;
}

void wzrd_label_list_raw(WzStr* item_names, unsigned int count,
	unsigned int width, unsigned int height, unsigned int color,
	WzWidget* handles, unsigned int* selected, bool* is_selected,
	WzWidget parent, const char* file, unsigned int line)
{
	wz_assert(width);
	wz_assert(height);

	WzWidget panel = wz_vbox(parent);
	wz_widget_set_main_axis_size_min(panel);
	wz_widget_add_source(panel, file, line);

	if (wz_widget_is_activating(panel))
	{
		*selected = 0;
		*is_selected = false;
	}

	WzWidget selected_label = { 0 };

	for (unsigned int i = 0; i < count; ++i)
	{
		bool is_label_clicked = false;
		WzWidget wdg = wzrd_label_button_activating(item_names[i], &is_label_clicked, panel);
		wz_widget_set_max_constraint_w(wdg, width);
		wz_widget_set_max_constraint_h(wdg, height);
		wz_widget_set_color(wdg, color);
		wz_widget_add_source(wdg, file, line);
		wz_widget_add_tag(wdg, item_names[i].str);

		if (handles)
		{
			// Deleted this piece of code, what is it doing?
			//handles[i] = gui->widgets[gui->widgets_count - 2].handle;
		}

		if (is_label_clicked) {
			*selected = i;
			*is_selected = true;
			selected_label = wdg;
		}

		if (*is_selected && *selected == i)
		{
			wz_widget_get(wdg)->color = gui->stylesheet.label_item_selected_color;
		}
	}
}

WzWidget wz_vpanel_raw(WzWidget parent, const char* file, unsigned int line)
{
	WzWidget widget = wz_vbox_raw(parent, file, line);
	wz_widget_set_margins(widget, 5);
	wz_widget_set_child_gap(widget, 5);
	wz_widget_set_border(widget, WZ_BORDER_TYPE_DEFAULT);
	wz_widget_set_cross_axis_alignment(widget, WZ_CROSS_AXIS_ALIGNMENT_CENTER);
	wz_widget_set_color_old(widget, EGUI_LIGHTGRAY);

	return widget;
}

WzWidget wz_hpanel_raw(WzWidget parent, const char* file, unsigned int line)
{
	WzWidget widget = wz_hbox_raw(parent, file, line);
	wz_widget_set_margins(widget, 5);
	wz_widget_set_child_gap(widget, 5);
	wz_widget_set_border(widget, WZ_BORDER_TYPE_DEFAULT);
	wz_widget_set_cross_axis_alignment(widget, WZ_CROSS_AXIS_ALIGNMENT_CENTER);
	wz_widget_set_color_old(widget, EGUI_LIGHTGRAY);

	return widget;
}

WzWidget wz_panel_raw(WzWidget parent, const char* file, unsigned int line)
{
	WzWidget widget = wz_widget(parent, file, line);
	wz_widget_set_border(widget, WZ_BORDER_TYPE_DEFAULT);
	wz_widget_set_cross_axis_alignment(widget, WZ_CROSS_AXIS_ALIGNMENT_CENTER);
	wz_widget_set_color_old(widget, EGUI_LIGHTGRAY);

	return widget;
}

void wzrd_label_list_sorted_raw(WzStr* item_names, unsigned int count, int* items,
	unsigned int width, unsigned int height, unsigned int color, unsigned int* selected,
	bool* is_selected, WzWidget parent, const char* file_name, unsigned int line)
{
	WzWidget handles[MAX_NUM_LABELS] = { 0 };

	wzrd_label_list_raw(item_names, count, width, height,
		color, handles, selected, is_selected, parent, file_name, line);

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
		for (int i = 0; i < gui->hovered_boxes_count; ++i)
		{
			if (wz_widget_is_equal(gui->hovered_boxes[i].handle, hovered_label))
			{
				hovered_parent = gui->hovered_boxes + i;
				break;
			}
		}

		// Set hover position
		if (hovered_parent)
		{

			if (gui->mouse_pos.y > hovered_parent->y + hovered_parent->actual_h / 2)
			{
				is_bottom = true;
			}

			// Label grabbed and hovering over another one
			if (wz_handle_is_valid(active_label) && wz_handle_is_valid(hovered_label) && !wz_widget_is_equal(hovered_label, active_label))
			{
				wz_widget_set_color(hovered_label, 0xFF00FFFF);
				WzWidgetData* p = wz_widget_get(hovered_label);
				WzWidgetData* c = 0;
				if (is_bottom)
				{
					WzWidget cc = wz_widget(p->handle);
					c = wz_widget_get(cc);
					wz_widget_add_source(c->handle, __FILE__, __LINE__);
					wz_widget_set_y(c->handle, hovered_parent->actual_h - 2);
					wz_widget_set_max_constraint_h(c->handle, 2);
					wz_widget_set_free_from_parent(c->handle);
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

typedef struct
{
	WzWidgetData widgets[MAX_NUM_WIDGETS];
} WzBinary;


void save_widgets(WzGui* gui)
{
	FILE* file = fopen("C:\\Users\\Elior\\source\\repos\\AdventureWizard\\Gui", "wb");
	wz_assert(file);

	for (unsigned i = 0; i < MAX_NUM_PERSISTENT_WIDGETS; ++i)
	{
		WzWidgetData* widget = &gui->persistent_widgets[i];
		fwrite(widget, sizeof(WzWidgetData), 1, file);
	}
}

void load_widgets(WzGui* gui)
{
	FILE* file = fopen("C:\\Users\\Elior\\source\\repos\\AdventureWizard\\Gui", "rb");
	if (!file) return;

	for (unsigned i = 0; i < MAX_NUM_WIDGETS; ++i)
	{
		WzWidgetData widget;
		fread(&widget, sizeof(WzWidgetData), 1, file);
		if (widget.handle.handle)
		{
			gui->widgets[widget.handle.handle] = widget;
			gui->persistent_widgets[gui->persistent_widgets_count++] = widget;
		}
	}
}

WzWidget wzrd_handle_button_raw(bool* active, WzRect rect,
	WzColor color, WzStr name, WzWidget handle, const char* file_name, unsigned int line)
{
	WzWidget parent = wz_widget(handle);
	wz_widget_set_free_from_parent(parent);
	wz_widget_add_source(parent, file_name, line);
	wz_widget_set_pos(parent, rect.x, rect.y);
	wz_widget_set_tight_constraints(parent, rect.w, rect.h);
	wz_widget_set_color_old(parent, color);
	*active = wzrd_box_is_active(wz_widget_get(parent));

	return parent;
}

void wzrd_drag(bool* drag) {

	if (gui->mouse_left == WZ_INACTIVE)
	{
		*drag = false;
	}

	if (!(*drag)) return;

	gui->dragged_box.x += gui->mouse_delta.x;
	gui->dragged_box.y += gui->mouse_delta.y;

	wzrd_crate(1, gui->dragged_box);
}

bool wzrd_box_is_active(WzWidgetData* box) {
	if (wz_widget_is_equal(box->handle, gui->active_item)) {
		return true;
	}

	return false;
}

bool wzrd_box_is_dragged(WzWidgetData* box) {
	if (wz_widget_is_equal(box->handle, gui->dragged_item)) {
		return true;
	}

	return false;
}

bool wzrd_box_is_hot_using_canvas(WzGui* c, WzWidgetData* box) {
	if (wz_widget_is_equal(box->handle, c->hovered_item)) {
		return true;
	}

	return false;
}

bool wzrd_box_is_hot(WzWidgetData* box) {
	if (wz_widget_is_equal(box->handle, gui->hovered_item)) {
		return true;
	}

	return false;
}


WzWidgetData* wzrd_box_get_released()
{
	WzWidgetData* result = 0;

	if (wz_handle_is_valid(gui->deactivating_item))
	{
		result = wz_widget_get(gui->deactivating_item);
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

WzWidget wz_command_button_raw(WzStr str, bool* released, WzWidget parent, const char* file_name, unsigned int line)
{
	WzWidget button = wz_label_raw(parent, str, file_name, line);
	wz_widget_set_border(button, WZ_BORDER_TYPE_DEFAULT);

	*released = wz_widget_is_deactivating(button);

	if (wz_widget_is_active(button) || wz_widget_is_activating(button))
	{
		wz_widget_set_border(button, WZ_BORDER_TYPE_CLICKED);
	}

	return button;
}

void wz_widget_set_layer(WzWidget handle, unsigned int layer)
{
	wz_widget_get(handle)->layer = layer;
}


#include "WzLayout.h"

//#define wz_log(x) (void)x;

unsigned int wz_layout_failed;

void wz_log(WzLogMessage* arr, unsigned int* count, const char* fmt, ...)
{
	WzLogMessage message = { 0 };
	message.str[0] = 0;
	va_list args;
	va_start(args, fmt);
	vsprintf_s(message.str, WZ_LOG_MESSAGE_MAX_SIZE, fmt, args);
	va_end(args);

	arr[*count] = message;
	*count = *count + 1;

	//printf("%s", message.str);
}

#define STACK_MAX_DEPTH 64

void wz_do_layout(unsigned int index,
	WzWidgetData* widgets, WzLayoutRect* rects,
	unsigned int count, unsigned int* failed)
{
	wz_assert(count);

	wz_layout_failed = 0;

	unsigned int widgets_stack_count = 0;

	unsigned int size_per_flex_factor;
	WzWidgetData* widget;
	WzWidgetData* child;

	unsigned int constraint_max_w, constraint_max_h;

	unsigned int w;
	unsigned int h;
	unsigned int available_size_main_axis, available_size_cross_axis;
	int i;
	unsigned int children_flex_factor;
	unsigned int children_h, max_child_w;
	unsigned int parent_index;

	unsigned int* widgets_visits = calloc(sizeof(*widgets_visits), count);
	unsigned int* widgets_stack = calloc(sizeof(*widgets_stack), STACK_MAX_DEPTH);

	WzLogMessage* log_messages = malloc(sizeof(*log_messages) * count * 20);
	unsigned int log_messages_count = 0;

	widgets_stack[widgets_stack_count++] = index;

	unsigned int* constraint_min_main_axis, * constraint_max_main_axis,
		* constraint_min_cross_axis = 0, * constraint_max_cross_axis = 0,
		* actual_size_main_axis, * actual_size_cross_axis;

	unsigned int* child_constraint_min_main_axis, * child_constraint_max_main_axis,
		* child_constraint_min_cross_axis, * child_constraint_max_cross_axis,
		* child_actual_size_main_axis, * child_cross_axis_actual_size;

	unsigned int screen_size_main_axis, screen_size_cross_axis;

	unsigned int parent_cross_axis_size;

	unsigned int margins_cross_axis;
	unsigned int margins_main_axis;
	//WzLayoutRect* child_rect, * widget_rect;
	unsigned int root_w, root_h;

	root_w = widgets[index].constraint_max_w;
	root_h = widgets[index].constraint_max_h;

	// Constraints pass
	while (widgets_stack_count)
	{
		unsigned int children_size, max_child_h;

		parent_index = widgets_stack[widgets_stack_count - 1];
		widget = &widgets[parent_index];
		//widget_rect = &rects[parent_index];

		//wz_assert(widget->constraint_max_w >= widget->constraint_min_w);
		//wz_assert(widget->constraint_max_h >= widget->constraint_min_h);

		if (!widget->children_count)
		{
			// Size leaf widgets, and pop immediately
			// For now all leaf widgets must have a finite constraint
			// Later on we'll let them decide their own size based on their content
			if (widget->constraint_max_w == WZ_UINT_MAX)
			{
				wz_log(log_messages, &log_messages_count, "(%s) ERROR: Widget width has unbounded constraints\n",
					widget->source);
			}

			if (widget->constraint_max_h == WZ_UINT_MAX)
			{
				wz_log(log_messages, &log_messages_count, "(%s) ERROR: Widget height has unbounded constraints\n",
					widget->source);
			}

			widget->actual_x = 0;
			widget->actual_y = 0;
			widget->actual_w = widget->constraint_max_w;
			widget->actual_h = widget->constraint_max_h;

			if (!widget->actual_w)
			{
				wz_log(log_messages, &log_messages_count, "(%s) ERROR: Widget width has no constraints\n",
					widget->source);
			}
			if (!widget->actual_h)
			{
				wz_log(log_messages, &log_messages_count,
					"(%s) ERROR: Widget height has no constraints\n",
					widget->source);
			}

			wz_assert(widget->actual_w <= widget->constraint_max_w);
			wz_assert(widget->actual_h <= widget->constraint_max_h);

			wz_log(log_messages, &log_messages_count,
				"(%s) LOG: Leaf widget with constraints (%u %u %u, %u) determined its size (%u, %u)\n",
				widget->source,
				widget->constraint_min_w, widget->constraint_min_h,
				widget->constraint_max_w, widget->constraint_max_h,
				widget->actual_w, widget->actual_h);

			widgets_stack_count--;
		}
		else
		{
			// Handle widgets with children
			if (widget->layout == WZ_LAYOUT_HORIZONTAL || widget->layout == WZ_LAYOUT_VERTICAL)
			{
				if (widget->layout == WZ_LAYOUT_HORIZONTAL)
				{
					constraint_min_main_axis = &widget->constraint_min_w;
					constraint_max_main_axis = &widget->constraint_max_w;
					constraint_min_cross_axis = &widget->constraint_min_h;
					constraint_max_cross_axis = &widget->constraint_max_h;
					actual_size_main_axis = &widget->actual_w;
					actual_size_cross_axis = &widget->actual_h;
					screen_size_main_axis = root_w;
					screen_size_cross_axis = root_h;
					margins_cross_axis = widget->margin_top + widget->margin_bottom;
					margins_main_axis = widget->margin_left + widget->margin_right;
				}
				else if (widget->layout == WZ_LAYOUT_VERTICAL)
				{
					constraint_min_main_axis = &widget->constraint_min_h;
					constraint_max_main_axis = &widget->constraint_max_h;
					constraint_min_cross_axis = &widget->constraint_min_w;
					constraint_max_cross_axis = &widget->constraint_max_w;
					actual_size_main_axis = &widget->actual_h;
					actual_size_cross_axis = &widget->actual_w;
					screen_size_main_axis = root_h;
					screen_size_cross_axis = root_w;
					margins_main_axis = widget->margin_top + widget->margin_bottom;
					margins_cross_axis = widget->margin_left + widget->margin_right;
				}
				else
				{
					child_constraint_min_main_axis = child_constraint_max_main_axis =
						child_constraint_max_cross_axis = child_constraint_min_cross_axis = 0;
					constraint_max_main_axis = 0;
					actual_size_main_axis = actual_size_cross_axis = 0;

					wz_assert(0);
				}

				// You got 3 visits for layout widget.
				// 1. Non Flex children get fixed constraints
				// 2. Above children determine their desired size, and now we allocate available space to flex children
				// 3. It's the turn of the flex children to determine their size, and then we can finally assess the 
				// Layout widget's size using it's children's 
				if (widgets_visits[parent_index] == WZ_LAYOUT_STAGE_NON_FLEX_CHILDREN)
				{
					// First give cross axis constraints to all children
					const char* widget_type = 0;
					if (widget->layout == WZ_LAYOUT_HORIZONTAL)
					{
						widget_type = "Row";
					}
					else if (widget->layout == WZ_LAYOUT_VERTICAL)
					{
						widget_type = "Column";
					}

					wz_log(log_messages, &log_messages_count,
						"(%s) LOG: %s with constraints (main %u, cross %u) begins allocating cross axis constraints to children\n",
						widget->source, widget_type,
						*constraint_max_main_axis, *constraint_max_cross_axis);


					for (int i = 0; i < widget->children_count; ++i)
					{
						child = &widgets[widget->children[i].handle];

						if (child->free_from_parent)
						{
							continue;
						}

						if (widget->layout == WZ_LAYOUT_HORIZONTAL)
						{
							child_constraint_min_cross_axis = &child->constraint_min_h;
							child_constraint_max_cross_axis = &child->constraint_max_h;
						}
						else if (widget->layout == WZ_LAYOUT_VERTICAL)
						{
							child_constraint_min_cross_axis = &child->constraint_min_w;
							child_constraint_max_cross_axis = &child->constraint_max_w;
						}
						else
						{
							wz_assert(0);
							child_constraint_min_cross_axis = 0;
							child_constraint_max_cross_axis = 0;
						}

						// Cross axis constraints
						if (*constraint_max_cross_axis > margins_cross_axis)
						{
							// Clamp cross axis constraints
							unsigned int parent_max_cross_axis_constraints = *constraint_max_cross_axis - margins_cross_axis;
							unsigned int parent_min_cross_axis_constraints = *constraint_min_cross_axis - margins_cross_axis;

							if (parent_max_cross_axis_constraints < *child_constraint_max_cross_axis)
							{
								*child_constraint_max_cross_axis = parent_max_cross_axis_constraints;
							}

							if (parent_min_cross_axis_constraints > *child_constraint_min_cross_axis)
							{
								*child_constraint_min_cross_axis = parent_min_cross_axis_constraints;
							}

							if (widget->cross_axis_alignment == WZ_CROSS_AXIS_ALIGNMENT_STRETCH)
							{
								*child_constraint_min_cross_axis = *child_constraint_max_cross_axis = parent_max_cross_axis_constraints;
							}

							wz_log(log_messages, &log_messages_count,
								"(%s) LOG: Widget recieved cross %u\n",
								child->source, *child_constraint_max_cross_axis);
						}
						else
						{
							*child_constraint_max_cross_axis = 0;
							wz_log(log_messages, &log_messages_count,
								"(%s) ERROR: Flex widget had no space available to it \n",
								child->source);
						}
					}

					wz_log(log_messages, &log_messages_count,
						"(%s) LOG: %s with constraints (main %u, cross %u) ends allocating cross axis constraints to children\n",
						widget->source, widget_type,
						*constraint_max_main_axis, *constraint_max_cross_axis);

					// Give constraints to non flex children
					// A child with flex factor 0 recieves unbounded constraints in the main axis
					wz_log(log_messages, &log_messages_count,
						"(%s) LOG: %s  with constraints (main %u, cross %u) begins constrains non-flex children\n",
						widget->source, widget_type,
						*constraint_max_main_axis, *constraint_max_cross_axis);

					// We leave a non-flex constraints unchanged
					for (int i = 0; i < widget->children_count; ++i)
					{
						child = &widgets[widget->children[i].handle];

						if (child->flex_factor == 0)
						{
							if (widget->layout == WZ_LAYOUT_HORIZONTAL)
							{
								child_constraint_min_main_axis = &child->constraint_min_w;
								child_constraint_max_main_axis = &child->constraint_max_w;
								child_constraint_min_cross_axis = &child->constraint_min_h;
								child_constraint_max_cross_axis = &child->constraint_max_h;
							}
							else if (widget->layout == WZ_LAYOUT_VERTICAL)
							{
								child_constraint_min_main_axis = &child->constraint_min_h;
								child_constraint_max_main_axis = &child->constraint_max_h;
								child_constraint_min_cross_axis = &child->constraint_min_w;
								child_constraint_max_cross_axis = &child->constraint_max_w;
							}
							else
							{
								// Just to appease the compiler
								child_constraint_min_main_axis = child_constraint_max_main_axis =
									child_constraint_min_cross_axis = child_constraint_max_cross_axis = 0;
								wz_assert(0);
							}

							wz_log(log_messages, &log_messages_count,
								"(%s) LOG: Non-flex widget with constraints (%u %u %u %u)\n",
								child->source,
								child->constraint_min_w, child->constraint_min_h,
								child->constraint_max_w, child->constraint_max_h);

							widgets_stack[widgets_stack_count] = widget->children[i].handle;
							widgets_stack_count++;
						}
					}

					wz_log(log_messages, &log_messages_count,
						"(%s) LOG: %s ends constrains non-flex children\n",
						widget->source, widget_type);

					widgets_visits[parent_index] = 1;
				}
				else if (widgets_visits[parent_index] == WZ_LAYOUT_STAGE_FLEX_CHILDREN)
				{
					// Give constraints to flex children, allocating from the availble space
					if (widget->layout == WZ_LAYOUT_HORIZONTAL)
					{
						available_size_main_axis = widget->constraint_max_w;
						available_size_cross_axis = widget->constraint_max_h;
					}
					else if (widget->layout == WZ_LAYOUT_VERTICAL)
					{
						available_size_main_axis = widget->constraint_max_h;
						available_size_cross_axis = widget->constraint_max_w;
					}

					children_flex_factor = 0;

					for (i = 0; i < widget->children_count; ++i)
					{
						child = &widgets[widget->children[i].handle];
						//child_rect = &rects[widget->children[i]];

						if (child->free_from_parent)
						{
							continue;
						}

						if (widget->layout == WZ_LAYOUT_HORIZONTAL)
						{
							child_actual_size_main_axis = &child->actual_w;
						}
						else if (widget->layout == WZ_LAYOUT_VERTICAL)
						{
							child_actual_size_main_axis = &child->actual_h;
						}
						else
						{
							child_actual_size_main_axis = 0;
						}

						if (!child->flex_factor)
						{
							//wz_assert(*child_actual_size_main_axis);
							if (available_size_main_axis >= *child_actual_size_main_axis)
							{
								available_size_main_axis -= *child_actual_size_main_axis;
							}
							else
							{
								available_size_main_axis = 0;
							}
						}
						else
						{
							children_flex_factor += child->flex_factor;
						}
					}

					// Widget allocating space to flex children cannot be unbounded in the main axis
					if (children_flex_factor && constraint_max_main_axis == UINT_MAX)
					{
						wz_log(log_messages, &log_messages_count,
							"(%s) ERROR: Widget allocating space to flex \
							children cannot be unbounded in the main axis\n",
							widget->source);
					}

					// Substract padding and child gap 
					if (widget->layout == WZ_LAYOUT_HORIZONTAL)
					{
						if (available_size_main_axis >= widget->margin_left + widget->margin_right)
						{
							available_size_main_axis -= widget->margin_left + widget->margin_right;
						}
						else
						{
							available_size_main_axis = 0;
						}
					}
					else if (widget->layout == WZ_LAYOUT_VERTICAL)
					{
						if (available_size_main_axis >= widget->margin_top + widget->margin_bottom)
						{
							available_size_main_axis -= widget->margin_top + widget->margin_bottom;
						}
						else
						{
							available_size_main_axis = 0;
						}
					}

					if (available_size_main_axis > widget->child_gap * (widget->children_count - 1))
					{
						available_size_main_axis -= widget->child_gap * (widget->children_count - 1);
					}
					else
					{
						available_size_main_axis = 0;
					}

					if (children_flex_factor)
					{
						size_per_flex_factor = available_size_main_axis / children_flex_factor;
					}

					const char* widget_type = 0;
					if (widget->layout == WZ_LAYOUT_HORIZONTAL)
					{
						widget_type = "Row";
					}
					else if (widget->layout == WZ_LAYOUT_VERTICAL)
					{
						widget_type = "Column";
					}

					wz_log(log_messages, &log_messages_count,
						"(%s) LOG: %s begins constrains flex children\n",
						widget->source, widget_type);

					for (int i = 0; i < widget->children_count; ++i)
					{
						child = &widgets[widget->children[i].handle];

						if (widget->layout == WZ_LAYOUT_HORIZONTAL)
						{
							child_constraint_min_main_axis = &child->constraint_min_w;
							child_constraint_max_main_axis = &child->constraint_max_w;
							child_constraint_min_cross_axis = &child->constraint_min_h;
							child_constraint_max_cross_axis = &child->constraint_max_h;
						}
						else if (widget->layout == WZ_LAYOUT_VERTICAL)
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
							wz_assert(0);
						}

						if (child->flex_factor)
						{
							unsigned int main_axis_size = size_per_flex_factor * child->flex_factor;

							if (widget->flex_fit == WZ_FLEX_FIT_TIGHT)
							{
								*child_constraint_min_main_axis = main_axis_size;
							}

							*child_constraint_max_main_axis = main_axis_size;

							if (child_constraint_max_main_axis == 0)
							{
								wz_log(log_messages, &log_messages_count,
									"(%s) ERROR: Flex widget had no space available to it \n",
									child->source);
							}

							wz_log(log_messages, &log_messages_count,
								"(%s) LOG: Flex widget recieved constraints (%u, %u) \n",
								child->source,
								child->constraint_max_w, child->constraint_max_h);

							widgets_stack[widgets_stack_count] = widget->children[i].handle;
							widgets_stack_count++;
						}
					}

					wz_log(log_messages, &log_messages_count,
						"(%s) LOG: %s  ends constrains flex children\n",
						widget->source, widget_type);

					widgets_visits[parent_index] = 2;
				}
				else if (widgets_visits[parent_index] == WZ_LAYOUT_STAGE_PARENT)
				{
					// We finally determined the size of all the children of a widget with a layout
					// Now we determine it's size

					wz_assert(widget->children_count);

					// Main axis size
					if (widget->main_axis_size_type == MAIN_AXIS_SIZE_TYPE_MIN)
					{

						if (widget->constraint_max_h == widget->constraint_min_h &&
							widget->layout == WZ_LAYOUT_HORIZONTAL)
						{
							wz_log(log_messages, &log_messages_count,
								"(%s) ERROR: Row has tight constraints in it's horizontal axis, but it's supposed to shrink-wrap.\n",
								widget->source);
						}

						if (widget->constraint_max_h == widget->constraint_min_h &&
							widget->layout == WZ_LAYOUT_VERTICAL)
						{
							wz_log(log_messages, &log_messages_count,
								"(%s) ERROR: Column has tight constraints in it's vertical axis, but it's supposed to shrink-wrap.\n",
								widget->source);
						}

						children_size = 0;
						for (i = 0; i < widget->children_count; ++i)
						{
							child = &widgets[widget->children[i].handle];

							if (widget->layout == WZ_LAYOUT_HORIZONTAL)
							{
								child_actual_size_main_axis = &child->actual_w;
							}
							else if (widget->layout == WZ_LAYOUT_VERTICAL)
							{
								child_actual_size_main_axis = &child->actual_h;
							}
							else
							{
								child_actual_size_main_axis = 0;
								wz_assert(0);
							}

							children_size += *child_actual_size_main_axis;
						}

						*actual_size_main_axis = children_size;
					}
					else
					{

						// Layout widget is unconstrained in the main axis
						// It must shrink-wrap
						if (widget->layout == WZ_LAYOUT_HORIZONTAL &&
							widget->constraint_max_w == WZ_UINT_MAX)
						{
							wz_log(log_messages, &log_messages_count,
								"(%s) ERROR: Row recieved unbounded constraints in the horizontal axis\n",
								widget->source);
						}

						if (widget->layout == WZ_LAYOUT_VERTICAL &&
							widget->constraint_max_h == WZ_UINT_MAX)
						{
							wz_log(log_messages, &log_messages_count,
								"(%s) ERROR: Column recieved unbounded constraints in the vertical axis\n",
								widget->source);
						}

						// Determine size of widget that is constrained in the main axis
						*actual_size_main_axis = *constraint_max_main_axis;
					}

					// Cross axis size (use tallest child)
					parent_cross_axis_size = 0;
					for (int i = 0; i < widget->children_count; ++i)
					{
						child = &widgets[widget->children[i].handle];
						//child_rect = &rects[widget->children[i]];

						if (widget->layout == WZ_LAYOUT_HORIZONTAL)
						{
							if (child->actual_h > parent_cross_axis_size)
							{
								parent_cross_axis_size = child->actual_h;
							}
						}
						else if (widget->layout == WZ_LAYOUT_VERTICAL)
						{
							if (child->actual_w > parent_cross_axis_size)
							{
								parent_cross_axis_size = child->actual_w;
							}
						}
					}


					{
						if (!parent_cross_axis_size)
						{
							wz_log(log_messages, &log_messages_count,
								"(%s) LOG: widget has not room for children in the cross axis\n", widget->source);
						}
						*actual_size_cross_axis = parent_cross_axis_size;
					}

					widget->actual_h += widget->margin_top + widget->margin_bottom;
					widget->actual_w += widget->margin_left + widget->margin_right;


					// Clamp
					if (widget->actual_h < widget->constraint_min_h)
					{
						widget->actual_h = widget->constraint_min_h;
					}

					if (widget->actual_h > widget->constraint_max_h)
					{
						widget->actual_h = widget->constraint_max_h;
					}

					if (widget->actual_w < widget->constraint_min_w)
					{
						widget->actual_w = widget->constraint_min_w;
					}

					if (widget->actual_w > widget->constraint_max_w)
					{
						widget->actual_w = widget->constraint_max_w;
					}

					if (widget->layout == WZ_LAYOUT_HORIZONTAL)
					{
						wz_log(log_messages, &log_messages_count,
							"(%s) LOG: Row widget  with constraints (%u, %u) determined its size (%u, %u)\n",
							widget->source,
							widget->constraint_max_w, widget->constraint_max_h, widget->actual_w, widget->actual_h);
					}
					else if (widget->layout == WZ_LAYOUT_VERTICAL)
					{
						wz_log(log_messages, &log_messages_count,
							"(%s) LOG: Column widget  with constraints (%u, %u) determined its size (%u, %u)\n",
							widget->source,
							widget->constraint_max_w, widget->constraint_max_h, widget->actual_w, widget->actual_h);
					}

					wz_assert(*actual_size_main_axis <= *constraint_max_main_axis);
					wz_assert(*actual_size_cross_axis <= *constraint_max_cross_axis);

					// Give positions to children
					unsigned int offset = 0;
					widget->actual_x = 0;
					widget->actual_y = 0;
					for (int i = 0; i < widget->children_count; ++i)
					{
						child = &widgets[widget->children[i].handle];
						//child_rect = &rects[widget->children[i]];

						if (child->free_from_parent)
						{
							continue;
						}

						if (widget->layout == WZ_LAYOUT_HORIZONTAL)
						{
							child_actual_size_main_axis = &child->actual_w;
							child->actual_x = offset;
							child->actual_y = 0;
						}
						else if (widget->layout == WZ_LAYOUT_VERTICAL)
						{
							child_actual_size_main_axis = &child->actual_h;
							child->actual_y = offset;
							child->actual_x = 0;
						}
						else
						{
							child_actual_size_main_axis = 0;
							wz_assert(0);
						}

						// Position padding
						child->actual_x += widget->margin_left;
						child->actual_y += widget->margin_top;

						offset += *child_actual_size_main_axis;
						offset += widget->child_gap;

						wz_log(log_messages, &log_messages_count,
							"(%s) LOG: Child widget (%u) will have the raltive position %u %u\n",
							child->source, widget->children[i],
							child->actual_x, child->actual_y);
					}

					widgets_stack_count--;
				}
				else
				{
					wz_assert(0);
				}
			}
			else if (widget->layout == WZ_LAYOUT_NONE)
			{
				if (widgets_visits[parent_index] == 0)
				{
					for (int i = 0; i < widget->children_count; ++i)
					{
						child = &widgets[widget->children[i].handle];

						constraint_max_w = widget->constraint_max_w - (widget->margin_left + widget->margin_right);
						constraint_max_h = widget->constraint_max_h - (widget->margin_top + widget->margin_bottom);

						if (constraint_max_w < child->constraint_max_w)
						{
							child->constraint_max_w = constraint_max_h;
						}
						if (constraint_max_h < child->constraint_max_h)
						{
							child->constraint_max_h = constraint_max_h;
						}

						child->actual_x = widget->margin_left;
						child->actual_y = widget->margin_right;

						wz_assert(i < MAX_NUM_CHILDREN);
						wz_assert(widgets_stack_count < STACK_MAX_DEPTH);
						widgets_stack[widgets_stack_count] = widget->children[i].handle;
						widgets_stack_count++;
						widgets_visits[parent_index] = 1;
						wz_log(log_messages, &log_messages_count, "(%s) LOG: Non-layout widget passes to child constraints (%u, %u) and position (%u %u)\n",
							child->source,
							child->constraint_max_w, child->constraint_max_h, child->actual_x, child->actual_y);
					}
				}
				else if (widgets_visits[parent_index] == 1)
				{
					widget->actual_x = 0;
					widget->actual_y = 0;
					widget->actual_w = widget->constraint_max_w;
					widget->actual_h = widget->constraint_max_h;

					wz_log(log_messages, &log_messages_count,
						"(%s) LOG: Non-layout widget  with constraints (%u, %u) determined its size (%u, %u)\n",
						widget->source,
						widget->constraint_max_w, widget->constraint_max_h, widget->actual_w, widget->actual_h);

					//wz_assert(widget->actual_w <= widget->constraint_max_w);
					//wz_assert(widget->actual_h <= widget->constraint_max_h);
					//wz_assert(widget->actual_w <= root_w);
					//wz_assert(widget->actual_h <= root_h);

					widgets_stack_count--;
				}
			}
			else
			{
				wz_assert(0);
			}

		}
	}

	// Final stage. Calculate the widgets non-relative final position and cull 
	for (int i = 1; i < count; ++i)
	{
		widget = &widgets[i];
		unsigned int offset = 0;
		unsigned int children_size_main_axis = 0;

		widget->actual_x += widget->x;
		widget->actual_y += widget->y;

		for (int j = 0; j < widget->children_count; ++j)
		{
			child = &widgets[widget->children[j].handle];
			//child_rect = &rects[widget->children[j]];

			if (widget->layout == WZ_LAYOUT_HORIZONTAL)
			{
				children_size_main_axis += child->actual_w;
			}
			else if (widget->layout == WZ_LAYOUT_VERTICAL)
			{
				children_size_main_axis += child->actual_h;
			}
		}

		children_size_main_axis += widget->child_gap * (widget->children_count - 1);

		for (int j = 0; j < widget->children_count; ++j)
		{
			child = &widgets[widget->children[j].handle];

			if (!widget->actual_w || !widget->actual_h)
			{
				child->actual_w = 0;
				child->actual_h = 0;
				continue;
			}

			unsigned int parent_size_h = widget->actual_h - widget->margin_top - widget->margin_bottom;
			unsigned int parent_size_w = widget->actual_w - widget->margin_left - widget->margin_right;

			if (widget->main_axis_alignment == WZ_MAIN_AXIS_ALIGNMENT_END)
			{
				if (widget->layout == WZ_LAYOUT_HORIZONTAL)
				{
					child->actual_x += parent_size_w - children_size_main_axis;
				}
				else if (widget->layout == WZ_LAYOUT_VERTICAL)
				{
					child->actual_y += parent_size_h - children_size_main_axis;
				}
			}
			else if (widget->main_axis_alignment == WZ_MAIN_AXIS_ALIGNMENT_CENTER)
			{
				if (widget->layout == WZ_LAYOUT_HORIZONTAL && parent_size_w > child->actual_w)
				{
					child->actual_x += (parent_size_w - children_size_main_axis) / 2;
				}
				else if (widget->layout == WZ_LAYOUT_VERTICAL && parent_size_h > child->actual_h)
				{
					child->actual_y += (parent_size_h - children_size_main_axis) / 2;
				}
			}

			if (widget->cross_axis_alignment == WZ_CROSS_AXIS_ALIGNMENT_CENTER)
			{
				if (widget->layout == WZ_LAYOUT_HORIZONTAL && parent_size_h > child->actual_h)
				{
					child->actual_y += (parent_size_h - child->actual_h) / 2;
				}
				else if (widget->layout == WZ_LAYOUT_VERTICAL && parent_size_w > child->actual_w)
				{
					child->actual_x += (parent_size_w - child->actual_w) / 2;
				}
			}
			else if (widget->cross_axis_alignment == CROSS_AXIS_ALIGNMENT_START)
			{
				// Do nothing
			}

			child->actual_x = widget->actual_x + child->actual_x;
			child->actual_y = widget->actual_y + child->actual_y;

			// Check the widgets size doesnt exceeds its parents
			if (child->actual_x + child->actual_w >= widget->actual_x + widget->actual_w)
			{
				wz_log(log_messages, &log_messages_count, "(%s) ERROR: Widget exceeds it's parents horizontally\n",
					child->source);
			}
			if (child->actual_y + child->actual_h >= widget->actual_y + widget->actual_h)
			{
				wz_log(log_messages, &log_messages_count, "(%s) ERROR: Widget exceeds it's parents vertically\n",
					child->source);
			}
		}
	}

	wz_log(log_messages, &log_messages_count, "---------------------------\n");
	wz_log(log_messages, &log_messages_count, "Final Layout:\n");

	for (unsigned int i = 0; i < count; ++i)
	{
		widget = &widgets[i];

		wz_log(log_messages, &log_messages_count, "(%u %s %u) : (%d %d %u %u)\n",
			i, widget->source, widget->actual_x, widget->actual_y, widget->actual_w, widget->actual_h);
	}
	wz_log(log_messages, &log_messages_count, "---------------------------\n");

	free(widgets_visits);
	free(widgets_stack);
	free(log_messages);

	*failed = wz_layout_failed;
}

void wz_gui_deinit(WzGui* gui)
{
	save_widgets(gui);
}

void wz_gui_init(WzGui* gui)
{
	// TODO: initialize each member individually
	//// TODO: FREE
	gui->scrollbars = malloc(sizeof(*gui->scrollbars) * MAX_NUM_SCROLLBARS);

	gui->widgets = calloc(MAX_NUM_WIDGETS, sizeof(*gui->widgets));

	gui->rects = calloc(MAX_NUM_WIDGETS, sizeof(*gui->rects));
	gui->boxes_indices = calloc(MAX_NUM_WIDGETS, sizeof(*gui->boxes_indices));
	gui->hovered_items_list = calloc(MAX_NUM_WIDGETS, sizeof(*gui->hovered_items_list));
	gui->hovered_boxes = calloc(MAX_NUM_WIDGETS, sizeof(*gui->hovered_boxes));
	gui->cached_boxes = calloc(MAX_NUM_WIDGETS, sizeof(*gui->cached_boxes));

	gui->free_widgets = calloc(MAX_NUM_WIDGETS, sizeof(*gui->free_widgets));

	load_widgets(gui);
}

