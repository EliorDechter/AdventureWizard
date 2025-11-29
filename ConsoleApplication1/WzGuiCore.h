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

#define WZ_LOG(...) printf(__VA_ARGS__)
//#define WZ_LOG(...) (void)0;

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
#define EGUI_BLACK     (WzColor){ 0, 0, 0, 255 }         // Black
#define EGUI_BLANK     (WzColor){ 0, 0, 0, 0 }           // Blank (Transparent)
#define EGUI_MAGENTA   (WzColor){ 255, 0, 255, 255 }     // Magenta
#define EGUI_RAYWHITE  (WzColor){ 245, 245, 245, 255 }   // My own White (raylib logo)

typedef enum WzSizePolicyFlag
{
	WzSizePolicyFlagGrow = 1 << 0,
	WzSizePolicyFlagExpand = 1 << 1,
	WzSizePolicyFlagShrink = 1 << 2,
} WzSizePolicyFlag;

typedef enum EguiState {
	WZRD_INACTIVE, EguiActivating, WZRD_ACTIVE, WZRD_DEACTIVATING
} wzrd_state;

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
	WzColor color;
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
} WzWidget;

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
	int widget_line_number;
	const char* file;
	wzrd_str str;
	WzRect dest_rect, src_rect;
	WzColor color;
	wzrd_texture texture;
	int z;
} WzDrawCommand;

typedef struct WzDrawCommandBuffer {
	WzDrawCommand commands[MAX_NUM_DRAW_COMMANDS];
	int count;
} WzDrawCommandBuffer;

typedef enum WzSizePolicy
{
	WzSizePolicyFixed,
	WzSizePolicyMinimum,
	WzSizePolicyMaximum,
	WzSizePolicyPreferred,
	WzSizePolicyExpanding,
	WzSizePolicyMinimumExpanding,
	WzSizePolicyMinimumIgnored,
} WzLayoutSizePolicy;

typedef enum FlexFit
{
	FlexFitLoose,
	FlexFitTight
} FlexFit;

#define	MAIN_AXIS_SIZE_TYPE_MIN  0X0
#define	MAIN_AXIS_SIZE_TYPE_MAX  0X1

typedef struct {
	WzWidget handle;
	int line;
	const char* file;

	// New layout stuff
	unsigned int constraint_min_w, constraint_min_h, constraint_max_h, constraint_max_w;
	WzWidget parent;
	unsigned char main_axis_size_type, size_type_vertical;
	FlexFit flex_fit; // Should the widget take all the space given to it 
	int w_offset, h_offset;

	// ...
	unsigned int actual_x, actual_y;
	unsigned int actual_w, actual_h;
	WzLayout layout;

	// Old
	bool disable_hover;
	WzWidget clip_widget;
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
	
	//int x, y;
	//unsigned int w, h;
	float percentage_w, percentage_h;
	//bool w_strech, h_strech;
	unsigned int flex_factor;

	//wzrd_structure structure;
	unsigned int pad_right, pad_bottom, pad_left, pad_top;

	int child_gap;
	bool fit_h, fit_w;
	WzAlignment alignment;
	bool best_fit;

	//wzrd_skin skin;
	WzColor font_color;
	WzColor color;
	WzColor b0, b1, b2, b3;
	WzBorderType border_type;
	WzBorderType window_border_type;

	const char* tag;
	const char* secondary_tag;

	unsigned int x, y;

	bool free_from_parent_horizontally, free_from_parent_vertically;

} WzWidgetData;

typedef struct CachedBox
{
	const char* tag;
	WzWidgetData box;
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
	WzColor label_color;
	WzColor label_item_selected_color;
} wzrd_stylesheet;

