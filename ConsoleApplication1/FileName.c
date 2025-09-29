
#if 0

// TODO:
// -log doesn't show first line
// -serialize entity animations map (currently segfaults)


#if 0
typedef struct PlatformRenderTexture {
	RenderTexture2D texture;
} PlatformRenderTexture;

typedef struct PlatformImage {
	Image image;
} PlatformImage;

typedef struct PlatformTexture {
	Texture2D texture;
} PlatformTexture;

typedef struct PlatformFont {
	Font font;
} PlatformFont;

PlatformImage PlatformImageLoad(const char* str) {
	PlatformImage result = (PlatformImage){ LoadImage(str) };

	return result;
}

void PlatformFontLoad() {

}

typedef struct Rect {
	float x, y, w, h;
} Rect;

RenderTexture2D PlatformLoadRenderTexture(int w, int h) {
	RenderTexture2D result = LoadRenderTexture(w, h);

	BeginTextureMode(result);
	ClearBackground(WHITE);
	EndTextureMode();

	return result;
}

void PlatformBeginDrawing() {
	BeginDrawing();
	ClearBackground(LIGHTGRAY);
}

void PlatformEndDrawing() {
	EndDrawing();
}

PlatformDrawText(const char* str, int x, int y) {
	DrawText(str, x, y, 13, BLACK);
}

PlatformTextureDraw(PlatformTexture texture, float x, float y, float w, float h) {
	DrawTexturePro(texture.texture,
		(Rectangle) {
		0, 0, (float)texture.texture.width, (float)-texture.texture.height
	},
		(Rectangle) {
		x, y, w, h
	},
		(Vector2) {
		0, 0
	},
		0,
		WHITE);
}

void PlatformRectDraw(Rect rect, PlatformColor color) {
	DrawRectangle(rect.x, rect.y, rect.w, rect.h, *((Color*)&color));
}
#else
#endif

#if 1
#define FIX_ME 0

#define NAME_SIZE 30
#include <assert.h>
#include <stdio.h>
#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"
#include <float.h>
#include "json.h"
#include <stdbool.h>

#include "raymath.h"
#include <stdarg.h>

//#include "raygui.h"
#include "editor.h"


// Gui definitions



const int NUM_NODES_X = 90;
const int NUM_NODES_Y = 50;

bool run_slice_spritesheet;

bool g_start_drawing;
bool g_is_drawing;
bool g_init;

#if 0
// Vector2 type
typedef struct Vector2 {
	float x;
	float y;
} Vector2;

// Vector3 type                 // -- ConvertHSVtoRGB(), ConvertRGBtoHSV()
typedef struct Vector3 {
	float x;
	float y;
	float z;
} Vector3;

// Color type, RGBA (32bit)
typedef struct Color {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
} Color;

// PlatformRect type
typedef struct PlatformRect {
	float x;
	float y;
	float width;
	float height;
} PlatformRect;

// TODO: PlatformTexture type is very coupled to raylib, required by Font type
// It should be redesigned to be provided by user
typedef struct PlatformTexture {
	unsigned int id;        // OpenGL texture id
	int width;              // Texture base width
	int height;             // Texture base height
	int mipmaps;            // Mipmap levels, 1 by default
	int format;             // Data format (PixelFormat type)
} PlatformTexture;

// Image, pixel data stored in CPU memory (RAM)
typedef struct Image {
	void* data;             // Image raw data
	int width;              // Image base width
	int height;             // Image base height
	int mipmaps;            // Mipmap levels, 1 by default
	int format;             // Data format (PixelFormat type)
} Image;

// Texture, tex data stored in GPU memory (VRAM)
typedef struct Texture {
	unsigned int id;        // OpenGL texture id
	int width;              // Texture base width
	int height;             // Texture base height
	int mipmaps;            // Mipmap levels, 1 by default
	int format;             // Data format (PixelFormat type)
} Texture;

typedef struct RenderTexture {
	unsigned int id;        // OpenGL framebuffer object id
	Texture texture;        // Color buffer attachment texture
	Texture depth;          // Depth buffer attachment texture
} RenderTexture;

typedef RenderTexture PlatformTexture;
#endif

//PlatformTexture target;
//PlatformTexture drop_down_panel_target;

#endif
#endif