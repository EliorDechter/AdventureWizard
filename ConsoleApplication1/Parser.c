
#if 0
/*
 * ===============================================================
 *
 *                          S_PARSER
 *
 * ===============================================================
 */
#define NUM_ENTITIES 16
#define NUM_EVENTS 16

typedef struct Area {
	Entity entities[NUM_ENTITIES];
	int num_entities;

	Event events[NUM_EVENTS];
	int num_events;
} Area;

//typedef struct ConfigEntity {
//	char name[256];
//	char texture_name[256];
//	Vector2 pos;
//} ConfigEntity;

typedef struct Tokenizer {
	char* token;
	int len;
	int line_number;
	char* str;

	bool error;
	char error_name[256];

	Event event;
	Entity* entities_buff;
} Tokenizer;

Tokenizer tokenizer;

typedef struct Token {
	char* str;
	int size;
} Token;

bool IsTokenEqual(Token t0, const char* str) {
	if (str == 0 || *str == 0 || *str == EOF || t0.str == 0) return false;
	if (t0.size != (int)strlen(str)) return false;

	for (int i = 0; i < t0.size; ++i) {
		if (t0.str[i] != str[i]) return false;
	}

	return true;
}

bool Peek(const char* name) {
	if (tokenizer.error) return false;

	char* str = tokenizer.str;
	Token token = { 0 };

	while (isspace(*str)) {
		str++;
	}

	if (*str == '\0' && *name == '\0') return true;

	token.str = str;

	while (!isspace(*str) && *str != EOF && *str != 0) {
		str++;
		token.size++;
	}

	if (IsTokenEqual(token, name))
		return true;

	return false;
}

bool GetNextToken() {
	tokenizer.len = 0;

	if (*tokenizer.str == EOF || *tokenizer.str == 0) return false;

	while (isspace(*tokenizer.str) || *tokenizer.str == ',') {
		if (*tokenizer.str == '\n') {
			tokenizer.line_number++;
		}
		tokenizer.str++;
		//if (*tokenizer.current_string == EOF)
			//return false;
	}

	tokenizer.token = tokenizer.str;

	while (!isspace(*tokenizer.str) && *tokenizer.str != ',' && *tokenizer.str != EOF && *tokenizer.str != 0) {
		//if (*tokenizer.current_string == EOF || *tokenizer.current_string == 0) return false;

		tokenizer.str++;
		tokenizer.len++;
	}

	return true;
}

void Expect(const char* str) {
	if (tokenizer.error) return;

	GetNextToken();
	Token token = { tokenizer.token, tokenizer.len };
	bool result = IsTokenEqual(token, str);
	if (!result) {
		tokenizer.error = true;
		OldLog("[LOG] Expected \"%s\" at line %d\n", str, tokenizer.line_number);
	}
}

void StoreString(char* name) {
	if (tokenizer.error) return;

	GetNextToken();
	for (int i = 0; i < tokenizer.len; ++i) {
		char c = tokenizer.token[i];
		assert((c <= 'z' && c >= 'a') || (c >= '0' && c <= '9') || c == '_');
		*name = c;
		name++;
	}
	*name = 0;
}

void StoreInt(int* i) {
	if (tokenizer.error) return;

	GetNextToken();

	*i = atoi(tokenizer.token);
}

void StoreFloat(float* f) {
	if (tokenizer.error) return;

	GetNextToken();
	char s[256] = { 0 };
	for (int i = 0; i < tokenizer.len; ++i) {
		char c = tokenizer.token[i];
		assert((c == '.') || (c >= '0' && c <= '9'));
		s[i] = c;
	}

	*f = atof(s);
}

//bool OptionalExpect(const char *str) {
//	Token token = Peek(str);
//	bool result = IsTokenEqual(token, str);
//
//	return result;
//
//}

//void OptionalStoreString(char *name) {
//	for (int i = 0; i < tokenizer.len; ++i) {
//		char c = tokenizer.str[i];
//		assert((c <= 'z' && c >= 'a') || (c >= '0' && c <= '9'));
//		*name = c;
//	}
//}



