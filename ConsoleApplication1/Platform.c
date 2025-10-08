#include "Platform.h"
//#include "Editor.h"
#include <SDL3/SDL_main.h>
#include "Game.h"
#include "Editor.h"

PlatformSystem g_platform;

void PlatformTextDrawColor(const char* str, float x, float y, char r, char g, char b, char a) {
	TTF_Text* text = TTF_CreateText(g_sdl.engine, g_sdl.font, str, 0);
	int w = 11, h = 22;
	TTF_SetTextColor(text, r, g, b, a);
	TTF_DrawRendererText(text, x, y);
	TTF_DestroyText(text);
}

void PlatformTextDraw(const char* str, float x, float y) {
	TTF_Text* text = TTF_CreateText(g_sdl.engine, g_sdl.font, str, 0);
	TTF_SetTextColor(text, 0, 0, 0, 255);
	TTF_DrawRendererText(text, x, y);
	TTF_DestroyText(text);
}

PlatformV2i PlatformTextGetSize(const char* str) {
	PlatformV2i result = { 0 };
	TTF_Text* text = TTF_CreateText(g_sdl.engine, g_sdl.font, str, 0);
	TTF_GetTextSize(text, &result.x, &result.y);
	TTF_DestroyText(text);

	return result;
}

void PlatformRectDraw(PlatformRect rect, platform_color color) {
	SDL_SetRenderDrawBlendMode(g_sdl.renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(g_sdl.renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(g_sdl.renderer, &rect);
}

void PlatformTextureDraw(PlatformTexture texture, PlatformRect rect) {
	SDL_RenderTexture(g_sdl.renderer, texture.data, 0, &rect);
}

void PlatformTextureDrawFromSource(PlatformTexture texture, PlatformRect dest, PlatformRect src, platform_color color) {
	if (!(color.r == 0 && color.g == 0 && color.b == 0))
		SDL_SetTextureColorMod(texture.data, color.r, color.g, color.b);
	SDL_RenderTexture(g_sdl.renderer, texture.data, &src, &dest);
}

PlatformTargetTexture PlatformTargetTextureCreate() {
	SDL_Texture* texture = SDL_CreateTexture(g_sdl.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, GAME_WIDTH, GAME_HEIGHT);
	SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
	PlatformTargetTexture result = { .data = texture, .w = GAME_WIDTH , .h = GAME_HEIGHT };

	return result;
}

void PlatformTextureBeginTarget(PlatformTargetTexture texture) {
	SDL_SetRenderTarget(g_sdl.renderer, texture.data);
}

void PlatformTextureEndTarget() {
	SDL_SetRenderTarget(g_sdl.renderer, 0);
}

void PlatformLineDraw(float x0, float y0, float x1, float y1, float r, float g, float b) {
	SDL_SetRenderDrawColor(g_sdl.renderer, r, g, b, 255);
	SDL_RenderLine(g_sdl.renderer, x0, y0, x1, y1);
}

void PlatformLineDrawHorizontal(float x0, float y0, float x1, float y1) {
	SDL_SetRenderDrawColor(g_sdl.renderer, 125, 125, 125, 255);

	for (int i = x0; i < x1; ++i) {
		if (i % 2 == 1) continue;
		SDL_RenderPoint(g_sdl.renderer, (int)i, (int)y0);
	}
}

void PlatformLineDrawVertical(float x0, float y0, float x1, float y1) {
	SDL_SetRenderDrawColor(g_sdl.renderer, 125, 125, 125, 255);

	for (int i = y0; i < y1; ++i) {
		if (i % 2 == 1) continue;
		SDL_RenderPoint(g_sdl.renderer, (int)x0, (int)i);
	}
}

PlatformTexture PlatformTextureLoad(const char* path) {
	float w, h;
	SDL_Surface* surface = IMG_Load(path);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(g_sdl.renderer, surface);
	SDL_GetTextureSize(texture, &w, &h);
	PlatformTexture result = (PlatformTexture){ .data = texture, .w = w, .h = h };

	return result;
}

#define DELETE_ME 0

SDL_Texture* g_target;
SDL_Texture* g_texture;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d");

	/* Create the window */
	if (!SDL_CreateWindowAndRenderer("Hello World", EDITOR_WIDTH, EDITOR_HEIGHT, SDL_WINDOW_RESIZABLE, &g_sdl.window, &g_sdl.renderer)) {
		SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

#if DELETE_ME
	SDL_SetRenderDrawColor(g_sdl.renderer, 0xc3, 0xc3, 0xc3, 255);
	SDL_RenderClear(g_sdl.renderer);
	
	g_target = SDL_CreateTexture(g_sdl.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, GAME_WIDTH, GAME_HEIGHT);
	SDL_SetTextureScaleMode(g_target, SDL_SCALEMODE_NEAREST);

	SDL_Surface* surface = IMG_Load("C:\\Users\\elior\\OneDrive\\Desktop\\ConsoleApplication1\\Resoruces\\Textures\\clouds.jpeg");
	g_texture = SDL_CreateTextureFromSurface(g_sdl.renderer, surface);

	SDL_SetRenderTarget(g_sdl.renderer, g_target);
	{
		SDL_SetRenderDrawColor(g_sdl.renderer, 0xFF, 0xFF, 0xFF, 255);
		SDL_RenderClear(g_sdl.renderer);

		SDL_RenderTexture(g_sdl.renderer, g_texture, 0, 0);
	}
	SDL_SetRenderTarget(g_sdl.renderer, 0);

#else
	TTF_Init();

	g_sdl.font = TTF_OpenFont("C:\\Users\\elior\\AppData\\Local\\Microsoft\\Windows\\Fonts\\FragmentMono-Regular.ttf", 16);
	assert(g_sdl.font);

	g_sdl.engine = TTF_CreateRendererTextEngine(g_sdl.renderer);
	assert(g_sdl.engine);

	game_init();

	g_sdl.mutex =  SDL_CreateMutex();

	g_sdl.cursor_hand = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER);
	g_sdl.cursor_default = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
	g_sdl.cursor_horizontal_arrow = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_EW_RESIZE);
	g_sdl.cursor_vertical_arrow = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NS_RESIZE);
	
