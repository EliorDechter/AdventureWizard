#if 0

/*
  * ===============================================================
  *
  *                          SERIALIZATION
  *
  * ===============================================================
  */
#define get_file_code(a, b, c, d) (((int)(a) << 0 | (int)(b) << 8 | (int)(c) << 16 | (int)(d) << 24))
#define save_file_magic_value get_file_code('p', 'a', 's', 'f')

#pragma pack(push, 1)
typedef struct Save_file_header {
	int magic_value;
	int big_string_size;
	int num_textures;
	int num_vertices;

	EntitySystem entity_system;
} Save_file_header;
#pragma pack(pop)

void SaveGame() {
	const char* save_file_name = "filey";
	FILE* file = fopen(save_file_name, "wb+");
	assert(file);

	int birg_string_size = string_system.size;
	int num_vertices = pathfinding_system.num_vertices;

	Save_file_header save_file_header = {
		.magic_value = save_file_magic_value,
		.big_string_size = birg_string_size,
		.num_vertices = num_vertices,
		.entity_system = entity_system
	};

	fwrite(&save_file_header, 1, sizeof(Save_file_header), file);
	fwrite(string_system.big_string, string_system.size, 1, file);
	fwrite(pathfinding_system.walking_area_vertices, sizeof(Vector2), pathfinding_system.num_vertices, file);
	//fwrite(entity_system.entities, sizeof(Entity), MAX_NUM_ENTITIES, file);

	fclose(file);

	Log(__LINE__, "Succesfully saved game");

}

void LoadGame(const char* save_file_name) {
	FILE* file = fopen(save_file_name, "rb");
	assert(file);

	fseek(file, 0, SEEK_END);
	int length = ftell(file);
	fseek(file, 0, SEEK_SET);

	assert(length);

	Save_file_header save_file_header = { 0 };
	fread(&save_file_header, 1, sizeof(Save_file_header), file);
	assert(save_file_header.magic_value == save_file_magic_value);
	entity_system = save_file_header.entity_system;

	fread(string_system.big_string, 1, save_file_header.big_string_size, file);

	for (int i = 0; i < save_file_header.num_vertices; ++i) {
		Vector2i v;
		fread(&v, sizeof(Vector2i), 1, file);
		AddWalkingAreaVertex((NodePos) { v });
	}

	//fread(&entity_system, sizeof(EntitySystem), 1, file);
#if 0
	for (int i = 0; i < MAX_NUM_ENTITIES; ++i) {
		Entity e;
		fread(&e, sizeof(Entity), 1, file);
		AddEntity(e);
		AddToHashTable(&entity_system.entities_map, e.name, entity_system.entities + (entity_system.num_entities - 1));

	}
#endif

	Log(__LINE__, "Succesfully loaded save file");
}


#endif