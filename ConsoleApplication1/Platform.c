#include "Platform.h"
//#include "Editor.h"
#include <SDL3/SDL_main.h>
#include "Game.h"
#include "Editor.h"

void PlatformTextDrawColor(const char* str, float x, float y, char r, char g, char b, char a) {
	TTF_Text* text = TTF_CreateText(sdl.engine, sdl.font, str, 0);
	int w = 11, h = 22;
	TTF_SetTextColor(text, r, g, b, a);
	TTF_DrawRendererText(text, x, y);
	TTF_DestroyText(text);
}

void PlatformTextDraw(const char* str, float x, float y) {
	TTF_Text* text = TTF_CreateText(sdl.engine, sdl.font, str, 0);
	TTF_SetTextColor(text, 0, 0, 0, 255);
	TTF_DrawRendererText(text, x, y);
	TTF_DestroyText(text);
}

PlatformV2i PlatformTextGetSize(const char* str) {
	PlatformV2i result = { 0 };
	TTF_Text* text = TTF_CreateText(sdl.engine, sdl.font, str, 0);
	TTF_GetTextSize(text, &result.x, &result.y);
	TTF_DestroyText(text);

	return result;
}

void PlatformRectDraw(PlatformRect rect, platform_color color) {
	SDL_SetRenderDrawBlendMode(sdl.renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(sdl.renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(sdl.renderer, &rect);
}

void PlatformTextureDraw(PlatformTexture texture, PlatformRect rect) {
	SDL_RenderTexture(sdl.renderer, texture.data, 0, &rect);
}

void PlatformTextureDrawFromSource(PlatformTexture texture, PlatformRect dest, PlatformRect src, platform_color color) {
	if (!(color.r == 0 && color.g == 0 && color.b == 0))
		SDL_SetTextureColorMod(texture.data, color.r, color.g, color.b);
	SDL_RenderTexture(sdl.renderer, texture.data, &src, &dest);
}

PlatformTargetTexture PlatformTargetTextureCreate() {
	SDL_Texture* texture = SDL_CreateTexture(sdl.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, TARGET_TEXTURE_WIDTH, TARGET_TEXTURE_HEIGHT);
	PlatformTargetTexture result = { .data = texture, .w = TARGET_TEXTURE_WIDTH , .h = TARGET_TEXTURE_HEIGHT };

	return result;
}

void PlatformTextureBeginTarget(PlatformTargetTexture texture) {
	SDL_SetRenderTarget(sdl.renderer, texture.data);
}

void PlatformTextureEndTarget() {
	SDL_SetRenderTarget(sdl.renderer, 0);
}

void PlatformLineDraw(float x0, float y0, float x1, float y1, float r, float g, float b) {
	SDL_SetRenderDrawColor(sdl.renderer, r, g, b, 255);
	SDL_RenderLine(sdl.renderer, x0, y0, x1, y1);
}

void PlatformLineDrawHorizontal(float x0, float y0, float x1, float y1) {
	SDL_SetRenderDrawColor(sdl.renderer, 125, 125, 125, 255);

	for (int i = x0; i < x1; ++i) {
		if (i % 2 == 1) continue;
		SDL_RenderPoint(sdl.renderer, (int)i, (int)y0);
	}
}

void PlatformLineDrawVertical(float x0, float y0, float x1, float y1) {
	SDL_SetRenderDrawColor(sdl.renderer, 125, 125, 125, 255);

	for (int i = y0; i < y1; ++i) {
		if (i % 2 == 1) continue;
		SDL_RenderPoint(sdl.renderer, (int)x0, (int)i);
	}
}

PlatformTexture PlatformTextureLoad(const char* path) {
	float w, h;
	SDL_Surface* surface = IMG_Load(path);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(sdl.renderer, surface);
	SDL_GetTextureSize(texture, &w, &h);
	PlatformTexture result = (PlatformTexture){ .data = texture, .w = w, .h = h };

	return result;
}

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d");

	/* Create the window */
	if (!SDL_CreateWindowAndRenderer("Hello World", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &sdl.window, &sdl.renderer)) {
		SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	TTF_Init();
	sdl.font = TTF_OpenFont("C:\\Users\\elior\\AppData\\Local\\Microsoft\\Windows\\Fonts\\FragmentMono-Regular.ttf", PLATFORM_FONT_HEIGHT);
	assert(sdl.font);

	sdl.engine = TTF_CreateRendererTextEngine(sdl.renderer);
	assert(sdl.engine);

	game_init();

	sdl.mutex =  SDL_CreateMutex();

	sdl.cursor_hand = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER);
	sdl.cursor_default = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
	sdl.cursor_horizontal_arrow = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_EW_RESIZE);
	sdl.cursor_vertical_arrow = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NS_RESIZE);

	return SDL_APP_CONTINUE;
}

