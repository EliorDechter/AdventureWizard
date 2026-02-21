#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>
#include <stdio.h>
#include "WzGuiCore.h"
#define _USE_MATH_DEFINES
#include <math.h>

#define STB_TEXTEDIT_IMPLEMENTATION
#include "stb_textedit.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#define MAX_NUM_EVENTS 128
unsigned events_count;
WzEvent events[MAX_NUM_EVENTS];

static unsigned short sdl_keymod_to_wz(SDL_Keymod sdl_mod)
{
	unsigned short wz_mod = WZ_KMOD_NONE;

	if (sdl_mod & SDL_KMOD_LSHIFT) wz_mod |= WZ_KMOD_LSHIFT;
	if (sdl_mod & SDL_KMOD_RSHIFT) wz_mod |= WZ_KMOD_RSHIFT;
	if (sdl_mod & SDL_KMOD_LCTRL)  wz_mod |= WZ_KMOD_LCTRL;
	if (sdl_mod & SDL_KMOD_RCTRL)  wz_mod |= WZ_KMOD_RCTRL;
	if (sdl_mod & SDL_KMOD_LALT)   wz_mod |= WZ_KMOD_LALT;
	if (sdl_mod & SDL_KMOD_RALT)   wz_mod |= WZ_KMOD_RALT;
	if (sdl_mod & SDL_KMOD_LGUI)   wz_mod |= WZ_KMOD_LGUI;
	if (sdl_mod & SDL_KMOD_RGUI)   wz_mod |= WZ_KMOD_RGUI;

	return wz_mod;
}

