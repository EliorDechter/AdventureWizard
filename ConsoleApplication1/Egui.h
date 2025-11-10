#ifndef WZRD_GUI_H
#define WZRD_GUI_H

#include <float.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include "Strings.h"

#define WZRD_BORDER_SIZE 1

// BUG: FONT SIZE CLASHES WITH THAT DEFINED IN PLATFORM.C
#define WZRD_FONT_HEIGHT 22
#define FONT_WIDTH WZRD_FONT_HEIGHT / 2

#define BUTTON_WIDTH 110
#define BUTTON_HEIGHT 24

#define ICON_BUTTON_WIDTH 24
#define ICON_BUTTON_HEIGHT 24

#define TEXT_BOX WIDTH 110
#define TEXT_BOX_HEIGHT 24

#define GAP_WIDTH 24
#define GAP_HEIGHT 24

#define MAX_NUM_WIDGETS 128

#define MAX_NUM_ITEMS 32
#define MAX_NUM_CHILDREN 256

#define Stringify1(x) #x
#define Stringify(x) Stringify1(x)
#define EditorDropDownBox(name) EditorDropDownBoxRaw(Stringify(__LINE__), name)
#define EditorToggle(name) EditorToggleRaw(Stringify(__LINE__), name)

#define MAX_NUM_BOXES 128

typedef enum EditorWindow { EditorWindow_Lists, EditorWindow_Slice_Spritesheet } EditorWindow;
typedef enum EditorTab { EditorTab_Entities, EditorTab_Events, EditorTab_Areas, EditorTab_Inventory } EditorTab;

typedef enum PanelId { PanelId_None, PanelId_Window, PanelId_TopPanel, PanelId_RightPanel, PanelId_GameScreen, PanelId_ModifyEntityDropDown } PanelId;
typedef enum EditorWindowId { EditorWindowId_ModifyEntity, EditorWindowId_DrawTool } EditorWindowId;

typedef struct wzrd_color {
	unsigned char r, g, b, a;
} wzrd_color;

// Some Basic Colors
// NOTE: Custom raylib color palette for amazing visuals on WHITE background
#define EGUI_LIGHTGRAY (wzrd_color){ 200, 200, 200, 255}   // Light Gray
#define EGUI_LIGHTESTGRAY (wzrd_color){ 225, 225, 225, 255 }   // Lightest Gray
#define EGUI_GRAY      (wzrd_color){ 180, 180, 180, 255 }   // Gray
#define EGUI_DARKGRAY  (wzrd_color){ 80, 80, 80, 255 }      // Dark Gray
#define EGUI_YELLOW    (wzrd_color){ 253, 249, 0, 255 }     // Yellow
#define EGUI_GOLD      (wzrd_color){ 255, 203, 0, 255 }     // Gold
#define EGUI_ORANGE    (wzrd_color){ 255, 161, 0, 255 }     // Orange
#define EGUI_PINK      (wzrd_color){ 255, 109, 194, 255 }   // Pink
#define EGUI_RED       (wzrd_color){ 230, 41, 55, 255 }     // Red
#define EGUI_MAROON    (wzrd_color){ 190, 33, 55, 255 }     // Maroon
#define EGUI_GREEN     (wzrd_color){ 0, 228, 48, 255 }      // Green
#define EGUI_LIME      (wzrd_color){ 0, 158, 47, 255 }      // Lime
#define EGUI_DARKGREEN (wzrd_color){ 0, 117, 44, 255 }      // Dark Green
#define EGUI_SKYBLUE   (wzrd_color){ 102, 191, 255, 255 }   // Sky Blue
#define EGUI_BLUE      (wzrd_color){ 0, 121, 241, 255 }     // Blue
#define EGUI_DARKBLUE  (wzrd_color){ 0, 82, 172, 255 }      // Dark Blue
#define EGUI_PURPLE    (wzrd_color){ 200, 122, 255, 255 }   // Purple
#define EGUI_VIOLET    (wzrd_color){ 135, 60, 190, 255 }    // Violet
#define EGUI_DARKPURPLE(EguiColor){ 112, 31, 126, 255 }    // Dark Purple
#define EGUI_BEIGE     (wzrd_color){ 211, 176, 131, 255 }   // Beige
#define EGUI_BROWN     (wzrd_color){ 127, 106, 79, 255 }    // Brown
#define EGUI_DARKBROWN (wzrd_color){ 76, 63, 47, 255 }      // Dark Brown

