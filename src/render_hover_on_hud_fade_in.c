#include "sobre_engine.h"

// render fading in hovered hud
// from edges of the screen toward center
// texture #1 is reserved for it's texture
// in : pointer to renderer
// in : pointer to current level tile_set
// in : number of current level tiles
// in : pointer to texture array
// in : pointer to HUD tile_set
// in : number of HUD tiles
// in : index of animating HUD tile
void render_hover_on_hud_fade_in
(
	SDL_Renderer* renderer,
	SDL_Texture* texture_bank[],
	tiles* hud_tiles,
	unsigned char hud_size,
	unsigned char target_hud_index
)
{
	unsigned char i, j;						// tile indices

	for (i = 0; i < hud_size; i++)			// avoid texture stuck on fast pointer movement
		hud_tiles[i].texture_index_cur =	// set current texture index to texture #0
		hud_tiles[i].texture_index_anim[0];

	// animate hovered in tile from anim #0 to #n
	for (i = 0; i < hud_tiles[target_hud_index].texture_index_anim_size; i++)
	{
		// curent texture = hovered texture
		hud_tiles[target_hud_index].texture_index_cur = hud_tiles[target_hud_index].texture_index_anim[i];

		SDL_RenderClear(renderer);				// clear screen
		SDL_Delay(HUD_TILE_FADE_IN_OUT_DELAY);	// prevent instant animation!

		// copy all tiles
		for (j = 0; j < hud_size; j++)
			// skip dummy tile
			if (hud_tiles[j].rect.w)
				SDL_RenderCopyEx
				(
					renderer,
					texture_bank[hud_tiles[j].texture_index_cur],
					0,
					&hud_tiles[j].rect,
					hud_tiles[j].angle,
					0,
					0
				);
			else
				continue;

		SDL_RenderPresent(renderer);
	}
}
