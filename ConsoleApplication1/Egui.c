#pragma once

#include "Egui.h"
#define TODO 0
// TODO: Do I have off by one errors all over the gui?
//  I had to subtract one in this piece of code to make it work:
// assert(box.absolute_rect.x + box.absolute_rect.width - 1 < egui.window_width);

// TODO: In the modify box, hover over the exit button, and see how the lock button gets a tooltip immediately

bool IsStr32Equal(Str32 a, Str32 b) {
	if (strcmp(a.str, b.str) == 0) return true;
	return false;
}

Str32 Str32Create(char* str) {
	Str32 result = { 0 };
	if (!str)
		return result;
	assert(strlen(str) < 32);
	strcpy(result.str, str);

	return result;
}


void RemoveFromHashTable(HashTable* hashtable, char* key) {
	for (int i = 0; i < MAX_NUM_HASHTABLE_ELEMENTS; ++i) {
		if (strcmp(hashtable->elements[i].key, key) == 0) hashtable->elements[i] = (HashTableElement){ 0 };
	}
}

void AddToHashTable(HashTable* hashtable, char* key, int value) {

	for (int i = 0; i < MAX_NUM_HASHTABLE_ELEMENTS; ++i) {
		if (strcmp(hashtable->elements[i].key, key) == 0) {
			hashtable->elements[i].value = value;
		}
	}

	HashTableElement element = {
		.value = value
	};

	strcpy(element.key, key);

	if (hashtable->num == 0) hashtable->num = 1;

	assert(hashtable->num < MAX_NUM_HASHTABLE_ELEMENTS - 1);
	hashtable->elements[hashtable->num++] = element;
}

int GetHashTableElement(HashTable hashtable, char* key) {
	for (int i = 1; i < MAX_NUM_HASHTABLE_ELEMENTS; ++i) {
		if (strcmp(key, hashtable.elements[i].key) == 0)
			return hashtable.elements[i].value;
	}

	return 0;
}
void EguiNext(int w, int h) {
	if (!egui.current_row_mode) {
		egui.current_pos.y += egui.current_child_gap + h;
	}
	else {
		egui.current_pos.x += egui.current_child_gap + w;
	}
}


Crate* EguiGetCurrentWindow() {
	Crate* result = &egui.windows_stack[egui.current_window_index];

	return result;
}

Crate* EguiGetPreviousWindow() {
	assert(egui.current_window_index > 0);
	Crate* result = &egui.windows_stack[egui.current_window_index - 1];

	return result;
}

Box* EguiBoxGetPrevious() {
	int current_box_index = egui.windows_stack[egui.current_window_index].current_panel_index;
	if (current_box_index < 1) return 0;
	Box* result = &egui.windows_stack[egui.current_window_index].panel_stack[current_box_index - 1];

	return result;
}

Box* EguiBoxGetCurrent() {
#if 1
	int i = egui.windows_stack[egui.current_window_index].current_panel_index;
	assert(i > -1);
	Box* result = &egui.windows_stack[egui.current_window_index].panel_stack[i];
#else
	int i = egui.windows_stack[egui.current_window_index].current_panel_index;
	assert(i > -1);
	Box* not_result = &egui.windows_stack[egui.current_window_index].panel_stack[i];

	Box* result = &egui.boxes[not_result->new_index];
#endif

	return result;
}


void EguiDrawBox(int box_index, EguiColor color) {
	assert(color.r >= 0);
	assert(color.g >= 0);
	assert(color.b >= 0);
	assert(color.a > 0);

	EguiDrawCommand command = {
		.type = EguiDrawCommandType_Box,
		.box_data = {.color = color, .box_index = box_index },
		.num = egui.num_draw_commands,
		.z = egui.current_window_id,
	};

	assert(egui.boxes[command.box_data.box_index].color.a);

	assert(egui.num_draw_commands < MAX_NUM_DRAW_COMMANDS);

	egui.draw_commands[egui.num_draw_commands++] = command;
}

void EguiDrawText(const char* text, EguiRect bounds, int alignment, EguiColor color) {
	assert(text);

	EguiDrawCommand command = {
		.type = EguiDrawCommandType_Text,
		.text_data = {
			.alignment = alignment,
			.color = color
		},
		.rect = bounds,
		.num = egui.num_draw_commands,
			.z = egui.current_window_id

	};

	strcpy(command.text_data.text, text);

	assert(egui.num_draw_commands < MAX_NUM_DRAW_COMMANDS);


	egui.draw_commands[egui.num_draw_commands++] = command;

}

void EguiDrawRect(EguiRect rect, int border_width, EguiColor border_color, EguiColor color) {

	assert(rect.x < egui.window_width);
	assert(rect.y < egui.window_height);

	EguiDrawCommand command = {
		.type = EguiDrawCommandType_Rect,
		.rect_data = {
			 .border_color = border_color,
			.border_width = border_width,
			.color = color,
			.border_color = border_color,
		},
			.z = egui.current_window_id,
		.rect = rect,
		.num = egui.num_draw_commands
	};

	assert(egui.num_draw_commands < MAX_NUM_DRAW_COMMANDS);

	egui.draw_commands[egui.num_draw_commands++] = command;
}

#if 0
void EguiDrawTexture(EguiRectrect, PlatformTexture texture) {

	EguiDrawCommand command = {
		.texture = texture,
		.type = EguiDrawCommandType_Texture,
		.z = egui.current_window_id,
		.rect = rect,
		.num = egui.num_draw_commands
	};

	assert(egui.num_draw_commands < MAX_NUM_DRAW_COMMANDS);


	egui.draw_commands[egui.num_draw_commands++] = command;
}
#endif

int CompareDrawCalls(const void* element1, const void* element2) {
	EguiDrawCommand* c1 = (EguiDrawCommand*)element1;
	EguiDrawCommand* c2 = (EguiDrawCommand*)element2;

	// TODO: Tooltip special case, should we handle drop down menus the same?
	if (c1->z == CrateId_Tooltip && c2->z == CrateId_Tooltip) {
		if (c1->num > c2->num) return 1;
		if (c1->num < c2->num) return -1;
		return 0;
	}
	else if (c1->z == CrateId_Tooltip)
		return 1;
	else if (c2->z == CrateId_Tooltip)
		return -1;

	if (c1->z > c2->z) {
		return 1;
	}
	if (c1->z < c2->z) {
		return -1;
	}
	if (c1->z == c2->z) {
		if (c1->num > c2->num) return 1;
		if (c1->num < c2->num) return -1;
		return 0;
	}

	return 0;
}
#if 0
void EguiTexturePanel(Vector2 pos, float p, PlatformTexture t) {

	float w = 0, h = 0;
	Box* current_box = EguiBoxGetCurrent();
	if (egui.current_row_mode) {
		w = current_box->w * p;
		h = current_box->h;
	}
	else {
		h = current_box->h * p;
		w = current_box->w;
	}

	EguiDrawTexture((EguiRect) { pos.x, pos.y, w, h }, t);
}
#endif

void EguiBoxBegin(Box box) {
	//assert(box.n > 0);

#if DEBUG_PANELS
	for (int i = 0; i < egui.total_num_panels; ++i) printf("\t");
	printf("Begin line %d\n", line);

	assert(egui.num_line_stack < 32);
	egui.panel_line_stack[egui.num_line_stack++] = line;
#endif

	// Increment the panel index in the window's stack
	egui.windows_stack[egui.current_window_index].current_panel_index++;
	egui.total_num_panels++;

	box.index = egui.windows_stack[egui.current_window_index].current_panel_index;
	box.window_index = egui.current_window_index;

	// Temporary
	assert(box.x >= 0 && box.w >= 0);

	Box* previous_box = EguiBoxGetPrevious();
	if (previous_box) {

		//Name
		if (!*box.name.str) {
			sprintf(box.name.str, "%s-%d", previous_box->name, previous_box->num_children);
		}

		// Children
		assert(previous_box->num_children < 255);
		previous_box->children[previous_box->num_children++] = egui.num_boxes;

		// Inner padding
		box.w += box.inner_padding.x + box.inner_padding.w;
		box.h += box.inner_padding.y + box.inner_padding.h;


		// Update the previous box in the box array 
		// TODO: Fix this mess
		egui.boxes[previous_box->new_index] = *previous_box;

	}
	else {
		//assert(box.w_internal != 0 && box.h_internal != 0 && box.wp == 0 && box.hp == 0);
	}

	// Padding
	// TODO: row mode, column mode?
	box.x += box.padding.x;
	box.y += box.padding.y;
	box.w -= box.padding.x * 2;
	box.h -= box.padding.y * 2;

	// Set current stuff
	egui.current_row_mode = box.row_mode;
	egui.current_child_gap = box.child_gap;

	// Add gap
	box.x += box.child_gap;
	box.y += box.child_gap;

	// Set current pos
	egui.current_pos.x += box.x;
	egui.current_pos.y += box.y;

	// Box's bsolute position
	box.absolute_rect = (EguiRect){ egui.current_pos.x, egui.current_pos.y, box.w, box.h };
	//assert(box.absolute_rect.x + box.absolute_rect.width - 1 < egui.window_width);
	//assert(box.absolute_rect.y + box.absolute_rect.height - 1 < egui.window_height);

	// Inner padding
	egui.current_pos.x += box.inner_padding.x;
	egui.current_pos.y += box.inner_padding.y;

	if (box.color.r == 0 && box.color.g == 0 && box.color.b == 0 && box.color.a == 0)
		box.color = EGUI_LIGHTGRAY;

	// Add box
	assert(egui.num_boxes < 256);
	box.new_index = egui.num_boxes;
	egui.boxes[egui.num_boxes++] = box;

	// Init current panel 
	*EguiBoxGetCurrent() = box;

	// Draw panel
	EguiDrawBox(box.new_index, box.color);
}