// Convert SDL3 scancode + modifiers to WZ keycode
// Returns 0-127 for ASCII characters, 128+ for special keys
WZ_Keycode wz_sdl_scancode_to_keycode(SDL_Scancode scancode, SDL_Keymod modifiers)
{
	bool shift = (modifiers & (SDL_KMOD_LSHIFT | SDL_KMOD_RSHIFT)) != 0;
	bool caps = (modifiers & SDL_KMOD_CAPS) != 0;

	// Letters A-Z
	if (scancode >= SDL_SCANCODE_A && scancode <= SDL_SCANCODE_Z) {
		char base = 'a' + (scancode - SDL_SCANCODE_A);
		// Caps lock XOR shift gives uppercase
		if (caps != shift) {
			return base - 32; // Uppercase ASCII
		}
		return base; // Lowercase ASCII
	}

	// Numbers 1-9, 0
	if (scancode >= SDL_SCANCODE_1 && scancode <= SDL_SCANCODE_9) {
		if (shift) {
			const char symbols[] = "!@#$%^&*(";
			return symbols[scancode - SDL_SCANCODE_1];
		}
		return '1' + (scancode - SDL_SCANCODE_1);
	}

	if (scancode == SDL_SCANCODE_0) {
		return shift ? ')' : '0';
	}

	// Punctuation and special keys (ASCII range)
	switch (scancode) {
	case SDL_SCANCODE_SPACE:      return ' ';
	case SDL_SCANCODE_RETURN:     return '\n';
	case SDL_SCANCODE_ESCAPE:     return 27;
	case SDL_SCANCODE_BACKSPACE:  return '\b';
	case SDL_SCANCODE_TAB:        return '\t';

	case SDL_SCANCODE_MINUS:      return shift ? '_' : '-';
	case SDL_SCANCODE_EQUALS:     return shift ? '+' : '=';
	case SDL_SCANCODE_LEFTBRACKET:  return shift ? '{' : '[';
	case SDL_SCANCODE_RIGHTBRACKET: return shift ? '}' : ']';
	case SDL_SCANCODE_BACKSLASH:    return shift ? '|' : '\\';
	case SDL_SCANCODE_SEMICOLON:    return shift ? ':' : ';';
	case SDL_SCANCODE_APOSTROPHE:   return shift ? '"' : '\'';
	case SDL_SCANCODE_GRAVE:        return shift ? '~' : '`';
	case SDL_SCANCODE_COMMA:        return shift ? '<' : ',';
	case SDL_SCANCODE_PERIOD:       return shift ? '>' : '.';
	case SDL_SCANCODE_SLASH:        return shift ? '?' : '/';
	}

	// Extended keys (128+)
	switch (scancode) {
	case SDL_SCANCODE_CAPSLOCK:     return WZ_KEY_CAPSLOCK;
	case SDL_SCANCODE_F1:           return WZ_KEY_F1;
	case SDL_SCANCODE_F2:           return WZ_KEY_F2;
	case SDL_SCANCODE_F3:           return WZ_KEY_F3;
	case SDL_SCANCODE_F4:           return WZ_KEY_F4;
	case SDL_SCANCODE_F5:           return WZ_KEY_F5;
	case SDL_SCANCODE_F6:           return WZ_KEY_F6;
	case SDL_SCANCODE_F7:           return WZ_KEY_F7;
	case SDL_SCANCODE_F8:           return WZ_KEY_F8;
	case SDL_SCANCODE_F9:           return WZ_KEY_F9;
	case SDL_SCANCODE_F10:          return WZ_KEY_F10;
	case SDL_SCANCODE_F11:          return WZ_KEY_F11;
	case SDL_SCANCODE_F12:          return WZ_KEY_F12;

	case SDL_SCANCODE_PRINTSCREEN:  return WZ_KEY_PRINTSCREEN;
	case SDL_SCANCODE_SCROLLLOCK:   return WZ_KEY_SCROLLLOCK;
	case SDL_SCANCODE_PAUSE:        return WZ_KEY_PAUSE;
	case SDL_SCANCODE_INSERT:       return WZ_KEY_INSERT;
	case SDL_SCANCODE_HOME:         return WZ_KEY_HOME;
	case SDL_SCANCODE_PAGEUP:       return WZ_KEY_PAGEUP;
	case SDL_SCANCODE_DELETE:       return WZ_KEY_DELETE;
	case SDL_SCANCODE_END:          return WZ_KEY_END;
	case SDL_SCANCODE_PAGEDOWN:     return WZ_KEY_PAGEDOWN;
	case SDL_SCANCODE_RIGHT:        return WZ_KEY_RIGHT;
	case SDL_SCANCODE_LEFT:         return WZ_KEY_LEFT;
	case SDL_SCANCODE_DOWN:         return WZ_KEY_DOWN;
	case SDL_SCANCODE_UP:           return WZ_KEY_UP;

	case SDL_SCANCODE_NUMLOCKCLEAR: return WZ_KEY_NUMLOCKCLEAR;
	case SDL_SCANCODE_KP_DIVIDE:    return WZ_KEY_KP_DIVIDE;
	case SDL_SCANCODE_KP_MULTIPLY:  return WZ_KEY_KP_MULTIPLY;
	case SDL_SCANCODE_KP_MINUS:     return WZ_KEY_KP_MINUS;
	case SDL_SCANCODE_KP_PLUS:      return WZ_KEY_KP_PLUS;
	case SDL_SCANCODE_KP_ENTER:     return WZ_KEY_KP_ENTER;
	case SDL_SCANCODE_KP_1:         return (modifiers & SDL_KMOD_NUM) ? '1' : WZ_KEY_KP_1;
	case SDL_SCANCODE_KP_2:         return (modifiers & SDL_KMOD_NUM) ? '2' : WZ_KEY_KP_2;
	case SDL_SCANCODE_KP_3:         return (modifiers & SDL_KMOD_NUM) ? '3' : WZ_KEY_KP_3;
	case SDL_SCANCODE_KP_4:         return (modifiers & SDL_KMOD_NUM) ? '4' : WZ_KEY_KP_4;
	case SDL_SCANCODE_KP_5:         return (modifiers & SDL_KMOD_NUM) ? '5' : WZ_KEY_KP_5;
	case SDL_SCANCODE_KP_6:         return (modifiers & SDL_KMOD_NUM) ? '6' : WZ_KEY_KP_6;
	case SDL_SCANCODE_KP_7:         return (modifiers & SDL_KMOD_NUM) ? '7' : WZ_KEY_KP_7;
	case SDL_SCANCODE_KP_8:         return (modifiers & SDL_KMOD_NUM) ? '8' : WZ_KEY_KP_8;
	case SDL_SCANCODE_KP_9:         return (modifiers & SDL_KMOD_NUM) ? '9' : WZ_KEY_KP_9;
	case SDL_SCANCODE_KP_0:         return (modifiers & SDL_KMOD_NUM) ? '0' : WZ_KEY_KP_0;
	case SDL_SCANCODE_KP_PERIOD:    return (modifiers & SDL_KMOD_NUM) ? '.' : WZ_KEY_KP_PERIOD;

	case SDL_SCANCODE_APPLICATION:  return WZ_KEY_APPLICATION;
	case SDL_SCANCODE_POWER:        return WZ_KEY_POWER;
	case SDL_SCANCODE_KP_EQUALS:    return WZ_KEY_KP_EQUALS;
	case SDL_SCANCODE_F13:          return WZ_KEY_F13;
	case SDL_SCANCODE_F14:          return WZ_KEY_F14;
	case SDL_SCANCODE_F15:          return WZ_KEY_F15;
	case SDL_SCANCODE_F16:          return WZ_KEY_F16;
	case SDL_SCANCODE_F17:          return WZ_KEY_F17;
	case SDL_SCANCODE_F18:          return WZ_KEY_F18;
	case SDL_SCANCODE_F19:          return WZ_KEY_F19;
	case SDL_SCANCODE_F20:          return WZ_KEY_F20;
	case SDL_SCANCODE_F21:          return WZ_KEY_F21;
	case SDL_SCANCODE_F22:          return WZ_KEY_F22;
	case SDL_SCANCODE_F23:          return WZ_KEY_F23;
	case SDL_SCANCODE_F24:          return WZ_KEY_F24;

	case SDL_SCANCODE_EXECUTE:      return WZ_KEY_EXECUTE;
	case SDL_SCANCODE_HELP:         return WZ_KEY_HELP;
	case SDL_SCANCODE_MENU:         return WZ_KEY_MENU;
	case SDL_SCANCODE_SELECT:       return WZ_KEY_SELECT;
	case SDL_SCANCODE_STOP:         return WZ_KEY_STOP;
	case SDL_SCANCODE_AGAIN:        return WZ_KEY_AGAIN;
	case SDL_SCANCODE_UNDO:         return WZ_KEY_UNDO;
	case SDL_SCANCODE_CUT:          return WZ_KEY_CUT;
	case SDL_SCANCODE_COPY:         return WZ_KEY_COPY;
	case SDL_SCANCODE_PASTE:        return WZ_KEY_PASTE;
	case SDL_SCANCODE_FIND:         return WZ_KEY_FIND;
	case SDL_SCANCODE_MUTE:         return WZ_KEY_MUTE;
	case SDL_SCANCODE_VOLUMEUP:     return WZ_KEY_VOLUMEUP;
	case SDL_SCANCODE_VOLUMEDOWN:   return WZ_KEY_VOLUMEDOWN;

	case SDL_SCANCODE_LCTRL:        return WZ_KEY_LCTRL;
	case SDL_SCANCODE_LSHIFT:       return WZ_KEY_LSHIFT;
	case SDL_SCANCODE_LALT:         return WZ_KEY_LALT;
	case SDL_SCANCODE_LGUI:         return WZ_KEY_LGUI;
	case SDL_SCANCODE_RCTRL:        return WZ_KEY_RCTRL;
	case SDL_SCANCODE_RSHIFT:       return WZ_KEY_RSHIFT;
	case SDL_SCANCODE_RALT:         return WZ_KEY_RALT;
	case SDL_SCANCODE_RGUI:         return WZ_KEY_RGUI;

	case SDL_SCANCODE_MODE:         return WZ_KEY_MODE;

	default:
		return WZ_KEY_UNKNOWN;
	}
}

