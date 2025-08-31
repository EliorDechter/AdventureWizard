#pragma once

//#define RAYGUI_IMPLEMENTATION
//#include "raylib.h"
#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <assert.h >
#include <SDL3_image/SDL_image.h >
#include <stdio.h>

#define FONT_HEIGHT 18
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

typedef struct PlatformColor {
	unsigned int r, g, b, a;
} PlatformColor;

typedef struct V2 {
	float x, y;
} PlatformV2;

typedef struct PlatformV2i {
	int x, y;
} PlatformV2i;

// Some Basic Colors
// NOTE: Custom raylib color palette for amazing visuals on WHITE background
#define PLATFORM_LIGHTGRAY (PlatformColor){ 200, 200, 200, 255 }   // Light Gray
#define PLATFORM_GRAY      (PlatformColor){ 130, 130, 130, 255 }   // Gray
#define PLATFORM_DARKGRAY  (PlatformColor){ 80, 80, 80, 255 }      // Dark Gray
#define PLATFORM_YELLOW    (PlatformColor){ 253, 249, 0, 255 }     // Yellow
#define PLATFORM_GOLD      (PlatformColor){ 255, 203, 0, 255 }     // Gold
#define PLATFORM_ORANGE    (PlatformColor){ 255, 161, 0, 255 }     // Orange
#define PLATFORM_PINK      (PlatformColor){ 255, 109, 194, 255 }   // Pink
#define PLATFORM_RED       (PlatformColor){ 230, 41, 55, 255 }     // Red
#define PLATFORM_MAROON    (PlatformColor){ 190, 33, 55, 255 }     // Maroon
#define PLATFORM_GREEN     (PlatformColor){ 0, 228, 48, 255 }      // Green
#define PLATFORM_LIME      (PlatformColor){ 0, 158, 47, 255 }      // Lime
#define PLATFORM_DARKGREEN (PlatformColor){ 0, 117, 44, 255 }      // Dark Green
#define PLATFORM_SKYBLUE   (PlatformColor){ 102, 191, 255, 255 }   // Sky Blue
#define PLATFORM_BLUE      (PlatformColor){ 0, 121, 241, 255 }     // Blue
#define PLATFORM_DARKBLUE  (PlatformColor){ 0, 82, 172, 255 }      // Dark Blue
#define PLATFORM_PURPLE    (PlatformColor){ 200, 122, 255, 255 }   // Purple
#define PLATFORM_VIOLET    (PlatformColor){ 135, 60, 190, 255 }    // Violet
#define PLATFORM_DARKPURPLE(PlatformColor){ 112, 31, 126, 255 }    // Dark Purple
#define PLATFORM_BEIGE     (PlatformColor){ 211, 176, 131, 255 }   // Beige
#define PLATFORM_BROWN     (PlatformColor){ 127, 106, 79, 255 }    // Brown
#define PLATFORM_DARKBROWN (PlatformColor){ 76, 63, 47, 255 }      // Dark Brown

#define PLATFORM_WHITE     (PlatformColor){ 255, 255, 255, 255 }   // White
#define PLATFORM_BLACK     (PlatformColor){ 0, 0, 0, 255 }         // Black
#define PLATFORM_BLANK     (PlatformColor){ 0, 0, 0, 0 }           // Blank (Transparent)
#define PLATFORM_MAGENTA   (PlatformColor){ 255, 0, 255, 255 }     // Magenta
#define PLATFORM_RAYWHITE  (PlatformColor){ 245, 245, 245, 255 }   // My own White (raylib logo)

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
	SDL_Texture *data;
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

typedef struct PlatformSystem {
	State keyboard[128];
	State mouse_left, mouse_right;
	float mouse_x, mouse_y;
	float previous_mouse_pos_x, previous_mouse_pos_y;
	int window_width, window_height;
} PlatformSystem;

PlatformSystem platform_system;

typedef struct Sdl {
	SDL_Renderer* renderer;
	SDL_Window* window;
	TTF_Font* font;
	TTF_Text* fps_text;
	SDL_Event* event;
	TTF_TextEngine* engine;
} Sdl;

Sdl sdl;

PlatformTexture g_texture;

// API
void PlatformRectDraw(PlatformRect rect, PlatformColor color);
void PlatformTextDraw(const char* txt, float x, float y);
void PlatformTextureDraw(PlatformTexture t, PlatformRect rect);
PlatformTexture PlatformTextureLoad(const char* path);
PlatformV2i PlatformTextGetSize(const char* str);
void PlatformLineDrawVertical(float x0, float y0, float x1, float y1);
void PlatformLineDrawHorizontal(float x0, float y0, float x1, float y1);
void PlatformLineDraw(float x0, float y0, float x1, float y1, float r, float g, float b);