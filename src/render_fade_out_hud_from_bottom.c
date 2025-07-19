#include "sobre_engine.h"

// in: renderer
// in: tiles array
// in: number of tiles
// in: array of textures
// in: array of HUD tiles
// in: number of HUD tiles
void render_fade_out_hud_from_bottom
(
	SDL_Renderer* renderer,
	tiles* tile,
	unsigned char num_of_tiles,
	SDL_Texture* texture_bank[],
	tiles* hud_tiles
)
{
	unsigned char i;
	unsigned char lvl_tiles_transparency = MIN_TRANSPARENCY_LVL_TILES;
	unsigned char hud_tiles_transparency = MAX_TRANSPARENCY;
	unsigned char lvl_tiles_transparency_step = (unsigned char)(((MAX_TRANSPARENCY - MIN_TRANSPARENCY_LVL_TILES) * HUD_FADE_IN_OUT_STEP) / (WIN_HEIGHT - hud_tiles[HUD_MAIN_MENU_BTN_ID].rect.y));
	unsigned char hud_tiles_transparency_step = (unsigned char)(((MAX_TRANSPARENCY - MIN_TRANSPARENCY_HUD_TILES) * HUD_FADE_IN_OUT_STEP) / (WIN_HEIGHT - hud_tiles[HUD_MAIN_BTN_ID].rect.y));

	// skip one final increment
	lvl_tiles_transparency -= lvl_tiles_transparency_step;

	do
	{
		// change level tiles opacity so they reappear fully opaque
		// increase level tiles transparency from MIN_TRANSPARENCY_LVL_TILES to MAX_TRANSPARENCY
		lvl_tiles_transparency += lvl_tiles_transparency_step;
		for (i = FIRST_LVL_TILE_TEXTURE_ID; i <= LAST_LVL_TILE_TEXTURE_ID; i++)
			SDL_SetTextureAlphaMod(texture_bank[i], lvl_tiles_transparency);
		// decrease hud transparency from MAX_TRANSPARENCY to MIN_TRANSPARENCY_HUD_TILES
		hud_tiles_transparency -= hud_tiles_transparency_step;
		for (i = 8; i <= 11; i++)
			SDL_SetTextureAlphaMod(texture_bank[i], hud_tiles_transparency);

		// clear screen
		SDL_RenderClear(renderer);
		// draw all tiles
		for (i = 0; i < num_of_tiles; i++)
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

		// draw main hud button
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

		// animate rest of hud buttons
		for (i = HUD_FIRST_ANIMATING_BTN_ID; i <= HUD_LAST_ANIMATING_BTN_ID; i++)
		{
			// depreciate tile
			hud_tiles[i].rect.y += 10;
			// draw tile
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
		}
		SDL_RenderPresent(renderer);
	} while (hud_tiles[1].rect.y <= WIN_HEIGHT);
}
