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
#include "WzLayout.h"
#include "Strings.h"
#include "WzLayout.h"

//#define WZ_LOG(...) printf(__VA_ARGS__)
#define wz_layout_log(...) (void)0;

#define WZ_BORDER_SIZE 1

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

#define MAX_NUM_WIDGETS 512

#define MAX_NUM_ITEMS 32
#define MAX_NUM_CHILDREN 256

#define Stringify1(x) #x
#define Stringify(x) Stringify1(x)
#define EditorDropDownBox(name) EditorDropDownBoxRaw(Stringify(__LINE__), name)
#define EditorToggle(name) EditorToggleRaw(Stringify(__LINE__), name)


typedef enum EditorWindow { EditorWindow_Lists, EditorWindow_Slice_Spritesheet } EditorWindow;
typedef enum EditorTab { EditorTab_Entities, EditorTab_Events, EditorTab_Areas, EditorTab_Inventory } EditorTab;

typedef enum PanelId { PanelId_None, PanelId_Window, PanelId_TopPanel, PanelId_RightPanel, PanelId_GameScreen, PanelId_ModifyEntityDropDown } PanelId;
typedef enum EditorWindowId { EditorWindowId_ModifyEntity, EditorWindowId_DrawTool } EditorWindowId;

typedef struct WzColor {
	unsigned char r, g, b, a;
} WzColor;

// Some Basic Colors
// NOTE: Custom raylib color palette for amazing visuals on WHITE background
#define EGUI_LIGHTGRAY (WzColor){ 200, 200, 200, 255}   // Light Gray
#define EGUI_LIGHTESTGRAY (WzColor){ 225, 225, 225, 255 }   // Lightest Gray
#define EGUI_GRAY      (WzColor){ 180, 180, 180, 255 }   // Gray
#define EGUI_DARKGRAY  (WzColor){ 80, 80, 80, 255 }      // Dark Gray
#define WZ_YELLOW    (WzColor){ 255, 255, 0, 255 }     // Yellow
#define EGUI_GOLD      (WzColor){ 255, 203, 0, 255 }     // Gold
#define EGUI_ORANGE    (WzColor){ 255, 161, 0, 255 }     // Orange
#define EGUI_PINK      (WzColor){ 255, 109, 194, 255 }   // Pink
#define WZ_RED       (WzColor){ 255, 0, 0, 255 }     // Red
#define EGUI_MAROON    (WzColor){ 190, 33, 55, 255 }     // Maroon
#define WZ_GREEN     (WzColor){ 0, 228, 48, 255 }      // Green
#define EGUI_LIME      (WzColor){ 0, 158, 47, 255 }      // Lime
#define EGUI_DARKGREEN (WzColor){ 0, 117, 44, 255 }      // Dark Green
#define EGUI_SKYBLUE   (WzColor){ 102, 191, 255, 255 }   // Sky Blue
#define WZ_BLUE      (WzColor){ 0, 0, 255, 255 }     // Blue
#define EGUI_DARKBLUE  (WzColor){ 0, 82, 172, 255 }      // Dark Blue
#define EGUI_PURPLE    (WzColor){ 200, 122, 255, 255 }   // Purple
#define EGUI_VIOLET    (WzColor){ 135, 60, 190, 255 }    // Violet
#define EGUI_DARKPURPLE(EguiColor){ 112, 31, 126, 255 }    // Dark Purple
#define EGUI_BEIGE     (WzColor){ 211, 176, 131, 255 }   // Beige
#define EGUI_BROWN     (WzColor){ 127, 106, 79, 255 }    // Brown
#define EGUI_DARKBROWN (WzColor){ 76, 63, 47, 255 }      // Dark Brown

#define WZ_WHITE     (WzColor){ 255, 255, 255, 255 }   // White
#define EGUI_WHITE2     (WzColor){ 230, 230, 230, 255 }   // White2
#define WZ_BLACK     (WzColor){ 0, 0, 0, 255 }         // Black
#define EGUI_BLANK     (WzColor){ 0, 0, 0, 0 }           // Blank (Transparent)
#define EGUI_MAGENTA   (WzColor){ 255, 0, 255, 255 }     // Magenta
#define EGUI_RAYWHITE  (WzColor){ 245, 245, 245, 255 }   // My own White (raylib logo)