// Helper: Check if keycode is printable ASCII
bool wz_keycode_is_printable(WZ_Keycode keycode)
{
	return keycode >= 32 && keycode <= 126;
}

// Helper: Check if keycode is ASCII (printable or control)
bool wz_keycode_is_ascii(WZ_Keycode keycode)
{
	return keycode >= 0 && keycode < 128;
}

// Helper: Check if keycode is extended (special key)
bool wz_keycode_is_extended(WZ_Keycode keycode)
{
	return keycode >= 128 && keycode < WZ_KEY_COUNT;
}

void render_text(SDL_Renderer* renderer, stbtt_fontinfo* font,
	const char* text, int x, int y, float font_size, unsigned len)
{
	float scale = stbtt_ScaleForPixelHeight(font, font_size);
	int ascent, descent, lineGap;
	stbtt_GetFontVMetrics(font, &ascent, &descent, &lineGap);
	int baseline = (int)(ascent * scale);
	float current_x = x;  // Changed to float
	int current_y = y;

	for (int i = 0; i < len; i++)
	{
		// Handle newlines
		if (text[i] == '\n') {
			current_x = x;
			current_y += (int)((ascent - descent + lineGap) * scale);
			continue;
		}
		// Get glyph bitmap
		int width, height, xoff, yoff;
		unsigned char* bitmap = stbtt_GetCodepointBitmap(font, 0, scale,
			text[i], &width, &height,
			&xoff, &yoff);
		// Get horizontal advance for this character
		int advance, lsb;
		stbtt_GetCodepointHMetrics(font, text[i], &advance, &lsb);
		if (bitmap)
		{
			// Render the bitmap pixel by pixel
			for (int row = 0; row < height; row++) {
				for (int col = 0; col < width; col++) {
					unsigned char pixel = bitmap[row * width + col];
					if (pixel > 0) {
						// Set color based on alpha value
						SDL_SetRenderDrawColor(renderer, 0, 0, 0, pixel);
						SDL_RenderPoint(renderer,
							(int)current_x + xoff + col,  // Cast to int only when rendering
							current_y + baseline + yoff + row);
					}
				}
			}
			stbtt_FreeBitmap(bitmap, NULL);
		}
		// Move to next character position - keep as float!
		current_x += advance * scale;  // Removed (int) cast
		// Apply kerning if there's a next character
		if (i + 1 < len)  // Also fixed this from text[i+1]
		{
			int kern = stbtt_GetCodepointKernAdvance(font, text[i], text[i + 1]);
			current_x += kern * scale;  // Removed (int) cast
		}
	}
}

typedef struct Wsdl
{
	WzKeyboard keyboard;
} Wsdl;

Wsdl wsdl;

typedef struct
{	
	SDL_Window* window;
	SDL_Renderer* renderer;
	TTF_TextEngine* text_engine;
	WzGui gui;
} WSDL_Context;

WSDL_Context main_window;
WSDL_Context gui_window;

stbtt_fontinfo font;
float font_height;
SDL_Texture* x_icon_texture;