#define EGUI_WHITE     (wzrd_color){ 255, 255, 255, 255 }   // White
#define EGUI_WHITE2     (wzrd_color){ 230, 230, 230, 255 }   // White2
#define EGUI_BLACK     (wzrd_color){ 0, 0, 0, 255 }         // Black
#define EGUI_BLANK     (wzrd_color){ 0, 0, 0, 0 }           // Blank (Transparent)
#define EGUI_MAGENTA   (wzrd_color){ 255, 0, 255, 255 }     // Magenta
#define EGUI_RAYWHITE  (wzrd_color){ 245, 245, 245, 255 }   // My own White (raylib logo)

typedef enum EguiState {
	WZRD_INACTIVE, EguiActivating, WZRD_ACTIVE, WZRD_DEACTIVATING
} wzrd_state;

typedef struct wzrd_rect_struct {
	int x, y, w, h;
} WzRect;

typedef struct wzrd_v2 {
	int x, y;
} wzrd_v2;

#define DEBUG_PANELS 0

typedef enum WzBorderType
{
	BorderType_None, BorderType_Default, BorderType_Black, BorderType_Clicked, BorderType_InputBox, BorderType_BottomLine, BorderType_LeftLine, BorderType_Custom
}
WzBorderType;

typedef struct wzrd_texture {
	void* data;
	float w, h;
} wzrd_texture;

typedef enum ItemType {
	ItemType_None,
	wzrd_item_type_str,
	ItemType_Rect,
	ItemType_Texture,
	ItemType_DropdownIcon,
	ItemType_CloseIcon,
	ItemType_VerticalDottedLine,
	ItemType_BottomVerticalDottedLine,
	ItemType_TopVerticalDottedLine,
	ItemType_HorizontalDottedLine,
	ItemType_LeftHorizontalDottedLine,
	ItemType_RightHorizontalDottedLine,
	ItemType_Line,
	ItemType_IconClose
} ItemType;

typedef struct Line {
	int x0, y0, x1, y1;
} Line;

typedef struct wzrd_str
{
	char* str;
	size_t len;
} wzrd_str;

typedef struct Item {
	ItemType type;
	wzrd_v2 size;
	wzrd_color color;
	int pad_left, pad_right, pad_top, pad_bottom;
	union {
		//wzrd_str str;
		wzrd_str str;
		wzrd_texture texture;
		WzRect rect;
		Line line;
	} val;
	bool scissor;
} Item;

typedef enum ButtonType { ButtonType_None, ButtonType_Flat, ButtonType_ThreeDimensional } ButtonType;

typedef struct wzrd_widget_id {
	wzrd_str val;
} wzrd_widget_id;

typedef struct wzrd_handle
{
	unsigned int handle;
} WzHandle;

typedef enum {
	wzrd_box_type_default,
	wzrd_box_type_button,
	wzrd_box_type_resizable,
	wzrd_box_type_input_box,
	wzrd_box_type_flat_button,
	wzrd_box_type_crate
} wzrd_box_type;

typedef enum WzLayout
{
	WzLayoutNone,
	WzLayoutHorizontal,
	WzLayoutVertical,
} WzLayout;

typedef enum {
	WzAlignVCenter = 1 << 0,
	WzAlignHCenter = 1 << 1,
} WzAlignment;

typedef struct Crate {
	int layer;
	int index;

	int box_stack[32];
	int box_stack_count;

	bool current_column_mode;
	int current_child_gap;
} Crate;

#define MAX_NUM_DRAW_COMMANDS 1024

typedef enum EguiDrawCommandType {
	DrawCommandType_Default,
	DrawCommandType_Rect,
	DrawCommandType_Line,
	DrawCommandType_VerticalLine,
	DrawCommandType_HorizontalLine,
	DrawCommandType_String,
	DrawCommandType_Texture,
	DrawCommandType_Clip,
	DrawCommandType_StopClip
} EguiDrawCommandType;

typedef struct EguiDrawCommand {
	EguiDrawCommandType type;
	int box_index;
	wzrd_str str;
	WzRect dest_rect, src_rect;
	wzrd_color color;
	wzrd_texture texture;
	int z;
} wzrd_draw_command;

