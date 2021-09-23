#include "sobre_engine.h"

// render background wave from left to right
// SINGLE_BMP_WHITE_DOT_TEXTURE_ID is reserved for it's texture
void render_lr_wave
(
	SDL_Renderer* renderer,
	tiles* tile,
	unsigned char number_of_tiles,
	SDL_Texture* texture_bank[],
	tiles* hud_tiles
)
{
	unsigned short int i;
	//unsigned long int startTime = 0, endTime = 0, delta = 0;
	SDL_Rect wave_rect;

	wave_rect.x = 0;
	wave_rect.y = 0;
	wave_rect.w = LR_WAVE_START_WIDTH;
	wave_rect.h = WIN_HEIGHT;

	// set the clock
	//startTime = SDL_GetTicks();			// set clock
	//endTime = startTime + 10;			// duration for first frame

	do
	{
		SDL_RenderClear(renderer);		// clear screen
		//delta = endTime - startTime;	// how many ms for a frame
		//if (delta < LR_WAVE_FRAMERATE)	// delay if less than defined
			//SDL_Delay(LR_WAVE_FRAMERATE - delta);

		SDL_RenderCopyEx
		(
			renderer,
			texture_bank[SINGLE_BMP_WHITE_DOT_TEXTURE_ID],
			0,
			&wave_rect,
			0,
			0,
			0
		);

		for (i = 0; i < number_of_tiles; i++)
			// skip dummy tile
			if (tile[i].rect.w)
				SDL_RenderCopyEx
				(
					renderer,
					texture_bank[tile[i].texture_index_anim[0]],
					0,
					&tile[i].rect,
					tile[i].angle,
					0,
					0
				);
			else
				continue;

		// transformations
		wave_rect.w += SWEEPER_WAVE_ANIMATION_INCEREMENT;

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

		SDL_RenderPresent(renderer);

		// set next clock tick
		//startTime = endTime;
		//endTime = SDL_GetTicks();
	} while (wave_rect.w <= WIN_WIDTH + SWEEPER_WAVE_ANIMATION_INCEREMENT);
}
