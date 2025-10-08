#ifndef WZRD_GUI_H
#define WZRD_GUI_H

#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "Strings.h"

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

#define Stringify1(x) #x
#define Stringify(x) Stringify1(x)
#define EditorDropDownBox(name) EditorDropDownBoxRaw(Stringify(__LINE__), name)
#define EditorToggle(name) EditorToggleRaw(Stringify(__LINE__), name)

typedef enum EditorWindow { EditorWindow_Lists, EditorWindow_Slice_Spritesheet } EditorWindow;
typedef enum EditorTab { EditorTab_Entities, EditorTab_Events, EditorTab_Areas, EditorTab_Inventory } EditorTab;

typedef enum PanelId { PanelId_None, PanelId_Window, PanelId_TopPanel, PanelId_RightPanel, PanelId_GameScreen, PanelId_ModifyEntityDropDown } PanelId;
typedef enum EditorWindowId { EditorWindowId_ModifyEntity, EditorWindowId_DrawTool } EditorWindowId;

typedef struct wzrd_color {
	unsigned char r, g, b, a;
} wzrd_color;

// Some Basic Colors
// NOTE: Custom raylib color palette for amazing visuals on WHITE background
#define EGUI_LIGHTGRAY (wzrd_color){ 200, 200, 200, 255 }   // Light Gray
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
	EguiInactive, EguiActivating, EguiActive, EguiDeactivating
} wzrd_state;

typedef struct wzrd_rect {
	float x, y, w, h;
} wzrd_rect;

typedef struct wzrd_v2 {
	union {
		struct { float x, y; };
		//struct { float w, h; };
	};
} wzrd_v2;

typedef struct wzrd_v2i {
	int x, y;
} wzrd_v2i;

#define DEBUG_PANELS 0

typedef enum CrateId { CrateId_None, CrateId_Screen, CrateId_Tooltip, CrateId_DropDown, CrateId_Total } CrateId;

typedef enum BorderType { BorderType_Default, BorderType_Black, BorderType_Clicked, BorderType_InputBox, BorderType_BottomLine, BorderType_LeftLine, BorderType_None } BorderType;
typedef enum Alignment { Alignment_Left, Alignment_Center, Alignment_Right } Alignment;
typedef enum SizeType { SizeType_Fixed, SizeType_Empty } SizeType;

typedef struct wzrd_texture{
	void* data;
	float w, h;
} wzrd_texture;

typedef enum ItemType {
	ItemType_None,
	wzrd_item_type_str,
	ItemType_Rect,
	ItemType_Texture,
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
	float x0, y0, x1, y1;
} Line;

typedef struct Item {
	ItemType type;
	wzrd_v2 size;
	wzrd_color color;
	float pad_left, pad_right, pad_top, pad_bottom;
	union {
		str128 str;
		wzrd_texture texture;
		wzrd_rect rect;
		Line line;
	};
	bool scissor;
} Item;


typedef enum ButtonType {ButtonType_None, ButtonType_Flat, ButtonType_ThreeDimensional} ButtonType;

typedef struct wzrd_widget_id {
	str128 val;
} wzrd_widget_id;

typedef struct Box {
	str128 name;
	float x, y, w, h;
	wzrd_rect absolute_rect;
	bool row_mode;
	float pad_right, pad_bottom, pad_left, pad_top;
	wzrd_color color;
	int window_index;
	BorderType border_type;
	//int depth;
#define MAX_NUM_CHILDREN 32
	int children[MAX_NUM_CHILDREN];
	int children_count;
	SizeType size_type;
	bool grow_horizontal;
	bool grow_vertical;
	float child_gap;
	bool center;
#define MAX_NUM_ITEMS 8
	Item items[MAX_NUM_ITEMS];
	int items_count;
	bool three_dimensional_button;
	bool flat_button;
	int relative_box;
	int index;
	bool resizable;
	bool fit_h, fit_w;
	bool center_x, center_y;
	bool is_crate;
	int parent;
	bool is_input_box;
	bool is_button;
	int z;
	bool disable_input;
	bool is_draggable, is_slot;
	bool disable_hover;
	bool best_fit;
} wzrd_box;

typedef struct Crate {
	int z;
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
} EguiDrawCommandType;

typedef struct EguiDrawCommand {
	EguiDrawCommandType type;
	wzrd_rect dest_rect, src_rect;
	str128 str;
	wzrd_color color;
	wzrd_texture texture;
} EguiDrawCommand;


