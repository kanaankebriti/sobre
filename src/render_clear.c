#include "sobre_engine.h"

// in: renderer
void render_clear
(
	SDL_Renderer* renderer,
	unsigned char r,
	unsigned char g,
	unsigned char b
)
{
	// set random background colour
	SDL_SetRenderDrawColor(renderer, r, g, b, 1);
	// clear screen
	SDL_RenderClear(renderer);
}