void EguiBoxEnd() {

	egui.total_num_panels--;
	assert(egui.total_num_panels >= 0);
	assert(egui.current_window_id != 0);

#if DEBUG_PANELS
	for (int i = 0; i < egui.total_num_panels; ++i) printf("\t");
	printf("End panel at line %d\n", line);
	egui.num_line_stack--;
	//printf("End line %d\n", egui.panel_line_stack[egui.num_line_stack]);
#endif

	Box* current_box = EguiBoxGetCurrent();
	Crate* current_crate = EguiGetCurrentWindow();

	//set to previous panel
	Box* previous_box = EguiBoxGetPrevious();
	if (previous_box) {

		// Handle sizing
		if (previous_box->size_type == SizeType_Empty) {

			if (previous_box->row_mode) {
				previous_box->w += current_box->w;
				previous_box->h = fmax(current_box->h + previous_box->inner_padding.y + previous_box->inner_padding.h, previous_box->h);
			}
			else {
				previous_box->h += current_box->h;
				previous_box->w = fmax(current_box->w + previous_box->inner_padding.x + previous_box->inner_padding.w, previous_box->w);
			}

			previous_box->absolute_rect.w = previous_box->w;
			previous_box->absolute_rect.h = previous_box->h;
		}

#if 0
		if (previous_panel->row_mode) {
			egui.current_pos.y = current_panel->absolute_rect.y + current_panel->absolute_rect.height;
			egui.current_pos.x = previous_panel->absolute_rect.x;
		}
		else {
			egui.current_pos.x = current_panel->absolute_rect.x + current_panel->absolute_rect.width;
			egui.current_pos.y = previous_panel->absolute_rect.y;
		}
#else 
		if (previous_box->row_mode) {
			egui.current_pos.x = current_box->absolute_rect.x + current_box->absolute_rect.w;
			egui.current_pos.y = current_box->absolute_rect.y;
		}
		else {
			egui.current_pos.y = current_box->absolute_rect.y + current_box->absolute_rect.h;
			egui.current_pos.x = current_box->absolute_rect.x;
		}
#endif

		// Go to previous settings
		egui.current_row_mode = previous_box->row_mode;
		egui.current_child_gap = previous_box->child_gap;
	}

	current_crate->current_panel_index--;
}


int PlatformCheckCollisionPointRect(EguiV2 v, EguiRect rect) {
	bool result = false;
	if (v.x >= rect.x &&
		v.y >= rect.y &&
		v.x <= rect.x + rect.w &&
		v.y <= rect.y + rect.h) result = true;

	return result;
}

void EguiCrateBegin(EguiV2 pos, EguiV2 size, int window_id, Box box) {

	//void EguiCrateBegin(Str32 name, Vector2 pos,
		//Vector2 size, Vector2 padding, BorderType border_type, Color color, int window_id, bool empty_size) {

	egui.total_num_windows++;
	egui.current_window_index++;

	Crate* current_window = EguiGetCurrentWindow();

	// Set current hot window
	if (PlatformCheckCollisionPointRect((EguiV2) { egui.mouse_pos.x, egui.mouse_pos.y },
		(EguiRect) {
		pos.x, pos.y, size.x, size.y
	})) {
		if (window_id > egui.hot_window)
			egui.hot_window = window_id;
	}
	else {
		if (window_id == egui.hot_window) egui.hot_window = 0;
	}

	// Save previous window current position
	if (egui.current_window_index > 0) {
		Crate* previous_window = EguiGetPreviousWindow();
		previous_window->current_pos = egui.current_pos;
		previous_window->current_column_mode = egui.current_row_mode;
		previous_window->current_child_gap = egui.current_child_gap;
	}

	// Set new window
	*current_window = (Crate){ .id = window_id, .index = egui.current_window_index, .current_pos = pos, .current_panel_index = -1 };
	egui.current_pos = pos;

	// Set window id
	assert(window_id != 0);
	egui.current_window_id = window_id;

	// Set color
	if (box.color.r == 0 && box.color.g == 0 && box.color.b == 0 && box.color.a == 0)
		box.color = EGUI_LIGHTGRAY;

	// Begin drawing panel
	if (box.size_type != SizeType_Empty) {
		box.w = size.x;
		box.h = size.y;
	}

	EguiBoxBegin(box);
}

EguiCrateEnd() {

	// Count number of windows for debugging
	egui.total_num_windows--;

	EguiBoxEnd();

	if (egui.current_window_index > 0) {
		Crate* previous_window = EguiGetPreviousWindow();

		assert(previous_window->id != 0);
		egui.current_window_id = previous_window->id;

		egui.current_pos = previous_window->current_pos;
		egui.current_row_mode = previous_window->current_column_mode;
		egui.current_child_gap = previous_window->current_child_gap;
	}

	egui.current_window_index--;
	assert(egui.current_window_index >= -1);
}

void EguiBegin(double time, EguiV2 padding, EguiV2 mouse_pos, EguiState mouse_left) {

	// Set parameters
	egui.time = time;
	egui.mouse_left = mouse_left;
	egui.mouse_pos = mouse_pos;
	egui.current_pos = (EguiV2){ 0 };
	egui.num_draw_commands = 0;
	//GuiEnableTooltip();
	egui.current_window_index = -1;
	egui.num_boxes = 0;

	// Begin drawing first window
	EguiCrateBegin((EguiV2) { 0, 0 },
		(EguiV2) {
		egui.window_width, egui.window_height
	},
		CrateId_Screen,
		(Box) {
		.name = Str32Create("Main window"), .padding = padding
	});
}

EguiDrawCommandsBuffer EguiEnd() {

	EguiCrateEnd();

	// Test each opened panel has been closed
	assert(egui.total_num_panels == 0);
	assert(egui.total_num_windows == 0);

	// n
#if 0
	for (int i = 0; i < egui.num_boxes; ++i) {
		Box* box = &egui.boxes[i];
		int n_total = 0;
		for (int j = 0; j < box->num_children; ++j) {
			int child = box->children[j];
			int n = egui.boxes[child].n;
			n_total += egui.boxes[child].n;
		}

		int size_per_n = 0;
		if (box->row_mode)
			size_per_n = box->w_internal / n_total;
		else
			size_per_n = box->h_internal / n_total;

		for (int j = 0; j < box->num_children; ++j) {
			int child_index = box->children[j];
			if (box->row_mode)
				egui.boxes[child_index].w_internal = size_per_n * egui.boxes[child_index].n;
			else
				egui.boxes[child_index].h_internal = size_per_n * egui.boxes[child_index].n;
		}
	}
#endif

	// Empty size
#if 0
	for (int i = 0; i < egui.num_boxes; ++i) {
		Box* box = &egui.boxes[i];

		if (box->size_type == SizeType_Empty) {

			float max_w = 0, max_h = 0;
			float total_h = 0, total_w = 0;
			for (int j = 0; j < box->num_children; ++j) {
				Box* child = &egui.boxes[box->children[j]];
				max_w = fmax(max_w, child->w_internal);
				max_h = fmax(max_h, child->h_internal);
				total_h += child->h_internal;
				total_w += child->h_internal;
			}

			if (box->row_mode) {
				box->w_internal = total_w + box->inner_padding.x + box->inner_padding.width;
				box->h_internal = max_h + box->inner_padding.y + box->inner_padding.height;
			}
			else {
				box->w_internal = max_w + box->inner_padding.x + box->inner_padding.width;
				box->h_internal = total_h + box->inner_padding.y + box->inner_padding.height;
			}

			box->absolute_rect.width = box->w_internal;
			box->absolute_rect.height = box->h_internal;

		}
	}
#endif

	// Grow
	for (int i = 0; i < egui.num_boxes; ++i) {
		Box* box = &egui.boxes[i];

		float max_w = 0, max_h = 0;
		float children_h = 0, children_w = 0;
		for (int j = 0; j < box->num_children; ++j) {
			Box* child = &egui.boxes[box->children[j]];

			if (!child->grow_horizontal)
				children_w += child->w;

			if (!child->grow_vertical)
				children_h += child->h;

			max_w = fmax(max_w, child->w);
			max_h = fmax(max_h, child->h);
		}

		for (int j = 0; j < box->num_children; ++j) {
			Box* child = &egui.boxes[box->children[j]];

			if (child->grow_horizontal) {
				if (box->row_mode) {
					child->w = box->w - children_w;
				}
				else {
					child->w = box->w;
				}
			}

			if (child->grow_vertical) {
				if (!box->row_mode) {
					child->h = box->h - children_h;
				}
				else {
					child->h = box->h;
				}
			}

			child->absolute_rect.w = child->w;
			child->absolute_rect.h = child->h;
		}
	}

	// Calculate positions
	for (int i = 0; i < egui.num_boxes; ++i) {
		Box* box = &egui.boxes[i];

		float x = box->x, y = box->y;

		for (int j = 0; j < box->num_children; ++j) {
			Box* child = &egui.boxes[box->children[j]];

			child->x = x;
			child->y = y;

			if (box->row_mode) {
				x += child->w;
			}
			else {
				y += child->h;
			}

			child->absolute_rect.x = child->x;
			child->absolute_rect.y = child->y;
		}
	}

	// Mouse interaction
	Box *hovered_box = 0;
	for (int i = 0; i < egui.num_boxes; ++i) {
		Box* box = egui.boxes + i;
		bool is_hover = PlatformCheckCollisionPointRect((EguiV2) { egui.mouse_pos.x, egui.mouse_pos.y },
			box->absolute_rect);
		if (is_hover) hovered_box = box;
#if 0
		if (is_active) {
			box->color = EGUI_RED;
			if (egui.mouse_left == EguiDeactivating) {
				egui.active_item = (Str32){ 0 };
			}
		}
		if (is_hot) {
			printf("hot: %d %s\n", i, egui.hot_item.str);
			//box->color = EGUI_BLUE;
			if (egui.mouse_left == EguiActivating) {
				egui.active_item = box->name;
			}

			if (!is_hover)
				egui.hot_item = (Str32){ 0 };
		}
		else if (is_hover) {
			//printf("hover: %s\n", box->name);
			egui.hot_item = box->name;
		}
#endif

	}

	if (hovered_box) {
		bool is_hot = IsStr32Equal(egui.hot_item, hovered_box->name);
		bool is_active = IsStr32Equal(egui.active_item, hovered_box->name);

		if (is_active) {
			hovered_box->color = EGUI_RED;
			if (egui.mouse_left == EguiDeactivating) {
				egui.active_item = (Str32){ 0 };
			}
		}
		else if (is_hot) {
			hovered_box->color = EGUI_BLUE;
			//printf("%d\n", egui.mouse_left);
			if (egui.mouse_left == EguiActivating) {
				egui.active_item = hovered_box->name;
			}

			//if (!is_hover)
				//egui.hot_item = (Str32){ 0 };
		}
		else {
			egui.hot_item = hovered_box->name;
		}
		//else if (box) {
			//printf("hover: %s\n", box->name);
		//}
	}

	// Drawing
	EguiDrawCommand draw_commands_ordered[MAX_NUM_DRAW_COMMANDS] = { 0 };

	for (int i = 0; i < egui.num_draw_commands; ++i) {
		assert(egui.draw_commands[i].type != EguiDrawCommandType_None);
	}

	for (int i = 0; i < egui.num_draw_commands; ++i) {
		draw_commands_ordered[i] = egui.draw_commands[i];
	}

	qsort(draw_commands_ordered, egui.num_draw_commands, sizeof(EguiDrawCommand), CompareDrawCalls);

	// assert ascending order 
	for (int i = 0; i < egui.num_draw_commands - 1; ++i) {
		assert(CompareDrawCalls(&draw_commands_ordered[i], &draw_commands_ordered[i + 1]) == -1);
	}

	// Mouse position
	egui.previous_mouse_pos = egui.mouse_pos;

	// Return
	EguiDrawCommandsBuffer result = { 0 };
	memcpy(result.commands, draw_commands_ordered, egui.num_draw_commands * sizeof(EguiDrawCommand));
	result.num = egui.num_draw_commands;

	return result;
}

