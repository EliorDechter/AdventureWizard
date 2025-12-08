#ifndef EDITOR_H
#define EDITOR_H

#include "WzGuiWidgets.h"
#include "Platform.h"
#include "Game.h"
void do_editor(wzrd_canvas* gui, PlatformTargetTexture target_texture, wzrd_icons icons, wzrd_str *debug_str, WzWidget *target_panel_out);

#endif