typedef enum WzSizePolicyFlag
{
	WzSizePolicyFlagGrow = 1 << 0,
	WzSizePolicyFlagExpand = 1 << 1,
	WzSizePolicyFlagShrink = 1 << 2,
} WzSizePolicyFlag;

typedef enum WzState {
	WZ_INACTIVE, WZ_ACTIVATING, WZ_ACTIVE, WZ_DEACTIVATING
} WzState;

typedef struct wzrd_rect_struct {
	int x, y;
	unsigned int w, h;
} WzRect;

typedef struct wzrd_v2 {
	int x, y;
} wzrd_v2;

#define DEBUG_PANELS 0

typedef enum WzBorderType
{
	WZ_BORDER_TYPE_NONE, WZ_BORDER_TYPE_DEFAULT, WZ_BORDER_TYPE_BLACK, WZ_BORDER_TYPE_CLICKED,
	BorderType_InputBox, BorderType_BottomLine, BorderType_LeftLine, BorderType_Custom
}
WzBorderType;

typedef struct wzrd_texture {
	void* data;
	float w, h;
} WzTexture;

typedef enum ItemType {
	ItemType_None,
	WZ_WIDGET_ITEM_TYPE_STRING,
	ItemType_Rect,
	ItemType_RectAbsolute,
	ItemType_Texture,
	ItemType_DropdownIcon,
	ItemType_CloseIcon,
	ItemType_VerticalLine,
	ItemType_LineAbsolute,
	ItemType_DottedLineAbsolute,
	ItemType_HorizontalLineAbsolute,
	ItemType_HorizontalLine,
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
	char str[128];
	size_t len;
} WzStr;

typedef struct Item {
	ItemType type;
	wzrd_v2 size;
	WzColor color;
	int margin_left, margin_right, margin_top, margin_bottom;
	union {
		//wzrd_str str;
		WzStr str;
		WzTexture texture;
		WzRect rect;
		Line line;
	} val;
	bool scissor;
	bool center_w, center_h;
	unsigned w, h;
	int x, y;
} WzWidgetItem;

typedef enum ButtonType { ButtonType_None, ButtonType_Flat, ButtonType_ThreeDimensional } ButtonType;

typedef struct wzrd_widget_id {
	WzStr val;
} wzrd_widget_id;

typedef struct wzrd_handle
{
	unsigned handle;
} WzWidget;

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
	DrawCommandType_RectAbsolute,
	DrawCommandType_Line,
	DrawCommandType_LineDotted,
	DrawCommandType_VerticalLine,
	DrawCommandType_HorizontalLine,
	DrawCommandType_String,
	DrawCommandType_Texture,
	DrawCommandType_Clip,
	DrawCommandType_StopClip,
	WZ_DRAW_COMMAND_TYPE_D
} EguiDrawCommandType;

typedef struct EguiDrawCommand {
	EguiDrawCommandType type;
	char* source;
	WzStr str;
	union {
		struct { WzRect dest_rect, src_rect; };
		Line line;
	};
	WzColor color;
	WzTexture texture;
	int z;
} WzDrawCommand;

typedef struct WzDrawCommandBuffer {
	WzDrawCommand commands[MAX_NUM_DRAW_COMMANDS];
	int count;
} WzDrawCommandBuffer;


#define MAX_NUM_SOURCES 8

typedef struct WzSource
{
	char* file;
	unsigned int line;
} WzSource;

#define MAX_SOURCE_SIZE 128