bool EguiDropDownBoxBegin(EguiV2 pos, EguiV2 size) {

	EguiRect rect = { pos.x, pos.y, size.x, size.y };

	EguiCrateBegin(pos,
		size,
		CrateId_DropDown,
		(Box) {
		.name = Str32Create("drop down"),
			.push = 20,
			.inner_padding = (EguiRect){ 1, 1, 1, 1 },
			//.color = YELLOW,
			.size_type = SizeType_Empty,
	});

	if (!PlatformCheckCollisionPointRect(egui.mouse_pos, *((EguiRect*)&rect)) &&
		(egui.mouse_left == EguiActive || egui.mouse_right == EguiActive)) {
		//egui.windows[WindowId_DropDown] = (Window){ 0 };
		return false;
	}

	return true;
}

void EguiDropDownBoxEnd() {
	EguiCrateEnd();
}

// Draw tooltip using control bounds
static void EguiTooltip(EguiRect controlRec, Str32 str)
{
	if (str.str != 0)
	{
		EguiV2 textSize = { 0 };//MeasureTextEx(GuiGetFont(), str.str, (float)GuiGetStyle(DEFAULT, TEXT_SIZE), (float)GuiGetStyle(DEFAULT, TEXT_SPACING));

		if ((controlRec.x + textSize.x + 16) > GetScreenWidth()) controlRec.x -= (textSize.x + 16 - controlRec.w);

		EguiRect rect = (EguiRect){ controlRec.x, controlRec.y + controlRec.h + 4, textSize.x + 16, 0 + 8.0f };

		//EguiCrateBegin(Str32Create("Tooltip"), (EguiV2) { rect.x, rect.y }, (Vector2) { rect.width, rect.height }, (Vector2) { 0 }, BorderType_Black, (PlatformColor) { 255, 255, 204, 255 }, CrateId_Tooltip, EguiCrateBegin);
		EguiCrateBegin((EguiV2) { rect.x, rect.y }, (EguiV2) { rect.w, rect.h },
			CrateId_Tooltip,
			(Box) {
			.name = Str32Create("Tooltip"),
				.border_type = BorderType_Black,
				.color = (EguiColor){ 255, 255, 204, 255 }
		});

		{
			EguiLabel((EguiRect) { controlRec.x, controlRec.y + controlRec.h + 4, textSize.x + 16, 8.0f }, str.str);
		}
		EguiCrateEnd();

	}
}

typedef enum {
	STATE_NORMAL = 0,
	STATE_FOCUSED,
	STATE_PRESSED,
	STATE_DISABLED
} GuiState;

GuiState guiState;

int EguiLabelButton(EguiRect bounds, const char* text)
{
	GuiState state = guiState;
	bool pressed = false;

	// NOTE: We force bounds.width to be all text
	float textWidth = (float)GetTextWidth(text);
	//if ((bounds.width - 2 * GuiGetStyle(LABEL, BORDER_WIDTH) - 2 * GuiGetStyle(LABEL, TEXT_PADDING)) < textWidth) bounds.width = textWidth + 2 * GuiGetStyle(LABEL, BORDER_WIDTH) + 2 * GuiGetStyle(LABEL, TEXT_PADDING) + 2;

	// Update control
	//--------------------------------------------------------------------
	if ((state != STATE_DISABLED))
	{
		EguiV2 mousePoint = egui.mouse_pos;

		//check if below a panel

		// Check checkbox state
		if (PlatformCheckCollisionPointRect(*((EguiV2*)&mousePoint), bounds))
		{
			if (egui.hot_window == egui.current_window_id) {
				if (egui.mouse_left == EguiActivating) state = STATE_PRESSED;
				else state = STATE_FOCUSED;

				if (egui.mouse_left == EguiDeactivating) pressed = true;
			}
		}
	}
	//--------------------------------------------------------------------

	// Draw control
	//--------------------------------------------------------------------
	//EguiDrawText(text, bounds, state, egui.panels[current_panel].z);
	//EguiDrawText(text, GetTextBounds(LABEL, bounds), GuiGetStyle(LABEL, TEXT_ALIGNMENT), BLACK);

	//--------------------------------------------------------------------

	EguiNext(bounds.w, bounds.h);

	return pressed;
}

// Panel control
#if 0
int EguiPanel(PlatformRect bounds, const char* text)
{
#if !defined(RAYGUI_PANEL_BORDER_WIDTH)
#define RAYGUI_PANEL_BORDER_WIDTH   1
#endif

	int result = 0;
	GuiState state = guiState;

	// Text will be drawn as a header bar (if provided)
	PlatformRect statusBar = { bounds.x, bounds.y, bounds.width, bounds.height };
	//if ((text != NULL) && (bounds.height < RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT * 2.0f)) bounds.height = RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT * 2.0f;

	if (text != NULL)
	{
		// Move panel bounds after the header bar
		bounds.y += (float)RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT - 1;
		bounds.height -= (float)RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT - 1;
	}

	// Draw control
	//--------------------------------------------------------------------
	if (text != NULL) GuiStatusBar(statusBar, text);  // Draw panel header as status bar

#if 0
	EguiDrawRect(bounds, RAYGUI_PANEL_BORDER_WIDTH, GetColor(GuiGetStyle(DEFAULT, (state == STATE_DISABLED) ? (int)BORDER_COLOR_DISABLED : (int)LINE_COLOR)),
		GetColor(GuiGetStyle(DEFAULT, (state == STATE_DISABLED) ? (int)BASE_COLOR_DISABLED : (int)BACKGROUND_COLOR)));
#else 
	EguiDrawRect((PlatformRect) { bounds.x, bounds.y, bounds.width, 1 }, 0, BLACK, BLACK);
	EguiDrawRect((PlatformRect) { bounds.x, bounds.y, 1, bounds.height }, 0, BLACK, BLACK);
	EguiDrawRect((PlatformRect) { bounds.x, bounds.y + bounds.height, bounds.width, 1 }, 0, LIGHTGRAY, LIGHTGRAY);
	EguiDrawRect((PlatformRect) { bounds.x + bounds.width, bounds.y, 1, bounds.height }, 0, LIGHTGRAY, LIGHTGRAY);

#endif

	//--------------------------------------------------------------------

	return result;
}
#endif

typedef struct EguiButton {
	float w, h;
	Str32 str;
	Str32 tooltip_str;
	Str32 id;
	Alignment alignment;
	float push;
	bool is_label;
} EguiButton;

int EguiDoButton(EguiButton button)
{
	int result = 0;
	GuiState state = guiState;

	// Percentage 
	Box* current_box = EguiBoxGetCurrent();
#if 0
	float per = 0;
	if (egui.current_row_mode)
		per = button.w / current_box->w_internal;
	else
		per = button.h / current_box->h_internal;
#endif

	// Row mode
	bool row_mode = 0;
	row_mode = !current_box->row_mode;

	EguiBoxBegin((Box) {
		.row_mode = row_mode, .size_type = SizeType_Fixed,
			.w = button.w, .h = button.h, .str = button.str
	});
	{

		// Hot and active
		Box* b = EguiBoxGetCurrent();
		Box* b2 = &egui.boxes[b->new_index];
		bool is_hot = IsStr32Equal(egui.hot_item, b2->name);
		bool is_active = IsStr32Equal(egui.active_item, b2->name);
		//b2->color = EGUI_RED;
#if 0

		// Alignment
		if (button.alignment == Alignment_Center) {
			Box* current_box = EguiBoxGetCurrent();
			if (row_mode) {

			}
			else {
				float box_height = current_box->h;
				float button_height = button.h;

				egui.current_pos.y += (box_height / 2) - (button_height / 2);
			}
		}

		// Rect
		EguiRect rect = { egui.current_pos.x, egui.current_pos.y, button.w, button.h };

		// Mouse interaction
		if ((state != STATE_DISABLED))
		{
			EguiV2 mousePoint = egui.mouse_pos;

			// Check button state
			if (CheckCollisionPointRec(mousePoint, rect)) {
				if (egui.hot_window == egui.current_window_id)
				{
					egui.hot_item = button.id;

					if (egui.mouse_left == EguiActive) {
						state = STATE_PRESSED;
					}
					else {
						state = STATE_FOCUSED;
					}
					if (egui.mouse_left == EguiDeactivating) result = 1;
				}
			}
			//else if (IsStr32Equal(egui.hot_item, button.id)) egui.hot_item = (Str32){ 0 };
		}

		if (button.is_label && state == STATE_FOCUSED)
			EguiDrawRect(rect, 0, EGUI_LIGHTGRAY, EGUI_BLUE);
		else
			EguiDrawRect(rect, 0, EGUI_LIGHTGRAY, EGUI_LIGHTGRAY);

		if (!button.is_label) {
			if (state == STATE_NORMAL || state == STATE_FOCUSED) {
#if 0
				// Top and left lines
				EguiDrawRect((PlatformRect) { rect.x, rect.y, rect.width, 1 }, 0, WHITE, WHITE);
				EguiDrawRect((PlatformRect) { rect.x, rect.y, 1, rect.height }, 0, WHITE, WHITE);

				// Bottom and right lines
				EguiDrawRect((PlatformRect) { rect.x, rect.y + rect.height, rect.width, 1 }, 0, BLACK, BLACK);
				EguiDrawRect((PlatformRect) { rect.x + rect.width, rect.y, 1, rect.height }, 0, BLACK, BLACK);
				EguiDrawRect((PlatformRect) { rect.x + 1, rect.y + rect.height - 1, rect.width - 1, 1 }, 0, GRAY, GRAY);
				EguiDrawRect((PlatformRect) { rect.x + rect.width - 1, rect.y + 1, 1, rect.height - 1 }, 0, GRAY, GRAY);
#endif

				// Draw top and left lines
				EguiDrawRect((EguiRect) { rect.x, rect.y, rect.w, 1 }, 0, EGUI_WHITE, EGUI_WHITE);
				EguiDrawRect((EguiRect) { rect.x, rect.y, 1, rect.h }, 0, EGUI_WHITE, EGUI_WHITE);
				EguiDrawRect((EguiRect) { rect.x, rect.y + 1, rect.w, 1 }, 0, EGUI_LIGHTGRAY, EGUI_LIGHTGRAY);
				EguiDrawRect((EguiRect) { rect.x + 1, rect.y, 1, rect.h }, 0, EGUI_LIGHTGRAY, EGUI_LIGHTGRAY);

				// Draw bottom and right lines
				EguiDrawRect((EguiRect) { rect.x, rect.y + rect.h - 1, rect.w, 1 }, 0, EGUI_BLACK, EGUI_BLACK);
				EguiDrawRect((EguiRect) { rect.x + rect.w - 1, rect.y, 1, rect.h }, 0, EGUI_BLACK, EGUI_BLACK);
				EguiDrawRect((EguiRect) { rect.x, rect.y + rect.h - 1 - 1, rect.w, 1 }, 0, EGUI_GRAY, EGUI_GRAY);
				EguiDrawRect((EguiRect) { rect.x + rect.w - 1 - 1, rect.y, 1, rect.h }, 0, EGUI_GRAY, EGUI_GRAY);

			}
			else if (state == STATE_PRESSED) {
#if 0
				EguiDrawRect((PlatformRect) { rect.x, rect.y + rect.height, rect.width, 1 }, 0, WHITE, WHITE);
				EguiDrawRect((PlatformRect) { rect.x + rect.width, rect.y, 1, rect.height }, 0, WHITE, WHITE);
				EguiDrawRect((PlatformRect) { rect.x, rect.y, rect.width, 1 }, 0, BLACK, BLACK);
				EguiDrawRect((PlatformRect) { rect.x, rect.y, 1, rect.height }, 0, BLACK, BLACK);
				EguiDrawRect((PlatformRect) { rect.x + 1, rect.y + 1, rect.width - 1, 1 }, 0, GRAY, GRAY);
				EguiDrawRect((PlatformRect) { rect.x + 1, rect.y + 1, 1, rect.height - 1 }, 0, GRAY, GRAY);
#else 
				// Draw top and left lines
				EguiDrawRect((EguiRect) { rect.x, rect.y, rect.w, 1 }, 0, EGUI_BLACK, EGUI_BLACK);
				EguiDrawRect((EguiRect) { rect.x, rect.y, 1, rect.h }, 0, EGUI_BLACK, EGUI_BLACK);
				EguiDrawRect((EguiRect) { rect.x, rect.y + 1, rect.w, 1 }, 0, EGUI_GRAY, EGUI_GRAY);
				EguiDrawRect((EguiRect) { rect.x + 1, rect.y, 1, rect.h }, 0, EGUI_GRAY, EGUI_GRAY);

				// Draw bottom and right lines
				EguiDrawRect((EguiRect) { rect.x, rect.y + rect.h - 1, rect.w, 1 }, 0, EGUI_WHITE, EGUI_WHITE);
				EguiDrawRect((EguiRect) { rect.x + rect.w - 1, rect.y, 1, rect.h }, 0, EGUI_WHITE, EGUI_WHITE);
				EguiDrawRect((EguiRect) { rect.x, rect.y + rect.h - 1 - 1, rect.w, 1 }, 0, EGUI_LIGHTGRAY, EGUI_LIGHTGRAY);
				EguiDrawRect((EguiRect) { rect.x + rect.w - 1 - 1, rect.y, 1, rect.h }, 0, EGUI_LIGHTGRAY, EGUI_LIGHTGRAY);
#endif
			}
		}

#if TODO
		EguiDrawText(button.str.str, GetTextBounds(BUTTON, rect), GuiGetStyle(BUTTON, TEXT_ALIGNMENT), BLACK);
#endif

#endif

#if 0
		// Tooltip
		if (*button.tooltip_str.str && IsStr32Equal(egui.hot_item, button.id)) {
			if (state == STATE_FOCUSED) {
				if (!egui.tooltip_count_time) {
					egui.tooltip_count_time = true;
					egui.tooltip_time = egui.time;
				}
				else {
					if (egui.time - egui.tooltip_time >= 0.5) {
						EguiTooltip(rect, button.tooltip_str);
					}
				}
			}
			else {
				if (egui.tooltip_count_time) {
					egui.tooltip_count_time = false;
				}
			}
		}
#endif
	}
	EguiBoxEnd();

	return result;

}

