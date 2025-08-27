#include "Platform.h"
#include "Game.h"

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
#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <assert.h >

#define FONT_HEIGHT 18

typedef struct Sdl {
	SDL_Renderer* renderer;
	SDL_Window* window;
	TTF_Font* font;
	TTF_Text* text;
	SDL_Event* event;
} Sdl;

Sdl sdl;

void PlatformTextDraw(const char* txt, float x, float y) {
	int w = 0, h = 0;
	TTF_GetTextSize(sdl.text, &w, &h);
	SDL_Surface* window_surface = SDL_GetWindowSurface(sdl.window);
	TTF_DrawRendererText(sdl.text, x, y);
}

void PlatformRectDraw(PlatformRect rect, PlatformColor color) {
	assert(color.a);

	SDL_SetRenderDrawColor(sdl.renderer, color.r, color.g, color.b, color.a);
	//SDL_RenderRect(sdl.renderer, &rect);
	SDL_RenderFillRect(sdl.renderer, &rect);
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

	TTF_TextEngine* engine = TTF_CreateRendererTextEngine(sdl.renderer);
	assert(engine);

	sdl.text = TTF_CreateText(engine, sdl.font, "asd", 0);


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
	
	SDL_RenderPresent(sdl.renderer);

	return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
}