// Color component extraction macros (for RGBA format: 0xRRGGBBAA)
#define WZ_COLOR_R(c) (((c) >> 24) & 0xFF)
#define WZ_COLOR_G(c) (((c) >> 16) & 0xFF)
#define WZ_COLOR_B(c) (((c) >> 8) & 0xFF)
#define WZ_COLOR_A(c) ((c) & 0xFF)

//#define assert(x) void(x)

#define MAX_NUM_TEXTURES_IN_ANIMATION 128

typedef struct Animation
{
	SDL_Texture* textures[MAX_NUM_TEXTURES_IN_ANIMATION];
	unsigned textures_count;
	unsigned w, h;
} Animation;

typedef struct WzWidgetPersistentData
{
	WzWidget widget;
	float transform_x, transform_y;
	float scale_x, scale_y;
	float rotation;

	Animation animation;
	unsigned current_animation_frame;
} WzWidgetPersistentData;

#define MAX_NUM_IMAGE_WIDGETS 32
WzWidgetPersistentData image_widgets[MAX_NUM_IMAGE_WIDGETS];
unsigned image_widgets_count;

WzGui gui;

WzTexture texture;

#define MAX_NUM_RENDERERS 16
#define MAX_NUM_WINDOWS 16


WzState left_mouse;

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 540

bool error;



bool load_animation(SDL_Renderer* renderer, Animation* animation, const char* path)
{
	bool success = true;
	IMG_Animation* anim = IMG_LoadAnimation(path);

	if (!anim)
	{
		return false;
	}

	if (anim->count == MAX_NUM_TEXTURES_IN_ANIMATION)
	{
		SDL_SetError("Loaded to many gifs");
		return false;
	}

	*animation = (Animation){ .textures_count = anim->count, .w = anim->w, .h = anim->h };
	for (int i = 0; i < anim->count; i++)
	{
		animation->textures[i] = SDL_CreateTextureFromSurface(renderer, anim->frames[i]);
		if (!animation->textures[i])
		{
			return false;
		}
	}
}

