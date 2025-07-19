#include "sobre_engine.h"
#include "mbasic.h"

unsigned char run_hud
(
	SDL_Renderer* renderer,
	tiles* hud_tiles,
	unsigned char hud_num_of_tiles,
	tiles* lvl_tiles,
	unsigned char lvl_num_of_tiles,
	SDL_Texture** texture_bank,
	unsigned char back_r,
	unsigned char back_g,
	unsigned char back_b
)
{
	unsigned char i;				// selected tile_set index
	int mouse_pos_x, mouse_pos_y;
	SDL_Event event;

	// clear screen
	render_clear(renderer, back_r, back_g, back_b);
	// fade in HUD while make level tiles more transparent
	render_fade_in_hud_from_bottom(renderer, lvl_tiles, lvl_num_of_tiles, texture_bank, hud_tiles);

	/* pause menu event loop */
	while (SDL_WaitEvent(&event))
		switch (event.type)
		{
		/* on mouse movement */
		case SDL_MOUSEMOTION:
			// retrive mouse relative location
			SDL_GetMouseState(&mouse_pos_x, &mouse_pos_y);
			// retrive hovered tile_set index
			i = get_selected_tile_index(hud_tiles, hud_num_of_tiles, mouse_pos_x, mouse_pos_y);

			// no hover on any tiles
			// then i = menu_num_of_tiles
			if (i == hud_num_of_tiles)
				render_hover_on_hud_fade_out_with_lvl_tiles(renderer, texture_bank, hud_tiles, hud_num_of_tiles, lvl_tiles, lvl_num_of_tiles);
			// catched hover
			else
				// hover fade in only if tile is not already in hovered state
				if (hud_tiles[i].texture_index_cur != hud_tiles[i].texture_index_anim[hud_tiles[i].texture_index_anim_size - 1])
					render_hover_on_hud_fade_in_with_lvl_tiles(renderer, texture_bank, hud_tiles, hud_num_of_tiles, i, lvl_tiles, lvl_num_of_tiles);

			// helps flickering reduction
			//SDL_Delay(HOVER_DELAY);

			break;

		/* on mouse button press */
		case SDL_MOUSEBUTTONDOWN:
			// retrive mouse relative location
			SDL_GetMouseState(&mouse_pos_x, &mouse_pos_y);

			// determine pressed mouse button
			switch (event.button.button)
			{
				// on left click press
				// on left or right click press
			case SDL_BUTTON_RIGHT:
			case SDL_BUTTON_LEFT:
				// retrive selected tile_set index
				i = get_selected_tile_index(hud_tiles, hud_num_of_tiles, mouse_pos_x, mouse_pos_y);
				// click on first tile
				// means return to main menu
				switch (i)
				{
				case 1:
					return CMD_RUN_MAIN_MENU;
				case 2:break;
					// return CMD_RUN_NEXT_LVL;
					// click on anwhere else other than HUD icons means resume game
				default:
					// reset textures opacity
					// TODO: 2 to 6 ??!! wtf? what if indices change?
					for (i = 12; i <= 16; i++)
						SDL_SetTextureAlphaMod(texture_bank[i], 255);
					// clear screen
					render_clear(renderer, back_r, back_g, back_b);
					// fade out tiles other than main button
					render_fade_out_hud_from_bottom(renderer, lvl_tiles, lvl_num_of_tiles, texture_bank, hud_tiles);
					return CMD_RESUME_GAME;
				}
			}
			break;

		/* on keyboard keydown */
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			// press esacpe key
			case SDLK_ESCAPE:
				// reset textures opacity
				// TODO: 2 to 6 ??!! wtf? what if indices change?
				for (i = 12; i <= 16; i++)
					SDL_SetTextureAlphaMod(texture_bank[i], 255);
				// clear screen
				render_clear(renderer, back_r, back_g, back_b);
				// fade out tiles other than main button
				render_fade_out_hud_from_bottom(renderer, lvl_tiles, lvl_num_of_tiles, texture_bank, hud_tiles);
				// exit pause menu
				return CMD_RESUME_GAME;
			}
			break;
		/* on quit event such as pressing the close button */
		case SDL_QUIT:
			return CMD_END_GAME;
		}
}