typedef struct
{
	WzWidget handle;
	//int line;
	//const char* file;
	char source[MAX_SOURCE_SIZE];

	// New layout stuff
	unsigned int constraint_min_w, constraint_min_h, constraint_max_h, constraint_max_w;
	WzWidget parent;
	unsigned char main_axis_size_type, size_type_vertical;
	unsigned int flex_fit; // Should the widget take all the space given to it 
	int w_offset, h_offset;

	// ...
	int actual_x, actual_y;
	unsigned int actual_w, actual_h;
	unsigned int layout;

	// Old
	bool disable_hover;
	WzWidget clip_widget;
	bool disable_input;
	bool is_draggable, is_slot;
	bool free;
	//int content_w, content_h;

	WzWidget children[MAX_NUM_CHILDREN];
	unsigned char children_count;

	unsigned int free_children[MAX_NUM_CHILDREN];
	unsigned char free_children_count;

	WzWidgetItem items[MAX_NUM_ITEMS];
	unsigned int items_count;
	unsigned int layer;
	bool bring_to_front;
	//unsigned int index;
	bool is_selected;
	
	float percentage_w, percentage_h;
	unsigned int flex_factor;

	unsigned int margin_right, margin_bottom, margin_left, margin_top;
	unsigned int pad_right, pad_bottom, pad_left, pad_top;

	int child_gap;
	bool fit_h, fit_w;
	bool best_fit;
	unsigned int cross_axis_alignment;
	unsigned int main_axis_alignment;

	//wzrd_skin skin;
	WzColor font_color;
	WzColor color;
	WzColor b0, b1, b2, b3;
	WzBorderType border_type;
	WzBorderType window_border_type;

	const char* tag;
	const char* secondary_tag;

	unsigned int x, y;

	bool free_from_parent;
	bool cull;
	
	bool ignore_unique_id;

	unsigned type;

} WzWidgetData;

typedef struct CachedBox
{
	const char* tag;
	WzWidgetData box;
} CachedBox;

typedef struct wzrd_keyboard_key {
	char val;
	WzState state;
} wzrd_keyboard_key;

typedef struct wzrd_keyboard_keys {
	wzrd_keyboard_key keys[32];
	int count;
} WzKeyboardKeys;

typedef struct wzrd_icons {
	WzTexture close, delete, entity, play, pause, stop, dropdown;
} wzrd_icons;

typedef enum wzrd_cursor { wzrd_cursor_default, wzrd_cursor_hand, wzrd_cursor_vertical_arrow, wzrd_cursor_horizontal_arrow } wzrd_cursor;


typedef struct wzrd_stylesheet
{
	WzColor label_color;
	WzColor label_item_selected_color;
} wzrd_stylesheet;

typedef void (*ScrollbarCallback)(WzWidget, WzWidget, WzWidget);

typedef struct WzScrollbar
{
	WzWidget panel, scrollbar, content, top_button, bottom_button;
	unsigned int *scroll;
} WzScrollbar;

#define MAX_NUM_CACHED_BOXES 128


typedef struct WzTreeNodeData
{
	const char* name;
	WzTexture texture;
	bool expand;
	unsigned children_index;
	unsigned children_count;
	unsigned depth;
	WzWidget row_widget, icon_widget, expand_widget;
	bool visible;
} WzTreeNodeData;

typedef struct WzTreeNode
{
	unsigned index;
} WzTreeNode;

typedef struct WzTree
{
	WzTreeNodeData* nodes;
	unsigned nodes_count;
	WzTreeNode* children_indices;
	unsigned children_count;
	WzWidget menu;
	WzWidget selected_row;
} WzTree;

#define MAX_NUM_PERSISTENT_WIDGETS 1024

typedef struct WzGui
{
	WzWidgetData persistent_widgets[MAX_NUM_PERSISTENT_WIDGETS];
	unsigned persistent_widgets_count;

	// Persistent
	WzWidget hovered_item, hot_item_previous, active_item, clicked_item, activating_item, deactivating_item, dragged_item;
	WzWidget right_resized_item, left_resized_item, bottom_resized_item, top_resized_item;
	WzWidget active_input_box;
	WzWidget *hovered_items_list;
	int hovered_items_list_count;
	WzWidgetData *hovered_boxes;
	int hovered_boxes_count;

	WzRect window;
	float input_box_timer;
	double tooltip_time;
	WzWidgetData dragged_box;
	bool clean;
	void (*get_string_size)(char*, int*, int*);

	wzrd_v2 mouse_pos, previous_mouse_pos, mouse_delta, screen_mouse_pos;
	WzKeyboardKeys keyboard_keys;
	WzState mouse_left, mouse_right;

	// Frame ?
	WzWidgetData *widgets;
	bool *free_widgets;
	//unsigned widgets_count;

	int boxes_in_stack_count, total_num_windows;

	Crate crates_stack[32];
	int current_crate_index;

	bool enable_input;

	int styles_count;
	wzrd_cursor cursor;
	WzDrawCommandBuffer commands_buffer;

	WzWidgetData *cached_boxes;
	int cached_boxes_count;

	wzrd_stylesheet stylesheet;

	WzRect *rects; // aka Final Layout

	WzScrollbar* scrollbars;
	unsigned int scrollbars_count;

	unsigned int *boxes_indices;

	WzWidget focused_widget;

	WzTree trees[8];

} WzGui;

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
	WzStr val[MAX_NUM_LABELS];
	int count;
} Label_list;

