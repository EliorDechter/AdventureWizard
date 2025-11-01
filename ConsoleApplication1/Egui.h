// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com


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
} wzrd_rect_struct;

typedef struct wzrd_v2 {
	int x, y;
} wzrd_v2;

#define DEBUG_PANELS 0

typedef enum BorderType { BorderType_None, BorderType_Default, BorderType_Black, BorderType_Clicked, BorderType_InputBox, BorderType_BottomLine, BorderType_LeftLine, BorderType_Custom } wzrd_border_type;

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
		wzrd_rect_struct rect;
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
} wzrd_handle;

typedef enum {
	wzrd_box_type_default,
	wzrd_box_type_button,
	wzrd_box_type_resizable,
	wzrd_box_type_input_box,
	wzrd_box_type_flat_button,
	wzrd_box_type_crate
} wzrd_box_type;

typedef struct wzrd_style_structure
{
	int pad_right, pad_bottom, pad_left, pad_top;
} wzrd_structure;

typedef struct wzrd_style_skin
{
	wzrd_color font_color;
	wzrd_color color;
	wzrd_color b0, b1, b2, b3;
	wzrd_border_type border_type;
	wzrd_border_type window_border_type;
} wzrd_skin;

typedef struct wzrd_style_space
{
	int x, y, w, h;
} wzrd_space;

typedef struct wzrd_style_layout
{
	int child_gap;
	bool row_mode;
	bool fit_h, fit_w;
	bool center_x, center_y;
	bool best_fit;
} wzrd_layout;

typedef struct Box {
	wzrd_handle handle;
	wzrd_box_type type;

	int x_internal, y_internal, w_internal, h_internal;

	bool disable_hover;
	bool clip;

	bool disable_input;
	bool is_draggable, is_slot;

	// TODO: Find a better more descriptive name!
	bool free;

	int* scrollbar_x, * scrollbar_y;
	wzrd_v2 content_size;

	int children[MAX_NUM_CHILDREN];
	unsigned char children_count;

	int free_children[MAX_NUM_CHILDREN];
	unsigned char free_children_count;

	Item items[MAX_NUM_ITEMS];
	int items_count;

	unsigned int layer;
	bool bring_to_front;
	int index;
	bool is_selected;

	int space, structure, layout, skin;

} wzrd_box;

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
	DrawCommandType_Clip
} EguiDrawCommandType;

typedef struct EguiDrawCommand {
	EguiDrawCommandType type;
	wzrd_rect_struct dest_rect, src_rect;
	wzrd_str str;
	wzrd_color color;
	wzrd_texture texture;
	int z;
} wzrd_draw_command;

typedef struct wzrd_draw_commands_buffer {
	wzrd_draw_command commands[MAX_NUM_DRAW_COMMANDS];
	int count;
} wzrd_draw_commands_buffer;

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