#endif
	return SDL_APP_CONTINUE;
}

SDL_AppResult handle_events(SDL_Event *event) {
	//if (!sdl.events_count) return;

	//SDL_Event *event = &sdl.events[sdl.events_count];	

	for (int i = 0; i < 128; ++i) {
		if (g_platform.keyboard_states[i] == Deactivating) {
			g_platform.keyboard_states[i] = Inactive;
		}
		if (g_platform.keyboard_states[i] == Activating) {
			g_platform.keyboard_states[i] = Active;
		}
	}

	if (event->type == SDL_EVENT_QUIT) {
		return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
	}

	if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
		if (event->button.button == SDL_BUTTON_LEFT) {
			if (g_platform.mouse_left == Inactive) {
				g_platform.mouse_left = Activating;
			}
		}
	}
	else if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
		if (event->button.button == SDL_BUTTON_LEFT) {
			g_platform.mouse_left = Deactivating;
		}
	}

	// Keyboard input
	SDL_Keycode keycode = 0;

	if (event->type == SDL_EVENT_KEY_DOWN) {
		keycode = SDL_GetKeyFromScancode(event->key.scancode, event->key.mod, false);
		if (keycode < 128) {
			if (g_platform.keyboard_states[keycode] == Inactive) {
				g_platform.keyboard_states[keycode] = Activating;
			}
			else if (g_platform.keyboard_states[keycode] == Activating) {
				g_platform.keyboard_states[keycode] = Active;
			}
		}
	}
	else if (event->type == SDL_EVENT_KEY_UP) {
		keycode = SDL_GetKeyFromScancode(event->key.scancode, event->key.mod, false);
		if (keycode < 128) {
			if (g_platform.keyboard_states[keycode] == Active || g_platform.keyboard_states[keycode] == Activating) {
				g_platform.keyboard_states[keycode] = Deactivating;
			}
			else if (g_platform.keyboard_states[keycode] == Deactivating) g_platform.keyboard_states[keycode] = Inactive;
		}
	}
	else if (event->type == SDL_EVENT_WINDOW_MOUSE_LEAVE) {
		g_platform.focus = false;
	}
	else if (event->type == SDL_EVENT_WINDOW_MOUSE_ENTER) {
		g_platform.focus = true;
	}

	if (keycode < 128 && keycode > 0) {
		g_platform.keys_pressed.keys[g_platform.keys_pressed.count++] = (PlatformKey){ .val = keycode, .state = g_platform.keyboard_states[keycode] };
	}

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
	SDL_AppResult result = handle_events(event);

	return result;
}

void platform_cursor_set(PlatformCursor cursor) {
	switch(cursor) {
		case PlatformCursorDefault:
			SDL_SetCursor(g_sdl.cursor_default);
			break;
		case PlatformCursorHand:
			SDL_SetCursor(g_sdl.cursor_hand);
			break;
		case PlatformCursorHorizontalArrow:
			SDL_SetCursor(g_sdl.cursor_horizontal_arrow);
			break;
		case PlatformCursorVerticalArrow:
			SDL_SetCursor(g_sdl.cursor_vertical_arrow);
			break;
	}
}

void platform_debug_view_add(str128 str)
{
	//platform
}

void platform_debug_view_draw() {
	PlatformRectDraw((PlatformRect) { g_platform.window_width - 300, 10, 295, 500 }, (platform_color){50, 50, 50, 200});
}

void platform_end()
{
	SDL_RenderPresent(g_sdl.renderer);

	unsigned int time1 = SDL_GetTicks();

	//unsigned int elapsed_time = time1 - time0;

	//if (elapsed_time < 16.3) {
		//SDL_Delay(16.3 - (double)elapsed_time);
	//}


	// Input
	g_platform.keys_pressed.count = 0;

	if (g_platform.mouse_left == Activating) {
		g_platform.mouse_left = Active;
	}
	else if (g_platform.mouse_left == Deactivating) {
		g_platform.mouse_left = Inactive;
	}
}