typedef struct wzrd_draw_commands_buffer {
	wzrd_draw_command commands[MAX_NUM_DRAW_COMMANDS];
	int count;
} wzrd_draw_commands_buffer;

typedef struct {
	WzHandle handle;
	int line_number;
	const char* file;

	int x_internal, y_internal;
	unsigned int w_internal, h_internal;
	bool disable_hover;
	WzHandle clip_widget;
	bool disable_input;
	bool is_draggable, is_slot;
	bool free;
	int content_w, content_h;

	unsigned int children[MAX_NUM_CHILDREN];
	unsigned char children_count;

	unsigned int free_children[MAX_NUM_CHILDREN];
	unsigned char free_children_count;

	Item items[MAX_NUM_ITEMS];
	unsigned int items_count;
	unsigned int layer;
	bool bring_to_front;
	unsigned int index;
	bool is_selected;
	
	//wzrd_space space;
	int x, y, w, h;
	float percentage_w, percentage_h;
	//bool w_strech, h_strech;
	unsigned int strech_factor;

	//wzrd_structure structure;
	unsigned int pad_right, pad_bottom, pad_left, pad_top;

	int child_gap;
	WzLayout layout_type;
	bool fit_h, fit_w;
	WzAlignment alignment;
	bool best_fit;

	//wzrd_skin skin;
	wzrd_color font_color;
	wzrd_color color;
	wzrd_color b0, b1, b2, b3;
	WzBorderType border_type;
	WzBorderType window_border_type;

	const char* tag;
	const char* secondary_tag;

} WzWidget;

typedef struct CachedBox
{
	const char* tag;
	WzWidget box;
} CachedBox;

typedef struct wzrd_keyboard_key {
	char val;
	wzrd_state state;
} wzrd_keyboard_key;

typedef struct wzrd_keyboard_keys {
	wzrd_keyboard_key keys[32];
	int count;
} wzrd_keyboard_keys;

typedef struct wzrd_icons {
	wzrd_texture close, delete, entity, play, pause, stop, dropdown;
} wzrd_icons;

typedef enum wzrd_cursor { wzrd_cursor_default, wzrd_cursor_hand, wzrd_cursor_vertical_arrow, wzrd_cursor_horizontal_arrow } wzrd_cursor;

#define MAX_NUM_HOVERED_ITEMS 32

typedef struct wzrd_stylesheet
{
	wzrd_color label_color;
	wzrd_color label_item_selected_color;
} wzrd_stylesheet;

typedef struct wzrd_canvas {

	// Persistent
	WzHandle hovered_item, hot_item_previous, active_item, clicked_item, activating_item, deactivating_item, dragged_item;
	WzHandle right_resized_item, left_resized_item, bottom_resized_item, top_resized_item;
	WzHandle active_input_box;
	WzHandle hovered_items_list[MAX_NUM_HOVERED_ITEMS];
	int hovered_items_list_count;
	WzWidget hovered_boxes[MAX_NUM_HOVERED_ITEMS];
	int hovered_boxes_count;

	WzRect window;
	float input_box_timer;
	double tooltip_time;
	WzWidget dragged_box;
	bool clean;
	void (*get_string_size)(char*, int*, int*);

	wzrd_v2 mouse_pos, previous_mouse_pos, mouse_delta, screen_mouse_pos;
	wzrd_keyboard_keys keyboard_keys;
	wzrd_state mouse_left, mouse_right;

	// Frame ?
	WzWidget boxes[MAX_NUM_BOXES];
	int boxes_count;

	int boxes_in_stack_count, total_num_windows;

	Crate crates_stack[32];
	int current_crate_index;

	bool enable_input;

	int styles_count;
	wzrd_cursor cursor;
	wzrd_draw_commands_buffer command_buffer;

	//wzrd_skin panel_skin, panel_border_skin, panel_border_click_skin, command_button_skin, label_item_skin, label_item_selected_skin, label_skin, input_box_skin, command_button_on_skin, list_skin;
	//wzrd_structure panel_structure, command_button_structure, label_structure, input_box_structure;
	//wzrd_layout v_panel_layout, h_panel_layout, command_button_layout, input_box_layout, top_label_panel_layout;
	//wzrd_space command_button_space, toggle_icon_space, input_box_space;

#define MAX_NUM_CACHED_BOXES 128
	WzWidget cached_boxes[MAX_NUM_CACHED_BOXES];
	int cached_boxes_count;

#define MAX_NUM_CLIP_BOXES 128
	WzRect clip_boxes[MAX_NUM_CLIP_BOXES];
	int clip_boxes_count;

	wzrd_stylesheet stylesheet;

} wzrd_canvas;

