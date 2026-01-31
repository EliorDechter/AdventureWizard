#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <stdio.h>
#include "WzGuiCore.h"

WzGui gui;

SDL_Renderer* renderer;
SDL_Window* window;
TTF_TextEngine* engine;
TTF_Font* font;


WzState left_mouse;

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 540

void PlatformTextDraw(const char* str, float x, float y, char r, char g, char b, char a) {
	TTF_Text* text = TTF_CreateText(engine, font, str, 0);
	//int w = 11, h = 22;
	TTF_SetTextColor(text, r, g, b, a);
	TTF_DrawRendererText(text, x, y);
	TTF_DestroyText(text);
}

void sdl_draw_wz(WzGui* gui, SDL_Renderer* renderer)
{
	WzDrawCommandBuffer* buffer = &gui->commands_buffer;

	for (int i = 0; i < buffer->count; ++i) {
		WzDrawCommand command = buffer->commands[i];

		if (command.type == DrawCommandType_Rect) {
			SDL_SetRenderDrawColor(renderer, command.color.r, command.color.g, command.color.b, command.color.a);
			SDL_FRect rect = { (float)command.dest_rect.x, (float)command.dest_rect.y, (float)command.dest_rect.w, (float)command.dest_rect.h };
			SDL_RenderFillRect(renderer, &rect);
		}
		else if (command.type == DrawCommandType_Texture) {
#if 0
			PlatformTextureDrawFromSource((PlatformTexture) { command.texture.data, .h = command.texture.h, .w = command.texture.w },
				(PlatformRect) {
				(float)command.dest_rect.x, (float)command.dest_rect.y, (float)command.dest_rect.w, (float)command.dest_rect.h
			}, (PlatformRect) { (float)command.src_rect.x, (float)command.src_rect.y, (float)command.src_rect.w, (float)command.src_rect.h }, (platform_color) { 255, 255, 255, 255 });
#endif
		}
		else if (command.type == DrawCommandType_String) {
			PlatformTextDraw(command.str.str, (float)command.dest_rect.x, (float)command.dest_rect.y,
				command.color.r, command.color.g, command.color.b, command.color.a);
		}
		else if (command.type == DrawCommandType_Line)
		{
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

			SDL_SetRenderDrawColor(renderer, command.color.r, command.color.g, command.color.b, 255);
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

			int x = command.dest_rect.x;
			int y = command.dest_rect.y;
			int h = command.dest_rect.h;
			for (int i = y; i < y + h; ++i) {
				if (i % 2 == 1) continue;
				SDL_RenderPoint(renderer, (float)x, (float)i);
			}
		}
		else if (command.type == DrawCommandType_HorizontalLine)
		{
			SDL_SetRenderDrawColor(renderer, 125, 125, 125, 255);
			int x = command.dest_rect.x;
			int y = command.dest_rect.y;
			int w = command.dest_rect.w;
			for (int i = x; i < x + w; ++i) {
				if (i % 2 == 1) continue;
				SDL_RenderPoint(renderer, (float)i, (float)y);
			}
		}
		else if (command.type == DrawCommandType_Clip)
		{
			SDL_Rect rect = { (int)command.dest_rect.x, (int)command.dest_rect.y, (int)command.dest_rect.w, (int)command.dest_rect.h };
			SDL_SetRenderClipRect(renderer, &rect);
		}
		else if (command.type == DrawCommandType_StopClip)
		{
			SDL_SetRenderClipRect(renderer, 0);
		}

	}

	SDL_SetRenderClipRect(renderer, 0);
}

int SDL_AppEvent(void* appstate, const SDL_Event* event)
{
	if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN)
	{
		if (event->button.button == SDL_BUTTON_LEFT)
		{
			if (left_mouse == WZ_INACTIVE)
			{
				left_mouse = WZ_ACTIVATING;
			}
		}
	}
	else if (event->type == SDL_EVENT_MOUSE_BUTTON_UP)
	{
		if (event->button.button == SDL_BUTTON_LEFT)
		{
			left_mouse = WZ_DEACTIVATING;
		}
	}
	else
	{
		if (left_mouse == WZ_ACTIVATING) {
			left_mouse = WZ_ACTIVE;
		}
		else if (left_mouse == WZ_DEACTIVATING) {
			left_mouse = WZ_INACTIVE;
		}
	}

	if (event->type == SDL_EVENT_QUIT)
	{
		return SDL_APP_SUCCESS;
	}

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate)
{
	wz_gui_deinit(&gui);
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d");

	/* Create the window */
	if (!SDL_CreateWindowAndRenderer("Hello World", WINDOW_WIDTH, WINDOW_HEIGHT,
		SDL_WINDOW_RESIZABLE, &window, &renderer))
	{
		SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	// Fonts
	{
		TTF_Init();

		font = TTF_OpenFont("C:\\Windows\\Fonts\\Arial.ttf", 16);
		assert(font);

		engine = TTF_CreateRendererTextEngine(renderer);
		assert(engine);
	}


	wz_gui_init(&gui);

	return SDL_APP_CONTINUE;
}

void platform_string_get_size(char* str, int* w, int* h)
{
	int w_int = 0, h_int = 0;
	str128 line = { 0 };
	size_t index = 0;
	size_t str_len = strlen(str);
	*w = 0;
	*h = 0;
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

		bool result = TTF_GetStringSize(font, line.val, 0, &w_int, &h_int);
		assert(result);

		*w += (int)w_int;
		*h += (int)h_int;
	}
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
	float mouse_x, mouse_y;
	SDL_GetMouseState(&mouse_x, &mouse_y);

	SDL_SetRenderDrawColor(renderer, 0x20, 0x8c, 0x71, 255);
	SDL_RenderClear(renderer);

	WzKeyboardKeys keyboard_keys = { 0 };
	WzWidget window = wz_gui_begin(&gui, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2,
		mouse_x, mouse_y, platform_string_get_size, left_mouse, keyboard_keys, true);

	static int x, y;
	static unsigned w = 800, h = 400;
	static bool active;
	WzWidget dialog = wzrd_dialog_begin(&x, &y, &w, &h,
		&active, wz_str_create("My Dialog"), 4, window);
	wz_widget_set_cross_axis_alignment(dialog, WZ_CROSS_AXIS_ALIGNMENT_CENTER);
	//wz_widget_set_main_axis_alignment(dialog, WZ_MAIN_AXIS_ALIGNMENT_CENTER);
	wz_widget_set_child_gap(dialog, 20);

	WzWidget text = wz_label(dialog, wz_str_create("Hi"));
	wz_widget_set_max_constraint_h(text, 100);

	wz_spacer(dialog);

	bool released = false;
	WzWidget ok_button = wz_command_button(wz_str_create("OK"), &released, dialog);
	wz_widget_set_margins(ok_button, 50);
	wz_widget_set_tight_constraints(ok_button, 160, 40);
	
	wz_gui_end(0);

	sdl_draw_wz(&gui, renderer);

	SDL_RenderPresent(renderer);

	return SDL_APP_CONTINUE;
}