typedef struct WzLayoutWidget
{
	unsigned int index;
} WzLayoutWidget;

#define MAX_NUM_VERTICES_IN_POLYGON 32

typedef struct wzrd_polygon {
	wzrd_v2 vertices[MAX_NUM_VERTICES_IN_POLYGON];
	int count;
} wzrd_polygon;

enum
{
	WZ_EXPANSION_NONE,
	WZ_EXPANSION_SHRUNK,
	WZ_EXPANSION_EXPANDED,
};

// CORE
WzWidget wz_create_handle();
void wz_gui_init(WzGui *gui);
void wz_gui_deinit(WzGui* gui);
WzWidget wz_gui_begin(WzGui* gui,
	unsigned window_w, unsigned  window_h,
	unsigned mouse_x, unsigned mouse_y,
	void (*get_string_size)(char*, int*, int*),
	WzState left_mouse_state,
	WzKeyboardKeys keys,
	bool enable_input);
void wz_spacer(WzWidget parent);
void wz_gui_end(WzStr* debug_str);
void wz_widget_set_margins(WzWidget w, unsigned int pad);
void wz_widget_set_pad(WzWidget w, unsigned  pad);
void wz_widget_set_child_gap(WzWidget widget, unsigned int child_gap);
void wz_widget_set_constraints(WzWidget widget, unsigned int min_w, unsigned int min_h, unsigned int max_w, unsigned int max_h);
WzWidget wz_widget_raw(WzWidget parent, const char* file, unsigned int line);
void wz_widget_add_rect(WzWidget widget, unsigned int w, unsigned int h, WzColor color);
void wz_widget_set_margin_bottom(WzWidget widget, unsigned int pad);
void wz_widget_set_margin_top(WzWidget widget, unsigned int pad);
void wz_widget_set_margin_left(WzWidget widget, unsigned int pad);
void wz_widget_set_margin_right(WzWidget widget, unsigned int pad);
void wz_widget_set_max_constraints(WzWidget widget, unsigned int w, unsigned int h);
void wz_widget_set_main_axis_size_min(WzWidget w);
void wz_widget_set_layer(WzWidget handle, unsigned int layer);
void wz_widget_set_fixed_size(WzWidget widget, unsigned int w, unsigned int h);
void wz_widget_set_layout(WzWidget handle, unsigned int layout);
void wz_widget_set_stretch_factor(WzWidget handle, unsigned int strech_factor);
WzWidget wzrd_widget_free(WzWidget parent);
void wz_widget_add_child(WzWidget parent, WzWidget child);
WzWidgetData* wz_widget_get(WzWidget handle);
WzStr wz_str_create(char* str);
int wzrd_box_get_current_index();
bool wzrd_box_is_active(WzWidgetData* box);
bool wzrd_box_is_activating(WzWidgetData* box);
bool wzrd_is_releasing();
bool wzrd_box_is_dragged(WzWidgetData* box);
void wzrd_box_resize(wzrd_v2* size);
void wz_widget_add_item(WzWidget box, WzWidgetItem item);
bool wzrd_box_is_hot(WzWidgetData* box);
bool wzrd_box_is_hot_using_canvas(WzGui* canvas, WzWidgetData* box);
bool wz_widget_is_equal(WzWidget a, WzWidget b);
void wz_widget_set_layer(WzWidget handle, unsigned int layer);
WzGui* wzrd_canvas_get();
WzWidget wz_widget_persistent(WzWidget parent, WzWidgetData widget_data);
WzWidgetData wzrd_widget_get_cached_box(const char* tag);
void wz_widget_add_tag(WzWidget widget, const void* str);
void wz_widget_clip(WzWidget handle);
bool wz_widget_is_deactivating(WzWidget handle);
bool wz_widget_is_active(WzWidget handle);
bool wz_widget_is_interacting(WzWidget handle);
bool wz_widget_is_activating(WzWidget handle);
WzWidgetData wzrd_widget_get_cached_box_with_secondary_tag(const char* tag, const char* secondary_tag);
void wz_widget_set_color_old(WzWidget widget, WzColor color);
void wz_widget_set_max_constraint_w(WzWidget w, int width);
void wz_widget_set_max_constraint_h(WzWidget h, int height);
void wz_widget_set_x(WzWidget w, int x);
void wz_widget_set_y(WzWidget h, int y);
void wz_widget_data_set_x(WzWidgetData *w, int x);
void wz_widget_data_set_y(WzWidgetData *h, int y);
void wz_widget_set_pos(WzWidget handle, int x, int y);
void wz_widget_data_set_pos(WzWidgetData *handle, int x, int y);
bool wz_handle_is_valid(WzWidget handle);
void wz_widget_set_tight_constraints(WzWidget handle, unsigned w, unsigned h);
void wz_widget_set_border(WzWidget w, WzBorderType border_type);
void wz_widget_data_set_tight_constraints(WzWidgetData *handle, unsigned w, unsigned h);
void wz_widget_data_set_border(WzWidgetData *w, WzBorderType border_type);
WzWidgetData wz_widget_create(WzWidget parent);
void wz_widget_resize(WzWidget widget, int* w, int* h);
void wz_widget_set_flex_factor(WzWidget widget, unsigned int flex_factor);
void wz_widget_set_expanded(WzWidget widget);
void wz_widget_set_flex(WzWidget widget);
void wz_widget_set_size(WzWidget c, unsigned int w, unsigned int h);
void wz_widget_set_free_from_parent(WzWidget w);
void wz_widget_set_free_from_parent_vertically(WzWidget w);
void wz_widget_set_color(WzWidget widget, unsigned int color);
void wz_widget_set_cross_axis_alignment(WzWidget widget, unsigned int cross_axis_alignment);
void wz_widget_set_main_axis_alignment(WzWidget widget, unsigned int cross_axis_alignment);
void wz_widget_ignore_unique_id(WzWidget widget);