void WSDL_WzEnd(WSDL_Context* context)
{
	Uint32 flags = SDL_GetWindowFlags(gui_window.window);
	float mouse_x = 0, mouse_y = 0;
	if (flags & SDL_WINDOW_MOUSE_FOCUS)
	{
		SDL_GetMouseState(&mouse_x, &mouse_y);
	}

	// Mouse Input
	{
		float mouse_x, mouse_y;
		Uint32 flags = SDL_GetMouseState(&mouse_x, &mouse_y);

		if (left_mouse == WZ_ACTIVATING)
		{
			left_mouse = WZ_ACTIVE;
		}
		else if (left_mouse == WZ_DEACTIVATING)
		{
			left_mouse = WZ_INACTIVE;
		}

		if (flags & SDL_BUTTON_LMASK)
		{
			if (left_mouse == WZ_INACTIVE)
			{
				left_mouse = WZ_ACTIVATING;
			}
		}
		else
		{
			if (left_mouse == WZ_ACTIVE)
			{
				left_mouse = WZ_DEACTIVATING;
			}
		}
	}


	wz_begin(WINDOW_WIDTH, WINDOW_HEIGHT,
		mouse_x, mouse_y, left_mouse, events, events_count, true);

	wz_end();

	// Draw
	SDL_Renderer* renderer = context->renderer;

	WzDrawCommandBuffer* buffer = &context->gui.commands_buffer;

	for (int i = 0; i < buffer->count; ++i)
	{
		WzDrawCommand command = buffer->commands[i];

		if (command.type == DrawCommandType_Rect)
		{
			SDL_SetRenderDrawColor(renderer,
				WZ_COLOR_R(command.color), WZ_COLOR_G(command.color),
				WZ_COLOR_B(command.color), WZ_COLOR_A(command.color));
			SDL_FRect rect = { command.x, command.y, command.w, command.h };
			SDL_RenderFillRect(renderer, &rect);
		}
		else if (command.type == DrawCommandType_Texture)
		{
			SDL_SetTextureBlendMode(command.texture.data, SDL_BLENDMODE_BLEND);

			SDL_FRect src = { (float)command.src_rect.x, (float)command.src_rect.y, (float)command.src_rect.w, (float)command.src_rect.h };
			SDL_FRect dest = { command.x, command.y, command.w, command.h };
			SDL_RenderTextureRotated(renderer, command.texture.data,
				&src, &dest, command.rotation_angle, 0, 0);
		}
		else if (command.type == WZ_DRAW_COMMAND_TYPE_TEXT)
		{
			render_text(renderer, &font,
				command.str.str, command.x, command.y, font_height, command.str.len);
		}
		else if (command.type == DrawCommandType_Line)
		{
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

			SDL_SetRenderDrawColor(renderer, WZ_COLOR_R(command.color), WZ_COLOR_G(command.color), WZ_COLOR_B(command.color), 255);
			SDL_RenderLine(renderer, command.line.x0, command.line.y0, command.line.x1, command.line.y1);
		}
		else if (command.type == DrawCommandType_LineDotted)
		{
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

			// Bresenheim
			int dx = abs(command.line.x1 - command.line.x0);
			int sx = command.line.x0 < command.line.x1 ? 1 : -1;
			int dy = -abs(command.line.y1 - command.line.y0);
			int sy = command.line.y0 < command.line.y1 ? 1 : -1;
			int error = dx + dy;
			int x0 = command.line.x0;
			int y0 = command.line.y0;
			int x1 = command.line.x1;
			int y1 = command.line.y1;

			bool draw = true;

			while (1)
			{
				if (draw)
				{
					SDL_RenderPoint(renderer, x0, y0);
				}
				draw = !draw;

				int e2 = 2 * error;

				if (e2 >= dy)
				{
					if (x0 == x1)
					{
						break;
					}

					error = error + dy;
					x0 = x0 + sx;
				}

				if (e2 <= dx)
				{
					if (y0 == y1)
					{
						break;
					}

					error = error + dx;
					y0 = y0 + sy;
				}
			}
		}
		else if (command.type == DrawCommandType_VerticalLine)
		{
			SDL_SetRenderDrawColor(renderer, 125, 125, 125, 255);

			int x = command.x;
			int y = command.y;
			int h = command.h;
			for (int i = y; i < y + h; ++i)
			{
				if (i % 2 == 1) continue;
				SDL_RenderPoint(renderer, (float)x, (float)i);
			}
		}
		else if (command.type == DrawCommandType_HorizontalLine)
		{
			SDL_SetRenderDrawColor(renderer, 125, 125, 125, 255);
			int x = command.x;
			int y = command.y;
			int w = command.w;
			for (int i = x; i < x + w; ++i)
			{
				if (i % 2 == 1) continue;
				SDL_RenderPoint(renderer, (float)i, (float)y);
			}
		}
		else if (command.type == DrawCommandType_Clip)
		{
			SDL_Rect rect = { (int)command.x, (int)command.y, (int)command.w, (int)command.h };
			SDL_SetRenderClipRect(renderer, &rect);
		}
		else if (command.type == DrawCommandType_StopClip)
		{
			SDL_SetRenderClipRect(renderer, 0);
		}
	}

	SDL_SetRenderClipRect(renderer, 0);

	SDL_RenderPresent(context->renderer);
}
void WSDL_HandleInput()
{
	for (unsigned i = 0; i < 512; ++i)
	{
		if (wsdl.keyboard.keys[i] == WZ_ACTIVATING)
		{
			wsdl.keyboard.keys[i] = WZ_ACTIVE;
		}
		if (wsdl.keyboard.keys[i] == WZ_DEACTIVATING)
		{
			wsdl.keyboard.keys[i] = WZ_INACTIVE;
		}
	}

	int num_keys = 0;
	bool* keys = SDL_GetKeyboardState(&num_keys);
	for (unsigned i = 0; i < num_keys; ++i)
	{
		WZ_Keycode keycode = wz_sdl_scancode_to_keycode(i, 0);
		{
			if (keys[i])
			{
				if (wsdl.keyboard.keys[keycode] == WZ_INACTIVE)
				{
					wsdl.keyboard.keys[keycode] = WZ_ACTIVATING;
				}
			}
			else
			{
				if (wsdl.keyboard.keys[keycode] == WZ_ACTIVE)
				{
					wsdl.keyboard.keys[keycode] = WZ_DEACTIVATING;
				}
			}
		}
	}

}