typedef struct wzrd_canvas {

	// Persistent
	WzWidget hovered_item, hot_item_previous, active_item, clicked_item, activating_item, deactivating_item, dragged_item;
	WzWidget right_resized_item, left_resized_item, bottom_resized_item, top_resized_item;
	WzWidget active_input_box;
	WzWidget hovered_items_list[MAX_NUM_HOVERED_ITEMS];
	int hovered_items_list_count;
	WzWidgetData hovered_boxes[MAX_NUM_HOVERED_ITEMS];
	int hovered_boxes_count;

	WzRect window;
	float input_box_timer;
	double tooltip_time;
	WzWidgetData dragged_box;
	bool clean;
	void (*get_string_size)(char*, int*, int*);

	wzrd_v2 mouse_pos, previous_mouse_pos, mouse_delta, screen_mouse_pos;
	wzrd_keyboard_keys keyboard_keys;
	wzrd_state mouse_left, mouse_right;

	// Frame ?
	WzWidgetData widgets[MAX_NUM_BOXES];
	int widgets_count;

	int boxes_in_stack_count, total_num_windows;

	Crate crates_stack[32];
	int current_crate_index;

	bool enable_input;

	int styles_count;
	wzrd_cursor cursor;
	WzDrawCommandBuffer command_buffer;

	//wzrd_skin panel_skin, panel_border_skin, panel_border_click_skin, command_button_skin, label_item_skin, label_item_selected_skin, label_skin, input_box_skin, command_button_on_skin, list_skin;
	//wzrd_structure panel_structure, command_button_structure, label_structure, input_box_structure;
	//wzrd_layout v_panel_layout, h_panel_layout, command_button_layout, input_box_layout, top_label_panel_layout;
	//wzrd_space command_button_space, toggle_icon_space, input_box_space;

#define MAX_NUM_CACHED_BOXES 128
	WzWidgetData cached_boxes[MAX_NUM_CACHED_BOXES];
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

typedef struct WzLayoutWidget
{
	unsigned int index;
} WzLayoutWidget;

#define MAX_NUM_VERTICES_IN_POLYGON 32

typedef struct wzrd_polygon {
	wzrd_v2 vertices[MAX_NUM_VERTICES_IN_POLYGON];
	int count;
} wzrd_polygon;

// CORE
WzWidget wz_begin(wzrd_canvas* gui, WzRect window,
	void (*get_string_size)(char*, int*, int*),
	wzrd_v2 mouse_pos, wzrd_state mouse_left, wzrd_keyboard_keys keys, bool enable_input);
void wzrd_end(wzrd_str* debug_str);
void wz_widget_set_pad(WzWidget w, unsigned int pad);
void wz_widget_set_child_gap(WzWidget widget, unsigned int child_gap);
void wz_widget_set_constraints(WzWidget widget, unsigned int min_w, unsigned int min_h, unsigned int max_w, unsigned int max_h);
WzWidget wz_widget_raw(WzWidget parent, const char* file, unsigned int line);
void wz_widget_add_rect(WzWidget widget, unsigned int w, unsigned int h, WzColor color);
void wz_widget_set_pad_bottom(WzWidget widget, unsigned int pad);
void wz_widget_set_pad_top(WzWidget widget, unsigned int pad);
void wz_widget_set_pad_left(WzWidget widget, unsigned int pad);
void wz_widget_set_pad_right(WzWidget widget, unsigned int pad);
void wz_widget_set_constraint_size(WzWidget widget, unsigned int w, unsigned int h);
void wz_widget_set_main_axis_size_min(WzWidget w);
void wz_widget_set_layer(WzWidget handle, unsigned int layer);
void wz_widget_set_fixed_size(WzWidget widget, unsigned int w, unsigned int h);
void wz_widget_set_layout(WzWidget handle, WzLayout layout);
void wz_widget_set_stretch_factor(WzWidget handle, unsigned int strech_factor);
WzWidget wzrd_widget_free(WzWidget parent);
void wzrd_box_add_child(WzWidget parent, WzWidget child);
WzWidgetData* wz_widget_get(WzWidget handle);
wzrd_str wzrd_str_create(char* str);
WzWidgetData* wzrd_box_get_last();
WzWidgetData* wzrd_box_find(wzrd_canvas* c, wzrd_str name);
int wzrd_box_get_current_index();
bool wzrd_box_is_active(WzWidgetData* box);
bool wzrd_is_releasing();
bool wzrd_box_is_dragged(WzWidgetData* box);
void wzrd_box_resize(wzrd_v2* size);
void wzrd_item_add(Item item, WzWidget box);
bool wzrd_box_is_hot(WzWidgetData* box);
bool wzrd_box_is_hot_using_canvas(wzrd_canvas* canvas, WzWidgetData* box);
bool wz_widget_is_equal(WzWidget a, WzWidget b);
void wz_widget_set_layer(WzWidget handle, unsigned int layer);
wzrd_canvas* wzrd_canvas_get();
WzWidgetData wzrd_widget_get_cached_box(const char* tag);
void wzrd_widget_tag(WzWidget widget, const char* str);
void wzrd_widget_clip(WzWidget handle);
bool wzrd_widget_is_deactivating(WzWidget handle);
bool wzrd_widget_is_active(WzWidget handle);
WzWidgetData wzrd_widget_get_cached_box_with_secondary_tag(const char* tag, const char* secondary_tag);
void wz_widget_set_color_old(WzWidget widget, WzColor color);
void wz_widget_set_constraint_w(WzWidget w, int width);
void wz_widget_set_constraint_h(WzWidget h, int height);
void wz_widget_set_x(WzWidget w, int width);
void wz_widget_set_y(WzWidget h, int height);
void wz_widget_set_pos(WzWidget handle, int x, int y);
bool wz_handle_is_valid(WzWidget handle);
void wz_widget_set_tight_constraints(WzWidget handle, int w, int h);
void wz_widget_set_border(WzWidget w, WzBorderType border_type);
void wz_widget_resize(WzWidget widget, int* w, int* h);
void wz_widget_set_flex_factor(WzWidget widget, unsigned int flex_factor);
void wz_widget_set_expanded(WzWidget widget);
void wz_widget_set_flex(WzWidget widget);
void wz_widget_set_size(WzWidget c, unsigned int w, unsigned int h);
void wz_widget_set_free_from_parent_horizontally(WzWidget w);
void wz_widget_set_free_from_parent_vertically(WzWidget w);
void wz_widget_set_color(WzWidget widget, unsigned int color);

// WIDGETS
WzWidget wzrd_label_button_raw(wzrd_str str, bool* result, WzWidget parent, const char *file, unsigned int line);
WzWidget wzrd_button_icon_raw(wzrd_texture texture, bool* released, WzWidget parent, const char *file, unsigned int line);
WzWidget wzrd_command_button_raw(wzrd_str str, bool* b, WzWidget parent, const char *file, unsigned int line);
WzWidget wzrd_dropdown_raw(int* selected_text, const wzrd_str* texts, int texts_count, int w, bool* active, WzWidget parent, const char *file, unsigned int line);
WzWidget wzrd_dialog_begin_raw(wzrd_v2* pos, wzrd_v2 size, bool* active, wzrd_str name, int layer, WzWidget parent, const char *file, unsigned int line);
WzWidget wzrd_command_toggle_raw(wzrd_str str, bool* active, WzWidget parent, const char *file, unsigned int line);
WzWidget wzrd_toggle_icon_raw(wzrd_texture texture, bool* active, WzWidget parent, const char *file, unsigned int line);
void wzrd_label_list_sorted_raw(wzrd_str* item_names, unsigned int count, int* items, unsigned int width, unsigned int height, unsigned int color, unsigned int* selected, bool* is_selected, WzWidget parent, const char *file, unsigned int line);
void wzrd_label_list_raw(wzrd_str* item_names, unsigned int count, unsigned int width, unsigned int height, unsigned int color, WzWidget* handles, unsigned int* selected, bool* is_selected, WzWidget parent, const char *file, unsigned int line);
WzWidget wzrd_input_box_raw(char* str, int* len, int max_num_keys, WzWidget parent, const char *file, unsigned int line);
WzWidget wzrd_handle_button_raw(bool* active, WzRect rect, WzColor color, wzrd_str name, WzWidget parent, const char *file, unsigned int line);
WzWidget wz_label_raw(wzrd_str str, WzWidget parent, const char *file, unsigned int line);
WzWidget wzrd_vbox_border_raw(wzrd_v2 size, WzWidget parent, const char *file, unsigned int line);
WzWidget wz_hbox_raw(WzWidget parent, const char *file, unsigned int line);
WzWidget wz_vbox_raw(WzWidget parent, const char *file, unsigned int line);

#define wzrd_label_button(str, result, parent) wzrd_label_button_raw(str, result, parent, __FILE__, __LINE__)
#define wzrd_button_icon(texture, released, parent) wzrd_button_icon_raw(texture, released, parent, __FILE__, __LINE__)
#define wzrd_command_button(str, b, parent) wzrd_command_button_raw(str, b, parent, __FILE__, __LINE__)
#define wzrd_dropdown(selected_text, texts, texts_count, w, active, parent) wzrd_dropdown_raw(selected_text, texts, texts_count, w, active, parent, __FILE__, __LINE__)
#define wzrd_dialog_begin(pos, size, active, name, layer, parent) wzrd_dialog_begin_raw(pos, size, active, name, layer, parent, __FILE__, __LINE__)
#define wzrd_command_toggle(str, active, parent) wzrd_command_toggle_raw(str, active, parent, __FILE__, __LINE__)
#define wzrd_toggle_icon(texture, active, parent) wzrd_toggle_icon_raw(texture, active, parent, __FILE__, __LINE__)
#define wzrd_label_list_sorted(item_names, count, items, width, height, color, selected, is_selected, parent) wzrd_label_list_sorted_raw(item_names, count, items, width, height, color, selected, is_selected, parent, __FILE__, __LINE__)
#define wzrd_label_list(item_names, count, width, height, color, handles, selected, is_selected, parent) wzrd_label_list_raw(item_names, count, width, height, color, handles, selected, is_selected, parent, __FILE__, __LINE__)
#define wzrd_input_box(str, len, max_num_keys, parent) wzrd_input_box_raw(str, len, max_num_keys, parent, __FILE__, __LINE__)
#define wzrd_handle_button(active, rect, color, name, parent) wzrd_handle_button_raw(active, rect, color, name, parent, __FILE__, __LINE__)
#define wz_label(str, parent) wz_label_raw(str, parent, __FILE__, __LINE__)
#define wzrd_vbox_border(size, parent) wzrd_vbox_border_raw(size, parent, __FILE__, __LINE__)
#define wz_hbox(parent) wz_hbox_raw(parent,  __FILE__, __LINE__)
#define wz_vbox(parent) wz_vbox_raw(parent,  __FILE__, __LINE__)
#define wz_widget(parent) wz_widget_raw(parent,  __FILE__, __LINE__)
void wzrd_dialog_end(bool active);

#endif