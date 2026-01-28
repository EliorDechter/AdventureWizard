#ifndef EDITOR_H
#define EDITOR_H

#include "WzGuiWidgets.h"
#include "Platform.h"
#include "Game.h"


typedef enum EditorTextures
{
	EditorTextureButton,
	EditorTextureTotal,
} EditorTextures;

WzTexture editor_textures[EditorTextureTotal];

typedef struct Editor
{
	bool is_drawing_widget;
	int drawing_widget_x, drawing_widget_y, drawing_widget_w, drawing_widget_h;

	bool create_object_dialog_active;
	wzrd_v2 create_object_dialog_pos;
} Editor;

void editor_run(WzGui* gui, PlatformTargetTexture target_texture, wzrd_icons icons, WzStr *debug_str, WzWidget *target_panel_out);

#endif