int EguiToggle(EguiRect bounds, const char* text, bool* active)
{
	int result = 0;
	GuiState state = guiState;

	bool temp = false;
	if (active == 0) active = &temp;

	// Update control
	//--------------------------------------------------------------------
	if ((state != STATE_DISABLED))
	{
		EguiV2 mousePoint = egui.mouse_pos;

		// Check toggle button state
		if (egui.hot_window == egui.current_window_id && PlatformCheckCollisionPointRect(mousePoint, bounds))
		{
			if (egui.mouse_left == EguiActivating) state = STATE_PRESSED;
			else if (egui.mouse_left == EguiDeactivating)
			{
				state = STATE_NORMAL;
				*active = !(*active);
			}
			else state = STATE_FOCUSED;
		}
	}
	//--------------------------------------------------------------------

	// Draw control
	//--------------------------------------------------------------------


	EguiDrawRect(bounds, 0, EGUI_LIGHTGRAY, EGUI_LIGHTGRAY);

	if (state == STATE_PRESSED || *active)
	{
		EguiDrawRect((EguiRect) { bounds.x, bounds.y + bounds.h, bounds.w, 1 }, 0, EGUI_WHITE, EGUI_WHITE);
		EguiDrawRect((EguiRect) { bounds.x + bounds.w, bounds.y, 1, bounds.h }, 0, EGUI_WHITE, EGUI_WHITE);
		EguiDrawRect((EguiRect) { bounds.x, bounds.y, bounds.w, 1 }, 0, EGUI_BLACK, EGUI_BLACK);
		EguiDrawRect((EguiRect) { bounds.x, bounds.y, 1, bounds.h }, 0, EGUI_BLACK, EGUI_BLACK);
		EguiDrawRect((EguiRect) { bounds.x + 1, bounds.y + 1, bounds.w - 1, 1 }, 0, EGUI_GRAY, EGUI_GRAY);
		EguiDrawRect((EguiRect) { bounds.x + 1, bounds.y + 1, 1, bounds.h - 1 }, 0, EGUI_GRAY, EGUI_GRAY);
	}
	else {
		EguiDrawRect((EguiRect) { bounds.x, bounds.y + bounds.h, bounds.w, 1 }, 0, EGUI_BLACK, EGUI_BLACK);
		EguiDrawRect((EguiRect) { bounds.x + bounds.w, bounds.y, 1, bounds.h }, 0, EGUI_BLACK, EGUI_BLACK);
		EguiDrawRect((EguiRect) { bounds.x + 1, bounds.y + bounds.h - 1, bounds.w - 1, 1 }, 0, EGUI_GRAY, EGUI_GRAY);
		EguiDrawRect((EguiRect) { bounds.x + bounds.w - 1, bounds.y + 1, 1, bounds.h - 1 }, 0, EGUI_GRAY, EGUI_GRAY);
		EguiDrawRect((EguiRect) { bounds.x, bounds.y, bounds.w, 1 }, 0, EGUI_WHITE, EGUI_WHITE);
		EguiDrawRect((EguiRect) { bounds.x, bounds.y, 1, bounds.h }, 0, EGUI_WHITE, EGUI_WHITE);
	}

	//EguiDrawText(text, GetTextBounds(TOGGLE, bounds), GuiGetStyle(TOGGLE, TEXT_ALIGNMENT), BLACK);


	if (state == STATE_FOCUSED) EguiTooltip(bounds, Str32Create("sadf"));
	//--------------------------------------------------------------------

	EguiNext(bounds.w, bounds.h);

	return result;
}

bool EguiDialogBegin(Str32 name, int id, float* x, float* y, float w, float h, EguiV2 padding, bool* active) {

	// TODO: WHY NOT WORK?! WHY?! FUUUUK!
	//if (!*active) return;

	id += CrateId_Total;

	// Check active and hot
	Str32 title_str = { 0 };
	char* title = "Fix window naming";
	strcpy(title_str.str, title);
	bool is_hot = IsStr32Equal(egui.hot_item, title_str);
	bool is_active = IsStr32Equal(egui.active_item, title_str);

	// Move around
	if (is_active) {
		*x += egui.mouse_pos.x - egui.previous_mouse_pos.x;
		*y += egui.mouse_pos.y - egui.previous_mouse_pos.y;
	}

	// Status bar
	EguiRect statusBar = { *x, *y, w, (float)8 };

	// Hot and active
	if (is_active) {
		if (egui.mouse_left == EguiDeactivating) {
			if (is_hot) {
				// return true 
				// TODO: deactivate?
				egui.active_item = (Str32){ 0 };
			}
			else {
				egui.active_item = (Str32){ 0 };
				is_active = false;
			}
		}
	}
	else if (is_hot) {
		if (egui.mouse_left == EguiActive) {
			egui.active_item = title_str;
		}
	}
	if (PlatformCheckCollisionPointRect(egui.mouse_pos, statusBar)) {
		//egui.hot_item = title_str;
		static int x;
		x++;
	}
	else {
		// TODO: Warning, this only works when the window is the only hot item around.
		//		 Add id's to all widgets.
		//egui.hot_item = (Str32){ 0 };
	}

	EguiRect bounds = { *x, *y, w, h };

	// Draw window
	//EguiCrateBegin(name, (Vector2) { *x, * y }, (Vector2) { w, h }, padding, 0, (PlatformColor) { 0 }, id, EguiCrateBegin);

	EguiCrateBegin((EguiV2) { *x, * y }, (EguiV2) { w, h },
		id,
		(Box) {
		.name = name,
			.padding = padding,
			.border_type = BorderType_Black,
			.color = (EguiColor){ 255, 255, 204, 255 }
	});



	// Window title bar height (including borders)
	// NOTE: This define is also used by GuiMessageBox() and GuiTextInputBox()
#if !defined(RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT)
#define RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT        24
#endif

#if !defined(RAYGUI_WINDOWBOX_CLOSEBUTTON_HEIGHT)
#define RAYGUI_WINDOWBOX_CLOSEBUTTON_HEIGHT      18
#endif

	int result = 0;
	//GuiState state = guiState;

	if (bounds.h < RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT * 2.0f) bounds.h = RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT * 2.0f;

	const float vPadding = RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT / 2.0f - RAYGUI_WINDOWBOX_CLOSEBUTTON_HEIGHT / 2.0f;
	EguiRect windowPanel = { bounds.x, bounds.y + (float)RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT - 1, bounds.w, bounds.h - (float)RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT + 1 };
	EguiRect closeButtonRec = { statusBar.x + statusBar.w,
								 statusBar.y + vPadding, RAYGUI_WINDOWBOX_CLOSEBUTTON_HEIGHT, RAYGUI_WINDOWBOX_CLOSEBUTTON_HEIGHT };

	// Top status bar
	EguiBoxBegin((Box) { .per = 0.12, .row_mode = true, .border_type = BorderType_None, .color = EGUI_GRAY });
	{
#if 1
		EguiBoxBegin((Box) { .per = 0.8, .border_type = BorderType_None, .color = EGUI_GRAY });
		{
			//EguiDrawText(title, GetTextBounds(STATUSBAR, statusBar), GuiGetStyle(STATUSBAR, TEXT_ALIGNMENT), BLACK);
		}
		EguiBoxEnd();
#endif
#if 1
		EguiBoxBegin((Box) { 0, .border_type = BorderType_Black, .padding = (EguiV2){ 5, 5 } });
		{
			Box* current_box = EguiBoxGetCurrent();
			EguiDoButton((EguiButton) {
				.w = current_box->w, .h = current_box->h, .str = Str32Create("X"), .tooltip_str = Str32Create("exit")
			});
		}
		EguiBoxEnd();
#endif
	}
	EguiBoxEnd();

	// Draw window close button
	//int tempBorderWidth = GuiGetStyle(BUTTON, BORDER_WIDTH);
	//int tempTextAlignment = GuiGetStyle(BUTTON, TEXT_ALIGNMENT);
	//GuiSetStyle(BUTTON, BORDER_WIDTH, 1);
	//GuiSetStyle(BUTTON, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
	//result = EguiDoButton((EguiButton) { .rect = closeButtonRec, .str = CreateStr32(GuiIconText(ICON_CROSS_SMALL, NULL)) });
	//GuiSetStyle(BUTTON, BORDER_WIDTH, tempBorderWidth);
	//GuiSetStyle(BUTTON, TEXT_ALIGNMENT, tempTextAlignment);

	//return result;      // Window close button clicked: result = 1

	EguiBoxBegin((Box) { 0 });

	return *active;
}