typedef struct wzrd_canvas {

	// Persistent
	wzrd_handle hovered_item, hot_item_previous, active_item, clicked_item, activating_item, deactivating_item, dragged_item;
	wzrd_handle right_resized_item, left_resized_item, bottom_resized_item, top_resized_item;
	wzrd_handle active_input_box;
	wzrd_handle hovered_items_list[MAX_NUM_HOVERED_ITEMS];
	int hovered_items_list_count;
	wzrd_box hovered_boxes[MAX_NUM_HOVERED_ITEMS];
	int hovered_boxes_count;

	wzrd_rect_struct window;
	float input_box_timer;
	double tooltip_time;
	wzrd_box dragged_box;
	bool clean;
	void (*get_string_size)(char*, int*, int*);

	wzrd_v2 mouse_pos, previous_mouse_pos, mouse_delta, screen_mouse_pos;
	wzrd_keyboard_keys keyboard_keys;
	wzrd_state mouse_left, mouse_right;

	// Frame ?
	wzrd_box boxes[MAX_NUM_BOXES];
	int boxes_count;

	int boxes_in_stack_count, total_num_windows;

	Crate crates_stack[32];
	int current_crate_index;

	bool enable_input;

#define MAX_NUM_STYLES 256
	wzrd_structure style_structures[MAX_NUM_STYLES];
	unsigned int style_structures_count;
	wzrd_space style_spaces[MAX_NUM_STYLES];
	unsigned int style_spaces_count;
	wzrd_layout style_layouts[MAX_NUM_STYLES];
	unsigned int style_layouts_count;
	wzrd_skin style_skins[MAX_NUM_STYLES];
	unsigned int style_skins_count;
	 
	int styles_count;
	wzrd_cursor cursor;
	wzrd_draw_commands_buffer command_buffer;

	int panel_skin, panel_border_skin, panel_border_click_skin, panel_structure, v_panel_layout, h_panel_layout;
	int command_button_space, command_button_skin, command_button_structure, command_button_layout;
	int toggle_icon_space;
	int label_item_skin;
	int label_skin, label_structure;
	int input_box_skin, input_box_structure, input_box_space, input_box_layout;
	int command_button_on_skin;
	int list_skin;

	int top_label_panel_layout;

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

typedef struct wzrd_style
{
	int skin, layout, structure, space;
} wzrd_style;


// GENERAL
wzrd_handle wzrd_begin(wzrd_canvas* gui, wzrd_rect_struct window,
	void (*get_string_size)(char*, int*, int*),
	wzrd_v2 mouse_pos, wzrd_state mouse_left, wzrd_keyboard_keys keys, bool enable_input);
void wzrd_end(wzrd_str* debug_str);

// WIDGETS
wzrd_handle wzrd_window(wzrd_rect_struct rect, wzrd_handle parent);
wzrd_handle wzrd_hbox(wzrd_v2 size, wzrd_handle parent);
wzrd_handle wzrd_label_button(wzrd_str str, bool* result, wzrd_handle parent);
wzrd_handle wzrd_button_icon(wzrd_texture texture, bool* released, wzrd_handle parent);
wzrd_handle wzrd_command_button(wzrd_str str, bool* b, wzrd_handle parent);
void wzrd_dropdown(int* selected_text, const wzrd_str* str, int str_count, int w, bool* active);
wzrd_handle wzrd_dialog_begin(wzrd_v2* pos, wzrd_v2 size, bool* active, wzrd_str name, int layer, wzrd_handle parent);
void wzrd_dialog_end(bool active);
wzrd_handle wzrd_command_toggle(wzrd_str str, bool* active, wzrd_handle parent);
wzrd_handle wzrd_toggle_icon(wzrd_texture texture, bool* active, wzrd_handle parent);
void wzrd_label_list_sorted(wzrd_str* item_names, unsigned int count, int* items, wzrd_v2 size, unsigned int* selected, bool* is_selected, wzrd_handle parent);
void wzrd_label_list(wzrd_str* item_names, unsigned int count, wzrd_v2 size, wzrd_handle* handles, unsigned int* selected, bool* is_selected, wzrd_handle parent);
void wzrd_input_box(char* str, int* len, int max_num_keys, wzrd_handle parent);
wzrd_handle wzrd_handle_button(bool* active, wzrd_rect_struct rect, wzrd_color color, wzrd_str name, wzrd_handle parent);
void wzrd_box_end();
wzrd_handle wzrd_vbox(wzrd_v2 size, wzrd_handle parent);
wzrd_handle wzrd_widget(wzrd_style style, wzrd_handle parent);
void wzrd_label(wzrd_str str, wzrd_handle parent);
wzrd_handle wzrd_widget_free(wzrd_style box_data, wzrd_handle parent);
wzrd_handle wzrd_vbox_border(wzrd_v2 size, wzrd_handle parent);

// UTILS
wzrd_handle wzrd_box_set_unique_handle(wzrd_handle handle, wzrd_str str);
int wzrd_skin_create(wzrd_skin skin);
int wzrd_structure_create(wzrd_structure structure);
int wzrd_space_create(wzrd_space space);
int wzrd_layout_create(wzrd_layout layout);
wzrd_layout wzrd_layout_get_by_handle(int handle);
wzrd_layout wzrd_layout_get(wzrd_handle handle);
wzrd_box* wzrd_box_get_by_handle(wzrd_handle handle);
wzrd_str wzrd_str_create(char* str);
wzrd_box* wzrd_box_get_last();
wzrd_box* wzrd_box_find(wzrd_canvas* c, wzrd_str name);
int wzrd_box_get_current_index();
bool wzrd_box_is_active(wzrd_box* box);
bool wzrd_is_releasing();
wzrd_handle wzrd_unique_handle_create(wzrd_str str);
bool wzrd_box_is_dragged(wzrd_box* box);
void wzrd_box_resize(wzrd_v2* size);
void wzrd_item_add(Item item, wzrd_handle box);
bool wzrd_box_is_hot(wzrd_box* box);
bool wzrd_box_is_hot_using_canvas(wzrd_canvas* canvas, wzrd_box* box);
bool wzrd_handle_is_equal(wzrd_handle a, wzrd_handle b);
wzrd_handle wzrd_rect_unique(wzrd_rect_struct rect, wzrd_str name, wzrd_handle parent);
void wzrd_handle_set_layer(wzrd_handle handle, unsigned int layer);
wzrd_canvas* wzrd_canvas_get();

#endif