typedef struct ConfigAction {
	char type_name[256];
	ActionType type;
	Vector2 pos;
	char depends_on[256];
} ConfigAction;

typedef struct ConfigEvent {
	char name[256];
	ConfigAction* config_action_buff;
} ConfigEvent;

SpriteSheet ParseSpriteData(const char* file_name) {
	char path[256] = { 0 };
	sprintf(path, "C:\\Users\\elior\\OneDrive\\Desktop\\ConsoleApplication1\\Resoruces\\%s", file_name);

	char* str = LoadFileText(path);

	if (!str) {
		tokenizer.error = true;
		return (SpriteSheet) { 0 };
	}

	tokenizer = (Tokenizer){ 0 };
	tokenizer.str = str;
	tokenizer.error = false;

	char texture_name[NAME_SIZE];
	int x, y, width, height, num_rows, num_columns;
	StoreString(texture_name);
	StoreInt(&x);
	StoreInt(&y);
	StoreInt(&width);
	StoreInt(&height);
	StoreInt(&num_rows);
	StoreInt(&num_columns);
	// TODO: store animation names

	SpriteSheet result = {
		.num_sprites = (Vector2i){num_rows, num_columns}
	};

	strcpy(result.texture_name, texture_name);

	return result;
}

void Parse(char* str) {
	if (!str) {
		tokenizer.error = true;
		return;
	}
	tokenizer = (Tokenizer){ 0 };
	tokenizer.str = str;
	tokenizer.error = false;

	while (isspace(*tokenizer.str)) tokenizer.str++;

	Expect("entities:");
	if (tokenizer.error) return;

	while (!tokenizer.error && !Peek("events:") && !Peek("\0")) {
		char name[NAME_SIZE];
		Vector2 pos = { 0 };
		Vector2 size = { 0 };

		Expect("name:");
		StoreString(name);

		Expect("pos:");
		StoreFloat(&pos.x);
		StoreFloat(&pos.y);

		Expect("size:");
		StoreFloat(&size.x);
		StoreFloat(&size.y);

#if 1
		Entity e = CreateEntityRaw(name, EntityType_Character, (Vector2i) { pos.x, pos.y }, size, GetSprite(name));
		arrput(tokenizer.entities_buff, e);
#endif

	}

	if (Peek("\0"))
		return;

	//TODO: add more than one event
	Event event = { 0 };
	Expect("events:");
	Expect("name:");
	StoreString(event.name);

	Expect("actions:");
	while (!tokenizer.error && !Peek("\0")) {
		Action action = { 0 };
		Expect("name:");
		StoreString(action.name);

		Expect("type:");
		char type_name[NAME_SIZE];
		StoreString(type_name);

		if (strcmp(type_name, "animate_once") == 0) {
			action.type = ActionType_AnimateOnce;
		}
		else if (strcmp(type_name, "create_item") == 0) {
			action.type = ActionType_CreateItem;
		}
		else {
			assert(0);
		}

		Expect("pos:");
		StoreFloat(&action.pos.x);
		StoreFloat(&action.pos.y);

		if (Peek("depends_on:")) {
			GetNextToken();
			StoreString(&action.depends_on);
		}

		action.size = (Vector2){ 50, 50 };
		arrput(event.actions_buff, action);
	}

	tokenizer.event = event;
}

void InitGameFromText() {
	char* txt = LoadFileText("C:\\Users\\elior\\OneDrive\\Desktop\\ConsoleApplication1\\Resoruces\\config.txt");
	Parse(txt);
	if (!tokenizer.error) {
		OldLog("LOG: Successfuly loaded config file\n");
		//TODO: remove code from the parse function that creates entities, and move it here
		//      and also handle adding events
#if 0
		ResetEntitySystem();
		//add entities from parsed config file
		for (int i = 0; i < arrlen(tokenizer.entities_buff); ++i) {
			CreateEntity(tokenizer.entities_buff[i]);
		}
#endif
	}
	else {
		OldLog("LOG: Failed to load config file\n");
	}

}

#endif