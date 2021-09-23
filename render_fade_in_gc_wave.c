#include "sobre_engine.h"

// render shrinking circle foreground wave
// from edges of the screen toward center
// CIRCLE_TEXTURE_ID is reserved for it's texture
// in : pointer to renderer
// in : pointer to current level tile_set
// in : number of current level tiles
// in : pointer to texture array
// in : pointer to HUD tile_set
void render_fade_in_gc_wave
(
	SDL_Renderer* renderer,
	tiles* tile,
	unsigned char number_of_tiles,
	SDL_Texture* texture_bank[],
	tiles* hud_tiles
)
{
	//unsigned long int startTime = 0, endTime = 0, delta = 0;
	unsigned short int i;				// tile index
	SDL_Rect snapshot_rect;				// rect of current level snapshot
	SDL_Rect back_circle_rect;			// rect of additional big circle added to current snapshot
	SDL_Surface* current_screen_surf;	// current screen snapshot surface
	SDL_Texture* current_screen_text;	// current screen snapshot texture

	// set snapshot rect
	snapshot_rect.x = 0;
	snapshot_rect.y = 0;
	snapshot_rect.w = WIN_WIDTH;
	snapshot_rect.h = WIN_HEIGHT;

	// set rect for sorouding circle
	back_circle_rect.x = (int)((WIN_WIDTH - GC_WAVE_START_PERC * WIN_WIDTH) / 2);
	back_circle_rect.y = (int)((WIN_HEIGHT - GC_WAVE_START_PERC * WIN_WIDTH) / 2);
	back_circle_rect.w = (int)(GC_WAVE_START_PERC * WIN_WIDTH);
	back_circle_rect.h = (int)(GC_WAVE_START_PERC * WIN_WIDTH);

	// save current screen snapshot in a texture
	current_screen_surf = SDL_CreateRGBSurface(0, WIN_WIDTH, WIN_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, current_screen_surf->pixels, current_screen_surf->pitch);
	current_screen_text = SDL_CreateTextureFromSurface(renderer, current_screen_surf);

	// set the clock
	//startTime = SDL_GetTicks();			// set clock
	//endTime = startTime + 10;			// duration for first frame

	do
	{
		SDL_RenderClear(renderer);		// clear screen
		//delta = endTime - startTime;	// how many ms for a frame
		//if (delta < GC_WAVE_FRAMERATE)	// delay if less than defined
			//SDL_Delay(GC_WAVE_FRAMERATE - delta);

		// draw new level first
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

		// draw hud main button
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

		// transformations
		snapshot_rect.x += GC_WAVE_FADE_IN_STEP / 2;
		snapshot_rect.y += GC_WAVE_FADE_IN_STEP / 4;
		snapshot_rect.w -= GC_WAVE_FADE_IN_STEP;
		snapshot_rect.h -= GC_WAVE_FADE_IN_STEP / 2;
		back_circle_rect.x += GC_WAVE_FADE_IN_STEP / 2;
		back_circle_rect.y += GC_WAVE_FADE_IN_STEP / 2;
		back_circle_rect.w -= GC_WAVE_FADE_IN_STEP;
		back_circle_rect.h -= GC_WAVE_FADE_IN_STEP;

		// draw shrinking circle
		SDL_RenderCopyEx
		(
			renderer,
			texture_bank[CIRCLE_TEXTURE_ID],
			0,
			&back_circle_rect,
			0,
			0,
			0
		);

		// draw shrinking previous level snapshot on top of shrinking circle
		SDL_RenderCopyEx
		(
			renderer,
			current_screen_text,
			&snapshot_rect,
			&snapshot_rect,
			0,
			0,
			0
		);

		SDL_RenderPresent(renderer);

		// set next clock tick
		//startTime = endTime;
		//endTime = SDL_GetTicks();
	} while (back_circle_rect.w > -GC_WAVE_FADE_OUT_STEP);

	// free up memory
	SDL_FreeSurface(current_screen_surf);
	SDL_DestroyTexture(current_screen_text);
}
