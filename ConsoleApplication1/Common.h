#pragma once

/*
 * ===============================================================
 *
 *                          Section_Rendering
 *
 * ===============================================================
 */
#define MAX_NUM_DRAW_COMMANDS 1024
typedef enum DrawCommandType { DrawCommandType_Default, DrawCommandType_Rect, DrawCommandType_Text } DrawCommandType;

typedef struct PlatformRect {
	float x, y, w, h;
} EguiRect;

typedef struct Str32 {
	char str[32];
} Str32;

typedef struct DrawCommand {
	DrawCommandType type;
	EguiRect rect;
	Str32 str;
	MyColor rect_color;
} DrawCommand;

typedef struct RenderingSystem {
	DrawCommand draw_commands[MAX_NUM_DRAW_COMMANDS];
	int num_commands;
} RenderingSystem;

RenderingSystem rendering_system;

void Draw(DrawCommand command) {
	assert(rendering_system.num_commands < MAX_NUM_DRAW_COMMANDS - 1);
	rendering_system.draw_commands[rendering_system.num_commands++] = command;
}

void DrawBatch() {
	for (int i = 0; i < rendering_system.num_commands; ++i) {
		DrawCommand command = rendering_system.draw_commands[i];
		DrawRectangle(command.rect.x, command.rect.y, command.rect.w, command.rect.h, command.rect_color);
		if (*command.str.str)
			DrawText(command.str.str, command.rect.x, command.rect.y, 13, BLACK);
	}

	rendering_system.num_commands = 0;
}