#define MAX_NUM_HASHTABLE_ELEMENTS 32

typedef struct HashTableElement {
	char key[32];
	int value;
} HashTableElement;

typedef struct HashTable {
	HashTableElement elements[MAX_NUM_HASHTABLE_ELEMENTS];
	int num;
} HashTable;

typedef struct WidgetData {
	int active; // ie selected box
	bool flag;
	char text[32];
} Widget;
#define EDITOR_LINE_GAP 10
#define EDITOR_ICON_SIZE 24
#define EDITOR_BUTTON_SIZE_X 72
#define EDITOR_BUTTON_SIZE_Y 24

#define MAX_NUM_LABELS 32

typedef struct Label_list {
	wzrd_str val[MAX_NUM_LABELS];
	int count;
} Label_list;

#define MAX_NUM_VERTICES_IN_POLYGON 32

typedef struct wzrd_polygon {
	wzrd_v2 vertices[MAX_NUM_VERTICES_IN_POLYGON];
	int count;
} wzrd_polygon;

#define WZ_STRINGIFY1(x) #x
#define WZ_STRINGIFY(x) WZ_STRINGIFY1(x)

// GENERAL
WzHandle wz_begin(wzrd_canvas* gui, WzRect window,
	void (*get_string_size)(char*, int*, int*),
	wzrd_v2 mouse_pos, wzrd_state mouse_left, wzrd_keyboard_keys keys, bool enable_input);
void wzrd_end(wzrd_str* debug_str);

// WIDGETS
WzHandle wzrd_label_button_raw(wzrd_str str, bool* result, WzHandle parent, const char *file, unsigned int line);
WzHandle wzrd_button_icon_raw(wzrd_texture texture, bool* released, WzHandle parent, const char *file, unsigned int line);
WzHandle wzrd_command_button_raw(wzrd_str str, bool* b, WzHandle parent, const char *file, unsigned int line);
WzHandle wzrd_dropdown_raw(int* selected_text, const wzrd_str* texts, int texts_count, int w, bool* active, WzHandle parent, const char *file, unsigned int line);
WzHandle wzrd_dialog_begin_raw(wzrd_v2* pos, wzrd_v2 size, bool* active, wzrd_str name, int layer, WzHandle parent, const char *file, unsigned int line);
WzHandle wzrd_command_toggle_raw(wzrd_str str, bool* active, WzHandle parent, const char *file, unsigned int line);
WzHandle wzrd_toggle_icon_raw(wzrd_texture texture, bool* active, WzHandle parent, const char *file, unsigned int line);
void wzrd_label_list_sorted_raw(wzrd_str* item_names, unsigned int count, int* items, wzrd_v2 size, unsigned int* selected, bool* is_selected, WzHandle parent, const char *file, unsigned int line);
void wzrd_label_list_raw(wzrd_str* item_names, unsigned int count, wzrd_v2 size, WzHandle* handles, unsigned int* selected, bool* is_selected, WzHandle parent, const char *file, unsigned int line);
WzHandle wzrd_input_box_raw(char* str, int* len, int max_num_keys, WzHandle parent, const char *file, unsigned int line);
WzHandle wzrd_handle_button_raw(bool* active, WzRect rect, wzrd_color color, wzrd_str name, WzHandle parent, const char *file, unsigned int line);
WzHandle wzrd_label_raw(wzrd_str str, WzHandle parent, const char *file, unsigned int line);
WzHandle wzrd_vbox_border_raw(wzrd_v2 size, WzHandle parent, const char *file, unsigned int line);
WzHandle wz_hbox_raw(WzHandle parent, const char *file, unsigned int line);

#define FUSE(x, y) x##y