// WIDGETS
WzWidget wzrd_label_button_raw(WzStr str, bool* result, WzWidget parent, const char *file, unsigned int line);
WzWidget wz_button_icon_raw(WzWidget parent, bool* result, WzTexture texture, const char* file, unsigned int line);
WzWidget wz_command_button_raw(WzStr str, bool* b, WzWidget parent, const char *file, unsigned int line);
WzWidget wzrd_dropdown_raw(int* selected_text, const WzStr* texts, int texts_count, int w, bool* active, WzWidget parent, const char *file, unsigned int line);
WzWidget wzrd_dialog_begin_raw(int *x, int *y, unsigned *w, unsigned *h, bool* active, WzStr name, int layer, WzWidget parent, const char *file, unsigned int line);
WzWidget wz_command_toggle_raw(WzWidget parent, WzStr str, bool* active,  const char *file, unsigned int line);
WzWidget wz_icon_toggle_raw(WzWidget parent, WzTexture texture, unsigned w, unsigned h, bool* active, const char *file, unsigned int line);
void wzrd_label_list_sorted_raw(WzStr* item_names, unsigned int count, int* items, unsigned int width, unsigned int height, unsigned int color, unsigned int* selected, bool* is_selected, WzWidget parent, const char *file, unsigned int line);
void wzrd_label_list_raw(WzStr* item_names, unsigned int count, unsigned int width, unsigned int height, unsigned int color, WzWidget* handles, unsigned int* selected, bool* is_selected, WzWidget parent, const char *file, unsigned int line);
WzWidget wzrd_input_box_raw(char* str, int* len, int max_num_keys, WzWidget parent, const char *file, unsigned int line);
WzWidget wzrd_handle_button_raw(bool* active, WzRect rect, WzColor color, WzStr name, WzWidget parent, const char *file, unsigned int line);
WzWidget wz_label_raw(WzWidget parent, WzStr str, const char *file, unsigned int line);
WzWidget wzrd_vbox_border_raw(wzrd_v2 size, WzWidget parent, const char *file, unsigned int line);
WzWidget wz_hbox_raw(WzWidget parent, const char *file, unsigned int line);
WzWidget wz_vbox_raw(WzWidget parent, const char *file, unsigned int line);
WzWidget wzrd_label_button_activating_raw(WzStr str, bool* active, WzWidget parent, const char* file, unsigned int line);
WzWidget wz_tree_add_row_raw(WzTree *tree, WzStr str, WzTexture texture, unsigned depth,
	bool* expand, bool* selected, WzTreeNodeData* node, const char* file, unsigned line);
