#include "sobre_engine.h"

// render growing circle background wave
// from last selected tile toward edges of the screen
// CIRCLE_TEXTURE_ID is reserved for it's texture
void render_back_gc_wave_from_tile_with_hud
(
	SDL_Renderer* renderer,
	tiles* tile,
	unsigned char number_of_tiles,
	SDL_Texture* texture_bank[],
	unsigned char center_tile_index,
	tiles* hud_tiles
)
{
	unsigned short int i;
	//unsigned long int startTime = 0, endTime = 0, delta = 0;
	SDL_Rect wave_rect;

	wave_rect.x = tile[center_tile_index].rect.x + tile[center_tile_index].rect.w / 2;
	wave_rect.y = tile[center_tile_index].rect.y + tile[center_tile_index].rect.h / 2;
	wave_rect.w = 0;
	wave_rect.h = 0;

	// set the clock
	//startTime = SDL_GetTicks();			// set clock
	//endTime = startTime + 10;			// duration for first frame

	do
	{
		//SDL_RenderClear(renderer);		// clear screen
		//delta = endTime - startTime;	// how many ms for a frame
		//if (delta < GC_WAVE_FRAMERATE)	// delay if less than defined
			//SDL_Delay(GC_WAVE_FRAMERATE - delta);

		// first draw hud main button
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

		// then draw circle
		SDL_RenderCopyEx
		(
			renderer,
			texture_bank[CIRCLE_TEXTURE_ID],
			0,
			&wave_rect,
			0,
			0,
			0
		);

		// draw all tiles on top of circle
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

		// transformations
		wave_rect.x -= GC_WAVE_FADE_OUT_STEP / 2;
		wave_rect.y -= GC_WAVE_FADE_OUT_STEP / 2;
		wave_rect.w += GC_WAVE_FADE_OUT_STEP;
		wave_rect.h += GC_WAVE_FADE_OUT_STEP;

		SDL_RenderPresent(renderer);

		// set next clock tick
		//startTime = endTime;
		//endTime = SDL_GetTicks();
	} while (wave_rect.w <= GC_WAVE_END_PERC * WIN_WIDTH);
}