void EguiEndDialog() {
	EguiBoxEnd();
	EguiCrateEnd();
}


int EguiTabBar(EguiRect bounds, const char** text, int count, int* active)
{
	//#define RAYGUI_TABBAR_ITEM_WIDTH    65

	int result = -1;
	//GuiState state = guiState;

	EguiRect tabBounds = bounds;

	if (*active < 0) *active = 0;
	else if (*active > count - 1) *active = count - 1;

	int offsetX = 0;    // Required in case tabs go out of screen
	//offsetX = (*active + 2) * RAYGUI_TABBAR_ITEM_WIDTH - GetScreenWidth();
	if (offsetX < 0) offsetX = 0;

	bool toggle = false;    // Required for individual toggles

	// Draw control
	//--------------------------------------------------------------------
	for (int i = 0; i < count; i++)
	{
		//tabBounds.x = bounds.x + (RAYGUI_TABBAR_ITEM_WIDTH + 4) * i - offsetX;
		tabBounds.x = bounds.x + bounds.w * i;

		if (tabBounds.x < GetScreenWidth())
		{
			// Draw tabs as toggle controls
			//int textAlignment = GuiGetStyle(TOGGLE, TEXT_ALIGNMENT);
			//int textPadding = GuiGetStyle(TOGGLE, TEXT_PADDING);
			//GuiSetStyle(TOGGLE, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
			//GuiSetStyle(TOGGLE, TEXT_PADDING, 8);

			if (i == (*active))
			{
				toggle = true;
				EguiToggle(tabBounds, text[i], &toggle);
			}
			else
			{
				toggle = false;
				EguiToggle(tabBounds, text[i], &toggle);
				if (toggle) *active = i;
			}

			// Close tab with middle mouse button pressed
			if (PlatformCheckCollisionPointRect(egui.mouse_pos, tabBounds) && egui.mouse_left == EguiActivating) result = i;

			//GuiSetStyle(TOGGLE, TEXT_PADDING, textPadding);
			//GuiSetStyle(TOGGLE, TEXT_ALIGNMENT, textAlignment);
#if 0
			// Draw tab close button
			// NOTE: Only draw close button for current tab: if (CheckCollisionPointRec(mousePosition, tabBounds))
			int tempBorderWidth = GuiGetStyle(BUTTON, BORDER_WIDTH);
			int tempTextAlignment = GuiGetStyle(BUTTON, TEXT_ALIGNMENT);
			GuiSetStyle(BUTTON, BORDER_WIDTH, 1);
			GuiSetStyle(BUTTON, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
#if defined(RAYGUI_NO_ICONS)
			if (GuiButton((PlatformRect) { tabBounds.x + tabBounds.width - 14 - 5, tabBounds.y + 5, 14, 14 }, "x")) result = i;
#else
			if (GuiButton((PlatformRect) { tabBounds.x + tabBounds.width - 14 - 5, tabBounds.y + 5, 14, 14 }, GuiIconText(ICON_CROSS_SMALL, NULL))) result = i;
#endif
			GuiSetStyle(BUTTON, BORDER_WIDTH, tempBorderWidth);
			GuiSetStyle(BUTTON, TEXT_ALIGNMENT, tempTextAlignment);
#endif

		}
	}

	// Draw tab-bar bottom line
	//GuiDrawPlatformRect((PlatformRect) { bounds.x, bounds.y + bounds.height - 1, bounds.width, 1 }, 0, BLANK, GetColor(GuiGetStyle));
	//--------------------------------------------------------------------
	EguiNext(bounds.w, bounds.h);


	return result;     // Return as result the current TAB closing requested
}

#if 0
// Text Box control
// NOTE: Returns true on ENTER pressed (useful for data validation)
int EguiTextBox(PlatformRect bounds, char* text, int textSize, bool editMode)
{
#if !defined(RAYGUI_TEXTBOX_AUTO_CURSOR_COOLDOWN)
#define RAYGUI_TEXTBOX_AUTO_CURSOR_COOLDOWN  30        // Frames to wait for autocursor movement
#endif
#if !defined(RAYGUI_TEXTBOX_AUTO_CURSOR_DELAY)
#define RAYGUI_TEXTBOX_AUTO_CURSOR_DELAY      2        // Frames delay for autocursor movement
#endif

#
	int result = 0;
	GuiState state = guiState;

	bool multiline = false;     // TODO: Consider multiline text input
	//int wrapMode = GuiGetStyle(DEFAULT, TEXT_WRAP_MODE);

	//PlatformRect textBounds = GetTextBounds(TEXTBOX, bounds);
	int textLength = (text != NULL) ? (int)strlen(text) : 0; // Get current text length
	//int thisCursorIndex = textBoxCursorIndex;
	//if (thisCursorIndex > textLength) thisCursorIndex = textLength;
	int textWidth = GetTextWidth(text) - GetTextWidth(text + thisCursorIndex);
	int textIndexOffset = 0;    // Text index offset to start drawing in the box

	// Cursor PlatformRect
	// NOTE: Position X value should be updated
	PlatformRect cursor = {
		//textBounds.x + textWidth + GuiGetStyle(DEFAULT, TEXT_SPACING),
		//textBounds.y + textBounds.height / 2 - GuiGetStyle(DEFAULT, TEXT_SIZE),
		2,
		(float)GuiGetStyle(DEFAULT, TEXT_SIZE) * 2
	};

	//if (cursor.height >= bounds.height) cursor.height = bounds.height - GuiGetStyle(TEXTBOX, BORDER_WIDTH) * 2;
	//if (cursor.y < (bounds.y + GuiGetStyle(TEXTBOX, BORDER_WIDTH))) cursor.y = bounds.y + GuiGetStyle(TEXTBOX, BORDER_WIDTH);

	// Mouse cursor PlatformRect
	// NOTE: Initialized outside of screen
	PlatformRect mouseCursor = cursor;
	mouseCursor.x = -1;
	mouseCursor.width = 1;

	// Blink-cursor frame counter
	//if (!autoCursorMode) blinkCursorFrameCounter++;
	//else blinkCursorFrameCounter = 0;

	// Update control
	//--------------------------------------------------------------------
	// WARNING: Text editing is only supported under certain conditions:
	if (state != STATE_DISABLED)               // Control not disabled
		//!GuiGetStyle(TEXTBOX, TEXT_READONLY) &&     // TextBox not on read-only mode
		//!guiLocked &&                               // Gui not locked
		//!guiControlExclusiveMode &&                       // No gui slider on dragging
		//(wrapMode == TEXT_WRAP_NONE))               // No wrap mode
	{
		Vector2 mousePosition = input_system.mouse_pos;

		if (editMode)
		{
			// GLOBAL: Auto-cursor movement logic
			// NOTE: Keystrokes are handled repeatedly when button is held down for some time
			//if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_BACKSPACE) || IsKeyDown(KEY_DELETE)) autoCursorCounter++;
			//else autoCursorCounter = 0;

			//bool autoCursorShouldTrigger = (autoCursorCounter > RAYGUI_TEXTBOX_AUTO_CURSOR_COOLDOWN) && ((autoCursorCounter % RAYGUI_TEXTBOX_AUTO_CURSOR_DELAY) == 0);

			state = STATE_PRESSED;

			if (textBoxCursorIndex > textLength) textBoxCursorIndex = textLength;

			// If text does not fit in the textbox and current cursor position is out of bounds,
			// we add an index offset to text for drawing only what requires depending on cursor
			while (textWidth >= textBounds.width)
			{
				int nextCodepointSize = 0;
				GetCodepointNext(text + textIndexOffset, &nextCodepointSize);

				textIndexOffset += nextCodepointSize;

				textWidth = GetTextWidth(text + textIndexOffset) - GetTextWidth(text + textBoxCursorIndex);
			}

			int codepoint = GetCharPressed();       // Get Unicode codepoint
			if (multiline && IsKeyPressed(KEY_ENTER)) codepoint = (int)'\n';

			// Encode codepoint as UTF-8
			int codepointSize = 0;
			const char* charEncoded = CodepointToUTF8(codepoint, &codepointSize);

			// Handle Paste action
			if (IsKeyPressed(KEY_V) && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)))
			{
				const char* pasteText = GetClipboardText();
				if (pasteText != NULL)
				{
					int pasteLength = 0;
					int pasteCodepoint;
					int pasteCodepointSize;
					// count how many codepoints to copy, stopping at the first unwanted control character
					while (true)
					{
						pasteCodepoint = GetCodepointNext(pasteText + pasteLength, &pasteCodepointSize);
						if (textLength + pasteLength + pasteCodepointSize >= textSize) break;
						if (!(multiline && (pasteCodepoint == (int)'\n')) && !(pasteCodepoint >= 32)) break;
						pasteLength += pasteCodepointSize;
					}
					if (pasteLength > 0)
					{
						// Move forward data from cursor position
						for (int i = textLength + pasteLength; i > textBoxCursorIndex; i--) text[i] = text[i - pasteLength];

						// Paste data in at cursor
						for (int i = 0; i < pasteLength; i++) text[textBoxCursorIndex + i] = pasteText[i];

						textBoxCursorIndex += pasteLength;
						textLength += pasteLength;
						text[textLength] = '\0';
					}
				}
			}
			// Add codepoint to text, at current cursor position
			// NOTE: Make sure we do not overflow buffer size
			else if (((multiline && (codepoint == (int)'\n')) || (codepoint >= 32)) && ((textLength + codepointSize) < textSize))
			{
				// Move forward data from cursor position
				for (int i = (textLength + codepointSize); i > textBoxCursorIndex; i--) text[i] = text[i - codepointSize];

				// Add new codepoint in current cursor position
				for (int i = 0; i < codepointSize; i++) text[textBoxCursorIndex + i] = charEncoded[i];

				textBoxCursorIndex += codepointSize;
				textLength += codepointSize;

				// Make sure text last character is EOL
				text[textLength] = '\0';
			}

			// Move cursor to start
			if ((textLength > 0) && IsKeyPressed(KEY_HOME)) textBoxCursorIndex = 0;

			// Move cursor to end
			if ((textLength > textBoxCursorIndex) && IsKeyPressed(KEY_END)) textBoxCursorIndex = textLength;

			// Delete related codepoints from text, after current cursor position
			if ((textLength > textBoxCursorIndex) && IsKeyPressed(KEY_DELETE) && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)))
			{
				int offset = textBoxCursorIndex;
				int accCodepointSize = 0;
				int nextCodepointSize;
				int nextCodepoint;
				// Check characters of the same type to delete (either ASCII punctuation or anything non-whitespace)
				// Not using isalnum() since it only works on ASCII characters
				nextCodepoint = GetCodepointNext(text + offset, &nextCodepointSize);
				bool puctuation = ispunct(nextCodepoint & 0xff);
				while (offset < textLength)
				{
					if ((puctuation && !ispunct(nextCodepoint & 0xff)) || (!puctuation && (isspace(nextCodepoint & 0xff) || ispunct(nextCodepoint & 0xff))))
						break;
					offset += nextCodepointSize;
					accCodepointSize += nextCodepointSize;
					nextCodepoint = GetCodepointNext(text + offset, &nextCodepointSize);
				}
				// Check whitespace to delete (ASCII only)
				while (offset < textLength)
				{
					if (!isspace(nextCodepoint & 0xff))
						break;
					offset += nextCodepointSize;
					accCodepointSize += nextCodepointSize;
					nextCodepoint = GetCodepointNext(text + offset, &nextCodepointSize);
				}

				// Move text after cursor forward (including final null terminator)
				for (int i = offset; i <= textLength; i++) text[i - accCodepointSize] = text[i];

				textLength -= accCodepointSize;
			}
			// Delete single codepoint from text, after current cursor position
			else if ((textLength > textBoxCursorIndex) && (IsKeyPressed(KEY_DELETE) || (IsKeyDown(KEY_DELETE))))
			{
				int nextCodepointSize = 0;
				GetCodepointNext(text + textBoxCursorIndex, &nextCodepointSize);

				// Move text after cursor forward (including final null terminator)
				for (int i = textBoxCursorIndex + nextCodepointSize; i <= textLength; i++) text[i - nextCodepointSize] = text[i];

				textLength -= nextCodepointSize;
			}

			// Delete related codepoints from text, before current cursor position
			if ((textBoxCursorIndex > 0) && IsKeyPressed(KEY_BACKSPACE) && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)))
			{
				int offset = textBoxCursorIndex;
				int accCodepointSize = 0;
				int prevCodepointSize;
				int prevCodepoint;

				// Check whitespace to delete (ASCII only)
				while (offset > 0)
				{
					prevCodepoint = GetCodepointPrevious(text + offset, &prevCodepointSize);
					if (!isspace(prevCodepoint & 0xff)) break;

					offset -= prevCodepointSize;
					accCodepointSize += prevCodepointSize;
				}
				// Check characters of the same type to delete (either ASCII punctuation or anything non-whitespace)
				// Not using isalnum() since it only works on ASCII characters
				bool puctuation = ispunct(prevCodepoint & 0xff);
				while (offset > 0)
				{
					prevCodepoint = GetCodepointPrevious(text + offset, &prevCodepointSize);
					if ((puctuation && !ispunct(prevCodepoint & 0xff)) || (!puctuation && (isspace(prevCodepoint & 0xff) || ispunct(prevCodepoint & 0xff)))) break;

					offset -= prevCodepointSize;
					accCodepointSize += prevCodepointSize;
				}

				// Move text after cursor forward (including final null terminator)
				for (int i = textBoxCursorIndex; i <= textLength; i++) text[i - accCodepointSize] = text[i];

				textLength -= accCodepointSize;
				textBoxCursorIndex -= accCodepointSize;
			}
			// Delete single codepoint from text, before current cursor position
			else if ((textBoxCursorIndex > 0) && (IsKeyPressed(KEY_BACKSPACE) || (IsKeyDown(KEY_BACKSPACE))))
			{
				int prevCodepointSize = 0;

				GetCodepointPrevious(text + textBoxCursorIndex, &prevCodepointSize);

				// Move text after cursor forward (including final null terminator)
				for (int i = textBoxCursorIndex; i <= textLength; i++) text[i - prevCodepointSize] = text[i];

				textLength -= prevCodepointSize;
				textBoxCursorIndex -= prevCodepointSize;
			}

			// Move cursor position with keys
			if ((textBoxCursorIndex > 0) && IsKeyPressed(KEY_LEFT) && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)))
			{
				int offset = textBoxCursorIndex;
				int accCodepointSize = 0;
				int prevCodepointSize;
				int prevCodepoint;

				// Check whitespace to skip (ASCII only)
				while (offset > 0)
				{
					prevCodepoint = GetCodepointPrevious(text + offset, &prevCodepointSize);
					if (!isspace(prevCodepoint & 0xff)) break;

					offset -= prevCodepointSize;
					accCodepointSize += prevCodepointSize;
				}

				// Check characters of the same type to skip (either ASCII punctuation or anything non-whitespace)
				// Not using isalnum() since it only works on ASCII characters
				bool puctuation = ispunct(prevCodepoint & 0xff);
				while (offset > 0)
				{
					prevCodepoint = GetCodepointPrevious(text + offset, &prevCodepointSize);
					if ((puctuation && !ispunct(prevCodepoint & 0xff)) || (!puctuation && (isspace(prevCodepoint & 0xff) || ispunct(prevCodepoint & 0xff)))) break;

					offset -= prevCodepointSize;
					accCodepointSize += prevCodepointSize;
				}

				textBoxCursorIndex = offset;
			}
			else if ((textBoxCursorIndex > 0) && (IsKeyPressed(KEY_LEFT) || (IsKeyDown(KEY_LEFT) && autoCursorShouldTrigger)))
			{
				int prevCodepointSize = 0;
				GetCodepointPrevious(text + textBoxCursorIndex, &prevCodepointSize);

				textBoxCursorIndex -= prevCodepointSize;
			}
			else if ((textLength > textBoxCursorIndex) && IsKeyPressed(KEY_RIGHT) && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)))
			{
				int offset = textBoxCursorIndex;
				int accCodepointSize = 0;
				int nextCodepointSize;
				int nextCodepoint;

				// Check characters of the same type to skip (either ASCII punctuation or anything non-whitespace)
				// Not using isalnum() since it only works on ASCII characters
				nextCodepoint = GetCodepointNext(text + offset, &nextCodepointSize);
				bool puctuation = ispunct(nextCodepoint & 0xff);
				while (offset < textLength)
				{
					if ((puctuation && !ispunct(nextCodepoint & 0xff)) || (!puctuation && (isspace(nextCodepoint & 0xff) || ispunct(nextCodepoint & 0xff)))) break;

					offset += nextCodepointSize;
					accCodepointSize += nextCodepointSize;
					nextCodepoint = GetCodepointNext(text + offset, &nextCodepointSize);
				}

				// Check whitespace to skip (ASCII only)
				while (offset < textLength)
				{
					if (!isspace(nextCodepoint & 0xff)) break;

					offset += nextCodepointSize;
					accCodepointSize += nextCodepointSize;
					nextCodepoint = GetCodepointNext(text + offset, &nextCodepointSize);
				}

				textBoxCursorIndex = offset;
			}
			else if ((textLength > textBoxCursorIndex) && (IsKeyPressed(KEY_RIGHT) || (IsKeyDown(KEY_RIGHT) && autoCursorShouldTrigger)))
			{
				int nextCodepointSize = 0;
				GetCodepointNext(text + textBoxCursorIndex, &nextCodepointSize);

				textBoxCursorIndex += nextCodepointSize;
			}

			// Move cursor position with mouse
			if (egui.hot_window == egui.current_window_id && CheckCollisionPointRec(mousePosition, textBounds))     // Mouse hover text
			{
				//float scaleFactor = (float)GuiGetStyle(DEFAULT, TEXT_SIZE) / (float)guiFont.baseSize;
				int codepointIndex = 0;
				float glyphWidth = 0.0f;
				float widthToMouseX = 0;
				int mouseCursorIndex = 0;

				for (int i = textIndexOffset; i < textLength; i += codepointSize)
				{
					codepoint = GetCodepointNext(&text[i], &codepointSize);
					//codepointIndex = GetGlyphIndex(guiFont, codepoint);

					//if (guiFont.glyphs[codepointIndex].advanceX == 0) glyphWidth = ((float)guiFont.recs[codepointIndex].width * scaleFactor);
					//else glyphWidth = ((float)guiFont.glyphs[codepointIndex].advanceX * scaleFactor);

					if (mousePosition.x <= (textBounds.x + (widthToMouseX + glyphWidth / 2)))
					{
						mouseCursor.x = textBounds.x + widthToMouseX;
						mouseCursorIndex = i;
						break;
					}

					widthToMouseX += (glyphWidth + (float)GuiGetStyle(DEFAULT, TEXT_SPACING));
				}

				// Check if mouse cursor is at the last position
				int textEndWidth = GetTextWidth(text + textIndexOffset);
				if (input_system.mouse_pos.x >= (textBounds.x + textEndWidth - glyphWidth / 2))
				{
					mouseCursor.x = textBounds.x + textEndWidth;
					mouseCursorIndex = textLength;
				}

				// Place cursor at required index on mouse click
				if ((mouseCursor.x >= 0) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
				{
					cursor.x = mouseCursor.x;
					textBoxCursorIndex = mouseCursorIndex;
				}
			}
			else mouseCursor.x = -1;

			// Recalculate cursor position.y depending on textBoxCursorIndex
			cursor.x = bounds.x + GuiGetStyle(TEXTBOX, TEXT_PADDING) + GetTextWidth(text + textIndexOffset) - GetTextWidth(text + textBoxCursorIndex) + GuiGetStyle(DEFAULT, TEXT_SPACING);
			//if (multiline) cursor.y = GetTextLines()

			// Finish text editing on ENTER or mouse click outside bounds
			if ((!multiline && IsKeyPressed(KEY_ENTER)) ||
				(!CheckCollisionPointRec(mousePosition, bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)))
			{
				textBoxCursorIndex = 0;     // GLOBAL: Reset the shared cursor index
				autoCursorCounter = 0;      // GLOBAL: Reset counter for repeated keystrokes
				result = 1;
			}
		}
		else
		{
			if (egui.hot_window == egui.current_window_id && CheckCollisionPointRec(mousePosition, bounds))
			{
				state = STATE_FOCUSED;

				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
				{
					textBoxCursorIndex = textLength;   // GLOBAL: Place cursor index to the end of current text
					autoCursorCounter = 0;             // GLOBAL: Reset counter for repeated keystrokes
					result = 1;
				}
			}
		}
	}
	//--------------------------------------------------------------------

	// Draw control
	//--------------------------------------------------------------------
	if (state == STATE_PRESSED)
	{
		EguiDrawRect(bounds, GuiGetStyle(TEXTBOX, BORDER_WIDTH), GetColor(GuiGetStyle(TEXTBOX, BORDER + (state * 3))), GetColor(GuiGetStyle(TEXTBOX, BASE_COLOR_PRESSED)));
	}
	else if (state == STATE_DISABLED)
	{
		EguiDrawRect(bounds, GuiGetStyle(TEXTBOX, BORDER_WIDTH), GetColor(GuiGetStyle(TEXTBOX, BORDER + (state * 3))), GetColor(GuiGetStyle(TEXTBOX, BASE_COLOR_DISABLED)));
	}
	else EguiDrawRect(bounds, GuiGetStyle(TEXTBOX, BORDER_WIDTH), GetColor(GuiGetStyle(TEXTBOX, BORDER + (state * 3))), BLANK);

	// Draw text considering index offset if required
	// NOTE: Text index offset depends on cursor position
	EguiDrawText(text + textIndexOffset, textBounds, GuiGetStyle(TEXTBOX, TEXT_ALIGNMENT), GetColor(GuiGetStyle(TEXTBOX, TEXT + (state * 3))));

	// Draw cursor
	if (editMode && !GuiGetStyle(TEXTBOX, TEXT_READONLY))
	{
		//if (autoCursorMode || ((blinkCursorFrameCounter/40)%2 == 0))
		EguiDrawRect(cursor, 0, BLANK, GetColor(GuiGetStyle(TEXTBOX, BORDER_COLOR_PRESSED)));

		// Draw mouse position cursor (if required)
		if (mouseCursor.x >= 0) EguiDrawRect(mouseCursor, 0, BLANK, GetColor(GuiGetStyle(TEXTBOX, BORDER_COLOR_PRESSED)));
	}
	else if (state == STATE_FOCUSED) GuiTooltip(bounds);
	//--------------------------------------------------------------------

	return result;      // Mouse button pressed: result = 1
}
#endif
#if 0
int EguiLine(PlatformRect bounds, const char* text)
{
#if !defined(RAYGUI_LINE_MARGIN_TEXT)
#define RAYGUI_LINE_MARGIN_TEXT  12
#endif
#if !defined(RAYGUI_LINE_TEXT_PADDING)
#define RAYGUI_LINE_TEXT_PADDING  4
#endif

	int result = 0;
	GuiState state = guiState;

	Color color = GetColor(GuiGetStyle(DEFAULT, (state == STATE_DISABLED) ? (int)BORDER_COLOR_DISABLED : (int)LINE_COLOR));

	// Draw control
	//--------------------------------------------------------------------
	if (text == NULL) GuiDrawPlatformRect((PlatformRect) { bounds.x, bounds.y + bounds.height / 2, bounds.width, 1 }, 0, BLANK, color);
	else
	{
		PlatformRect textBounds = { 0 };
		textBounds.width = (float)GetTextWidth(text) + 2;
		textBounds.height = bounds.height;
		textBounds.x = bounds.x + RAYGUI_LINE_MARGIN_TEXT;
		textBounds.y = bounds.y;

		// Draw line with embedded text label: "--- text --------------"
		EguiDrawRect((PlatformRect) { bounds.x, bounds.y + bounds.height / 2, RAYGUI_LINE_MARGIN_TEXT - RAYGUI_LINE_TEXT_PADDING, 1 }, 0, BLANK, color);
		EguiDrawText(text, textBounds, TEXT_ALIGN_LEFT, color);
		EguiDrawRect((PlatformRect) { bounds.x + 12 + textBounds.width + 4, bounds.y + bounds.height / 2, bounds.width - textBounds.width - RAYGUI_LINE_MARGIN_TEXT - RAYGUI_LINE_TEXT_PADDING, 1 }, 0, BLANK, color);
	}
	//--------------------------------------------------------------------

	return result;
}
#endif

