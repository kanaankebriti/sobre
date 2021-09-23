#include "sobre_engine.h"

// in: tiles array
// in: number of tiles in array
// in: selected tile to be rotated
void render_fade_out_to_left
(
	SDL_Renderer* renderer,
	tiles* tile,
	unsigned char number_of_tiles,
	SDL_Texture* texture_bank[]
)
{
	unsigned short int i;
	//unsigned long int startTime = 0, endTime = 0, delta = 0;

	// set the clock
	//startTime = SDL_GetTicks();			// set clock
	//endTime = startTime + 10;			// duration for first frame

	// fade to left animation at constant framerate
	do
	{
		SDL_RenderClear(renderer);					// clear screen
		//delta = endTime - startTime;				// how many ms for a frame
		//if (delta < FADE_TO_LEFT_FRAMERATE)			// delay if less than 33ms
			//SDL_Delay(FADE_TO_LEFT_FRAMERATE - delta);
		// copy all tiles again
		for (i = 0; i < number_of_tiles; i++)
		{
			// skip dummy tile
			if (tile[i].rect.w)
			{
				tile[i].rect.x -= FADE_TO_LEFT_STEP;
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
			}
			else
				continue;
		}
		SDL_RenderPresent(renderer);
		// set next clock tick
		//startTime = endTime;
		//endTime = SDL_GetTicks();
	} while (tile[i - 1].rect.x + tile[i - 1].rect.w >= 0);
}
