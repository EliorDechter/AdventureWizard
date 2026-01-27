#include "Textures.h"

Texture texture_get_by_name(str128 s) {
	int i = hashtable_get(g_texture_system.hashtable, s);
	Texture result = g_texture_system.textures[i];
	
	if (!result.name.len)
	{
		return (Texture) { 0 };
	}

	return result;
}

SDL_EnumerationResult enumerate_directory(void* userdata, const char* dirname, const char* fname) {
	(void)userdata;
	if (dirname) printf("%s\n", dirname);
	if (fname) printf("%s\n", fname);

	char path[256] = { 0 };
	sprintf(path, "%s%s", dirname, fname);

	PlatformTexture texture = PlatformTextureLoad(path);

	str128 name = str128_create(fname);
	for (int i = 0; i < name.len; ++i) {
		if (name.val[i] == '.') {
			name.val[i] = 0;
			name.len = i;
		}
	}

	g_texture_system.textures[g_texture_system.textures_count++] = (Texture){ .val = texture, .name = name };
	hashtable_add(&g_texture_system.hashtable, name, g_texture_system.textures_count - 1);

	return SDL_ENUM_CONTINUE;
}

void textures_load() {
	const char* path = "C:\\Users\\Elior\\source\\repos\\AdventureWizard\\Resoruces\\Textures\\";
	//SDL_PathInfo info;
	//bool result = SDL_GetPathInfo(path, &info);
	//assert(result);
	//SDL_EnumerateDirectory(path, enumerate_directory, 0);
}