WzWidget wz_toggle_raw(WzWidget parent, unsigned w, unsigned h, WzColor color,
	bool* active, const char* file_name, unsigned int line);
WzWidget wz_texture_raw(WzWidget parent, WzTexture texture, unsigned w, unsigned h, const char* file_name, unsigned int line);
void wz_widget_add_horizontal_line(WzWidget widget, unsigned w);
void wz_widget_add_vertical_line(WzWidget widget, unsigned h);
void wz_do_layout_refactor_me();
void wz_widget_add_rect_absolute(WzWidget widget, int x, int y, unsigned w, unsigned h, WzColor color);
WzWidget wz_vpanel_raw(WzWidget parent, const char* file, unsigned int line);
WzWidget wz_hpanel_raw(WzWidget parent, const char* file, unsigned int line);
WzWidget wz_panel_raw(WzWidget parent, const char* file, unsigned int line);


#define wz_texture(parent, texture, w, h, file_name, line) wz_texture_raw(parent, texture, w, h, __FILE__, __LINE__)
#define wz_toggle(parent, w, h, color, active, file_name, line) wz_toggle_raw(parent, w, h, color, active, __FILE__, __LINE__)
#define wz_tree_add_row(tree, str, texture, depth, expand, selected, node) wz_tree_add_row_raw(tree, str, texture, depth, expand, selected, node, __FILE__, __LINE__)
#define wz_label_button(str, result, parent) wzrd_label_button_raw(str, result, parent, __FILE__, __LINE__)
#define wz_button_icon(parent, result, texture) wz_button_icon_raw(parent, result, texture, __FILE__, __LINE__)
#define wz_command_button(str, b, parent) wz_command_button_raw(str, b, parent, __FILE__, __LINE__)
#define wzrd_dropdown(selected_text, texts, texts_count, w, active, parent) wzrd_dropdown_raw(selected_text, texts, texts_count, w, active, parent, __FILE__, __LINE__)
#define wzrd_dialog_begin(x, y, w, h, active, name, layer, parent) wzrd_dialog_begin_raw(x, y, w, h, active, name, layer, parent, __FILE__, __LINE__)
#define wz_command_toggle(parent, str, active) wz_command_toggle_raw(parent, str, active, __FILE__, __LINE__)
#define wz_icon_toggle(parent, texture, w, h, active) wz_icon_toggle_raw(parent, texture, w, h, active, __FILE__, __LINE__)
#define wzrd_label_list_sorted(item_names, count, items, width, height, color, selected, is_selected, parent) wzrd_label_list_sorted_raw(item_names, count, items, width, height, color, selected, is_selected, parent, __FILE__, __LINE__)
#define wzrd_label_list(item_names, count, width, height, color, handles, selected, is_selected, parent) wzrd_label_list_raw(item_names, count, width, height, color, handles, selected, is_selected, parent, __FILE__, __LINE__)
#define wzrd_input_box(str, len, max_num_keys, parent) wzrd_input_box_raw(str, len, max_num_keys, parent, __FILE__, __LINE__)
#define wzrd_handle_button(active, rect, color, name, parent) wzrd_handle_button_raw(active, rect, color, name, parent, __FILE__, __LINE__)
#define wz_label(parent, str) wz_label_raw(parent, str, __FILE__, __LINE__)
#define wzrd_vbox_border(size, parent) wzrd_vbox_border_raw(size, parent, __FILE__, __LINE__)
#define wz_hbox(parent) wz_hbox_raw(parent,  __FILE__, __LINE__)
#define wz_vbox(parent) wz_vbox_raw(parent,  __FILE__, __LINE__)
#define wz_widget(parent) wz_widget_raw(parent,  __FILE__, __LINE__)
#define wzrd_label_button_activating(str, active, parent) wzrd_label_button_activating_raw(str, active, parent, __FILE__, __LINE__)
#define wz_vpanel(parent) wz_vpanel_raw(parent, __FILE__, __LINE__)
#define wz_hpanel(parent) wz_hpanel_raw(parent, __FILE__, __LINE__)
#define wz_panel(parent) wz_panel_raw(parent, __FILE__, __LINE__)
WzWidget wz_widget_add_to_frame(WzWidget parent, WzWidgetData widget);

