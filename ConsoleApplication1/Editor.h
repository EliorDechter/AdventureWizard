#ifndef EDITOR_H
#define EDITOR_H

#include "Egui.h"
#include "Platform.h"
void editor_do(Egui* gui, wzrd_draw_commands_buffer* buffer, wzrd_cursor* cursor, bool enable_input, PlatformTargetTexture target_texture, wzrd_icons icons);

#endif