
#include <stdbool.h>
#include <assert.h>

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


typedef struct EguiColor {
	unsigned int r, g, b, a;
} EguiColor;

// Some Basic Colors
// NOTE: Custom raylib color palette for amazing visuals on WHITE background
#define EGUI_LIGHTGRAY (EguiColor){ 200, 200, 200, 255 }   // Light Gray
#define EGUI_GRAY      (EguiColor){ 130, 130, 130, 255 }   // Gray
#define EGUI_DARKGRAY  (EguiColor){ 80, 80, 80, 255 }      // Dark Gray
#define EGUI_YELLOW    (EguiColor){ 253, 249, 0, 255 }     // Yellow
#define EGUI_GOLD      (EguiColor){ 255, 203, 0, 255 }     // Gold
#define EGUI_ORANGE    (EguiColor){ 255, 161, 0, 255 }     // Orange
#define EGUI_PINK      (EguiColor){ 255, 109, 194, 255 }   // Pink
#define EGUI_RED       (EguiColor){ 230, 41, 55, 255 }     // Red
#define EGUI_MAROON    (EguiColor){ 190, 33, 55, 255 }     // Maroon
#define EGUI_GREEN     (EguiColor){ 0, 228, 48, 255 }      // Green
#define EGUI_LIME      (EguiColor){ 0, 158, 47, 255 }      // Lime
#define EGUI_DARKGREEN (EguiColor){ 0, 117, 44, 255 }      // Dark Green
#define EGUI_SKYBLUE   (EguiColor){ 102, 191, 255, 255 }   // Sky Blue
#define EGUI_BLUE      (EguiColor){ 0, 121, 241, 255 }     // Blue
#define EGUI_DARKBLUE  (EguiColor){ 0, 82, 172, 255 }      // Dark Blue
#define EGUI_PURPLE    (EguiColor){ 200, 122, 255, 255 }   // Purple
#define EGUI_VIOLET    (EguiColor){ 135, 60, 190, 255 }    // Violet
#define EGUI_DARKPURPLE(EguiColor){ 112, 31, 126, 255 }    // Dark Purple
#define EGUI_BEIGE     (EguiColor){ 211, 176, 131, 255 }   // Beige
#define EGUI_BROWN     (EguiColor){ 127, 106, 79, 255 }    // Brown
#define EGUI_DARKBROWN (EguiColor){ 76, 63, 47, 255 }      // Dark Brown

#define EGUI_WHITE     (EguiColor){ 255, 255, 255, 255 }   // White
#define EGUI_BLACK     (EguiColor){ 0, 0, 0, 255 }         // Black
#define EGUI_BLANK     (EguiColor){ 0, 0, 0, 0 }           // Blank (Transparent)
#define EGUI_MAGENTA   (EguiColor){ 255, 0, 255, 255 }     // Magenta
#define EGUI_RAYWHITE  (EguiColor){ 245, 245, 245, 255 }   // My own White (raylib logo)

typedef enum EguiState {
	EguiInactive, EguiActivating, EguiActive, EguiDeactivating
} EguiState;

typedef struct Str32 {
	char str[32];
} Str32;

typedef struct EguiRect {
	float x, y, w, h;
} EguiRect;

typedef struct EguiV2 {
	float x, y;
} EguiV2;

#define DEBUG_PANELS 0

#if 0
#define EguiButton(button) EguiButton(__LINE__, button)
#endif

typedef enum CrateId { CrateId_None, CrateId_Screen, CrateId_Tooltip, CrateId_DropDown, CrateId_Total } CrateId;

typedef enum BorderType { BorderType_Default, BorderType_Black, BorderType_None } BorderType;
typedef enum Alignment { Alignment_Left, Alignment_Center, Alignment_Right } Alignment;
typedef enum SizeType { SizeType_Fixed, SizeType_Empty } SizeType;

