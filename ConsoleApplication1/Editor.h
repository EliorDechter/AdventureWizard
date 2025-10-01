#ifndef EDITOR_H
#define EDITOR_H

#include "Egui.h"

void editor_do(Egui* gui, wzrd_draw_commands_buffer* buffer, wzrd_cursor *cursor, bool * is_interacting_with_editor, bool *is_hovering);
#endif