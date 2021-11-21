#include "sobre_engine.h"

bool backgroud_music_thread_state = true;

static void music_generator_loop(Mix_Chunk** sounds_bank)
{
	unsigned char num_of_sounds = 0;
	unsigned char rand_note_num;
	unsigned short int rand_wait_time;

	// retrieve length of sounds_bank
	while (sounds_bank[num_of_sounds])
		num_of_sounds++;

	while (true)
	{
		if (!backgroud_music_thread_state)														// kill thread when sound bank has been cleared
			return;
		rand_note_num = rand() % num_of_sounds + FIRST_SOUND_ID;								// choose a random note between FIRST_SOUND_ID (reserved sounds) and num_of_sounds
		rand_wait_time = (rand() % (NOTE_MAX_WAIT_MS - NOTE_MIN_WAIT_MS)) + NOTE_MIN_WAIT_MS;	// choose a random pause period between NOTE_MIN_WAIT_MS and NOTE_MAX_WAIT_MS
		Mix_PlayChannel(-1, sounds_bank[rand_note_num], 0);										// play the note
		SDL_Delay(rand_wait_time);																// make a pause
	}
}

unsigned char run_main_menu(SDL_Renderer* renderer)
{
	unsigned char i;							// selected tile_set index
	unsigned char back_r, back_g, back_b;		// background color
	unsigned char ret_cmd = CMD_NULL;			// command on return from this function
	unsigned char number_of_textures;
	int mouse_pos_x, mouse_pos_y;
	unsigned char menu_num_of_tiles = 0;
	unsigned char menu_num_of_sounds = 0;
	SDL_Event event;
	SDL_Thread* music_thread;
	SDL_Texture** texture_bank = NULL;			// array of textures
	Mix_Chunk** sounds_bank = NULL;				// array of sounds
	tiles* menu_tiles = NULL;					// array of tiles

	texture_bank = load_textures(renderer, 0, &number_of_textures);
	if (!texture_bank)							// check if there exist any texture
		return CMD_RUN_FAIL;
	menu_tiles = load_hud(0, &menu_num_of_tiles);
	if (!menu_tiles)							// check if HUD has been loaded successfully
		return CMD_RUN_FAIL;
	sounds_bank = load_sounds(0, &menu_num_of_sounds);
	if (!sounds_bank)							// check if sounds has been loaded successfully
		return CMD_RUN_FAIL;
	backgroud_music_thread_state = true;

	music_thread = SDL_CreateThread(music_generator_loop, "music_generator_loop_thread", sounds_bank);
	if (!music_thread)							// check if background music thread has been created successfully
		return CMD_RUN_FAIL;
	SDL_DetachThread(music_thread);				// background music loop is running forever and detached from game

	// set random background colour
	back_r = (rand() % (COLOR_MAX_ILL - COLOR_MIN_ILL)) + COLOR_MIN_ILL;
	back_g = (rand() % (COLOR_MAX_ILL - COLOR_MIN_ILL)) + COLOR_MIN_ILL;
	back_b = (rand() % (COLOR_MAX_ILL - COLOR_MIN_ILL)) + COLOR_MIN_ILL;

	// set same random colour tone for tiles
	for (i = FIRST_COMMON_TILE_TEXTURE_ID; i <= LAST_COMMON_TILE_TEXTURE_ID; i++)
		SDL_SetTextureColorMod(texture_bank[i], back_r, back_g, back_b);

	// first render
	render_clear(renderer, back_r, back_g, back_b);
	render_still_layer(renderer, menu_tiles, menu_num_of_tiles, texture_bank);

	// main menu loop
	while (SDL_WaitEvent(&event))
		switch (event.type)
		{
			// on mouse move and hover
		case SDL_MOUSEMOTION:
			// retrive mouse relative location
			SDL_GetMouseState(&mouse_pos_x, &mouse_pos_y);
			// retrive hovered tile_set index
			i = get_selected_tile_index(menu_tiles, menu_num_of_tiles, mouse_pos_x, mouse_pos_y);

			// no hover on any tiles
			// then i = menu_num_of_tiles
			if (i == menu_num_of_tiles)
				render_hover_on_hud_fade_out(renderer, texture_bank, menu_tiles, menu_num_of_tiles);
			// catched hover
			else
				// hover fade in only if tile is not already in hovered state
				if (menu_tiles[i].texture_index_cur != menu_tiles[i].texture_index_anim[menu_tiles[i].texture_index_anim_size - 1])
					render_hover_on_hud_fade_in(renderer, texture_bank, menu_tiles, menu_num_of_tiles, i);

			// helps flickering reduction
			SDL_Delay(HOVER_DELAY);

			break;

			// on mouse button press
		case SDL_MOUSEBUTTONDOWN:
			// determine pressed mouse button
			switch (event.button.button)
			{
				// on right or left click press
			case SDL_BUTTON_RIGHT:
			case SDL_BUTTON_LEFT:
				// retrive mouse relative location
				SDL_GetMouseState(&mouse_pos_x, &mouse_pos_y);
				// retrive clicked tile_set index
				i = get_selected_tile_index(menu_tiles, menu_num_of_tiles, mouse_pos_x, mouse_pos_y);
				// check for get_selected_tile_index out of bound
				if (i == menu_num_of_tiles + 1)
					break;

				switch(i)
				{
				// first option
				// runs game 1
				case 0:
					// render animation
					render_fore_gc_wave_from_xy(renderer, texture_bank, mouse_pos_x, mouse_pos_y);

					// end of main menu loop
					ret_cmd = CMD_RUN_GAME_1;
					goto free_up_memory_ad_return;

				// second option is exit
				case 1:
					ret_cmd = CMD_END_GAME;
					goto free_up_memory_ad_return;
				}
			}
			break;

			// on keyboard keydown
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
				// press esacpe key
			case SDLK_ESCAPE:
				ret_cmd = CMD_END_GAME;
				goto free_up_memory_ad_return;
			}
			break;

			// on quit event such as pressing the close button
		case SDL_QUIT:
			ret_cmd = CMD_END_GAME;
			goto free_up_memory_ad_return;
		}

free_up_memory_ad_return:
	for (i = 0; i < number_of_textures; i++)
	{
		SDL_DestroyTexture(texture_bank[i]);
		texture_bank[i] = NULL;
	}
	free(texture_bank);
	texture_bank = NULL;
	for (i = 0; i < menu_num_of_tiles; i++)
		free(menu_tiles[i].texture_index_anim);
	free(menu_tiles);
	menu_tiles = NULL;
	backgroud_music_thread_state = false;
	for (i = 0; i < menu_num_of_sounds; i++)
	{
		Mix_FreeChunk(sounds_bank[i]);
		sounds_bank[i] = NULL;
	}
	free(sounds_bank);
	sounds_bank = NULL;

	return ret_cmd;
}