static int WSDL_TranslateToWzKeycodes(SDL_Scancode scancode) {
	// Handle ASCII range (a-z, 0-9, punctuation, etc.)
	// SDL scancodes for letters are offset from ASCII
	if (scancode >= SDL_SCANCODE_A && scancode <= SDL_SCANCODE_Z) {
		return 'a' + (scancode - SDL_SCANCODE_A);  // Map to lowercase ASCII
	}

	if (scancode >= SDL_SCANCODE_1 && scancode <= SDL_SCANCODE_9) {
		return '1' + (scancode - SDL_SCANCODE_1);
	}

	if (scancode == SDL_SCANCODE_0) return '0';


	// Special ASCII characters
	switch (scancode) {
	case SDL_SCANCODE_SPACE: return ' ';
	case SDL_SCANCODE_RETURN: return '\n';
	case SDL_SCANCODE_TAB: return '\t';
	case SDL_SCANCODE_BACKSPACE: return '\b';
	case SDL_SCANCODE_ESCAPE: return 27;  // ESC

	// Punctuation (you may need to adjust these based on your keyboard layout)
	case SDL_SCANCODE_MINUS: return '-';
	case SDL_SCANCODE_EQUALS: return '=';
	case SDL_SCANCODE_LEFTBRACKET: return '[';
	case SDL_SCANCODE_RIGHTBRACKET: return ']';
	case SDL_SCANCODE_BACKSLASH: return '\\';
	case SDL_SCANCODE_SEMICOLON: return ';';
	case SDL_SCANCODE_APOSTROPHE: return '\'';
	case SDL_SCANCODE_GRAVE: return '`';
	case SDL_SCANCODE_COMMA: return ',';
	case SDL_SCANCODE_PERIOD: return '.';
	case SDL_SCANCODE_SLASH: return '/';
	}

	// Extended keys (>= 128)
	switch (scancode) {
	case SDL_SCANCODE_CAPSLOCK: return WZ_KEY_CAPSLOCK;
	case SDL_SCANCODE_F1: return WZ_KEY_F1;
	case SDL_SCANCODE_F2: return WZ_KEY_F2;
	case SDL_SCANCODE_F3: return WZ_KEY_F3;
	case SDL_SCANCODE_F4: return WZ_KEY_F4;
	case SDL_SCANCODE_F5: return WZ_KEY_F5;
	case SDL_SCANCODE_F6: return WZ_KEY_F6;
	case SDL_SCANCODE_F7: return WZ_KEY_F7;
	case SDL_SCANCODE_F8: return WZ_KEY_F8;
	case SDL_SCANCODE_F9: return WZ_KEY_F9;
	case SDL_SCANCODE_F10: return WZ_KEY_F10;
	case SDL_SCANCODE_F11: return WZ_KEY_F11;
	case SDL_SCANCODE_F12: return WZ_KEY_F12;
	case SDL_SCANCODE_PRINTSCREEN: return WZ_KEY_PRINTSCREEN;
	case SDL_SCANCODE_SCROLLLOCK: return WZ_KEY_SCROLLLOCK;
	case SDL_SCANCODE_PAUSE: return WZ_KEY_PAUSE;
	case SDL_SCANCODE_INSERT: return WZ_KEY_INSERT;
	case SDL_SCANCODE_HOME: return WZ_KEY_HOME;
	case SDL_SCANCODE_PAGEUP: return WZ_KEY_PAGEUP;
	case SDL_SCANCODE_DELETE: return WZ_KEY_DELETE;
	case SDL_SCANCODE_END: return WZ_KEY_END;
	case SDL_SCANCODE_PAGEDOWN: return WZ_KEY_PAGEDOWN;
	case SDL_SCANCODE_RIGHT: return WZ_KEY_RIGHT;
	case SDL_SCANCODE_LEFT: return WZ_KEY_LEFT;
	case SDL_SCANCODE_DOWN: return WZ_KEY_DOWN;
	case SDL_SCANCODE_UP: return WZ_KEY_UP;
	case SDL_SCANCODE_NUMLOCKCLEAR: return WZ_KEY_NUMLOCKCLEAR;
	case SDL_SCANCODE_KP_DIVIDE: return WZ_KEY_KP_DIVIDE;
	case SDL_SCANCODE_KP_MULTIPLY: return WZ_KEY_KP_MULTIPLY;
	case SDL_SCANCODE_KP_MINUS: return WZ_KEY_KP_MINUS;
	case SDL_SCANCODE_KP_PLUS: return WZ_KEY_KP_PLUS;
	case SDL_SCANCODE_KP_ENTER: return WZ_KEY_KP_ENTER;
	case SDL_SCANCODE_KP_1: return WZ_KEY_KP_1;
	case SDL_SCANCODE_KP_2: return WZ_KEY_KP_2;
	case SDL_SCANCODE_KP_3: return WZ_KEY_KP_3;
	case SDL_SCANCODE_KP_4: return WZ_KEY_KP_4;
	case SDL_SCANCODE_KP_5: return WZ_KEY_KP_5;
	case SDL_SCANCODE_KP_6: return WZ_KEY_KP_6;
	case SDL_SCANCODE_KP_7: return WZ_KEY_KP_7;
	case SDL_SCANCODE_KP_8: return WZ_KEY_KP_8;
	case SDL_SCANCODE_KP_9: return WZ_KEY_KP_9;
	case SDL_SCANCODE_KP_0: return WZ_KEY_KP_0;
	case SDL_SCANCODE_KP_PERIOD: return WZ_KEY_KP_PERIOD;
	case SDL_SCANCODE_APPLICATION: return WZ_KEY_APPLICATION;
	case SDL_SCANCODE_POWER: return WZ_KEY_POWER;
	case SDL_SCANCODE_KP_EQUALS: return WZ_KEY_KP_EQUALS;
	case SDL_SCANCODE_F13: return WZ_KEY_F13;
	case SDL_SCANCODE_F14: return WZ_KEY_F14;
	case SDL_SCANCODE_F15: return WZ_KEY_F15;
	case SDL_SCANCODE_F16: return WZ_KEY_F16;
	case SDL_SCANCODE_F17: return WZ_KEY_F17;
	case SDL_SCANCODE_F18: return WZ_KEY_F18;
	case SDL_SCANCODE_F19: return WZ_KEY_F19;
	case SDL_SCANCODE_F20: return WZ_KEY_F20;
	case SDL_SCANCODE_F21: return WZ_KEY_F21;
	case SDL_SCANCODE_F22: return WZ_KEY_F22;
	case SDL_SCANCODE_F23: return WZ_KEY_F23;
	case SDL_SCANCODE_F24: return WZ_KEY_F24;
	case SDL_SCANCODE_EXECUTE: return WZ_KEY_EXECUTE;
	case SDL_SCANCODE_HELP: return WZ_KEY_HELP;
	case SDL_SCANCODE_MENU: return WZ_KEY_MENU;
	case SDL_SCANCODE_SELECT: return WZ_KEY_SELECT;
	case SDL_SCANCODE_STOP: return WZ_KEY_STOP;
	case SDL_SCANCODE_AGAIN: return WZ_KEY_AGAIN;
	case SDL_SCANCODE_UNDO: return WZ_KEY_UNDO;
	case SDL_SCANCODE_CUT: return WZ_KEY_CUT;
	case SDL_SCANCODE_COPY: return WZ_KEY_COPY;
	case SDL_SCANCODE_PASTE: return WZ_KEY_PASTE;
	case SDL_SCANCODE_FIND: return WZ_KEY_FIND;
	case SDL_SCANCODE_MUTE: return WZ_KEY_MUTE;
	case SDL_SCANCODE_VOLUMEUP: return WZ_KEY_VOLUMEUP;
	case SDL_SCANCODE_VOLUMEDOWN: return WZ_KEY_VOLUMEDOWN;
	case SDL_SCANCODE_LCTRL: return WZ_KEY_LCTRL;
	case SDL_SCANCODE_LSHIFT: return WZ_KEY_LSHIFT;
	case SDL_SCANCODE_LALT: return WZ_KEY_LALT;
	case SDL_SCANCODE_LGUI: return WZ_KEY_LGUI;
	case SDL_SCANCODE_RCTRL: return WZ_KEY_RCTRL;
	case SDL_SCANCODE_RSHIFT: return WZ_KEY_RSHIFT;
	case SDL_SCANCODE_RALT: return WZ_KEY_RALT;
	case SDL_SCANCODE_RGUI: return WZ_KEY_RGUI;
	case SDL_SCANCODE_MODE: return WZ_KEY_MODE;

	default: return WZ_KEY_UNKNOWN;
	}
}


SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
	WzEvent wz_event = { 0 };

	switch (event->type) {
	case SDL_EVENT_KEY_DOWN: {
		unsigned wz_key = (unsigned)WSDL_TranslateToWzKeycodes(event->key.scancode);
		if (wz_key != WZ_KEY_UNKNOWN) {
			bool is_repeat = (event->key.repeat != 0);
			wz_event.key = (WzKeyboardEvent)
			{
				.type = WZ_EVENT_TYPE_KEYBOARD,
				.key = wz_key,
				.down = true,
				.repeat = is_repeat,
				.mod = sdl_keymod_to_wz(event->key.mod),
			};
		}
		break;
	}

	case SDL_EVENT_KEY_UP: {
		int wz_key = WSDL_TranslateToWzKeycodes(event->key.scancode);
		if (wz_key != WZ_KEY_UNKNOWN) {
			wz_event.key = (WzKeyboardEvent)
			{
				.type = WZ_EVENT_TYPE_KEYBOARD ,
				.mod = sdl_keymod_to_wz(event->key.mod),
			};
		}
		break;
	}

	// Mouse button events
	case SDL_EVENT_MOUSE_BUTTON_DOWN: {
		wz_input_mouse_button_event(
			event->button.button, 
			true,
			event->button.x,
			event->button.y
		);

		wz_event.button = (WzButtonEvent)
		{
			.button = event->button.button,
			.down = true,
			.x = event->button.x,
			.y = event->button.y,
		};
		break;
	}

	case SDL_EVENT_MOUSE_BUTTON_UP: {
		wz_input_mouse_button_event(
			event->button.button,
			false,
			event->button.x,
			event->button.y
		);

		wz_event.button = (WzButtonEvent)
		{
			.button = event->button.button,
			.x = event->button.x,
			.y = event->button.y,
		};

		break;
	}

	case SDL_EVENT_QUIT:
		return SDL_APP_SUCCESS;
	}

	if (wz_event.type)
	{
		events[events_count++] = wz_event;
		assert(events_count < MAX_NUM_EVENTS);
	}

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate)
{
	wz_gui_deinit(&gui);
}

SDL_EnumerationResult SDLCALL onFile(void* userdata, const char* dirname, const char* filename)
{
#if 0
	char path[512];
	sprintf(path, "%s%s", dirname, filename);

	assert(image_widgets_count < MAX_NUM_IMAGE_WIDGETS - 1);
	load_animation(renderer, &image_widgets[image_widgets_count].animation, path);
	image_widgets[image_widgets_count].scale_x = 1;
	image_widgets[image_widgets_count].scale_y = 1;
	image_widgets_count++;
#endif

	return SDL_ENUM_CONTINUE;
}

