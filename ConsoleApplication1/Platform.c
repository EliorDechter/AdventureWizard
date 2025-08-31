#include "Platform.h"
//#include "Game.h"
#include <SDL3/SDL_main.h>

/*
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

void PlatformFontLoad() {

}

void PlatformRectDraw(PlatformRect rect, PlatformColor color) {
	DrawRectangle(rect.x, rect.y, rect.w, rect.h, *((Color*)&color));
}

PlatformImage PlatformImageLoad(const char* str) {
	PlatformImage result = (PlatformImage){ LoadImage(str) };

	return result;
}

void PlatformInit() {
	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "my game");
	SetTargetFPS(60);
}
*/
/*
  Copyright (C) 1997-2025 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely.
*/

void PlatformTextDraw(const char* str, float x, float y) {
	TTF_Text* text = TTF_CreateText(sdl.engine, sdl.font, str, 0);
	int w = 0, h = 0;
	TTF_GetTextSize(text, &w, &h);
	SDL_Surface* window_surface = SDL_GetWindowSurface(sdl.window);
	TTF_SetTextColor(text, 0, 0, 0, 255);
	TTF_DrawRendererText(text, x, y);
}

PlatformV2i PlatformTextGetSize(const char* str) {
	PlatformV2i result = { 0 };
	TTF_Text* text = TTF_CreateText(sdl.engine, sdl.font, str, 0);
	TTF_GetTextSize(text, &result.x, &result.y);

	return result;
}

void PlatformRectDraw(PlatformRect rect, PlatformColor color) {
	//assert(color.a);

	SDL_SetRenderDrawColor(sdl.renderer, color.r, color.g, color.b, color.a);
	//SDL_RenderRect(sdl.renderer, &rect);
	SDL_RenderFillRect(sdl.renderer, &rect);
}

void PlatformTextureDraw(PlatformTexture texture, PlatformRect rect) {
	SDL_RenderTexture(sdl.renderer, texture.data, 0, &rect);
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
	SDL_Surface* surface = IMG_Load(path);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(sdl.renderer, surface);
	PlatformTexture result = (PlatformTexture){ .data = texture };

	return result;
}


/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	/* Create the window */
	if (!SDL_CreateWindowAndRenderer("Hello World", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &sdl.window, &sdl.renderer)) {
		SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	TTF_Init();
	sdl.font = TTF_OpenFont("C:\\Windows\\Fonts\\Arial.ttf", FONT_HEIGHT);
	assert(sdl.font);

	sdl.engine = TTF_CreateRendererTextEngine(sdl.renderer);
	assert(sdl.engine);

	GameInit();

	return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
	if (event->type == SDL_EVENT_QUIT) {
		return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
	}

	sdl.event = event;

	return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void* appstate)
{

	static Uint64 time = 0;
	Uint64 delta_time = SDL_GetTicks() - time;
	time = SDL_GetTicks();

	// Input
	SDL_Event* event = sdl.event;
	if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
		if (event->button.button == SDL_BUTTON_LEFT) {
			if (platform_system.mouse_left == Inactive) {
				platform_system.mouse_left = Activating;
				printf("Activating\n");
			}

		}
	}
	else if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
		if (event->button.button == SDL_BUTTON_LEFT) {
			printf("Deactivating\n");
			platform_system.mouse_left = Deactivating;
		}
	}
	else {
		if (platform_system.mouse_left == Activating) {
			platform_system.mouse_left = Active;
			printf("Active\n");
		}
		else if (platform_system.mouse_left == Deactivating) {
			platform_system.mouse_left = Inactive;
			printf("Inactive\n");
		}
	}

#if 0
	const char* message = "Hello World!";
	int w = 0, h = 0;
	float x, y;
	const float scale = 4.0f;

	/* Center the message and scale it up */
	SDL_GetRenderOutputSize(sdl.renderer, &w, &h);
	SDL_SetRenderScale(sdl.renderer, scale, scale);
	x = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(message)) / 2;
	y = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) / 2;

	/* Draw the message */
	SDL_SetRenderDrawColor(sdl.renderer, 220, 220, 220, 255);
	SDL_RenderClear(sdl.renderer);
	SDL_SetRenderDrawColor(sdl.renderer, 0, 0, 255, 255);
	SDL_RenderDebugText(sdl.renderer, x, y, message);
#endif
	SDL_GetMouseState(&platform_system.mouse_x, &platform_system.mouse_y);

	PlatformColor color = PLATFORM_LIGHTGRAY;
	SDL_SetRenderDrawColor(sdl.renderer, color.r, color.g, color.b, color.a);
	SDL_RenderClear(sdl.renderer);
	SDL_GetWindowSize(sdl.window, &platform_system.window_width, &platform_system.window_height);
	GameRun(platform_system.window_width, platform_system.window_height);
	PlatformTextDraw("asdas", 500, 500);

	char buff[8] = { 0 };
	sprintf(buff, "%u", delta_time);
	sdl.fps_text = TTF_CreateText(sdl.engine, sdl.font, buff, 0);

	SDL_RenderPresent(sdl.renderer);

	return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
}
