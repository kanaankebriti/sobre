#include "sobre_engine.h"

// in: renderer
// in: tiles array
// in: number of tiles
// in: array of textures
void render_still_layer
(
	SDL_Renderer* renderer,
	tiles* tile,
	unsigned char num_of_tiles,
	SDL_Texture* texture_bank[]
)
{
	// copy all tiles
	for (unsigned char i = 0; i < num_of_tiles; i++)
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

	SDL_RenderPresent(renderer);
}