// Status Bar control
int EguiStatusBar(EguiRect bounds, const char* text)
{
	int result = 0;
	GuiState state = guiState;

	// Draw control
	//--------------------------------------------------------------------
	//EguiDrawRect(bounds, GuiGetStyle(STATUSBAR, BORDER_WIDTH), GetColor(GuiGetStyle(STATUSBAR, BORDER + (state * 3))), GetColor(GuiGetStyle(STATUSBAR, BASE + (state * 3))));
	//EguiDrawText(text, GetTextBounds(STATUSBAR, bounds), GuiGetStyle(STATUSBAR, TEXT_ALIGNMENT), BLACK);
	//--------------------------------------------------------------------

	return result;
}

// Label control
int EguiLabel(EguiRect bounds, const char* text)
{
	int result = 0;
	GuiState state = guiState;

	// Update control
	//--------------------------------------------------------------------
	//...
	//--------------------------------------------------------------------

	// Draw control
	//--------------------------------------------------------------------
	//EguiDrawText(text, bounds, GuiGetStyle(LABEL, TEXT_ALIGNMENT), BLACK);
	//--------------------------------------------------------------------

	EguiNext(bounds.w, bounds.h);


	return result;
}

void EguiLog(EguiRect rect) {

	static int num_lines;
	static EguiV2 scroll;
	static s_is_log_dirty;
	static char log_buffer[128];
	static char multi_line_text[128];

	if (s_is_log_dirty) {
		const int max_chars_in_line = 20;
		int counter = 0;
		int line_len = 0;
		int line_num = 0;
		multi_line_text[counter++] = line_num + '0';
		multi_line_text[counter++] = ' ';
		char line_num_string[8] = { 0 };
		for (int i = 0; i < 1024 * 10; ++i) {
			if (log_buffer[i] == '\0') {
				multi_line_text[counter] = 0;
				break;
			}
			else if (log_buffer[i] == '\n') {
				line_len = 0;
				line_num++;
				multi_line_text[counter++] = '\n';
				sprintf(line_num_string, "%d", line_num);
				strcpy(multi_line_text + counter, line_num_string);
				counter += strlen(line_num_string);
				multi_line_text[counter++] = ' ';
			}
			else if (line_len == 50) {
				multi_line_text[counter++] = '\n';
				line_num++;
				multi_line_text[counter++] = line_num + '0';
				multi_line_text[counter++] = ' ';
				line_len = 0;
			}
			else {
				multi_line_text[counter++] = log_buffer[i];
			}
		}

		num_lines = line_num;
		scroll.y = -1 * 10 * (num_lines - 10);
	}

	EguiRect scissorRec = { 0 };
	if (s_is_log_dirty) {
		scroll.y = -(10 * (num_lines - 10));
	}
	//GuiScrollPanel(rect, "Log", (PlatformRect) { rect.x, rect.y, rect.width, 966 }, & scroll, & scissorRec);
	BeginScissorMode(scissorRec.x, scissorRec.y, scissorRec.w, scissorRec.h);

	EguiRect label_rect1 = (EguiRect){ rect.x + 10, rect.y + scroll.y + 30, rect.w, rect.h };
	//GuiLabel(label_rect1, multi_line_text);

	EndScissorMode();
}