unsigned char run_game_1(SDL_Renderer* renderer)
{
	unsigned char i;							// selected tile_set index
	unsigned char ret_cmd = CMD_NULL;			// command on return from this function
	unsigned char back_r, back_g, back_b;		// background color
	int mouse_pos_x, mouse_pos_y;				// position of mouse
	unsigned char lvl_num_of_tiles;				// number of current level tiles
	unsigned char hud_num_of_tiles;				// number of HUD tiles
	unsigned char lvl_width = 0, lvl_height = 0;// level dimensions
	unsigned char hud_response;					// response of HUD event loop
	unsigned char game_num_of_sounds = 0;
	SDL_Event event;							// event handler
	SDL_Texture** texture_bank = NULL;			// textures array
	tiles* tile_set = NULL;						// tile_set for current level
	tiles* hud_tiles = NULL;					// tile set of HUD
	unsigned char lvl_num = 1;					// start by loading lvl 1
	unsigned char number_of_textures;
	Mix_Chunk** sounds_bank = NULL;				// array of sounds

	texture_bank = load_textures(renderer, 1, &number_of_textures);
	if (!texture_bank)							// check if there exist any texture
		return CMD_RUN_FAIL;
	tile_set = load_level(1, lvl_num, &lvl_width, &lvl_height);
	if (!tile_set)
		return CMD_RUN_FAIL;
	hud_tiles = load_hud(1, &hud_num_of_tiles);
	if (!hud_tiles)								// check if HUD has been loaded successfully
		return CMD_RUN_FAIL;
	lvl_num_of_tiles = lvl_width * lvl_height;
	if (!lvl_num_of_tiles)						// zero tile level is not accepted
		return CMD_RUN_FAIL;
	sounds_bank = load_sounds(0, &game_num_of_sounds);
	if (!sounds_bank)							// check if sounds has been loaded successfully
		return CMD_RUN_FAIL;

	// set random background colour
	back_r = (unsigned char)((rand() % (COLOR_MAX_ILL - COLOR_MIN_ILL)) + COLOR_MIN_ILL);
	back_g = (unsigned char)((rand() % (COLOR_MAX_ILL - COLOR_MIN_ILL)) + COLOR_MIN_ILL);
	back_b = (unsigned char)((rand() % (COLOR_MAX_ILL - COLOR_MIN_ILL)) + COLOR_MIN_ILL);

	// set new level background color
	SDL_SetRenderDrawColor(renderer, back_r, back_g, back_b, 1);

	// set same random colour tone for tiles
	for (i = FIRST_COMMON_TILE_TEXTURE_ID; i <= LAST_COMMON_TILE_TEXTURE_ID; i++)
		SDL_SetTextureColorMod(texture_bank[i], back_r, back_g, back_b);

	// set level counter of HUD
	set_hud_lvl_counter_digits(hud_tiles, lvl_num);

	// first render
	render_fade_in_gc_wave(renderer, tile_set, lvl_num_of_tiles, texture_bank, hud_tiles);

	// game 1 loop
	while (SDL_WaitEvent(&event))
		switch (event.type)
		{
			// on mouse button press
		case SDL_MOUSEBUTTONDOWN:
			// retrive mouse relative location
			SDL_GetMouseState(&mouse_pos_x, &mouse_pos_y);
			// check if clicked on main menu button
			// reserved index is HUD_MAIN_BTN_ID e.g. hud_tiles[HUD_MAIN_BTN_ID]
			i = get_selected_tile_index(hud_tiles, 1, mouse_pos_x, mouse_pos_y);
			if (i == HUD_MAIN_BTN_ID)
			{
				hud_response = run_hud(renderer, hud_tiles, hud_num_of_tiles, tile_set, lvl_num_of_tiles, texture_bank, back_r, back_g, back_b);
				switch (hud_response)
				{
				case CMD_RESUME_GAME:
					break;
				case CMD_RUN_MAIN_MENU:
					ret_cmd = CMD_RUN_MAIN_MENU;
					goto free_up_memory_and_return;
				case CMD_END_GAME:
					ret_cmd = CMD_END_GAME;
					goto free_up_memory_and_return;
				}
			}
			else
			{
				// retrive selected tile_set index
				i = get_selected_tile_index(tile_set, lvl_num_of_tiles, mouse_pos_x, mouse_pos_y);

				// check for get_selected_tile_index out of bound
				if (i == lvl_num_of_tiles + 1)
					break;

				// runs the associated script if defined
				if (tile_set[i].script_path[0] != '\0') {
					mbasic_load_script(tile_set[i].script_path);
					mbasic_exec();
					mbasic_destroy_prgm();
				}

				// determine pressed mouse button
				switch (event.button.button)
				{
					// on left or right click press
				case SDL_BUTTON_LEFT:
					render_counterclockwise_rotation(renderer, tile_set, lvl_num_of_tiles, i, texture_bank, hud_tiles);
					rotate_tile_counterclockwise(&tile_set[i]);
					break;
				case SDL_BUTTON_RIGHT:
					render_clockwise_rotation(renderer, tile_set, lvl_num_of_tiles, i, texture_bank, hud_tiles);
					rotate_tile_clockwise(&tile_set[i]);
					break;
				}

				if (no_connection(tile_set, lvl_width, lvl_height))
				{
					// play a sound
					Mix_PlayChannel(-1, sounds_bank[0], 0);

					// show background wave animation
					render_back_gc_wave_from_tile_with_hud(renderer, tile_set, lvl_num_of_tiles, texture_bank, i, hud_tiles);
					// increase level number
					lvl_num++;
					// free up previous level
					for (i = 0; i < lvl_num_of_tiles; i++)
						free(tile_set[i].texture_index_anim);
					free(tile_set);
					// load new level
					tile_set = load_level(1, lvl_num, &lvl_width, &lvl_height);
					lvl_num_of_tiles = lvl_width * lvl_height;
					if (!tile_set)
					{
						ret_cmd = CMD_RUN_FAIL;
						goto free_up_memory_and_return;
					}

					// set new random background colour
					back_r = (unsigned char)((rand() % (COLOR_MAX_ILL - COLOR_MIN_ILL)) + COLOR_MIN_ILL);
					back_g = (unsigned char)((rand() % (COLOR_MAX_ILL - COLOR_MIN_ILL)) + COLOR_MIN_ILL);
					back_b = (unsigned char)((rand() % (COLOR_MAX_ILL - COLOR_MIN_ILL)) + COLOR_MIN_ILL);

					// set new level background color
					SDL_SetRenderDrawColor(renderer, back_r, back_g, back_b, 1);

					// set same random colour tone for tiles
					for (i = FIRST_COMMON_TILE_TEXTURE_ID; i <= LAST_COMMON_TILE_TEXTURE_ID; i++)
						SDL_SetTextureColorMod(texture_bank[i], back_r, back_g, back_b);

					// set level counter of HUD
					set_hud_lvl_counter_digits(hud_tiles, lvl_num);

					// render
					render_fade_in_gc_wave(renderer, tile_set, lvl_num_of_tiles, texture_bank, hud_tiles);
				}
			}
			break;

			// on keyboard keydown
		case SDL_KEYDOWN:
			//case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
				// press esacpe key
			case SDLK_ESCAPE:
				// run pause menu
				hud_response = run_hud(renderer, hud_tiles, hud_num_of_tiles, tile_set, lvl_num_of_tiles, texture_bank, back_r, back_g, back_b);
				switch (hud_response)
				{
				case CMD_RESUME_GAME:
					break;
				case CMD_RUN_MAIN_MENU:
					ret_cmd = CMD_RUN_MAIN_MENU;
					goto free_up_memory_and_return;
				case CMD_END_GAME:
					ret_cmd = CMD_END_GAME;
					goto free_up_memory_and_return;
				}
			}
			break;

			// on quit event such as pressing the close button
		case SDL_QUIT:
			ret_cmd = CMD_END_GAME;
			goto free_up_memory_and_return;
		}

free_up_memory_and_return:
	for (i = 0; i < number_of_textures; i++)
	{
		SDL_DestroyTexture(texture_bank[i]);
		texture_bank[i] = NULL;
	}
	free(texture_bank);
	texture_bank = NULL;
	for (i = 0; i < hud_num_of_tiles; i++)
		free(hud_tiles[i].texture_index_anim);
	free(hud_tiles);
	hud_tiles = NULL;
	for (i = 0; i < game_num_of_sounds; i++)
	{
		Mix_FreeChunk(sounds_bank[i]);
		sounds_bank[i] = NULL;
	}
	free(sounds_bank);
	sounds_bank = NULL;

	return ret_cmd;
}
