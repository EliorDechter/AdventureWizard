#include <stdbool.h>
#include "platform.h"

#if 0
int main(void)
{
	//InitEditor();
	PlatformInit();

	bool is_playing = false;

	bool run_game_loop = true;
	bool close = false;
	bool init = true;
	bool run_slice_spritesheet = false;
	bool is_log_dirty = false;
	//PlatformFont font = { LoadFontEx("C:\\Users\\elior\\OneDrive\\Desktop\\ConsoleApplication1\\Resoruces\\Roboto-Regular.ttf", 26, 0, 250) };

	while (!close)
	{

		if (init) {
			//InitGame();

			init = false;
		}

		EguiInit(GetScreenWidth(), GetScreenHeight());

		/*if (WindowShouldClose() || IsKeyPressed(KEY_ESCAPE)) {
			SaveGame();
			close = true;
		}*/

		/*BeginTextureMode(target.texture);
		ClearBackground(GRAY);
		EndTextureMode();*/

#if 0
		if (IsKeyReleased(KEY_P)) {
			run_game_loop = false;
		}
#endif

		PlatformBeginDrawing();
		{
			bool enable_input = false;
			/*	if (egui.hot_window == 2)
					enable_input = true;*/

			if (run_game_loop) {
				//RunGameLoop(target, enable_input);
				Run();
			}

			if (run_slice_spritesheet) {
				run_game_loop = false;
			}

			//DrawTargetTexture(target, (Vector2) { GAME_SCREEN_X, GAME_SCREEN_Y });

			//DoEditor();

#if 0
			BeginTextureMode(target);
			{
				if (!is_playing) {
					DoWalkingAreaVertices();
					DrawWalkingArea();
					DrawCurrentPath();
				}
			}
			EndTextureMode();
#endif

			DrawBatch();

		}
		PlatformEndDrawing();

		is_log_dirty = false;
	}

	CloseWindow();

	return 0;
}
#endif