void platform_begin() 
{

	unsigned int time0 = SDL_GetTicks();

	SDL_GetMouseState(&g_platform.mouse_x, &g_platform.mouse_y);

	g_platform.mouse_delta_x = g_platform.mouse_x - g_platform.previous_mouse_x;
	g_platform.mouse_delta_y = g_platform.mouse_y - g_platform.previous_mouse_y;

	g_platform.previous_mouse_x = g_platform.mouse_x;
	g_platform.previous_mouse_y = g_platform.mouse_y;

	if (!g_platform.focus) {
		g_platform.mouse_x = -1;
		g_platform.mouse_y = -1;
	}

	platform_color color = PLATFORM_LIGHTGRAY;
	SDL_SetRenderDrawColor(g_sdl.renderer, color.r, color.g, color.b, color.a);
	SDL_RenderClear(g_sdl.renderer);
	SDL_GetWindowSize(g_sdl.window, &g_platform.window_width, &g_platform.window_height);

}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void* appstate)
{

#if DELETE_ME
	platform_begin();
	{
		static SDL_FRect rect = { 0, 0, 1000, 1000 };

		if (g_platform.mouse_x >= rect.x &&
			g_platform.mouse_y >= rect.y &&
			g_platform.mouse_x < rect.x + rect.w &&
			g_platform.mouse_y < rect.x + rect.w)
		{
			if (g_platform.mouse_left == Active)
			{
				rect.x += g_platform.mouse_delta_x;
				rect.y += g_platform.mouse_delta_y;
			}
		}

		SDL_SetRenderDrawColor(g_sdl.renderer, 0XFF / 2, 0XFF / 2, 0XFF / 2, 0xFF);
		SDL_RenderClear(g_sdl.renderer);

		SDL_SetRenderTarget(g_sdl.renderer, g_target);
		{
			SDL_SetRenderDrawColor(g_sdl.renderer, 0xFF, 0xFF, 0xFF, 255);
			SDL_RenderClear(g_sdl.renderer);

			SDL_RenderTexture(g_sdl.renderer, g_texture, 0, 0);
		}
		SDL_SetRenderTarget(g_sdl.renderer, 0);

		SDL_RenderTexture(g_sdl.renderer, g_target, 0, &rect);
	}
	platform_end();

#else
	
	platform_begin();
	{
		// Editor
		static Egui editor_gui, game_gui;
		static wzrd_draw_commands_buffer editor_buffer;
		wzrd_cursor editor_cursor = wzrd_cursor_default;
		bool enable_editor_input = true;
		if (game_gui.is_interacting)
		{
			enable_editor_input = false;
		}

		wzrd_icons icons = game_icons_get();

		// Clear target
		PlatformTextureBeginTarget(game_target_texture_get());
		{
			SDL_SetRenderDrawColor(g_sdl.renderer, 0xc3, 0xc3, 0xc3, 255);
			SDL_RenderClear(g_sdl.renderer);
		}
		PlatformTextureEndTarget();

		editor_do(&editor_gui, &editor_buffer, &editor_cursor, enable_editor_input, game_target_texture_get(), icons);

		wzrd_box* box = wzrd_box_get_by_name_from_gui(&editor_gui, str128_create("Target"));
		wzrd_rect rect = wzrd_box_get_rect(box);
		PlatformRect game_screen_rect = *(PlatformRect*)&rect;

		float mouse_x = g_platform.mouse_x - game_screen_rect.x;
		float mouse_y = g_platform.mouse_y - game_screen_rect.y;
		g_game.mouse_delta = (v2){ mouse_x - g_game.mouse_pos.x, mouse_y - g_game.mouse_pos.y };
		g_game.mouse_pos.x = mouse_x;
		g_game.mouse_pos.y = mouse_y;
		//printf("%f %f\n", game.mouse_pos.x, game.mouse_pos.y);

		// Game
		wzrd_cursor game_cursor = wzrd_cursor_default;
		v2 game_screen_size = (v2){ game_screen_rect.w, game_screen_rect.h };
		static wzrd_draw_commands_buffer game_gui_buffer;
		bool enable_game_input = true;
		if (editor_gui.is_interacting || editor_gui.is_hovering)
		{
			enable_game_input = false;
		}

		unsigned int scale = game_screen_size.x / game_target_texture_get().w;

		game_run(game_screen_size, enable_game_input && !game_gui.is_interacting, scale);

		v2 mouse_pos = { mouse_x / scale, mouse_y / scale };

		game_gui_do(&game_gui_buffer, &game_gui, *(wzrd_v2*)&game_screen_size, &game_cursor, enable_game_input, scale, mouse_pos);

		game_draw(game_screen_size, mouse_pos);
		game_draw_gui(&game_gui_buffer);
		game_draw_gui_commands(&editor_buffer);

		platform_debug_view_draw();

		// Cursor
		if (enable_game_input) {
			platform_cursor_set(*(PlatformCursor*)&game_cursor);
		}
		else if (enable_editor_input) {
			platform_cursor_set(*(PlatformCursor*)&editor_cursor);
		}
		else {
			platform_cursor_set(wzrd_cursor_default);
		}
	}
	platform_end();

#endif

	return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
}
