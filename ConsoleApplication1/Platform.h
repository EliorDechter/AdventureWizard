#ifndef PLATFORM_H
#define PLATFORM_H

#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <assert.h>
#include <SDL3_image/SDL_image.h>
#include <stdio.h>

#define TARGET_TEXTURE_WIDTH 1920
#define TARGET_TEXTURE_HEIGHT 1080

#define PLATFORM_FONT_HEIGHT 18
#define FILE_TO_PANEL_X 5
#define FILE_TO_PANEL_Y 5
#define FILE_TO_PANEL_WIDTH WINDOW_WIDTH
#define FILE_TO_PANEL_HEIGHT 5

#define EDITOR_RIGHT_PANEL_X  GAME_SCREEN_X + GAME_SCREEN_WIDTH + 5
#define EDITOR_RIGHT_PANEL_Y  GAME_SCREEN_Y
#define EDITOR_RIGHT_PANEL_WIDTH  300
#define EDITOR_RIGHT_PANEL_HEIGHT  LOG_ORIGIN_Y + LOG_HEIGHT - EDITOR_RIGHT_PANEL_Y

#define EDITOR_TOP_PANEL_X 5
#define EDITOR_TOP_PANEL_Y 5
#define EDITOR_TOP_PANEL_WIDTH 900 + 5 + EDITOR_RIGHT_PANEL_WIDTH
#define EDITOR_TOP_PANEL_HEIGHT 35

#define GAME_SCREEN_X  5
#define GAME_SCREEN_Y  EDITOR_TOP_PANEL_HEIGHT + 5
#define GAME_SCREEN_WIDTH 900
#define GAME_SCREEN_HEIGHT 450

#define LOG_ORIGIN_X GAME_SCREEN_X
#define LOG_ORIGIN_Y GAME_SCREEN_Y + GAME_SCREEN_HEIGHT + INVENTORY_HEIGHT + 5
#define LOG_WIDTH GAME_SCREEN_WIDTH
#define LOG_HEIGHT 200

#define WINDOW_WIDTH EDITOR_RIGHT_PANEL_X + EDITOR_RIGHT_PANEL_WIDTH + 5
#define WINDOW_HEIGHT EDITOR_RIGHT_PANEL_Y + EDITOR_RIGHT_PANEL_HEIGHT + 5

#define INVENTORY_X 0
#define INVENTORY_Y 100
#define INVENTORY_WIDTH 200
#define INVENTORY_HEIGHT 100

typedef struct platform_color {
	unsigned char r, g, b, a;
} platform_color;

typedef struct V2 {
	float x, y;
} PlatformV2;

typedef struct PlatformV2i {
	int x, y;
} PlatformV2i;

// Some Basic Colors
// NOTE: Custom raylib color palette for amazing visuals on WHITE background
#define PLATFORM_LIGHTGRAY (platform_color){ 200, 200, 200, 255 }   // Light Gray
#define PLATFORM_GRAY      (platform_color){ 130, 130, 130, 255 }   // Gray
#define PLATFORM_DARKGRAY  (platform_color){ 80, 80, 80, 255 }      // Dark Gray
#define PLATFORM_YELLOW    (platform_color){ 253, 249, 0, 255 }     // Yellow
#define PLATFORM_GOLD      (platform_color){ 255, 203, 0, 255 }     // Gold
#define PLATFORM_ORANGE    (platform_color){ 255, 161, 0, 255 }     // Orange
#define PLATFORM_PINK      (platform_color){ 255, 109, 194, 255 }   // Pink
#define PLATFORM_RED       (platform_color){ 230, 41, 55, 255 }     // Red
#define PLATFORM_MAROON    (platform_color){ 190, 33, 55, 255 }     // Maroon
#define PLATFORM_GREEN     (platform_color){ 0, 228, 48, 255 }      // Green
#define PLATFORM_LIME      (platform_color){ 0, 158, 47, 255 }      // Lime
#define PLATFORM_DARKGREEN (platform_color){ 0, 117, 44, 255 }      // Dark Green
#define PLATFORM_SKYBLUE   (platform_color){ 102, 191, 255, 255 }   // Sky Blue
#define PLATFORM_BLUE      (platform_color){ 0, 121, 241, 255 }     // Blue
#define PLATFORM_DARKBLUE  (platform_color){ 0, 82, 172, 255 }      // Dark Blue
#define PLATFORM_PURPLE    (platform_color){ 200, 122, 255, 255 }   // Purple
#define PLATFORM_VIOLET    (platform_color){ 135, 60, 190, 255 }    // Violet
#define PLATFORM_DARKPURPLE(PlatformColor){ 112, 31, 126, 255 }    // Dark Purple
#define PLATFORM_BEIGE     (platform_color){ 211, 176, 131, 255 }   // Beige
#define PLATFORM_BROWN     (platform_color){ 127, 106, 79, 255 }    // Brown
#define PLATFORM_DARKBROWN (platform_color){ 76, 63, 47, 255 }      // Dark Brown

