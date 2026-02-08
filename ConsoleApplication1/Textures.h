#pragma once

#include "Strings.h"
#include "Platform.h"
#include "HashTable.h"

typedef struct texture {
	str128 name;
	PlatformTexture val;
	int rows_count, columns_count;
} Texture;


#define TEXTURES_MAX_COUNT 256

typedef struct Texture_system {
	hashtable hashtable;
	Texture textures_widget_a[TEXTURES_MAX_COUNT];
	int textures_count;
} Texture_system;

static Texture_system g_texture_system;


Texture texture_get_by_name(str128 s);
void textures_load();