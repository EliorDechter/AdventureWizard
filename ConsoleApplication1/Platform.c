#include "Platform.h"
//#include "Editor.h"
#include <SDL3/SDL_main.h>
#include "Game.h"
#include "Editor.h"

#define WZRD_UNUSED(x) (void)x;


PlatformSystem g_platform;

void PlatformTextDrawColor(const char* str, float x, float y, char r, char g, char b, char a) {
	TTF_Text* text = TTF_CreateText(g_sdl.engine, g_sdl.font, str, 0);
	//int w = 11, h = 22;
	TTF_SetTextColor(text, r, g, b, a);
	TTF_DrawRendererText(text, x, y);
	TTF_DestroyText(text);
}

void PlatformTextDraw(const char* str, float x, float y, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
	TTF_Text* text = TTF_CreateText(g_sdl.engine, g_sdl.font, str, 0);
	TTF_SetTextColor(text, r, g, b, a);
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

void platform_string_get_size(char* str, float* w, float* h)
{
	int w_int = 0, h_int = 0;
	str128 line = { 0 };
	size_t index = 0;
	size_t str_len = strlen(str);
	while (str[index])
	{
		line = (str128){ 0 };
		for (; index < str_len; ++index)
		{
			if (str[index] == '\n')
			{
				++index;
				line.val[line.len] = 0;
				break;
			}

			line.val[line.len++] = str[index];
		}

		bool result = TTF_GetStringSize(g_sdl.font, line.val, 0, &w_int, &h_int);
		assert(result);

		*w += (float)w_int;
		*h += (float)h_int;
	}

}

void PlatformRectDraw(PlatformRect rect, platform_color color) {
	SDL_SetRenderDrawBlendMode(g_sdl.renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(g_sdl.renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(g_sdl.renderer, (SDL_FRect*)&rect);
}

void PlatformTextureDraw(PlatformTexture texture, PlatformRect rect) {
	SDL_RenderTexture(g_sdl.renderer, texture.data, 0, (SDL_FRect*)&rect);
}

void PlatformTextureDrawFromSource(PlatformTexture texture, PlatformRect dest, PlatformRect src, platform_color color) {
	if (!(color.r == 0 && color.g == 0 && color.b == 0))
		SDL_SetTextureColorMod(texture.data, color.r, color.g, color.b);
	SDL_RenderTexture(g_sdl.renderer, texture.data, (SDL_FRect*)&src, (SDL_FRect*)&dest);
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

void PlatformLineDraw(float x0, float y0, float x1, float y1, unsigned char r, unsigned char g, unsigned char b) {
	SDL_SetRenderDrawColor(g_sdl.renderer, r, g, b, 255);
	SDL_RenderLine(g_sdl.renderer, x0, y0, x1, y1);
}

void PlatformLineDrawHorizontal(float x0, float y0, float x1) {
	SDL_SetRenderDrawColor(g_sdl.renderer, 125, 125, 125, 255);

	for (int i = (int)x0; i < x1; ++i) {
		if (i % 2 == 1) continue;
		SDL_RenderPoint(g_sdl.renderer, (float)i, y0);
	}
}

void PlatformLineDrawVertical(float x0, float y0, float y1) {
	SDL_SetRenderDrawColor(g_sdl.renderer, 125, 125, 125, 255);

	for (int i = (int)y0; i < y1; ++i) {
		if (i % 2 == 1) continue;
		SDL_RenderPoint(g_sdl.renderer, x0, (float)i);
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

void platform_draw_wzrd(wzrd_draw_commands_buffer* buffer) {

	for (int i = 0; i < buffer->count; ++i) {
		wzrd_draw_command command = buffer->commands[i];

		if (command.type == DrawCommandType_Texture) {
			PlatformTextureDrawFromSource(*(PlatformTexture*)&command.texture,
				*(PlatformRect*)&command.dest_rect, *(PlatformRect*)&command.src_rect, (platform_color) { 255, 255, 255, 255 });
		}
		else if (command.type == DrawCommandType_String) {
			PlatformTextDraw(command.str.val, command.dest_rect.x, command.dest_rect.y,
				command.color.r, command.color.g, command.color.b, command.color.a);
		}
		else if (command.type == DrawCommandType_Rect) {
			SDL_SetRenderDrawColor(g_sdl.renderer, command.color.r, command.color.g, command.color.b, command.color.a);
			SDL_RenderFillRect(g_sdl.renderer, (SDL_FRect*)&command.dest_rect);
		}
		else if (command.type == DrawCommandType_Line) {
			PlatformLineDraw(command.dest_rect.x, command.dest_rect.y, command.dest_rect.w, command.dest_rect.h,
				command.color.r, command.color.g, command.color.b);
		}
		else if (command.type == DrawCommandType_VerticalLine) {
			PlatformLineDrawVertical(command.dest_rect.x, command.dest_rect.y, command.dest_rect.h);
		}
		else if (command.type == DrawCommandType_HorizontalLine) {
			PlatformLineDrawHorizontal(command.dest_rect.x, command.dest_rect.y, command.dest_rect.w);
		}
		else if (command.type == DrawCommandType_Clip)
		{
			SDL_Rect rect = { (int)command.dest_rect.x, (int)command.dest_rect.y, (int)command.dest_rect.w, (int)command.dest_rect.h };
			SDL_SetRenderClipRect(g_sdl.renderer, &rect);
		}
	}

	SDL_SetRenderClipRect(g_sdl.renderer, 0);
}

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	(void)appstate;
	(void)argc;
	(void)argv;


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

	g_sdl.mutex = SDL_CreateMutex();

	g_sdl.cursor_hand = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER);
	g_sdl.cursor_default = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
	g_sdl.cursor_horizontal_arrow = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_EW_RESIZE);
	g_sdl.cursor_vertical_arrow = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NS_RESIZE);

#endif
	return SDL_APP_CONTINUE;
}

SDL_AppResult handle_events(SDL_Event* event) {
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
		g_platform.keys_pressed.keys[g_platform.keys_pressed.count++] = (PlatformKey){ .val = (unsigned int)keycode, .state = g_platform.keyboard_states[keycode] };
	}

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
	(void)appstate;

	SDL_AppResult result = handle_events(event);

	return result;
}

void platform_cursor_set(PlatformCursor cursor) {
	switch (cursor) {
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

str1024 g_debug_text;

void platform_debug_gui_do(int layer) {
	static wzrd_gui gui;
	static wzrd_cursor cursor;
	static wzrd_draw_commands_buffer buffer;
	wzrd_update_input((wzrd_v2) { g_platform.mouse_x, g_platform.mouse_y },
		(wzrd_state)g_platform.mouse_left,
		* (wzrd_keyboard_keys*)&g_platform.keys_pressed);

	wzrd_style style = wzrd_style_get_default();
	style.window_border_type = BorderType_Default;

	wzrd_begin(&gui,
		(wzrd_rect) {
		(float)g_platform.window_width - 300.0f, 10.0f, 295.0f, 500.0f
	},
		style,
		platform_string_get_size,
		layer
	);
	static float scrollbar_x, scrollbar_y;
	wzrd_box_get_last()->clip = true;
	wzrd_box_get_last()->scrollbar_x = &scrollbar_x;
	wzrd_box_get_last()->scrollbar_y = &scrollbar_y;
	wzrd_label(str128_create(g_debug_text.val));
	wzrd_box_do((wzrd_box) { .h = 1000, .w = 50, .color = EGUI_BLUE });
	wzrd_end(&cursor, &buffer);
	platform_draw_wzrd(&buffer);
}

typedef struct Second
{
	uint64_t val;
} Second;

Second platform_get_time()
{
	Second result = { SDL_GetTicks() };

	return result;
}

void platform_end()
{
	SDL_RenderPresent(g_sdl.renderer);

	// Input
	g_platform.keys_pressed.count = 0;

	if (g_platform.mouse_left == Activating) {
		g_platform.mouse_left = Active;
	}
	else if (g_platform.mouse_left == Deactivating) {
		g_platform.mouse_left = Inactive;
	}

	float time = SDL_GetTicks() / 1000.0f;
	g_platform.last_frame_time_in_seconds = time - g_platform.time;

	g_platform.time_samples[g_platform.time_samples_count] = g_platform.last_frame_time_in_seconds;
	g_platform.time_samples_count = (g_platform.time_samples_count + 1) % 32;
	g_platform.average_spf = 0;
	for (int i = 0; i < 32; ++i)
	{
		g_platform.average_spf += g_platform.time_samples[i];
	}
	g_platform.average_spf = g_platform.average_spf / 32;

	const float spf = 1.0f / 60.0f;
	if (g_platform.last_frame_time_in_seconds < spf)
	{
		SDL_Delay((int)(spf * 1000.0f - g_platform.last_frame_time_in_seconds * 1000.0f));
	}

}

void platform_begin()
{

	g_platform.time = SDL_GetTicks() / 1000.0f;

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
	(void)appstate;

	platform_begin();
	{
		static wzrd_gui editor_gui, game_gui;
		static wzrd_rect game_screen_rect;

		//wzrd_layer_add(0, (wzrd_rect) { 0, 0, (float)g_platform.window_width, (float)g_platform.window_height });
		//wzrd_layer_add(1, game_screen_rect);
		//wzrd_layer_add(2, (wzrd_rect) {
		//	(float)g_platform.window_width - 300.0f, 10.0f, 295.0f, 500.0f
		//});

		wzrd_update_layers((wzrd_v2) {
			g_platform.mouse_x, g_platform.mouse_y
		},
			(wzrd_state)g_platform.mouse_left);


		// Editor
		const bool enable_editor = true;
		bool enable_editor_input = true;
		wzrd_cursor editor_cursor = wzrd_cursor_default;

		if (enable_editor) {
			static wzrd_draw_commands_buffer editor_buffer;
	/*		if (game_gui.is_interacting)
			{
				enable_editor_input = false;
			}*/

			wzrd_icons icons = game_icons_get();

			uint64_t time_a = SDL_GetTicksNS();

			editor_do(&editor_gui, &editor_buffer, &editor_cursor, enable_editor_input, game_target_texture_get(), icons, 0);
			platform_draw_wzrd(&editor_buffer);

			static uint64_t samples[32];
			static size_t samples_count;
			uint64_t editor_time = SDL_GetTicksNS() - time_a;
			samples[samples_count] = editor_time;
			samples_count = (samples_count + 1) % 32;
			float average_time = 0;
			for (int i = 0; i < 32; ++i)
			{
				average_time += (float)samples[i] / 1000000;
			}
			average_time /= 32;
			g_debug_text = str1024_create("%f\n", average_time);

		}


#if 1
		{
			// Clear target
			PlatformTextureBeginTarget(game_target_texture_get());
			{
				SDL_SetRenderDrawColor(g_sdl.renderer, 0xc3, 0xc3, 0xc3, 255);
				SDL_RenderClear(g_sdl.renderer);
			}
			PlatformTextureEndTarget();

			wzrd_box* box = wzrd_box_get_by_name_from_gui(&editor_gui, str128_create("Target"));
			game_screen_rect = wzrd_box_get_rect(box);
			//PlatformRect rect = *(PlatformRect*)&game_screen_rect;

			float mouse_x = g_platform.mouse_x - game_screen_rect.x;
			float mouse_y = g_platform.mouse_y - game_screen_rect.y;
			g_game.mouse_delta = (v2){ mouse_x - g_game.mouse_pos.x, mouse_y - g_game.mouse_pos.y };
			g_game.mouse_pos.x = mouse_x;
			g_game.mouse_pos.y = mouse_y;
			// Game
			wzrd_cursor game_cursor = wzrd_cursor_default;
			static wzrd_draw_commands_buffer game_gui_buffer;
			bool enable_game_input = true;
		/*	if (editor_gui.is_interacting || editor_gui.is_hovering)
			{
				enable_game_input = false;
			}*/

			//game_run(game_screen_size, enable_game_input && !game_gui.is_interacting, scale);

			//v2 mouse_pos = { mouse_x / scale, mouse_y / scale };

			// Draw entities
			PlatformTextureBeginTarget(g_game.target_texture);
			{
				int iterator_index = 0;
				Entity* entity = 0;
				while ((entity = entity_get_next(&iterator_index))) {
					Texture* texture = game_texture_get(entity->texture);
					if (texture)
					{
						PlatformTextureDrawFromSource(game_texture_get(entity->texture)->val, *(PlatformRect*)&entity->rect,
							(PlatformRect) {
							0, 0, texture->val.w, texture->val.h
						}, * (platform_color*)&entity->color);
					}
					else {
						PlatformRectDraw(*(PlatformRect*)&entity->rect, (platform_color) { 255, 255, 0, 255 });
					}

				}
			}
			PlatformTextureEndTarget();

			// Inside game editing gui
			if (!g_game.run)
			{
				//v2 game_screen_size = (v2){ game_screen_rect.w, game_screen_rect.h };
				unsigned int scale = (unsigned int)(game_screen_rect.w / game_target_texture_get().w);
				game_gui_do(&game_gui_buffer, &game_gui, *(wzrd_rect*)&game_screen_rect, &game_cursor, enable_game_input, scale, 1);
				//game_draw_gui(&game_gui_buffer);
			}
		}
		//
#endif

		// Debug view
		str1024_concat(&g_debug_text, str1024_create("frame time: %f ms", g_platform.average_spf * 1000.0f));
		platform_debug_gui_do(2);

		
#if 0
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
#endif
	}
	platform_end();

	return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
	WZRD_UNUSED(appstate);
	WZRD_UNUSED(result);

}