#define PLATFORM_WHITE     (platform_color){ 255, 255, 255, 255 }   // White
#define PLATFORM_BLACK     (platform_color){ 0, 0, 0, 255 }         // Black
#define PLATFORM_BLANK     (platform_color){ 0, 0, 0, 0 }           // Blank (Transparent)
#define PLATFORM_MAGENTA   (platform_color){ 255, 0, 255, 255 }     // Magenta
#define PLATFORM_RAYWHITE  (platform_color){ 245, 245, 245, 255 }   // My own White (raylib logo)

typedef enum State {
	Inactive, Activating, Active, Deactivating
} State;

typedef struct PlatformRenderTexture {
	SDL_Texture* texture;
} PlatformRenderTexture;

typedef struct PlatformImage {
	int i;
	//Image image;
} PlatformImage;

typedef struct PlatformTexture {
	SDL_Texture* data;
	float w;
	float h;
} PlatformTexture;

typedef struct PlatformFont {
	int i;
	//Font font;
} PlatformFont;

typedef struct PlatformRect {
	float x, y, w, h;
} PlatformRect;

PlatformRenderTexture target;
PlatformRenderTexture drop_down_panel_target;


typedef struct PlatformKey {
	char val;
	State state;
} PlatformKey;

typedef struct KeysPressed {
	PlatformKey keys[32];
	int count;
} KeysPressed;

typedef struct PlatformSystem {
	State keyboard[128];
	State mouse_left, mouse_right;
	float mouse_x, mouse_y;
	float previous_mouse_x, previous_mouse_y;
	int window_width, window_height;
	State keyboard_states[128];
	KeysPressed keys_pressed;
	bool focus;
} PlatformSystem;

PlatformSystem platform;

typedef struct PlatformTargetTexture {
	SDL_Texture* data;
	float w;
	float h;
} PlatformTargetTexture;

#define SDL_TEXTURES_MAX_COUNT 1024

typedef struct Sdl {
	SDL_Renderer* renderer;
	SDL_Window* window;
	TTF_Font* font;
	TTF_Text* fps_text;
	SDL_Event* event;
	TTF_TextEngine* engine;
	//PlatformTargetTexture a;
	SDL_Event events[32];
	int events_count;
	SDL_Mutex *mutex;
	SDL_Cursor* cursor_hand, *cursor_default, *cursor_horizontal_arrow, *cursor_vertical_arrow;

	SDL_Texture textures[SDL_TEXTURES_MAX_COUNT];
	int textures_count;
} Sdl;

Sdl sdl;

PlatformTexture g_texture;
PlatformTexture g_checkmark_texture;
PlatformTargetTexture g_target;
PlatformTexture g_close_texture;

typedef enum PlatformCursor { PlatformCursorDefault, PlatformCursorHand, PlatformCursorVerticalArrow, PlatformCursorHorizontalArrow } PlatformCursor;

// API
void PlatformRectDraw(PlatformRect rect, platform_color color);
void PlatformTextDraw(const char* txt, float x, float y);
void PlatformTextureDraw(PlatformTexture texture, PlatformRect rect);
PlatformTexture PlatformTextureLoad(const char* path);
PlatformV2i PlatformTextGetSize(const char* str);
void PlatformLineDrawVertical(float x0, float y0, float x1, float y1);
void PlatformLineDrawHorizontal(float x0, float y0, float x1, float y1);
void PlatformLineDraw(float x0, float y0, float x1, float y1, float r, float g, float b);
void PlatformTextureBeginTarget(PlatformTargetTexture texture);
void PlatformTextureEndTarget();
PlatformTargetTexture PlatformTargetTextureCreate();
void platform_cursor_set(PlatformCursor cursor);
void PlatformTextDrawColor(const char* str, float x, float y, char r, char g, char b, char a);
void PlatformTextureDrawFromSource(PlatformTexture texture, PlatformRect dest, PlatformRect src, platform_color color);

#endif