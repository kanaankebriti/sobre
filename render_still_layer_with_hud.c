#include "sobre_engine.h"

// in: renderer
// in: tiles array
// in: number of tiles
// in: array of textures
// in: array of HUD tiles
// in: number of HUD tiles
void render_still_layer_with_hud
(
	SDL_Renderer* renderer,
	tiles* tile,
	unsigned char num_of_tiles,
	SDL_Texture* texture_bank[],
	tiles* hud_tiles,
	unsigned char hud_size
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

	// draw hud main button on top of everything
	for (unsigned char i = 0; i < hud_size; i++)
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
}