#define wzrd_label_button(str, result, parent) wzrd_label_button_raw(str, result, parent, __FILE__, __LINE__)
#define wzrd_button_icon(texture, released, parent) wzrd_button_icon_raw(texture, released, parent, __FILE__, __LINE__)
#define wzrd_command_button(str, b, parent) wzrd_command_button_raw(str, b, parent, __FILE__, __LINE__)
#define wzrd_dropdown(selected_text, texts, texts_count, w, active, parent) wzrd_dropdown_raw(selected_text, texts, texts_count, w, active, parent, __FILE__, __LINE__)
#define wzrd_dialog_begin(pos, size, active, name, layer, parent) wzrd_dialog_begin_raw(pos, size, active, name, layer, parent, __FILE__, __LINE__)
#define wzrd_command_toggle(str, active, parent) wzrd_command_toggle_raw(str, active, parent, __FILE__, __LINE__)
#define wzrd_toggle_icon(texture, active, parent) wzrd_toggle_icon_raw(texture, active, parent, __FILE__, __LINE__)
#define wzrd_label_list_sorted(item_names, count, items, size, selected, is_selected, parent) wzrd_label_list_sorted_raw(item_names, count, items, size, selected, is_selected, parent, __FILE__, __LINE__)
#define wzrd_label_list(item_names, count, size, handles, selected, is_selected, parent) wzrd_label_list_raw(item_names, count, size, handles, selected, is_selected, parent, __FILE__, __LINE__)
#define wzrd_input_box(str, len, max_num_keys, parent) wzrd_input_box_raw(str, len, max_num_keys, parent, __FILE__, __LINE__)
#define wzrd_handle_button(active, rect, color, name, parent) wzrd_handle_button_raw(active, rect, color, name, parent, __FILE__, __LINE__)
#define wzrd_label(str, parent) wzrd_label_raw(str, parent, __FILE__, __LINE__)
#define wzrd_vbox_border(size, parent) wzrd_vbox_border_raw(size, parent, __FILE__, __LINE__)
#define wz_hbox(parent) wz_hbox_raw(parent,  __FILE__, __LINE__)
#define wz_vbox(parent) wz_vbox_raw(parent,  __FILE__, __LINE__)

void wzrd_dialog_end(bool active);

// UTILS
void wz_widget_set_layout(WzHandle handle, WzLayout layout_type);
void wz_widget_set_strech_factor(WzHandle handle, unsigned int strech_factor);
WzHandle wz_widget(WzHandle parent);
WzHandle wzrd_widget_free(WzHandle parent);
void wzrd_box_add_child(WzHandle parent, WzHandle child);
WzWidget* wz_widget_get(WzHandle handle);
wzrd_str wzrd_str_create(char* str);
WzWidget* wzrd_box_get_last();
WzWidget* wzrd_box_find(wzrd_canvas* c, wzrd_str name);
int wzrd_box_get_current_index();
bool wzrd_box_is_active(WzWidget* box);
bool wzrd_is_releasing();
bool wzrd_box_is_dragged(WzWidget* box);
void wzrd_box_resize(wzrd_v2* size);
void wzrd_item_add(Item item, WzHandle box);
bool wzrd_box_is_hot(WzWidget* box);
bool wzrd_box_is_hot_using_canvas(wzrd_canvas* canvas, WzWidget* box);
bool wzrd_handle_is_equal(WzHandle a, WzHandle b);
void wzrd_handle_set_layer(WzHandle handle, unsigned int layer);
wzrd_canvas* wzrd_canvas_get();
WzWidget wzrd_widget_get_cached_box(const char *tag);
void wzrd_widget_tag(WzHandle widget, const char* str);
void wzrd_widget_clip(WzHandle handle);
bool wzrd_widget_is_deactivating(WzHandle handle);
bool wzrd_widget_is_active(WzHandle handle);
WzWidget wzrd_widget_get_cached_box_with_secondary_tag(const char* tag, const char* secondary_tag);
void wzrd_widget_set_color(WzHandle widget, wzrd_color color);
void wz_widget_set_w(WzHandle w, int width);
void wz_widget_set_h(WzHandle h, int height);
void wz_widget_set_x(WzHandle w, int width);
void wz_widget_set_y(WzHandle h, int height);
void wz_widget_set_pos(WzHandle handle, int x, int y);
bool wzrd_handle_is_valid(WzHandle handle);
void wz_widget_set_size(WzHandle handle, int w, int h);
void wz_widget_set_border(WzHandle w, WzBorderType border_type);

#define wzrd_widget_set_style2(widget, style) wzrd_widget_set_style2_explicit(__LINE__, widget, style)

#endif