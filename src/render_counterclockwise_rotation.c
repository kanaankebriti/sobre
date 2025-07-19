#include "sobre_engine.h"

// in: tiles array
// in: number of tiles in array
// in: selected tile to be rotated
void render_counterclockwise_rotation
(
	SDL_Renderer* renderer,
	tiles* tile,
	unsigned char number_of_tiles,
	unsigned char target,
	SDL_Texture* texture_bank[],
	tiles* hud_tiles
)
{
	//unsigned long int startTime = 0, endTime = 0, delta = 0;
	unsigned short int i;				// index
	unsigned short int final_angle = 0;	// final angle to be reached

	// set the clock
	//startTime = SDL_GetTicks();			// set clock
	//endTime = startTime + 10;			// duration for first frame

	// one whole turn. 360deg turn is same as 0deg turn!
	// and we avoid signed tile angle
	if (tile[target].angle == 0)
		tile[target].angle = 360;

	// angle that must be reached. current angle - 90deg
	final_angle = tile[target].angle - 90;

	// rotation animation at constant framerate
	do
	{
		SDL_RenderClear(renderer);					// clear screen
		//delta = endTime - startTime;				// how many ms for a frame
		//if (delta < ROTATION_FRAMERATE)				// delay if less than 33ms
			//SDL_Delay(ROTATION_FRAMERATE - delta);
		tile[target].angle -= ROTATION_ANIMATION_INCEREMENT;// do rotation
		// copy all tiles again
		for (i = 0; i < number_of_tiles; i++)
			// skip dummy tile
			if (tile[i].rect.w)
				SDL_RenderCopyEx
				(
					renderer,
					texture_bank[tile[i].texture_index_cur],
					0,
					&tile[i].rect,
					tile[i].angle,
					0,
					0
				);
			else
				continue;

		// copy hud main button again
		SDL_RenderCopyEx
		(
			renderer,
			texture_bank[hud_tiles[HUD_MAIN_BTN_ID].texture_index_cur],
			0,
			&hud_tiles[HUD_MAIN_BTN_ID].rect,
			hud_tiles[HUD_MAIN_BTN_ID].angle,
			0,
			0
		);

		// draw HUD digits
		for (i = HUD_FIRST_DIGIT_ID; i <= HUD_LAST_DIGIT_ID; i++)
			SDL_RenderCopyEx
			(
				renderer,
				texture_bank[hud_tiles[i].texture_index_cur],
				0,
				&hud_tiles[i].rect,
				hud_tiles[i].angle,
				0,
				0
			);

		SDL_RenderPresent(renderer);
		// set next clock tick
		//startTime = endTime;
		//endTime = SDL_GetTicks();
	} while (tile[target].angle != final_angle);
}