WzWidget wz_scroll_box(wzrd_v2 size, unsigned int* scroll, WzWidget parent, const void* tag);
void wz_add_persistent_widget(WzWidgetData widget);


// LAYOUT

#define WZL_LOG(...) (void)0;

#define WZ_FLEX_FIT_LOOSE 0
#define WZ_FLEX_FIT_TIGHT 1

#define WZ_LAYOUT_NONE 0
#define WZ_LAYOUT_HORIZONTAL 1
#define WZ_LAYOUT_VERTICAL 2

#define	MAIN_AXIS_SIZE_TYPE_MIN  0
#define	MAIN_AXIS_SIZE_TYPE_MAX  1

#define WZ_UINT_MAX 4294967295

#define WZ_LOG_MESSAGE_MAX_SIZE 256

//typedef enum {
//	WzAlignVCenter = 1 << 0,
//	WzAlignHCenter = 1 << 1,
//} WzAlignment;

typedef struct wzl_str
{
	char* str;
	unsigned int len;
} wzl_str;

typedef struct WzlRect
{
	//unsigned int index;
	int x, y;
	unsigned int w, h;
} WzLayoutRect;

enum
{
	CROSS_AXIS_ALIGNMENT_START,
	CROSS_AXIS_ALIGNMENT_END,
	WZ_CROSS_AXIS_ALIGNMENT_CENTER,
	WZ_CROSS_AXIS_ALIGNMENT_STRETCH,
	CROSS_AXIS_ALIGNMENT_BASELINE,
	CROSS_AXIS_ALIGNMENT_TOTAL,
};

enum
{
	WZ_MAIN_AXIS_ALIGNMENT_START,
	WZ_MAIN_AXIS_ALIGNMENT_END,
	WZ_MAIN_AXIS_ALIGNMENT_CENTER,
};

#define WZ_LAYOUT_MAX_NUM_SOURCES 8

typedef struct WzLayoutSource
{
	char* file;
	unsigned int line;
} WzLayoutSource;

#if 0
typedef struct WzWidgetDescriptor
{
	char* source;
	unsigned handle;
	unsigned int constraint_min_w, constraint_min_h, constraint_max_w, constraint_max_h;
	unsigned int layout;
	unsigned int pad_left, pad_right, pad_top, pad_bottom;
	unsigned int gap;
	unsigned int* children;
	unsigned int children_count;
	unsigned int flex_factor;
	unsigned char free_from_parent;
	unsigned char flex_fit;
	unsigned char main_axis_size_type;
	unsigned int alignment;
	unsigned int cross_axis_alignment;
	int x, y;
	unsigned margin_left, margin_right, margin_top, margin_bottom;
} WzWidgetDescriptor;
#endif


enum
{
	WZ_LAYOUT_STAGE_NON_FLEX_CHILDREN,
	WZ_LAYOUT_STAGE_FLEX_CHILDREN,
	WZ_LAYOUT_STAGE_PARENT,
};


typedef struct WzDebugInfo
{
	unsigned int stage;
	unsigned int index;
	unsigned int constraint_min_w, constraint_min_h, constraint_max_w, constraint_max_h;
	unsigned int x, y, w, h;
} WzDebugInfo;

typedef struct WzLogMessage
{
	char str[WZ_LOG_MESSAGE_MAX_SIZE];
} WzLogMessage;

void wz_do_layout(unsigned int index,
	WzWidgetData* widgets, WzLayoutRect* rects,
	unsigned int count, unsigned int* failed);

#endif