SDL_AppResult handle_events(SDL_Event *event) {
	//if (!sdl.events_count) return;

	//SDL_Event *event = &sdl.events[sdl.events_count];	

	for (int i = 0; i < 128; ++i) {
		if (platform.keyboard_states[i] == Deactivating) {
			platform.keyboard_states[i] = Inactive;
		}
		if (platform.keyboard_states[i] == Activating) {
			platform.keyboard_states[i] = Active;
		}
	}

	if (event->type == SDL_EVENT_QUIT) {
		return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
	}

	if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
		if (event->button.button == SDL_BUTTON_LEFT) {
			if (platform.mouse_left == Inactive) {
				platform.mouse_left = Activating;
			}
		}
	}
	else if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
		if (event->button.button == SDL_BUTTON_LEFT) {
			platform.mouse_left = Deactivating;
		}
	}

	// Keyboard input
	SDL_Keycode keycode = 0;

	if (event->type == SDL_EVENT_KEY_DOWN) {
		keycode = SDL_GetKeyFromScancode(event->key.scancode, event->key.mod, false);
		if (keycode < 128) {
			if (platform.keyboard_states[keycode] == Inactive) {
				platform.keyboard_states[keycode] = Activating;
			}
			else if (platform.keyboard_states[keycode] == Activating) {
				platform.keyboard_states[keycode] = Active;
			}
		}
	}
	else if (event->type == SDL_EVENT_KEY_UP) {
		keycode = SDL_GetKeyFromScancode(event->key.scancode, event->key.mod, false);
		if (keycode < 128) {
			if (platform.keyboard_states[keycode] == Active || platform.keyboard_states[keycode] == Activating) {
				platform.keyboard_states[keycode] = Deactivating;
			}
			else if (platform.keyboard_states[keycode] == Deactivating) platform.keyboard_states[keycode] = Inactive;
		}
	}
	else if (event->type == SDL_EVENT_WINDOW_MOUSE_LEAVE) {
		platform.focus = false;
	}
	else if (event->type == SDL_EVENT_WINDOW_MOUSE_ENTER) {
		platform.focus = true;
	}

	if (keycode < 128 && keycode > 0) {
		platform.keys_pressed.keys[platform.keys_pressed.count++] = (PlatformKey){ .val = keycode, .state = platform.keyboard_states[keycode] };
	}

	return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
	SDL_AppResult result = handle_events(event);

	return result;
}

void platform_cursor_set(PlatformCursor cursor) {
	switch(cursor) {
		case PlatformCursorDefault:
			SDL_SetCursor(sdl.cursor_default);
			break;
		case PlatformCursorHand:
			SDL_SetCursor(sdl.cursor_hand);
			break;
		case PlatformCursorHorizontalArrow:
			SDL_SetCursor(sdl.cursor_horizontal_arrow);
			break;
		case PlatformCursorVerticalArrow:
			SDL_SetCursor(sdl.cursor_vertical_arrow);
			break;
	}
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void* appstate)
{
	/*if (sdl.event->type == ) {
		SDL_Delay(16.3);
		return SDL_APP_CONTINUE;
	}*/

	unsigned int time0 = SDL_GetTicks();

	SDL_GetMouseState(&platform.mouse_x, &platform.mouse_y);
	if (!platform.focus) {
		platform.mouse_x = -1;
		platform.mouse_y = -1;
	}

	if (platform.mouse_x == platform.previous_mouse_x && platform.mouse_y == platform.previous_mouse_y) {
		//SDL_Delay(16.3);
		//return SDL_APP_CONTINUE;
	}

	platform_color color = PLATFORM_LIGHTGRAY;
	SDL_SetRenderDrawColor(sdl.renderer, color.r, color.g, color.b, color.a);
	SDL_RenderClear(sdl.renderer);
	SDL_GetWindowSize(sdl.window, &platform.window_width, &platform.window_height);

	// Editor
	static Egui gui;
	static wzrd_draw_commands_buffer buffer;
	static wzrd_cursor cursor;
	editor_do(&gui, &buffer, &cursor);

	wzrd_box* box = wzrd_box_get_by_name(&gui, str128_create("Target"));
	wzrd_rect rect = wzrd_box_get_rect(box);
	PlatformRect game_screen_rect = *(PlatformRect*)&rect;

	float mouse_x = platform.mouse_x - game_screen_rect.x;
	float mouse_y = platform.mouse_y - game_screen_rect.y;
	game.mouse_delta = (v2){ mouse_x - game.mouse_pos.x, mouse_y - game.mouse_pos.y };
	game.mouse_pos.x = mouse_x;
	game.mouse_pos.y = mouse_y;

	// ...
	game_run((wzrd_v2){game_screen_rect.w, game_screen_rect.h}, &cursor);

	platform_cursor_set(*(PlatformCursor *)&cursor);

	// ...
	game_draw_gui_commands(&buffer);

	SDL_RenderPresent(sdl.renderer);

	unsigned int time1 = SDL_GetTicks();

	unsigned int elapsed_time = time1 - time0;
	
	if (elapsed_time < 16.3) {
		SDL_Delay(16.3 - (double)elapsed_time);
	}

	platform.previous_mouse_x = platform.mouse_x;
	platform.previous_mouse_y = platform.mouse_y;

	// Input
	platform.keys_pressed.count = 0;

	if (platform.mouse_left == Activating) {
		platform.mouse_left = Active;
	}
	else if (platform.mouse_left == Deactivating) {
		platform.mouse_left = Inactive;
	}

	return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
}