typedef struct wzrd_draw_commands_buffer {
	EguiDrawCommand commands[MAX_NUM_DRAW_COMMANDS];
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

typedef struct Egui {

	int boxes_count;
#define MAX_NUM_BOXES 128
	wzrd_box boxes[MAX_NUM_BOXES];

	str128 hot_item, active_item, clicked_item, half_clicked_item, released_item, dragged_item, selected_item;

	str128 right_resized_item,
		left_resized_item, bottom_resized_item, top_resized_item;

	double time;

	double tooltip_time;
	bool tooltip_count_time;

	// For debugging close end panels
	int total_num_panels, total_num_windows;
	int panel_line_stack[32];
	int num_line_stack;

	int window_width, window_height;

	wzrd_v2 mouse_pos, previous_mouse_pos, mouse_delta;

	//EguiV2 current_pos;
	/*bool current_row_mode;
	int current_child_gap;*/

	Crate crates_stack[32];
	int current_crate_index;

	//PlatformFont font;
	wzrd_state mouse_left, mouse_right;

#define MAX_NUM_TOGGLES 256
	struct { str128 name; bool val; } toggles[MAX_NUM_TOGGLES];
	int toggles_count;

	struct { str128 name; str128 val; } input_box_strings[MAX_NUM_TOGGLES];
	int input_box_strings_count;

	int line_size;
	
	str128 active_input_box;

	wzrd_keyboard_keys keyboard_keys;

	wzrd_texture checkmark;

	bool double_click;

	wzrd_icons icons;

	float input_box_timer;

	wzrd_color default_color;

	bool enable_input;

	bool is_interacting, is_hovering;

	wzrd_box hovered_cached_box, dragged_box;

	int scale;

} Egui;

static Egui *g_gui;

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
#define EDITOR_POS_X EDITOR_RIGHT_PANEL_X + 5 
#define EDITOR_POS_Y EDITOR_RIGHT_PANEL_Y + 5
#define EDITOR_ICON_SIZE 24
#define EDITOR_BUTTON_SIZE_X 72
#define EDITOR_BUTTON_SIZE_Y 24


typedef struct Label_list {
	str128 val[32];
	int count;
} Label_list;


#define MAX_NUM_VERTICES_IN_POLYGON 32

typedef struct wzrd_polygon {
	wzrd_v2 vertices[MAX_NUM_VERTICES_IN_POLYGON];
	int count;
} wzrd_polygon;

// API
void wzrd_begin(Egui* gui, double time, wzrd_v2 mouse_pos, wzrd_state moues_left, wzrd_keyboard_keys input_keys, wzrd_v2 size, wzrd_icons icons, wzrd_color default_color, bool enable_input, unsigned int scale);
void wzrd_end(wzrd_cursor* cursor, wzrd_draw_commands_buffer* buffer);
bool wzrd_box_begin(wzrd_box box);
void wzrd_box_end();
bool EditorButtonIcon(const char* str, const char* tooltip_str);
void EguiDrawText(const char* text, wzrd_rect bounds, int alignment, wzrd_color color);
wzrd_box* wzrd_box_get_current();
wzrd_box* wzrd_box_get_parent();
wzrd_box* wzrd_box_get_previous();
void EguiLabel(str128 str);
void wzrd_item_add(Item item);
bool EguiButtonRaw(wzrd_box box);
bool EguiButton(str128 str);
bool EguiButtonRawBegin(wzrd_box button);
void EguiButtonRawEnd();
bool IsStr32Equal(Str32 a, Str32 b);
bool EguiBox(wzrd_box box);
bool EguiLabelButton(str128 str);
bool EguiLabelButtonBegin(str128 str);
bool EguiLabelButtonEnd();
str128 EguiInputBox(int max_num_keys);
void wzrd_crate_begin(int window_id, wzrd_box box);
int wzrd_box_get_current_index();
void wzrd_crate(int window_id, wzrd_box box);
int wzrd_dropdown(int* selected_text, const str128* str, int str_count, int w, bool* active);
void EguiToggleEnd();
bool *EguiToggleBegin(wzrd_box box);
bool *EguiToggle(wzrd_box box);
void wzrd_texture_add(wzrd_texture texture, wzrd_v2 size);
void EguiStringAdd(str128 str);
void wzrd_dialog_begin(wzrd_v2 *pos, wzrd_v2 size, bool *active, str128 name, int parent);
void EguiDialogEnd(bool active);
void EguiBoxResize(wzrd_v2* size);
wzrd_box* EguiHotItemGet();
void wzrd_label_list(Label_list label_list, int *selected);
bool wzrd_button_icon(wzrd_texture texture);
void wzrd_label_list2(Label_list label_list, wzrd_box box, int* selected);
wzrd_rect wzrd_box_get_rect(wzrd_box * box);
void wzrd_input_box(str128* str, int max_num_keys);
void wzrd_crate_end();
wzrd_box* wzrd_box_get_by_name(str128 str);
bool wzrd_game_buttonesque(wzrd_v2 pos, wzrd_v2 size, wzrd_color color, str128 name);
void wzrd_drag(wzrd_box box, wzrd_v2* pos, bool *drag);
wzrd_box* wzrd_box_get_last();
bool wzrd_box_is_active(wzrd_box *box);
bool wzrd_box_is_dragged(wzrd_box* box);
bool wzrd_box_is_hot(wzrd_box* box);
bool wzrd_box_is_selected(wzrd_box* box);
wzrd_box *wzrd_box_get_released();
wzrd_box* wzrd_box_get_by_name_from_gui(Egui* gui, str128 name);
bool wzrd_is_releasing();
wzrd_v2 wzrd_lerp(wzrd_v2 pos, wzrd_v2 end_pos);
bool EguiToggle2(wzrd_box box, str128 str, wzrd_color color, bool active);
void EguiToggle3(wzrd_box box, str128 str, bool *active);
#endif