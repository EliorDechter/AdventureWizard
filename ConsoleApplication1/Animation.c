#if 0
/*
 * ===============================================================
 *
 *                          S_ANIMATION
 *
 * ===============================================================
 */

typedef struct Animation {
	bool valid;
	char name[NAME_SIZE];
	SpriteSheet sprite_sheet;
	double time_per_sprite;
	int current_frame;
	int row;
	Status status;
} Animation;

struct {
	int key;
	Animation* value;
} *animations_map;


void BeginAnimation(Animation* animation) {
	animation->status = Status_Running;
}

void EndAnimation(Animation* animation) {
	animation->status = Status_Not_Running;
}


void Animate(Animation* animation) {
	assert(animation->valid);
	assert(animation);
	assert(animation->sprite_sheet.num_sprites.x > 0 && animation->sprite_sheet.num_sprites.y > 0);

	if (DoTimer(animation->name, animation->time_per_sprite)) {
		return;
	}

	animation->current_frame = (animation->current_frame + 1) %
		(int)animation->sprite_sheet.num_sprites.y;
}

void DrawAnimation(Animation animation, Rect dest, bool flip_flag) {

	// handle bad input
	if (!animation.sprite_sheet.num_sprites.x || !animation.sprite_sheet.num_sprites.y) {
		PlatformDrawTextureRect((PlatformTexture) { 0 }, dest);
		return;
	}

	PlatformTexture texture = GetSprite(animation.sprite_sheet.texture_name).texture;
	Vector2i num_sprites = animation.sprite_sheet.num_sprites;
	int row = animation.row;

	float width = texture.texture.width / num_sprites.y;
	float height = texture.texture.height / num_sprites.x;
	float x = width * animation.current_frame;
	float y = height * row;

	Rect source = { x, y, width, height };

	int flip = 1;
	if (flip_flag) flip = -1;


	PlatformTextureDraw(texture, dest.x, dest.y, dest.w, dest.h);

}

Animation CreateAnimation(const char* name, SpriteSheet sprite_sheet, int row) {
	assert(row < sprite_sheet.num_sprites.x);

	Animation animation = {
			.valid = true,
			.time_per_sprite = 0.016,
			.sprite_sheet = sprite_sheet,
			.row = row,
	};

	strcpy(animation.name, name);

	return animation;
}

#endif