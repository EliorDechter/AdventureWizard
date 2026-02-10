#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>
#include <stdio.h>
#include "WzGuiCore.h"
#define _USE_MATH_DEFINES
#include <math.h>

//#define assert(x) void(x)

WzKeyboard keyboard;

typedef struct
{
	SDL_Window* window;
	SDL_Renderer* renderer;
	TTF_TextEngine* text_engine;
	// Textures
} WindowContext;

WindowContext main_window;
WindowContext gui_window;

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

TTF_Font* font;
SDL_Texture* x_icon_texture;

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

void sdl_draw_wz(WzGui* wz, WindowContext* window_context)
{
	SDL_Renderer* renderer = window_context->renderer;

	WzDrawCommandBuffer* buffer = &wz->commands_buffer;

	for (int i = 0; i < buffer->count; ++i) {
		WzDrawCommand command = buffer->commands[i];

		if (command.type == DrawCommandType_Rect) {
			SDL_SetRenderDrawColor(renderer, command.color.r, command.color.g, command.color.b, command.color.a);
			SDL_FRect rect = { (float)command.dest_rect.x, (float)command.dest_rect.y, (float)command.dest_rect.w, (float)command.dest_rect.h };
			SDL_RenderFillRect(renderer, &rect);
		}
		else if (command.type == DrawCommandType_Texture)
		{
			//if (!(command.color.r == 0 && command.color.g == 0 && command.color.b == 0))
				//SDL_SetTextureColorMod(command.texture.data, command.color.r, command.color.g, command.color.b);

			SDL_SetTextureBlendMode(command.texture.data, SDL_BLENDMODE_BLEND);

			SDL_FRect src = { (float)command.src_rect.x, (float)command.src_rect.y, (float)command.src_rect.w, (float)command.src_rect.h };
			SDL_FRect dest = { (float)command.dest_rect.x, (float)command.dest_rect.y, (float)command.dest_rect.w, (float)command.dest_rect.h };
			SDL_RenderTextureRotated(renderer, command.texture.data,
				(SDL_FRect*)&src, (SDL_FRect*)&dest, command.rotation_angle, 0, 0);
		}
		else if (command.type == WZ_DRAW_COMMAND_TYPE_TEXT) {

			TTF_Text* text = TTF_CreateText(window_context->text_engine, font, command.str.str, 0);
			bool error = false;
			error = TTF_SetTextColor(text, command.color.r, command.color.g, command.color.b, command.color.a);
			TTF_DrawRendererText(text, command.dest_rect.x, command.dest_rect.y);
			TTF_DestroyText(text);
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

#define MAX_NUM_SDL_EVENTS 128
SDL_Event events[MAX_NUM_SDL_EVENTS];
unsigned events_count;
SDL_Mutex* events_mutex;

void sdl_to_wz_keyboard(WzKeyboard* keyboard)
{
#if 0
	for (int i = 0; i < 128; ++i) {
		if (keyboard->keys[i] == WZ_ACTIVATING)
		{
			keyboard->keys[i] = WZ_INACTIVE;
		}
		if (keyboard->keys[i] == WZ_ACTIVATING)
		{
			keyboard->keys[i] = WZ_ACTIVE;
		}
	}

	SDL_Keycode keycode = 0;

	if (event->type == SDL_EVENT_KEY_DOWN)
	{
		keycode = SDL_GetKeyFromScancode(event->key.scancode, event->key.mod, false);
		if (keycode < 128) {
			if (keyboard->keys[keycode] == WZ_INACTIVE)
			{
				keyboard->keys[keycode] = WZ_ACTIVATING;
			}
			else if (keyboard->keys[keycode] == WZ_ACTIVATING)
			{
				keyboard->keys[keycode] = WZ_ACTIVE;
			}
		}
	}
	else if (event->type == SDL_EVENT_KEY_UP)
	{
		keycode = SDL_GetKeyFromScancode(event->key.scancode, event->key.mod, false);
		if (keycode < 128) {
			if (keyboard->keys[keycode] == WZ_ACTIVE || keyboard->keys[keycode] == WZ_ACTIVATING)
			{
				keyboard->keys[keycode] = WZ_DEACTIVATING;
			}
			else if (keyboard->keys[keycode] == WZ_DEACTIVATING)
			{
				keyboard->keys[keycode] = WZ_INACTIVE;
			}
		}
	}

	if (keycode < 128 && keycode > 0) {
		/*	g_platform.keys_pressed.keys[g_platform.keys_pressed.count++] =
				(PlatformKey){ .val = (unsigned int)keycode, .state = [keycode] };*/
	}
#else

	for (unsigned i = 0; i < 128; ++i)
	{
		if (keyboard->keys[i] == WZ_ACTIVATING)
		{
			keyboard->keys[i] = WZ_ACTIVE;
		}
		if (keyboard->keys[i] == WZ_DEACTIVATING)
		{
			keyboard->keys[i] = WZ_INACTIVE;
		}
	}

	int num_keys = 0;
	bool* keys = SDL_GetKeyboardState(&num_keys);
	for (unsigned i = 0; i < num_keys; ++i)
	{
		SDL_Keycode key = SDL_GetKeyFromScancode(i, SDL_KMOD_NONE, false);
		if (key > 0 && key < 128)
		{
			if (keys[i])
			{
				if (keyboard->keys[key] == WZ_INACTIVE)
				{
					keyboard->keys[key] = WZ_ACTIVATING;
				}
			}
			else
			{
				if (keyboard->keys[key] == WZ_ACTIVE)
				{
					keyboard->keys[key] = WZ_DEACTIVATING;
				}
			}
		}
	}

#endif
}

void sdl_handle_input()
{
	sdl_to_wz_keyboard(&keyboard);

#if 0
	// Finish handling input
	{
		if (left_mouse == WZ_ACTIVATING) {
			left_mouse = WZ_ACTIVE;
		}
		else if (left_mouse == WZ_DEACTIVATING) {
			left_mouse = WZ_INACTIVE;
		}
	}

	if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN)
	{
		if (event->button.button == SDL_BUTTON_LEFT)
		{
			if (left_mouse == WZ_INACTIVE)
			{
				left_mouse = WZ_ACTIVATING;
				printf("wow\n");

			}
			else
			{
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
#else

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

#endif
}

int SDL_AppEvent(void* appstate, const SDL_Event* event)
{
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

WindowContext sdl_create_window()
{
	bool success = true;
	SDL_Renderer* renderer;
	SDL_Window* window;
	TTF_TextEngine* text_engine;

	success &= SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d");

	success &= SDL_CreateWindowAndRenderer("Niga", WINDOW_WIDTH, WINDOW_HEIGHT,
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

	WindowContext context =
	{
		.renderer = renderer,
		.window = window,
		.text_engine = text_engine
	};

	return context;
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	bool success = false;

	//main_window = sdl_create_window();
	gui_window = sdl_create_window();
	SDL_SetWindowResizable(gui_window.window, false);

	// Fonts
	{
		success |= TTF_Init();

		font = TTF_OpenFont("C:\\Windows\\Fonts\\Arial.ttf", 16);
		success |= (bool)font;
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

	//wz_gui_init(&gui);
	load_animations();

	events_mutex = SDL_CreateMutex();

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

		bool success = TTF_GetStringSize(font, line.val, 0, &w_int, &h_int);

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

		*w += (int)w_int;
		*h += (int)h_int;
	}
}


void window_context_begin(WindowContext* context)
{
	bool success = true;
	success &= SDL_SetRenderDrawColor(context->renderer, 0x20, 0x8c, 0x71, 255);
	success &= SDL_RenderClear(context->renderer);
}

void window_context_end(WindowContext* context)
{
	SDL_RenderPresent(context->renderer);
}

bool is_key_interacting(char k)
{
	bool result = keyboard.keys[k] == WZ_ACTIVATING || keyboard.keys[k] == WZ_ACTIVE;

	return result;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
	// Handle input
	for (unsigned i = 0; i < events_count; ++i)
	{
#if 0
		SDL_LockMutex(events_mutex);
		events_count--;
		SDL_UnlockMutex(events_mutex);
#endif
	}
	sdl_handle_input();

	unsigned time_beginning = SDL_GetTicks();

	bool success = true;

#if 0
	window_context_begin(&main_window);
	{
		Uint32 flags = SDL_GetWindowFlags(main_window.window);
		float mouse_x = 0, mouse_y = 0;
		if (flags & SDL_WINDOW_MOUSE_FOCUS) 
		{
			SDL_GetMouseState(&mouse_x, &mouse_y);
		}

		int window_width, window_height;
		success |= SDL_GetWindowSize(main_window.window, &window_width, &window_height);

		WzWidget window = wz_begin(&gui, window_width, window_height,
			mouse_x, mouse_y, platform_string_get_size, left_mouse, &keyboard, true);
		wz_widget_set_layout(window, WZ_LAYOUT_NONE);
		wz_widget_clip(window);

		WzScene scene =
		{
			.w = WINDOW_WIDTH / 2,
			.h = WINDOW_HEIGHT / 2,
		};

		WzWidget wid = wz_scene(scene, window, texture, 0, 0, 200, 200);

		// ... 
		for (unsigned i = 0; i < image_widgets_count; ++i)
		{
			WzWidgetPersistentData data = image_widgets[i];

			WzTexture widget_a_wz_texture = (WzTexture)
			{
				.data = data.animation.textures[data.current_animation_frame],
				.w = data.animation.textures[data.current_animation_frame]->w,
				.h = data.animation.textures[data.current_animation_frame]->h,
			};

			WzWidget widget = wz_texture(wid,
				widget_a_wz_texture,
				100, 100);
			wz_widget_set_x(widget, 0);
			wz_widget_set_y(widget, 0);
			wz_widget_set_tight_constraints(widget, 100, 100);
			wz_widget_set_color(widget, 0);

			image_widgets[i].widget = widget;
		}

		// Resize items
		static WzWidget selected_widget;

		if (wz_widget_is_activating(window))
		{
			selected_widget = (WzWidget){ 0 };
		}

		WzWidget text_widget = wz_vbox(window);
		const unsigned text_widget_width = 200;
		const unsigned text_widget_height = 400;
		wz_widget_set_pos(text_widget, window_width - text_widget_width - 5, 5);
		wz_widget_set_color(text_widget, 0x000000cc);
		wz_widget_set_tight_constraints(text_widget, text_widget_width, text_widget_height);

		const char* text =
			"WASD - Movement\n"
			"Z & X - Zoom in Zoom out\n"
			"F - Duplicate";
		WzWidget label = wz_label(text_widget, wz_str_create(text));
		wz_widget_set_font_color(label, 0xffffffff);
		wz_widget_set_color(label, 0);


		WzWidget buttons_panel = wz_hbox(window);
		wz_widget_disable(buttons_panel, true);

		bool rotate_right = false;
		WzWidget rotate_right_widget = wz_command_button(wz_str_create("rotate right"),
			&rotate_right, buttons_panel);
		wz_widget_set_pad(rotate_right_widget, 5);

		bool rotate_left = false;
		WzWidget rotate_left_widget = wz_command_button(wz_str_create("rotate left"),
			&rotate_left, buttons_panel);
		wz_widget_set_pad(rotate_left_widget, 5);

		bool duplicate = false;
		WzWidget duplicate_widget = wz_command_button(wz_str_create("duplicate"),
			&duplicate, buttons_panel);
		wz_widget_set_pad(duplicate_widget, 5);

		bool zoom_out = false;
		bool zoom_in = false;
		bool move_left = false;
		bool move_right = false;
		bool move_up = false;
		bool move_down = false;

		if (keyboard.keys['f'] == WZ_ACTIVATING)
		{
			duplicate = true;
		}

		if (keyboard.keys['z'] == WZ_ACTIVATING || keyboard.keys['z'] == WZ_ACTIVE)
		{
			zoom_in = true;
		}
		else if (keyboard.keys['x'] == WZ_ACTIVATING || keyboard.keys['x'] == WZ_ACTIVE)
		{
			zoom_out = true;
		}

		if (keyboard.keys['w'] == WZ_ACTIVATING || keyboard.keys['w'] == WZ_ACTIVE)
		{
			move_up = true;
		}
		else if (keyboard.keys['s'] == WZ_ACTIVATING || keyboard.keys['s'] == WZ_ACTIVE)
		{
			move_down = true;
		}

		if (keyboard.keys['a'] == WZ_ACTIVATING || keyboard.keys['a'] == WZ_ACTIVE)
		{
			move_left = true;
		}
		else if (keyboard.keys['d'] == WZ_ACTIVATING || keyboard.keys['d'] == WZ_ACTIVE)
		{
			move_right = true;
		}

		for (unsigned i = 0; i < image_widgets_count; ++i)
		{
			if (wz_widget_is_activating(image_widgets[i].widget))
			{
				selected_widget = image_widgets[i].widget;
			}

			if (wz_widget_is_equal(selected_widget, image_widgets[i].widget))
			{
#if 1
				wz_add_resize_widgets_maintain_aspect_ratio(selected_widget,
					&image_widgets[i].scale_x,
					&image_widgets[i].scale_y,
					&image_widgets[i].transform_x,
					&image_widgets[i].transform_y);
#else
				wz_add_resize_widgets_maintain_aspect_ratio2(selected_widget, &image_widgets[i].rotation);
#endif
				const float rotation_factor = 5;
				if (wz_widget_is_interacting(rotate_right_widget))
				{
					image_widgets[i].rotation += fmod(rotation_factor, 360);
				}

				if (wz_widget_is_interacting(rotate_left_widget))
				{
					image_widgets[i].rotation -= fmod(rotation_factor, 360);
				}

				if (duplicate)
				{
					image_widgets[image_widgets_count] = image_widgets[i];
				}
			}
		}

		if (duplicate && wz_widget_is_valid(selected_widget))
		{
			image_widgets_count++;
		}

		wz_widget_set_border(selected_widget, WZ_BORDER_TYPE_BLACK);

		// transform and scale
		for (unsigned i = 0; i < image_widgets_count; ++i)
		{
			WzWidgetPersistentData* widget = &image_widgets[i];

			image_widgets[i].current_animation_frame =
				(widget->current_animation_frame + 1) % widget->animation.textures_count;

			if (wz_widget_is_active(widget->widget))
			{
				float mouse_delta_x = (gui.mouse_delta.x) / gui.zoom_factor;
				float mouse_delta_y = (gui.mouse_delta.y) / gui.zoom_factor;

				widget->transform_x += mouse_delta_x;
				widget->transform_y += mouse_delta_y;
			}

			wz_widget_transform(widget->widget, widget->transform_x, widget->transform_y);
			wz_widget_scale(widget->widget, widget->scale_x, widget->scale_y);
			wz_widget_rotate(widget->widget, widget->rotation);
		}

		WzWidget zoom_out_widget = wz_command_button(wz_str_create("zoom out"),
			&zoom_out, buttons_panel);
		wz_widget_set_pad(zoom_out_widget, 5);

		WzWidget zoom_in_widget = wz_command_button(wz_str_create("zoom in"),
			&zoom_in, buttons_panel);
		wz_widget_set_pad(zoom_in_widget, 5);

		WzWidget move_left_widget = wz_command_button(wz_str_create("move left"),
			&move_left, buttons_panel);
		wz_widget_set_pad(move_left_widget, 5);
		WzWidget move_right_widget = wz_command_button(wz_str_create("move right"),
			&move_right, buttons_panel);
		wz_widget_set_pad(move_right_widget, 5);
		WzWidget move_up_widget = wz_command_button(wz_str_create("move up"),
			&move_up, buttons_panel);
		wz_widget_set_pad(move_up_widget, 5);

		WzWidget move_down_widget = wz_command_button(wz_str_create("move down"),
			&move_down, buttons_panel);
		wz_widget_set_pad(move_down_widget, 5);

		const float zoom_factor = 0.09;
		if (wz_widget_is_interacting(zoom_in_widget) || zoom_in)
		{
			gui.zoom_factor += zoom_factor;
		}
		else if (wz_widget_is_interacting(zoom_out_widget) || zoom_out)
		{
			gui.zoom_factor -= zoom_factor;
		}

		const int move_offset = 10;
		if (wz_widget_is_interacting(move_left_widget) || move_left)
		{
			gui.camera_x -= move_offset;
		}

		if (wz_widget_is_interacting(move_right_widget) || move_right)
		{
			gui.camera_x += move_offset;
		}

		if (wz_widget_is_interacting(move_up_widget) || move_up)
		{
			gui.camera_y -= move_offset;
		}

		if (wz_widget_is_interacting(move_down_widget) || move_down)
		{
			gui.camera_y += move_offset;
		}

		wz_end();
	}
	sdl_draw_wz(&gui, &main_window);

	window_context_end(&main_window);
#endif

	// Gui 
	static WzGui gui_window_wz;

	window_context_begin(&gui_window);
	{
		int window_width, window_height;
		SDL_GetWindowSize(gui_window.window, &window_width, &window_height);

		Uint32 flags = SDL_GetWindowFlags(gui_window.window);
		float mouse_x = 0, mouse_y = 0;
		if (flags & SDL_WINDOW_MOUSE_FOCUS) {
			SDL_GetMouseState(&mouse_x, &mouse_y);
		}

		// Zooming
		{
			const float zoom_factor = 0.01;
			if (is_key_interacting('z'))
			{
				wz_zoom(zoom_factor);
			}
			if (is_key_interacting('x'))
			{
				wz_zoom(-zoom_factor);
			}
		}

		WzWidget window = wz_begin(&gui_window_wz, window_width, window_height,
			mouse_x, mouse_y, platform_string_get_size, left_mouse, &keyboard, true);

		WzStr strs[2];
		WzWidget panels[2] = { 0 };
		bool bb;
		WzWidget null_widget = (WzWidget){ 0 };
		static unsigned current_tab;
		panels[0] = wz_command_button(wz_str_create("wow"), &bb, null_widget);
		strs[0] = wz_str_create("a");
		strs[1] = wz_str_create("b");
		wz_tabs(window, strs, 2, panels, &current_tab);

		//bool b;
		//wz_command_button(wz_str_create("asd"), &b, window);

		wz_end();
	}

	sdl_draw_wz(&gui_window_wz, &gui_window);
	window_context_end(&gui_window);

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

	return SDL_APP_CONTINUE;
}

