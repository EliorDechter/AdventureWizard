#ifndef EDITOR_H
#define EDITOR_H

#include "Egui.h"
#include "Platform.h"
#include "Game.h"
void editor_do(wzrd_canvas* gui, wzrd_draw_commands_buffer* buffer, wzrd_cursor* cursor,PlatformTargetTexture target_texture, wzrd_icons icons, unsigned int layer, wzrd_str *debug_str);

#endif