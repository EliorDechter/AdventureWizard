#if 0
/*
 * ===============================================================
 *
 *                          GAME STATE
 *
 * ===============================================================
 */

bool IsGrabbingItem() {
	return item_system.grabbed_item.index;
}

Rect GetEntityRect(Entity entity) {
	Rect rect = {
			entity.pixel_pos.pos.x - entity.size.x / 2,
			entity.pixel_pos.pos.y - entity.size.y / 2,
			entity.size.x,
			entity.size.y
	};

	return rect;
}

void HandleEntityGrabbing(bool is_playing) {
	// Reset released and hovered entities
	if (grab_system.grab_status == GrabStatus_Releasing) {
		grab_system.grabbed_entity = (EntityId){ 0 };
		grab_system.grab_status = GrabStatus_None;
	}

	grab_system.num_hovered_entities = 0;

	// Get hovered entities
	EntityId* ordered_entities = GetOrderedEntities();
	for (int i = 0; i < arrlen(ordered_entities); ++i) {
		Entity* current_entity = GetEntity(ordered_entities[i]);

		Rect rect = GetEntityRect(*current_entity);

		if (IsHovered(rect)) {
			assert(grab_system.num_hovered_entities < MAX_NUM_ENTITIES - 1);
			grab_system.hovered_entities[grab_system.num_hovered_entities++] = current_entity->id;
		}
	}
	arrfree(ordered_entities);

	// Get the first hovered entity
	if (grab_system.num_hovered_entities) {
		EntityId entity = grab_system.hovered_entities[grab_system.num_hovered_entities - 1];
		grab_system.hovered_entity = entity;
	}
	else {
		grab_system.hovered_entity = (EntityId){ 0 };
	}

	// Set grabbed entity
	if (input_system.input_event == InputEvent_LeftClickPressed && grab_system.hovered_entity.index) {
		if ((!is_playing && !GetEntity(grab_system.hovered_entity)->is_locked) || GetEntity(grab_system.hovered_entity)->is_grabbable) {
			grab_system.grabbed_entity = grab_system.hovered_entity;
			grab_system.grab_status = GrabStatus_Grabbing;

			UndoSystemDo();
		}
	}

	// Get the released entity
	if (input_system.input_event == InputEvent_LeftClickReleased && grab_system.grabbed_entity.index) {
		grab_system.grab_status = GrabStatus_Releasing;
	}

}

void SetEntityPixelPos(EntityId id, Vector2i pos) {
	GetEntity(id)->pixel_pos = (PixelPos){ pos };
}

void HandleInventoryItemGrabbing() {

	if (grab_system.grabbed_entity.index) {
		item_system.grabbed_item = grab_system.grabbed_entity;
		for (int i = 0; i < arrlen(item_system.inventory_items_buff); ++i) {
			if (IsIdEqual(item_system.grabbed_item, item_system.inventory_items_buff[i])) {
				item_system.grabbed_item_index = i;
			}
		}
	}
	else {
		item_system.grabbed_item = (EntityId){ 0 };
	}

	//check if released entity
	if (IsEntityNone(item_system.grabbed_item) || grab_system.grab_status != GrabStatus_Releasing) {
		return;
	}

	//get the most forefront entity that is not currently released item
	EntityId entity = { 0 };

	for (int i = 0; i < grab_system.num_hovered_entities; ++i) {
		EntityId current_entity = grab_system.hovered_entities[i];

		if (IsIdEqual(current_entity, grab_system.grabbed_entity)) {
			continue;
		}

		entity = current_entity;
	}

	bool interaction_success = false;

	switch (GetEntity(entity)->type) {
	case EntityType_None: {
		break;
	}
	case EntityType_InventoryItem: {
		interaction_success = DoInventoryItemInteractionWithInventoryItem(entity);
		break;
	}
	case EntityType_Screen: {
		interaction_success = DoInventoryItemInteractionWithScreenItem(entity);
		break;
	}
	}

	if (!interaction_success)
		SetEntityPixelPos(item_system.grabbed_item, GetItemPos(item_system.grabbed_item_index));
}

void DrawEntities(bool is_playing) {
	EntityId* ordered_entities = GetOrderedEntities();

	for (int i = 0; i < arrlen(ordered_entities); ++i) {
		Entity entity = *GetEntity(ordered_entities[i]);
		Rect rect = GetEntityRect(entity);

		if (entity.type == EntityType_None) continue;

		if (entity.current_animation) {
			Animation animation = shget(entity.animations_map, entity.current_animation);
			DrawAnimation(animation, rect, entity.flipped);
		}
		else {
			if (entity.sprite.type == SpriteType_Texture) {
				//TODO: draw center flipped
				if (!entity.flipped)
					PlatformDrawTextureRect(entity.sprite.texture, rect);
				else
					DrawTextureRectFlipped(entity.sprite.texture, rect);
			}
			else if (entity.sprite.type == SpriteType_Rect) {
				PlatformRectDraw(rect, entity.sprite.color);
			}
			else if (entity.sprite.type == SpriteType_Text) {
				PlatformDrawText(entity.sprite.text, entity.pixel_pos.pos.x, entity.pixel_pos.pos.y);
			}

		}

		if (!is_playing) {
			PlatformRectDraw(rect, (PlatformColor) { 150, 150, 150, 200 });
			//DrawPlatformRectLinesEx(rect, 1, (PlatformColor) { 150, 150, 150, 200 });
		}

	}

	arrfree(ordered_entities);
}

#endif