/*
 * ===============================================================
 *
 *                          S_EDITOR
 *
 * ===============================================================
 */


Widget* GetWidget(char* id, const char* str) {
	int index = GetHashTableElement(editor.widgets_hashtable, id);
	if (index == 0) {
		assert(editor.num_widgets < MAX_NUM_WIDGETS);
		editor.widgets[editor.num_widgets] = (Widget){ 0 };
		if (str) {
			strcpy(editor.widgets[editor.num_widgets].text, str);
		}
		index = editor.num_widgets;
		AddToHashTable(&editor.widgets_hashtable, id, editor.num_widgets);
		editor.num_widgets++;
	}

	Widget* result = &editor.widgets[index];

	return result;
}

bool EditorButton(const char* str, const char* tooltip_str) {

	Str32 id = Str32Create(str);
	Str32 s = Str32Create(str);
	Str32 ts = Str32Create(tooltip_str);
	bool result = EguiDoButton(((EguiButton) { .alignment = Alignment_Center, .id = id, .w = 110, .h = 24, .str = s, .tooltip_str = ts }));

	return result;
}


bool EditorLabel2(const char* str, const char* tooltip_str) {


	Str32 id = Str32Create(str);
	Str32 s = Str32Create(str);
	Str32 ts = Str32Create(tooltip_str);
	bool result = EguiDoButton(((EguiButton) { .alignment = Alignment_Center, .id = id, .w = 110, .h = 24, .str = s, .tooltip_str = ts, .is_label = true }));

	return result;
}


bool EditorButtonIcon(const char* str, const char* tooltip_str) {

	Str32 id = Str32Create(str);
	Str32 s = Str32Create(str);
	Str32 ts = Str32Create(tooltip_str);
	bool result = EguiDoButton(((EguiButton) {
		.alignment = Alignment_Center,
			.id = id, .w = EDITOR_ICON_SIZE, .h = EDITOR_ICON_SIZE,
			.str = s, .tooltip_str = ts
	}));

	return result;
}

void EditorToggleRaw(const char* id, const char* str) {
	EguiRect rect = { egui.current_pos.x, egui.current_pos.y, EDITOR_ICON_SIZE, EDITOR_ICON_SIZE };
	EguiToggle(rect, str, GetWidget(id, 0)->flag);
}

bool EditorToggleIcon(const char* str, bool* b) {
	EguiRect rect = { egui.current_pos.x, egui.current_pos.y, EDITOR_ICON_SIZE, EDITOR_ICON_SIZE };
	EguiToggle(rect, str, b);
}

float GetTextWidth() {
	return 0;
}

bool EditorLabel(const char* str) {
	int w = GetTextWidth(str);
	//int h = GuiGetStyle(DEFAULT, TEXT_SIZE);
	EguiRect rect = { egui.current_pos.x, egui.current_pos.y, w,  EDITOR_BUTTON_SIZE_Y };

	EguiLabel(rect, str);
}

bool EditorLabelButton(const char* str) {
	EguiRect rect = { egui.current_pos.x, egui.current_pos.y, EDITOR_BUTTON_SIZE_X, EDITOR_BUTTON_SIZE_Y };
	bool result = EguiLabelButton(rect, str);

	return result;
}

#define EditorTextBox(str) EditorInputBoxRaw(Stringify(__LINE__), str)