void load_animations()
{
	bool result = SDL_EnumerateDirectory("Gifs", onFile, 0);
	if (!result)
	{
		SDL_ShowSimpleMessageBox(
			SDL_MESSAGEBOX_ERROR,
			"Fatal Error",
			SDL_GetError(),
			NULL
		);

		return SDL_APP_FAILURE;
	}
}

void WSDL_ContextCreate(WSDL_Context* context, unsigned window_width, unsigned window_height)
{
	bool success = true;
	SDL_Renderer* renderer;
	SDL_Window* window;
	TTF_TextEngine* text_engine;

	success &= SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d");

	success &= SDL_CreateWindowAndRenderer("my window", window_width, window_height,
		SDL_WINDOW_RESIZABLE, &window, &renderer);

	success &= SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	text_engine = TTF_CreateRendererTextEngine(renderer);
	success &= (bool)text_engine;

	if (!success)
	{
		SDL_ShowSimpleMessageBox(
			SDL_MESSAGEBOX_ERROR,
			"Fatal Error",
			SDL_GetError(),
			NULL
		);
	}

	context->renderer = renderer;
	context->window = window;
	context->text_engine = text_engine;
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	bool success = false;

	WSDL_ContextCreate(&gui_window, WINDOW_WIDTH, WINDOW_HEIGHT);

	// stb fonts
	{
		font_height = 18;
		size_t file_size;
		char *data = SDL_LoadFile("C:\\Windows\\Fonts\\Arial.ttf", &file_size);
		if (!data) {
			SDL_Log("Failed to open file: %s", SDL_GetError());
			SDL_Quit();
			return 1;
		}

		if (!stbtt_InitFont(&font, data, 0)) {
			fprintf(stderr, "Failed to initialize font\n");
			SDL_Quit();
			return 1;
		}
	}

	// X icon
	{
		// TODO: Remove width and height fields of wz texture
		SDL_Surface* surface = SDL_CreateSurfaceFrom(16, 16, SDL_PIXELFORMAT_RGBA8888, wz_x_icon, 16 * 4);
		success |= (bool)surface;
		gui.x_icon = (WzTexture){ .w = surface->w, .h = surface->h, .data = SDL_CreateTextureFromSurface(gui_window.renderer, surface) };
	}

	if (!success)
	{
		SDL_ShowSimpleMessageBox(
			SDL_MESSAGEBOX_ERROR,
			"Fatal Error",
			SDL_GetError(),
			NULL
		);

		return SDL_APP_FAILURE;
	}

	load_animations();

	return SDL_APP_CONTINUE;
}

void get_string_size(char* str, unsigned start, unsigned end, float* w, float* h)
{
	float scale = stbtt_ScaleForPixelHeight(&font, font_height);
	float width = 0;
	for (int i = start; i < end; i++)
	{
		int advance, lsb;
		stbtt_GetCodepointHMetrics(&font, str[i], &advance, &lsb);
		width += advance * scale;
		// Add kerning if there's a next character WITHIN THE RANGE
		if (i + 1 < end) {  // Changed from if (str[i + 1])
			int kern = stbtt_GetCodepointKernAdvance(&font, str[i], str[i + 1]);
			width += kern * scale;
		}
	}
	*w = width;
	*h = font_height;
}


void WSDL_WindowContextBegin(WSDL_Context* context)
{
	bool success = true;
	success &= SDL_SetRenderDrawColor(context->renderer, 0x20, 0x8c, 0x71, 255);
	success &= SDL_RenderClear(context->renderer);
}

WzWidget WSDL_WzBegin(WSDL_Context* context)
{
	int window_width, window_height;
	SDL_GetWindowSize(context->window, &window_width, &window_height);

	WSDL_WindowContextBegin(&gui_window);
}

bool WSDL_IsInteracting(char k)
{
	bool result = wsdl.keyboard.keys[k] == WZ_ACTIVATING || wsdl.keyboard.keys[k] == WZ_ACTIVE;

	return result;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
	unsigned time_beginning = SDL_GetTicks();

	bool success = true;

	WSDL_WzBegin(&gui_window);
	wz_set_gui(&gui_window.gui);
	wz_set_string_size_callback(get_string_size);
	
	WzWidget window = wz_vbox((WzWidget) { 0 });
	wz_widget_set_tight_constraints(window, WINDOW_WIDTH, WINDOW_HEIGHT);
	wz_widget_set_border(window, WZ_BORDER_TYPE_NONE);	
	
	{
		gui_window.gui.pasted_text = SDL_GetClipboardText();
		WzWidget input_box = wz_input_box(window);
	}
	
	WSDL_WzEnd(&gui_window);

	unsigned delta_time = SDL_GetTicks() - time_beginning;
	if (delta_time < 42)
	{
		SDL_Delay(42 - delta_time);
	}

	if (!success)
	{
		SDL_ShowSimpleMessageBox(
			SDL_MESSAGEBOX_ERROR,
			"Fatal Error",
			SDL_GetError(),
			NULL
		);

		return SDL_APP_FAILURE;
	}
	
	events_count = 0;

	return SDL_APP_CONTINUE;
}