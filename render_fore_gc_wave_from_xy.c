#include "sobre_engine.h"

// render growing circle background wave
// from given (x,y) toward edges of the screen
// CIRCLE_TEXTURE_ID is reserved for it's texture
void render_fore_gc_wave_from_xy
(
	SDL_Renderer* renderer,
	SDL_Texture* texture_bank[],
	int x,
	int y
)
{
	//unsigned long int startTime = 0, endTime = 0, delta = 0;
	SDL_Rect wave_rect;

	wave_rect.x = x;
	wave_rect.y = y;
	wave_rect.w = 0;
	wave_rect.h = 0;

	// set the clock
	//startTime = SDL_GetTicks();		// set clock
	//endTime = startTime + 10;			// duration for first frame

	do
	{
		//delta = endTime - startTime;	// how many ms for a frame
		//if (delta < GC_WAVE_FRAMERATE)	// delay if less than defined
			//SDL_Delay(GC_WAVE_FRAMERATE - delta);

		// draw circle first
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