char* EditorInputBoxRaw(char* id, char* str) {
	Widget* widget = GetWidget(id, str);
	assert(widget);

	EguiRect rect = { egui.current_pos.x, egui.current_pos.y, EDITOR_BUTTON_SIZE_X, EDITOR_BUTTON_SIZE_Y };
#if 0
	if (EguiTextBox(rect, widget->text, 10, widget->flag)) {
		widget->flag = !widget->flag;
	}
#endif

	EguiNext(EDITOR_BUTTON_SIZE_X, EDITOR_BUTTON_SIZE_Y);

	char* result = widget->text;

	return result;
}

#define EditorInputBoxAndLabel(label, text) EditorInputBoxAndLabelRaw(Stringify(__LINE__), label, text)

void EditorInputBoxAndLabelRaw(const char* id, const char* label, char* text_box) {

	Widget* widget = GetWidget(id, text_box);

	//int x = EguiBeginRow();
	EditorLabel(label);
	EditorInputBoxRaw(id, text_box);
	//EguiEndRow(x);
}


void DeInitGame() {
	//ResetEntitySystem();
}

//void EditorToggle(const char *str, bool *b) {
//	PlatformRect r = { editor.pos.x, editor.pos.y, 50, 50 };
//	GuiToggle(r, str, b);
//
//	editor.pos.y += 10;
//}
//
//void EditorInput(const char* str) {
//	PlatformRect r = { editor.pos.x, editor.pos.y, 50, 50 };
//	DrawText("Name:", rect.x, rect.y, 5, DARKGRAY);
//
//	rect.x += rect.width + 5;
//	GuiTextBox(rect, editor.text0, 100, false);
//}

void EditorTabBar(char** str, int count, int* active) {
	EguiTabBar((EguiRect) { egui.current_pos.x, egui.current_pos.y, 70, 24 }, str, count, active);
}

EguiV2 V2Add(EguiV2 a, EguiV2 b) {
	EguiV2 result = (EguiV2){ a.x + b.x, a.y + b.y };

	return result;
}

EguiV2 V2Sub(EguiV2 a, EguiV2 b) {
	EguiV2 result = (EguiV2){ a.x - b.x, a.y - b.y };

	return result;
}

void EditorEndOffset() {
	egui.current_pos = V2Sub(egui.current_pos, editor.offset);
	editor.offset = (EguiV2){ 0 };
}

void EditorBeginOffset(EguiV2 offset) {
	egui.current_pos = V2Add(offset, egui.current_pos);
	editor.offset = offset;
}

char multi_line_text[1024 * 10];


bool IsNumber(const char* str) {
	if (!str || str[0] == '\0')
		return false;
	int i = 0;
	if (str[0] == '-') i = 1;
	for (; i < strlen(str); ++i) {
		if (str[i] > '9' || str[i] < '0') return false;
	}

	return true;
}

void EditorSliceSpritesheet() {
#if 0
	//TODO: function fails when x and y are in the center
	static char file_name[NAME_SIZE];
	static bool b1, b2, b3, b4, b5, b6, b7, b8;
	static bool do_it;
	static int num_rows = 1;
	static int num_columns = 1;
	static int x = 0;
	static int y = 0;
	static int width = GAME_SCREEN_WIDTH;
	static int height = GAME_SCREEN_HEIGHT;
	static PlatformTexture t;
	static char animation_names[NAME_SIZE * 10];
	static char rows_str[NAME_SIZE], columns_str[NAME_SIZE], x_str[NAME_SIZE], y_str[NAME_SIZE], height_str[NAME_SIZE], width_str[NAME_SIZE];
	static bool init;

	if (!init) {
		sprintf(rows_str, "%d", num_rows);
		sprintf(columns_str, "%d", num_columns);
		sprintf(x_str, "%d", x);
		sprintf(y_str, "%d", y);
		sprintf(width_str, "%d", width);
		sprintf(height_str, "%d", height);
		init = true;
	}

	EguiDrawPanel((PlatformRect) { egui.current_pos.x, egui.current_pos.y - 5, EDITOR_RIGHT_PANEL_WIDTH - 10, 600 }, WHITE);

	Vector2 offset = (Vector2){ 5, 0 };
	EditorBeginOffset(offset);

	if (EditorInputBox2("Texture:", file_name, b1)) {
		b1 = !b1;
	}

	if (EditorButton("Load texture")) {
		t = GetSprite(file_name).texture;
		sprintf(width_str, "%d", t.width);
		sprintf(height_str, "%d", t.height);
	}
	if (EditorInputBox2("x:", x_str, b4)) {
		b4 = !b4;
	}
	if (EditorInputBox2("y:", y_str, b5)) {
		b5 = !b5;
	}

	if (EditorInputBox2("width:", width_str, b7)) {
		b7 = !b7;
	}
	if (EditorInputBox2("height:", height_str, b8)) {
		b8 = !b8;
	}

	if (EditorInputBox2("Rows:", rows_str, b2)) {
		b2 = !b2;
	}

	if (EditorInputBox2("Columns:", columns_str, b3)) {
		b3 = !b3;
	}

	if (EditorInputBox2("Animation names:", animation_names, b6)) {
		b6 = !b6;
	}

	if (IsNumber(rows_str)) {
		num_rows = TextToInteger(rows_str);
	}

	if (IsNumber(columns_str)) {
		num_columns = TextToInteger(columns_str);
	}

	if (IsNumber(x_str)) {
		x = TextToInteger(x_str);
	}

	if (IsNumber(y_str)) {
		y = TextToInteger(y_str);
	}

	if (IsNumber(width_str)) {
		width = TextToInteger(width_str);
	}

	if (IsNumber(height_str)) {
		height = TextToInteger(height_str);
	}

	if (EditorButton("Save!")) {
		if (file_name && file_name[0] != 0) {
			char text[1000] = { 0 };
			sprintf(text, "%s\n%d %d\n%d %d\n%d %d\n%s", file_name, x, y, width, height, num_rows, num_columns, animation_names);
			char path[256] = { 0 };
			sprintf(path, "C:\\Users\\elior\\OneDrive\\Desktop\\ConsoleApplication1\\Resoruces\\%s", file_name);
			SaveFileText(path, text);

			OldLog("Saved spritesheet data\n");
		}
	}

	float new_width, new_height;
	float w_ratio = 1, h_ratio = 1;

	if (t.id) {
		BeginTextureMode(target);
		float texture_ratio = t.width / t.height;
		float screen_ratio = GAME_SCREEN_WIDTH / GAME_SCREEN_HEIGHT;
		if (screen_ratio > texture_ratio) {
			new_width = t.width * GAME_SCREEN_HEIGHT / t.height;
			new_height = GAME_SCREEN_HEIGHT;
		}
		else {
			new_width = GAME_SCREEN_WIDTH;
			new_height = t.height * GAME_SCREEN_WIDTH / t.width;
			DrawTextureRect(t, (PlatformRect) { 0, 0, GAME_SCREEN_WIDTH, t.height* GAME_SCREEN_WIDTH / t.width });
		}
		DrawTextureRect(t, (PlatformRect) { 0, 0, new_width, new_height });

		w_ratio = new_width / t.width;
		h_ratio = new_height / t.height;

		EndTextureMode();
	}

	float delta_x = width * w_ratio / num_columns;
	float pos_x = x;
	for (int i = 0; i < num_columns + 1; ++i) {
		BeginTextureMode(target);
		DrawLineEx((Vector2) { pos_x, y }, (Vector2) { pos_x, height* h_ratio + y }, 2, BLUE);
		EndTextureMode();

		pos_x += delta_x;
	}

	float delta_y = height * h_ratio / num_rows;
	float pos_y = y;
	for (int i = 0; i < num_rows + 1; ++i) {
		BeginTextureMode(target);
		DrawLineEx((Vector2) { x, pos_y }, (Vector2) { width* w_ratio + x, pos_y }, 2, BLUE);
		EndTextureMode();
		pos_y += delta_y;
	}

	EditorEndOffset();
#endif
}

void EditorChangeWindow(EditorWindow window) {
	if (window == editor.window) {
		//editor.window = EditorWindow_Default;
		return;
	}

	editor.window = window;
}

int EditorDropDownBoxRaw(char* id, char* text) {
#if 0
	int index = GetHashTableElement(editor.widgets_hashtable, id);
	if (index == 0) {
		assert(editor.num_widgets < MAX_NUM_WIDGETS);
		editor.widgets[editor.num_widgets] = (Widget){ 0 };
		index = editor.num_widgets;
		AddToHashTable(&editor.widgets_hashtable, id, editor.num_widgets);
		editor.num_widgets++;
	}

	PlatformRect rect = { egui.current_pos.x, egui.current_pos.y, 110, 24 };

	if (EguiDropDownBox(rect, text, &editor.widgets[index].active, editor.widgets[index].flag)) {
		editor.widgets[index].flag = !editor.widgets[index].flag;
	}


	int result = editor.widgets[index].active;

	return result;
#endif
}

void InitEditor() {
	editor.num_widgets = 1;
}

void AddWindowBox() {
#if 0
	static bool bb;
	static bool bb2;
	static bool bb3;
	static char text_box1[NAME_SIZE];
	static char text_box2[NAME_SIZE] = "100";
	static char text_box3[NAME_SIZE] = "100";

	PlatformRect rect = (PlatformRect){ 50, 50, 200, 200 };
	if (GuiWindowBox(rect, "hi!")) {
		editor.show_add_entity_box = false;
		strcpy(text_box1, "\0");
		strcpy(text_box2, "100");
		strcpy(text_box3, "100");

	}
	else {
		Vector2 pos = { rect.x + 5, rect.y + 20 };

		egui.current_pos = pos;


		if (EditorInputBox2("name:", text_box1, bb)) {
			bb = !bb;
		}

		if (EditorInputBox2("width:", text_box2, bb2)) {
			bb2 = !bb2;
		}

		if (EditorInputBox2("height:", text_box3, bb3)) {
			bb3 = !bb3;
		}

		/*static bool em;
		static int ac;
		if (EguiDropdownBox((PlatformRect) { 0, 0, 100, 100 }, "hello;goodbye", & ac, em)) {
			em = !em;
		}*/

		int selection = EditorDropDownBox("character;background");

		int w = TextToInteger(text_box2);
		int h = TextToInteger(text_box3);

		if (EditorButton("create!")) {
			if (selection == 0)
				CreateCharacter(text_box1, (Vector2) { GAME_SCREEN_WIDTH / 2, GAME_SCREEN_HEIGHT / 2 }, (Vector2) { w, h });
			else if (selection == 1)
				CreateEntity(text_box1, EntityType_Background, (Vector2i) { GAME_SCREEN_WIDTH / 2, GAME_SCREEN_HEIGHT / 2 }, (Vector2) { w, h }, GetSprite(text_box1));

		}
	}
#endif
}
