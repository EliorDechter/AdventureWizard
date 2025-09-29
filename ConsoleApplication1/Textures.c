#include "Textures.h"

Texture texture_get_by_name(str128 s) {
	int i = hashtable_get(texture_system.hashtable, s);
	Texture result = texture_system.textures[i];

	return result;
}

SDL_EnumerationResult enumerate_directory(void* userdata, const char* dirname, const char* fname) {
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

	texture_system.textures[texture_system.textures_count++] = (Texture){ .val = texture, .name = name };
	hashtable_add(&texture_system.hashtable, name, texture_system.textures_count - 1);

	return SDL_ENUM_CONTINUE;
}

void textures_load() {
	const char* path = "C:\\Users\\elior\\OneDrive\\Desktop\\ConsoleApplication1\\Resoruces\\Textures\\";
	SDL_EnumerateDirectory(path, enumerate_directory, 0);
}