typedef struct Box {
	Str32 name;
	float x, y, w, h;
	float per;
	float fill;
	EguiRect absolute_rect;
	bool row_mode;
	EguiV2 padding;
	EguiRect inner_padding;
	float push;
	EguiColor color;
	int child_gap;
	int index;
	int window_index;
	BorderType border_type;
	Alignment alignment;
	int n;
	int children[256];
	int num_children;
	int new_index;
	SizeType size_type;
	bool grow_horizontal;
	bool grow_vertical;
	Str32 str;
} Box;

typedef struct Crate {
	int id;
	int index;

	Box panel_stack[32];
	int current_panel_index;

	EguiV2 current_pos;
	bool current_column_mode;
	int current_child_gap;
} Crate;

typedef enum EguiDrawCommandType { EguiDrawCommandType_None, EguiDrawCommandType_Text, EguiDrawCommandType_Box, EguiDrawCommandType_Rect, EguiDrawCommandType_Texture } EguiDrawCommandType;

typedef struct EguiDrawCommand {
	EguiDrawCommandType type;
	int z;
	int num;
	EguiRect rect;

	union {
		//TODO: PlatformTexture texture;

		struct {
			char text[32];
			EguiColor color;
			int alignment;
		} text_data;

		struct {
			int box_index;
			EguiColor color;
		} box_data;

		struct {
			EguiColor color;
			EguiColor border_color;
			int border_width;
		} rect_data;
	};

} EguiDrawCommand;


#define MAX_NUM_DRAW_COMMANDS 1024



typedef struct Egui {

	int num_boxes;
	Box boxes[256];

	Str32 hot_item;
	Str32 active_item;

	double time;


	double tooltip_time;
	bool tooltip_count_time;

	// For debugging close end panels
	int total_num_panels, total_num_windows;
	int panel_line_stack[32];
	int num_line_stack;

	int window_width, window_height;

	int current_window_id;
	int hot_window;

	EguiDrawCommand draw_commands[MAX_NUM_DRAW_COMMANDS];
	int num_draw_commands;

	EguiV2 mouse_pos;
	EguiV2 previous_mouse_pos;

	EguiV2 current_pos;
	bool current_row_mode;
	int current_child_gap;

	Crate windows_stack[32];
	int current_window_index;

	//PlatformFont font;
	EguiState mouse_left, mouse_right;

} Egui;

Egui egui;


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


typedef struct Editor {

	EguiV2 offset;

	bool column_mode;
	char str[32];
	int num_lines;
	EguiV2 scroll;
	EditorWindow window;

	// Data for modifying entities
	bool show_modify_entity_box;
	EguiV2 modify_entity_window_pos, modify_entity_window_size;
	char entity_name[32];
	EguiV2 entity_size;
	EguiRect modify_entity_rect;
	char entity_width[32];
	char entity_height[32];

	// Drop down panel data
	bool modify_entity_drop_down_panel;
	//EntityId entity;
	EguiV2 drop_down_panel_pos;

	// Widgets
	HashTable widgets_hashtable;
	Widget widgets[MAX_NUM_WIDGETS];
	int num_widgets;

	bool windows[MAX_NUM_WIDGETS];
	bool show_add_entity_box;
	bool show_add_item_box;

	//asd
	EditorWindow editor_window;
	EditorTab editor_tab;

	// Draw data
	EguiV2 draw_tool_pos, draw_tool_size;
	bool draw_tool_active;

} Editor;

Editor editor;

typedef struct EguiDrawCommandsBuffer {
	EguiDrawCommand commands[MAX_NUM_DRAW_COMMANDS];
	int num;
} EguiDrawCommandsBuffer;


void EguiBegin(double time, EguiV2 padding, EguiV2 mouse_pos, EguiState moues_left);
EguiDrawCommandsBuffer EguiEnd();
Str32 Str32Create();
void EguiInit(int window_width ,int window_height);
void EguiBoxBegin(Box box);
void EguiBoxEnd();
bool EditorButtonIcon(const char* str, const char* tooltip_str);